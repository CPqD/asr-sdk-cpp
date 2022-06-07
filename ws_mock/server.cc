#include <websocketpp/config/asio.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <fstream>
#include <set>

#include "ws_parser.h"

namespace cpqd {

namespace asr {

std::set<std::string> voiceprint_set;

struct session {
    int session_id;
    int audio_counter;
    std::string mode;
    std::string voiceprint;
    int answer_delay;
};

// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
// the TLS modes. The code below demonstrates how to implement both the modern
enum tls_mode { MOZILLA_INTERMEDIATE = 1, MOZILLA_MODERN = 2 };
typedef websocketpp::server<websocketpp::config::asio_tls> server_tls;
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
// Non TLS server
typedef websocketpp::server<websocketpp::config::asio> server;

typedef std::map<websocketpp::connection_hdl, session, std::owner_less<websocketpp::connection_hdl>>
    con_list;
typedef con_list::iterator con_list_it;

class utility_server {
   public:
    WsParser FillResponseWithParams(const std::string& message, const std::string& version) {
        static std::string session_status = "IDLE";
        static std::string media_type;
        std::map<std::string, std::string> par_values;
        WsParser pp(message), p;

        if (pp.GetCmd() == "START_RECOGNITION") {
            session_status = "LISTENING";
        }

        p.SetCmd("RESPONSE");
        p.SetVersion(version);
        auto ret = GenerateCompletionCause(pp);
        p.Add("Message", std::get<2>(ret));
        p.Add("Error-Code", std::get<1>(ret));
        if (std::get<0>(ret))
            p.Add("Result", "SUCCESS ");
        else
            p.Add("Result", "FAILURE ");

        p.Add("Session-Status", session_status);
        if (pp.GetCmd() == "CREATE_SESSION") {
            session_status = "IDLE";
            media_type = "application/octet-stream";
            par_values.clear();
        } else if (pp.GetCmd() == "GET_PARAMETERS") {
            WsParser pp(message);
            auto pars = pp.GetParams();
            std::string value = "";
            for (auto& par : pars) {
                auto t_value = parser_.Get(par.first);

                if (t_value.valid()) {
                    value = t_value.get();
                }
                std::cout << "Response: " << par.first << " : " << value << std::endl;
                p.Add(par.first, value);
            }
        } else if (pp.GetCmd() == "SET_PARAMETERS") {
            WsParser pp(message);
            parser_ = pp;
            par_values = pp.GetParams();

            auto search = par_values.find("Media-Type");
            if (search != par_values.end()) {
                media_type = search->second;
            }

            search = par_values.find("InvalidParam");
            if (search != par_values.end()) {
                parser_.Add("InvalidParam", "ERR_INVALID_PARAMETER");
                p.Add("InvalidParam", "ERR_INVALID_PARAMETER");
            }
        }
        p.Add("Method", pp.GetCmd());
        p.Add("Handle", "202106091928510001 ");

        return p;
    }

    std::tuple<bool, std::string, std::string> GenerateCompletionCause(WsParser&) {
        return std::make_tuple(true, "000", "success");
    }

    WsParser FillErrorResult(const std::string& version, const std::string& hndl,
                             const std::string& code, const std::string& msg) {
        std::string header = "REGONITION-RESULT";

        WsParser p;

        p.SetCmd(header);
        p.SetVersion(version);
        p.Add("Handle", hndl);
        p.Add("Session-Status", "VERIFYING");
        p.Add("Result-Status", "FAILURE");
        p.Add("Error-Code", code);
        p.Add("Message", msg);

        return p;
    }

    void send(const websocketpp::connection_hdl& hdl, const std::string& msg,
              websocketpp::frame::opcode::value opcode) {
        if (tls_enable)
            m_endpoint_tls.send(hdl, msg.c_str(), opcode);
        else
            m_endpoint.send(hdl, msg.c_str(), opcode);
    }

