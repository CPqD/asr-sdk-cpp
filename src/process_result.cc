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

#include "src/process_result.h"

#include <cpqd/asr-client/recognition_exception.h>

#include "json11.hpp"

#include "src/message_utils.h"

bool ASRProcessResult::handle(SpeechRecognizer::Impl &impl,
                ASRMessageResponse &response) {
  std::string method = split(response.get_start_line(), ' ')[2];
  if (method != getMethodString(Method::RecognitionResult)) {
    return ASRProcessMsg::handle(impl, response);
  }

//  impl.lock_.lock();
//  if( impl.sendAudioMessage_thread_.joinable()){
//    impl.lock_.unlock();
//    impl.sendAudioMessage_thread_.join();
//  } else impl.lock_.unlock();

  std::string status = getString(Header::SessionStatus);
  std::string value = response.get_header(status);

  if (value == getString(SessionStatus::IDLE))
    impl.session_status_ = SpeechRecognizer::Impl::SessionStatus::kIdle;

  std::string result_status = getString(ASRProcessResult::Header::ResultStatus);
  value = response.get_header(result_status);

  if (value == RecognitionResult::getString(ResultStatus::CANCELED)) {
    // On CANCEL, do not populate result list
    impl.recognizing_ = false;
    impl.cv_.notify_one();
    return false;
  } else if (value == RecognitionResult::getString(ResultStatus::RECOGNIZED)) {
    std::string err;
    auto json = json11::Json::parse(response.get_extra(), err);

    for (auto &k : json["alternatives"].array_items()) {
      RecognitionResult::Alternative alt;

      int score = k["score"].int_value();
      alt.confidence(score).text(k["text"].string_value());
      for (auto &interp : k["interpretations"].array_items()) {
        alt.addInterpretation(interp.string_value());
      }

      RecognitionResult res;
      res.addAlternatives(alt);

      impl.result_.push_back(res);
    }

    // invoking result callback
    for (RecognitionResult &res : impl.result_) {
      for (std::unique_ptr<RecognitionListener>& listener : impl.listener_) {
        listener->onRecognitionResult(res);
      }
    }

    impl.recognizing_ = false;
    impl.cv_.notify_one();
    return true;
  } else if (value == RecognitionResult::getString(ResultStatus::PROCESSING)) {
    std::string err;
    auto json = json11::Json::parse(response.get_extra(), err);

    auto k = json["alternatives"].array_items()[0];
    PartialRecognition partial;
    partial.text_ = k["text"].string_value();
    partial.speech_segment_index_ = 0; //TODO index not yet implemented

    // invoking partial result callback
    for (std::unique_ptr<RecognitionListener>& listener : impl.listener_) {
      listener->onPartialRecognition(partial);
    }
    return true;
  }
  else {
    // Default case populates result with an empty list and returns status
    RecognitionResult res(value);
    impl.result_.push_back(res);

    // invoking result callback
    for (RecognitionResult &res : impl.result_) {
      for (std::unique_ptr<RecognitionListener> &listener : impl.listener_) {
        listener->onRecognitionResult(res);
      }
    }

    impl.recognizing_ = false;
    impl.cv_.notify_one();
    return false;
  }
}

std::string ASRProcessResult::getString(ASRProcessResult::Header hdr) {
  switch (hdr) {
  case Header::Handle:
    return "Handle";
  case Header::ResultStatus:
    return "Result-Status";
  case Header::SessionStatus:
    return "Session-Status";
  case Header::ContentLength:
    return "Content-Length";
  case Header::ContentType:
    return "Content-Type";
  }
}

std::string ASRProcessResult::getString(ASRProcessResult::SessionStatus st) {
  switch (st) {
  case SessionStatus::IDLE:
    return "IDLE";
  }
}
