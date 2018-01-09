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

#include <cpqd/asr-client/speech_recog.h>

#include <mutex>
#include <vector>

#include <cpqd/asr-client/recognition_exception.h>
#include "src/asr_message_request.h"
#include "src/speech_recog_impl.h"
#include "src/send_message.h"

SpeechRecognizer::SpeechRecognizer(std::unique_ptr<Properties> properties)
    : properties_(std::move(properties)) {
  resetImpl();
}

void SpeechRecognizer::resetImpl() {
  impl_ = std::make_shared<Impl>();

  if (properties_->recog_config_) {
    impl_->config_ = std::move(properties_->recog_config_);
    properties_->recog_config_ = nullptr;
  } else
    impl_->config_ = nullptr;

  if (!properties_->listener_.empty())
    impl_->listener_ = std::move(properties_->listener_);

  impl_->out_.open((properties_->log_path_).c_str(), std::fstream::app);
  impl_->logger_.set_ostream(&impl_->out_);
  impl_->logger_.set_channels(websocketpp::log::alevel::all);

  // Only connect if connect_on_recognize_ is false, i.e. if we are in the
  // default behaviour
  if(!properties_->connect_on_recognize_) {
    impl_->open(properties_->url_, properties_->user_, properties_->passwd_);
    // connect_on_recognize_ = false and auto_close_ = true has
    // undefined behaviour, so we force both true after 1st connection
    if(properties_->auto_close_){
      properties_->connect_on_recognize_ = true;
    }
  }
}

SpeechRecognizer::~SpeechRecognizer() {
  impl_->out_.close();
  close();
}

void SpeechRecognizer::close() {
  if (!impl_->open_) return;
  impl_->close();
}

void SpeechRecognizer::cancelRecognition() {
  // TODO(brunog): check if session state is listening

  ASRMessageRequest request(Method::CancelRecognition);
  std::string raw_message = request.raw();

  impl_->logger_.write(websocketpp::log::elevel::info,
                "[SEND] " + raw_message);

  impl_->sendMessage(raw_message);
  
  if(impl_->sendAudioMessage_thread_.joinable()){
    impl_->sendAudioMessage_terminate_ = true;
    impl_->sendAudioMessage_thread_.join();
  }

  impl_->sendAudioMessage_terminate_ = false;
  
  // Close after canceled recognition
  if(properties_->auto_close_ && impl_->open_){
    impl_->close();
  }
}

void SpeechRecognizer::recognize(
    const std::shared_ptr<AudioSource> &audio_src,
    std::unique_ptr<LanguageModelList> lm) {
  if(impl_->recognizing_){
    throw RecognitionException(RecognitionError::Code::ACTIVE_RECOGNITION,
      "There is a recognition already running in this recognizier!"
    );
  }
  start_ = std::chrono::system_clock::now();
  impl_->recognizing_ = true;
  impl_->eptr_ = nullptr;
  impl_->result_.clear();

  impl_->audio_src_ = audio_src;
  impl_->lm_ = std::move(lm);

  // Only try to connect if connection is closed. This will be true
  // if connect_on_recognize_ is true or if auto_close is true and we already
  // performed one recognition
  if(!impl_->open_){
    impl_->open(properties_->url_, properties_->user_, properties_->passwd_);
  }

  ASRSendMessage send_msg_;
  if (impl_->session_status_ == SpeechRecognizer::Impl::SessionStatus::kNone) {
    send_msg_.createSession(*impl_);
    return;
  }
  send_msg_.startRecognition(*impl_);
}

std::vector<RecognitionResult> SpeechRecognizer::waitRecognitionResult() {
  if (!impl_->open_){
    auto ret = impl_->result_;
    impl_->result_.clear();
    if (impl_->eptr_)
      std::rethrow_exception(impl_->eptr_);
    // Close after successful recognition
    if(properties_->auto_close_ && impl_->open_){
      resetImpl();
    }
    return ret; 
  }
  std::unique_lock<std::mutex> lk(impl_->lock_);
  auto end = std::chrono::system_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::seconds>(end - start_);
  auto time_waiting = std::chrono::seconds(
    properties_->max_wait_seconds_
  ) - duration;

  if (impl_->cv_.wait_for(lk, time_waiting, [this]() {
        return impl_->result_.size() > 0
            || impl_->eptr_
            || !impl_->recognizing_;
      })) {
    impl_->terminateSendMessageThread();
    auto ret = impl_->result_;
    impl_->result_.clear();
    if (impl_->eptr_){
      std::rethrow_exception(impl_->eptr_);
    }
    // Close after successful recognition
    if(properties_->auto_close_ && impl_->open_){
      resetImpl();
    }
    return ret; 
  } else {
    throw RecognitionException(
      RecognitionError::Code::FAILURE,
      "Timeout on speech recog"                          
    );
  }
  return {};
}

bool SpeechRecognizer::isOpen() {
  return impl_->open_;
}
