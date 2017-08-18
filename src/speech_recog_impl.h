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

#ifndef SRC_SPEECH_RECOG_IMPL_H_
#define SRC_SPEECH_RECOG_IMPL_H_

#include <websocketpp/common/thread.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/logger/basic.hpp>

#include <cpqd/asr-client/speech_recog.h>
#include <cpqd/asr-client/language_model_list.h>
#include <cpqd/asr-client/recognition_result.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_listener.h>
#include <cpqd/asr-client/recognition_error.h>

#include <condition_variable>
#include <exception>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#ifdef _MSC_VER 
#include <atomic>
#endif

class SpeechRecognizer::Impl {
 public:
    typedef websocketpp::client<websocketpp::config::asio_tls_client> Client_tls;
    typedef websocketpp::client<websocketpp::config::asio_client> Client;
    typedef websocketpp::lib::shared_ptr<asio::ssl::context> Context_ptr;
    typedef websocketpp::log::basic<websocketpp::concurrency::basic,
                                    websocketpp::log::alevel>
        AccessLog;
    typedef websocketpp::log::basic<websocketpp::concurrency::basic,
                                    websocketpp::log::elevel>
        ErrorLog;

    enum class Status{ kOpen, kClose, kFailed, kConnecting };

    enum class SessionStatus{ kNone, kIdle, kListening, kRecognizing };

    Impl();

    ~Impl();

    void open(const std::string& url,
              std::string user = std::string(),
              std::string pass = std::string());

    void close();

    void recognitionError(RecognitionError::Code code,
                         std::string message = std::string());

    void sendMessage(std::string& raw_message);

    void terminateSendMessageThread();

    Context_ptr onTlsInit(websocketpp::connection_hdl);

    Client client_;
    Client_tls client_tls_;

    std::atomic<bool> secure_{false};

    AccessLog logger_;
    std::ofstream out_;
    std::mutex lock_;
    std::condition_variable cv_;
    websocketpp::connection_hdl connection_hdl_;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread_;
    std::atomic<bool> recognizing_{false};
    std::atomic<bool> sendAudioMessage_terminate_{false};
    std::atomic<bool> open_{false};
    std::atomic<Status> status_{Status::kConnecting};
    std::atomic<SessionStatus> session_status_{SessionStatus::kNone};
    std::shared_ptr<AudioSource> audio_src_ = nullptr;
    std::unique_ptr<LanguageModelList> lm_ = nullptr;
    std::unique_ptr<RecognitionConfig> config_ = nullptr;
    std::vector<std::unique_ptr<RecognitionListener>> listener_;
    std::vector<RecognitionResult> result_;
    std::exception_ptr eptr_ = nullptr;
    std::thread sendAudioMessage_thread_;
};

#endif  // SRC_SPEECH_RECOG_IMPL_H_
