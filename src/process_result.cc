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

RecognitionResult recognitionResultFromJson(json11::Json json){
  RecognitionResult res(json["result_status"].string_value());
  for (auto &k : json["alternatives"].array_items()) {
    RecognitionResult::Alternative alt;

    int score = k["score"].int_value();
    alt.confidence(score).text(k["text"].string_value());
    for (auto &interp : k["interpretations"].array_items()) {
      alt.addInterpretation(interp.string_value());
    }
    for (auto &word : k["words"].array_items()) {
      alt.addWord(word["text"].string_value(),
                  word["score"].int_value(),
                  word["start_time"].number_value(),
                  word["end_time"].number_value());
    }
    res.addAlternatives(alt);
  }
  return res;
}

PartialRecognition partialRecognitionFromJson(json11::Json json){
  // Partial result case
  auto k = json["alternatives"].array_items()[0];
  PartialRecognition partial;
  partial.text_ = k["text"].string_value();

  // Default segment index is 0 (pre-3.0)
  partial.speech_segment_index_ = 0;
  if(json["segment_index"].is_number()){
    partial.speech_segment_index_ = json["segment_index"].int_value();
  }
  return partial;
}


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
//  std::cout << response.get_extra() << std::endl;

  if (value == RecognitionResult::getString(ResultStatus::CANCELED)) {
    // On CANCEL, do not populate result list
    impl.recognizing_ = false;
    impl.cv_.notify_one();
    return false;
  }
  else if (response.get_extra().empty()){
    // On empty body, assume no result with only the header status
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
  else {
    std::string err;
    json11::Json json = json11::Json::parse(response.get_extra(), err);

    // Behaviour of variables pre-3.0
    bool final_result = value == RecognitionResult::getString(ResultStatus::RECOGNIZED);
    bool last_segment = true;

    // On 3.0, the aforedefined variables are present in the body of the message
    if(json["final_result"].is_bool())
      final_result = json["final_result"].bool_value();
    if(json["last_segment"].is_bool())
      last_segment = json["last_segment"].bool_value();

    if (final_result) {
      // Final result case
      auto res = recognitionResultFromJson(json);
      if(json["start_time"].is_number())
        res.setStartTime(json["start_time"].number_value());
      if(json["end_time"].is_number())
        res.setEndTime(json["end_time"].number_value());
      impl.result_.push_back(res);
      res.setLastSpeechSegment(last_segment);

      // invoking result callback
      for (std::unique_ptr<RecognitionListener>& listener : impl.listener_) {
        listener->onRecognitionResult(res);
      }

      // Default behaviour in the absence of the "last_segment" field in json is
      // assuming last_segment=true (pre-3.0)
      if(last_segment){
        impl.recognizing_ = false;
        impl.cv_.notify_one();
      }

      return true;
    } else {
      auto partial = partialRecognitionFromJson(json);

      // invoking partial result callback
      for (std::unique_ptr<RecognitionListener>& listener : impl.listener_) {
        listener->onPartialRecognition(partial);
      }
      return true;
    }
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