    void init() {
        if (tls_enable) {
            // Set logging settings
            m_endpoint_tls.set_error_channels(websocketpp::log::elevel::all);
            m_endpoint_tls.set_access_channels(websocketpp::log::alevel::all ^
                                               websocketpp::log::alevel::frame_payload);

            // Initialize Asio
            m_endpoint_tls.init_asio();

            // Set the default message handler to the echo handler
            m_endpoint_tls.set_message_handler(std::bind(
                &utility_server::echo_handler, this, std::placeholders::_1, std::placeholders::_2));

            m_endpoint_tls.set_tls_init_handler(bind(&utility_server::on_tls_init, this,
                                                     MOZILLA_INTERMEDIATE, std::placeholders::_1));

            m_endpoint_tls.set_open_handler(
                bind(&utility_server::on_open, this, std::placeholders::_1));
            m_endpoint_tls.set_close_handler(
                bind(&utility_server::on_close, this, std::placeholders::_1));
        } else {
            // Set logging settings
            m_endpoint.set_error_channels(websocketpp::log::elevel::all);
            m_endpoint.set_access_channels(websocketpp::log::alevel::all ^
                                           websocketpp::log::alevel::frame_payload);

            // Initialize Asio
            m_endpoint.init_asio();

            // Set the default message handler to the echo handler
            m_endpoint.set_message_handler(std::bind(&utility_server::echo_handler, this,
                                                     std::placeholders::_1, std::placeholders::_2));

            m_endpoint.set_open_handler(
                bind(&utility_server::on_open, this, std::placeholders::_1));
            m_endpoint.set_close_handler(
                bind(&utility_server::on_close, this, std::placeholders::_1));
        }
    }
    utility_server(int p, bool enable) : port(p), tls_enable(enable) {
        init();
        std::string err;
    }

    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "======================================= ON OPEN ===\n";
        session data;
        websocketpp::lib::error_code ec;
        std::string path;

        if (tls_enable) {
            server_tls::connection_ptr conn_tls;
            conn_tls = m_endpoint_tls.get_con_from_hdl(hdl, ec);
            path = conn_tls->get_resource();
            conn_tls->set_pong_timeout(120000);
        } else {
            server::connection_ptr conn;
            conn = m_endpoint.get_con_from_hdl(hdl, ec);
            path = conn->get_resource();
            conn->set_pong_timeout(120000);
        }

        WriteMessage("/tmp/token", path, std::ios_base::out);
        std::cout << "                                           PATH: " << path << std::endl;

        data.session_id = m_next_sessionid++;

        data.answer_delay = 0;
        session_map.emplace(hdl, data);
    }

    void on_close(websocketpp::connection_hdl hdl) {
        websocketpp::lib::error_code ec;
        websocketpp::close::status::value code;

        if (tls_enable) {
            server_tls::connection_ptr conn_tls;
            conn_tls = m_endpoint_tls.get_con_from_hdl(hdl, ec);
            code = conn_tls->get_remote_close_code();
        } else {
            server::connection_ptr conn;
            conn = m_endpoint.get_con_from_hdl(hdl, ec);
            code = conn->get_remote_close_code();
        }

        std::cout << "======================================= ON CLOSE ===\n";
        if (code != websocketpp::close::status::normal)
            std::cout << "=========================== ERROR CODE: "
                      << websocketpp::close::status::get_string(code) << " ===\n";
        session_map.erase(hdl);
    }

    static std::string get_password() { return "test"; }

