/*****************************************************************************
 * Copyright 2017 CPqD. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include "src/process_msg.h"
#include "src/process_result.h"
#include "src/speech_recog_impl.h"

#include <string>

template <typename EndpointType>
class WsClient {
 public:
  typedef typename EndpointType::connection_ptr connection_ptr;

  static void init(SpeechRecognizer::Impl* impl, EndpointType* client_config,
                   const std::string& url,
                   std::string user = std::string(),
                   std::string pass = std::string()) {
    client_config->clear_access_channels(websocketpp::log::alevel::all);
    client_config->clear_error_channels(websocketpp::log::alevel::all);

    // Initialize the Asio transport policy
    client_config->init_asio();
    client_config->start_perpetual();

    using std::placeholders::_1;
    using std::placeholders::_2;

    client_config->set_open_handler(
        std::bind(WsClient<EndpointType>::on_open, impl, _1));
    client_config->set_message_handler(
        std::bind(WsClient<EndpointType>::on_message, impl, _1, _2));
    client_config->set_fail_handler(
        std::bind(WsClient<EndpointType>::on_fail, impl, client_config, _1));
    client_config->set_close_handler(
        std::bind(WsClient<EndpointType>::on_close, impl, _1));

    // Create a new connection to the given URI
    websocketpp::lib::error_code err_code;
    connection_ptr connection = client_config->get_connection(url, err_code);
    if (err_code) {
      impl->logger_.write(websocketpp::log::alevel::app,
                          "Get Connection Error: " + err_code.message());
      return;
    }

    if (err_code) {
      impl->logger_.write(websocketpp::log::alevel::app,
                          "Get Connection Error: " + err_code.message());
      return;
    }

    impl->connection_hdl_ = connection->get_handle();

    if (impl->secure_) {
      std::string extra = websocketpp::base64_encode(user + ":" + pass);
      connection->append_header("Authorization", "Basic " + extra);
    }

    client_config->connect(connection);

    impl->thread_.reset(
        new websocketpp::lib::thread(&EndpointType::run, client_config));
    impl->open_ = true;
  }

  static void send_msg(SpeechRecognizer::Impl* impl,
                       EndpointType* client_config, std::string& raw_message) {
    try {
      client_config->send(impl->connection_hdl_, raw_message,
                          websocketpp::frame::opcode::binary);
    } catch (...) {
      impl->eptr_ = std::current_exception();
      impl->cv_.notify_one();
    }
  }

  static void close(SpeechRecognizer::Impl* impl, EndpointType* client_config) {
    client_config->stop_perpetual();
    websocketpp::lib::error_code err_code;
    client_config->close(impl->connection_hdl_,
                         websocketpp::close::status::going_away, "", err_code);
    if (err_code) {
      impl->logger_.write(websocketpp::log::alevel::app,
                          "Error closing connection: " + err_code.message());
    }
    if(impl->thread_->joinable())
      impl->thread_->join();
    impl->open_ = false;
  }

 private:
  static void on_open(SpeechRecognizer::Impl* impl,
                      websocketpp::connection_hdl) {
    impl->status_ = SpeechRecognizer::Impl::Status::kOpen;
    impl->cv_.notify_one();

    impl->logger_.write(websocketpp::log::elevel::info,
                        "Connection opened, starting...!");
  }

  static void on_close(SpeechRecognizer::Impl* impl,
                       websocketpp::connection_hdl) {
    impl->status_ = SpeechRecognizer::Impl::Status::kClose;
    impl->cv_.notify_one();
  }

  static void on_fail(SpeechRecognizer::Impl* impl, EndpointType* client_config,
                      websocketpp::connection_hdl hdl) {
    connection_ptr con = client_config->get_con_from_hdl(hdl);
    std::string server = con->get_response_header("Server");
    std::string reason = con->get_ec().message();

    impl->eptr_ = std::make_exception_ptr(std::invalid_argument(reason));
    impl->status_ = SpeechRecognizer::Impl::Status::kFailed;
    impl->cv_.notify_one();
  }

  static void on_message(SpeechRecognizer::Impl* impl,
                         websocketpp::connection_hdl,
                         typename EndpointType::message_ptr msg) {
    websocketpp::lib::error_code err_code;
    std::string payload = msg->get_payload();

    ASRMessageResponse response;
    response.consume(payload);

    std::shared_ptr<ASRProcessMsg> root =
        std::make_shared<ASRProcessResponse>();
    std::shared_ptr<ASRProcessMsg> proc_res =
        std::make_shared<ASRProcessResult>();
    std::shared_ptr<ASRProcessMsg> proc_start_speech =
        std::make_shared<ASRProcessStartSpeech>();
    std::shared_ptr<ASRProcessMsg> proc_end_speech =
        std::make_shared<ASRProcessEndSpeech>();
    root->add(proc_res);
    root->add(proc_start_speech);
    root->add(proc_end_speech);

    root->handle(*impl, response);

    impl->logger_.write(websocketpp::log::elevel::info,
                        "[RECEIVE] " + response.raw());
  }
};

#endif  // WEBSOCKETCLIENT_H
