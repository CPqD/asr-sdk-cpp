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

#include "src/speech_recog_impl.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include <cpqd/asr-client/recognition_exception.h>
#include <websocketpp/uri.hpp>

#include "src/asr_message_request.h"
#include "src/asr_message_response.h"
#include "src/process_msg.h"
#include "src/process_result.h"
#include "src/websocket_client.h"

SpeechRecognizer::Impl::Impl() {
}

SpeechRecognizer::Impl::~Impl() {
  if(open_) close();
}

void SpeechRecognizer::Impl::open(const std::string& url,
                                  std::string user,
                                  std::string pass) {
  if (open_) return;

  using std::placeholders::_1;
  using std::placeholders::_2;

  websocketpp::uri uri(url);

  if (uri.get_secure()) {
    secure_ = true;
    client_tls_.set_tls_init_handler(
          std::bind(&SpeechRecognizer::Impl::onTlsInit, this, _1)
          );
    WsClient<Client_tls>::init(this, &client_tls_, url, user, pass);
  } else {
    secure_ = false;
    WsClient<Client>::init(this, &client_, url);
  }
  {
    std::unique_lock<std::mutex> lk(lock_);
    cv_.wait_for(lk,
                 std::chrono::seconds(10), //TODO parameterize connection timeout
                 [this]()
    { return status_ != SpeechRecognizer::Impl::Status::kConnecting;}
    );
  }
  if (status_ != SpeechRecognizer::Impl::Status::kOpen) {
    auto code = RecognitionError::Code::CONNECTION_FAILURE;
    std::string msg("Failure on connecting to server " + url);
    RecognitionError error(code, msg);
    for (std::unique_ptr<RecognitionListener>& listener : listener_)
      listener->onError(error);
    // Connection error shouldn't be ignorable, as pretty much nothing can be
    // done with the SpeechRecognition instance if a connection isn't
    // estabilished
    throw RecognitionException(code, msg);
  }
  else{
    open_ = true;
  }
}


void SpeechRecognizer::Impl::terminateSendMessageThread(){
  if(sendAudioMessage_thread_.joinable()){
    sendAudioMessage_terminate_ = true;
    try{
      sendAudioMessage_thread_.join();
    } catch (std::system_error e){
      out_ << "Warning: Failure on sendAudioMessage thread close: "
           << e.what()
           << std::endl;
    }
  }
  if(this->audio_src_ != nullptr)
    this->audio_src_->close();
  sendAudioMessage_terminate_ = false;
}


void SpeechRecognizer::Impl::close() {
  terminateSendMessageThread();
  if (secure_) {
    WsClient<Client_tls>::close(this, &client_tls_);
  } else {
    WsClient<Client>::close(this, &client_);
  }
}

void SpeechRecognizer::Impl::recognitionError(RecognitionError::Code code,
                                              std::string message) {
  // invoking callback
  for (std::unique_ptr<RecognitionListener>& listener : listener_) {
    RecognitionError error(code, message);
    listener->onError(error);
  }

  eptr_ = std::make_exception_ptr(RecognitionException(code, message));
  cv_.notify_one();
}

void SpeechRecognizer::Impl::sendMessage(std::string &raw_message) {
  if (secure_) {
    WsClient<Client_tls>::send_msg(this, &client_tls_, raw_message);
  } else {
    WsClient<Client>::send_msg(this, &client_, raw_message);
  }
}

SpeechRecognizer::Impl::Context_ptr SpeechRecognizer::Impl::onTlsInit(
    websocketpp::connection_hdl) {
  Context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(
        asio::ssl::context::sslv23);

  try {
    ctx->set_default_verify_paths();
    ctx->set_options(asio::ssl::context::default_workarounds);
    ctx->set_verify_mode(asio::ssl::context::verify_none);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return ctx;
}