    context_ptr on_tls_init(tls_mode mode, websocketpp::connection_hdl hdl) {
        namespace asio = websocketpp::lib::asio;

        std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
        std::cout << "using TLS mode: "
                  << (mode == MOZILLA_MODERN ? "Mozilla Modern" : "Mozilla Intermediate")
                  << std::endl;

        context_ptr ctx =
            websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

        try {
            if (mode == MOZILLA_MODERN) {
                // Modern disables TLSv1
                ctx->set_options(asio::ssl::context::default_workarounds |
                                 asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 |
                                 asio::ssl::context::no_tlsv1 | asio::ssl::context::single_dh_use);
            } else {
                ctx->set_options(asio::ssl::context::default_workarounds |
                                 asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 |
                                 asio::ssl::context::single_dh_use);
            }
            ctx->set_password_callback(bind(&get_password));
            ctx->use_certificate_chain_file("server.pem");
            ctx->use_private_key_file("server.pem", asio::ssl::context::pem);

            // Example method of generating this file:
            // `openssl dhparam -out dh.pem 2048`
            // Mozilla Intermediate suggests 1024 as the minimum size to use
            // Mozilla Modern suggests 2048 as the minimum size to use.
            ctx->use_tmp_dh_file("dh.pem");

            std::string ciphers;

            if (mode == MOZILLA_MODERN) {
                ciphers =
                    "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-"
                    "GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-"
                    "AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-"
                    "SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:"
                    "ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-"
                    "AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:"
                    "DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!"
                    "MD5:!PSK";
            } else {
                ciphers =
                    "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-"
                    "GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-"
                    "AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-"
                    "SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:"
                    "ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-"
                    "AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:"
                    "DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:"
                    "AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!"
                    "aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-"
                    "RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
            }

            if (SSL_CTX_set_cipher_list(ctx->native_handle(), ciphers.c_str()) != 1) {
                std::cout << "Error setting cipher list" << std::endl;
            }
        } catch (std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
        }
        return ctx;
    }

    void SendStartOfInput(const websocketpp::connection_hdl& hdl) {
        std::string start_of_input_msg =
            "ASR 2.4 START_OF_SPEECH\r\n"
            "Handle: dee87ac1-ab75-464c-8f1d-8cb62fc24c57\r\n"
            "Session-Status: LISTENING\r\n\r\n";
        try {
            std::cout << "=== SENDING START-OF-INPUT ===============================\n";

            send(hdl, start_of_input_msg.c_str(), websocketpp::frame::opcode::binary);
        } catch (const std::exception& e) {
            std::cout << "Failed to send result: " << e.what() << std::endl;
        }
    }

    std::string ReadMessage(const std::string& path) {
        std::ostringstream ss{};
        std::ifstream input_file(path);
        if (input_file.is_open())
            ss << input_file.rdbuf();
        else
            throw std::runtime_error("Verification result file not found: " + path);

        return ss.str();
    }

    void WriteMessage(const std::string& path, std::string str, std::ios_base::openmode mode) {
        std::ofstream output_file(path, mode);
        if (output_file.is_open()) {
            output_file << str;
            output_file.close();
        }
    }
#define END_RECOGNIZE 20
    void echo_handler(websocketpp::connection_hdl hdl, server_tls::message_ptr msg) {
        // write a new message
        std::string payload = msg->get_payload();
        // WsParser parser(payload);
        std::ios_base::openmode mode = std::ios_base::app;

        auto it = session_map.find(hdl);
        std::string err;
        WsParser parser(payload);

        // std::string msg_type = json["mType"].string_value();

        auto ans = payload;
        // Simulate answer delay
        std::cout << "<<<<< Received message: " << payload << std::endl;
        if (parser.GetCmd() != "SEND_AUDIO") {
            auto ans = FillResponseWithParams(payload, "2.4");
            if (parser.GetCmd() == "CREATE_SESSION") {
                it->second.audio_counter = 0;
                it->second.answer_delay = 0;

                // Restart the message dump to file
                mode = std::ios_base::out;
            } else if (parser.GetCmd() == "RELEASE_SESSION") {
                it->second.audio_counter = 0;
            }
            WriteMessage("/tmp/message", payload, mode);
            // Simulate answer delay
            if (it->second.answer_delay) {
                std::cout << "##### Delaying answer: " << ans.Str().c_str() << std::endl;
                sleep(it->second.answer_delay);
            }
            std::cout << ">>>>> Response answer: " << ans.Str().c_str() << std::endl;
            send(hdl, ans.Str().c_str(), msg->get_opcode());

            // Reset answer delay
            it->second.answer_delay = 0;

        } else {
            if (it != session_map.end()) {
                if (it->second.audio_counter < END_RECOGNIZE) it->second.audio_counter++;
            }
            try {
                auto last = parser.Get("LastPacket");
                if (last.valid()) {
                    if (last.get() == "true") {
                        it->second.audio_counter = END_RECOGNIZE;
                        SendStartOfInput(hdl);
                    }
                }
            } catch (...) {
                std::cout << "**** WARNING: Key not found \n";
            }
            // std::cout << "<<<<< Audio frame: " << it->second.audio_counter << std::endl;
        }
        if (it->second.audio_counter == END_RECOGNIZE / 5) {
            SendStartOfInput(hdl);
        }
        if (it->second.audio_counter == END_RECOGNIZE) {
            SendResult(hdl, it);
            it->second.audio_counter = END_RECOGNIZE + 1;    // stop counter
        }
    }

    std::string GetResultName() {
        auto t_value = parser_.Get("hints.words");

        if (t_value.valid()) {
            auto value = t_value.get();
            if (value.find("rover") != std::string::npos) return "result-cars.ws";
        }
        return "result-default.ws";
    }

    void SendResult(const websocketpp::connection_hdl& hdl, const con_list_it&) {
        char result_char[] =
            "ASR 2.4 RECOGNITION_RESULT\r\n"
            "Handle: 202204121653500000\r\n"
            "Result-Status: RECOGNIZED\r\n"
            "Session-Status: IDLE\r\n"
            "Content-Length: 269\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"alternatives\":[{\"text\":\"2345678\",\"words\":[{\"text\":\"2345678\",\"score\":"
            "99,"
            "\"start_time\":1.1,\"end_time\":3.92}],\"score\":99,\"lm\":\"builtin:slm/general\"}],"
            "\"segment_index\":0,\"last_segment\":true,\"final_result\":true,\"start_time\":0.95,"
            "\"end_time\":4.01,\"result_status\":\"RECOGNIZED\"}";

        std::string file_name = GetResultName();
        try {
            std::cout << "=== SENDING RESULT =======================================\n";
            std::cout << "=== File name: " << file_name << std::endl;
            std::string result = result_char;
            std::cout << result << std::endl;
            std::cout << "==========================================================\n";
            send(hdl, result.c_str(), websocketpp::frame::opcode::binary);
        } catch (const std::exception& e) {
            std::cout << "Failed to send result: " << e.what() << std::endl;
            auto p = FillErrorResult("1.0", "67314b44-8491-47e9-a418-332eb8839bbe", "016",
                                     "connection-timeout");
            send(hdl, p.Str().c_str(), websocketpp::frame::opcode::binary);
        }
    }

    void run() {
        std::cout << "Listening on port " << port << std::endl;

        if (tls_enable) {
            // Listen on port 9002
            m_endpoint_tls.listen(port);

            // Queues a connection accept operation
            m_endpoint_tls.start_accept();

            // Start the Asio io_service run loop
            m_endpoint_tls.run();
        } else {
            // Listen on port 9002
            m_endpoint.listen(port);

            // Queues a connection accept operation
            m_endpoint.start_accept();

            // Start the Asio io_service run loop
            m_endpoint.run();
        }
    }

   private:
    server_tls m_endpoint_tls;
    server m_endpoint;
    int port = 9002;
    bool tls_enable = false;
    int m_next_sessionid = 0;
    con_list session_map;
    WsParser parser_;
};

}    // namespace asr

}    // namespace cpqd

int main() {
    bool tls_enable = false;
    char* env_p;

    if ((env_p = std::getenv("TLS_ON"))) {
        tls_enable = true;
        std::cout << "Enabling TLS" << std::endl;
    }

    int port = 9002;
    if ((env_p = std::getenv("WS_PORT"))) {
        port = std::stoul(env_p);
    }

    cpqd::asr::utility_server s(port, tls_enable);
    s.run();
    return 0;
}
