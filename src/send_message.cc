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

#include "src/send_message.h"

#include <string>

#include <cpqd/asr-client/recognition_exception.h>

#include "src/asr_message_request.h"
#include "src/message_utils.h"

ASRSendMessage::ASRSendMessage() {}

void ASRSendMessage::createSession(SpeechRecognizer::Impl &impl) {
  ASRMessageRequest request(Method::CreateSession);
  request.set_header("User-Agent", "ASR Client");
  std::string raw_message = request.raw();

  impl.logger_.write(websocketpp::log::elevel::info,
                            "[SEND] " + raw_message);

  impl.sendMessage(raw_message);
}

void ASRSendMessage::setParameters(SpeechRecognizer::Impl &impl) {
  ASRMessageRequest request(Method::SetParameters);

  unsigned int confidence_threshold = impl.config_->confidenceThreshold();
  unsigned int max_sentences = impl.config_->maxSentences();
  unsigned int no_input_timeout_milliseconds =
      impl.config_->noInputTimeoutMilliseconds();
  unsigned int recog_timeout_seconds = impl.config_->recognitionTimeoutSeconds();
  bool no_input_timeout_enabled = impl.config_->noInputTimeoutEnabled();
  bool recog_timeout_enabled = impl.config_->recognitionTimeoutEnabled();
  bool infer_age_enabled = impl.config_->inferAgeEnabled();
  bool infer_emotion_enabled = impl.config_->inferEmotionEnabled();
  bool infer_gender_enabled = impl.config_->inferGenderEnabled();
  unsigned int head_margin_milliseconds = impl.config_->headMarginMilliseconds();
  unsigned int tail_margin_milliseconds = impl.config_->tailMarginMilliseconds();
  unsigned int wait_end_milliseconds = impl.config_->waitEndMilliseconds();
  bool start_input_timers = impl.config_->startInputTimers();
  unsigned int endpointer_auto_level_len = impl.config_->endpointerAutoLevelLen();
  unsigned int endpointer_level_mode = impl.config_->endpointerLevelMode();
  unsigned int endpointer_level_threshold = impl.config_->endpointerLevelThreshold();

  if (confidence_threshold)
    request.set_header("decoder.confidenceThreshold",
                       std::to_string(confidence_threshold));
  if (max_sentences)
    request.set_header("decoder.maxSentences", std::to_string(max_sentences));

  if (no_input_timeout_enabled) {
    request.set_header("noInputTimeout.enabled",
                       std::to_string(no_input_timeout_enabled));
    if (no_input_timeout_milliseconds)
      request.set_header("noInputTimeout.value",
                         std::to_string(no_input_timeout_milliseconds));
  }

  if (recog_timeout_enabled) {
    request.set_header("recognitionTimeout.enabled",
                       std::to_string(recog_timeout_enabled));
    if (recog_timeout_seconds)
      request.set_header("recognitionTimeout.value",
                         std::to_string(recog_timeout_seconds));
  }

  if (infer_age_enabled) {
    request.set_header("Infer-age-enabled", "true");
  }

  if (infer_emotion_enabled) {
    request.set_header("Infer-emotion-enabled", "true");
  }

  if (infer_gender_enabled) {
    request.set_header("Infer-gender-enabled", "true");
  }

  if (head_margin_milliseconds)
    request.set_header("endpointer.headMargin",
                       std::to_string(head_margin_milliseconds));

  if (tail_margin_milliseconds)
    request.set_header("endpointer.tailMargin",
                       std::to_string(tail_margin_milliseconds));

  if (wait_end_milliseconds)
    request.set_header("endpointer.waitEnd",
                       std::to_string(wait_end_milliseconds));

  if (start_input_timers) {
    request.set_header("decoder.startInputTimers",
                       std::to_string(start_input_timers));
  }

  if (endpointer_level_mode) {
    request.set_header("endpointer.levelMode",
                       std::to_string(endpointer_level_mode));
    if (endpointer_level_mode == 1 && endpointer_auto_level_len)
      request.set_header("endpointer.autoLevelLen",
                         std::to_string(endpointer_auto_level_len));
    if (endpointer_level_mode == 2 && endpointer_level_threshold)
      request.set_header("endpointer.levelThreshold",
                         std::to_string(endpointer_level_threshold));
  }

  std::string raw_message = request.raw();

  impl.logger_.write(websocketpp::log::elevel::info, "[SEND] " + raw_message);

  impl.sendMessage(raw_message);
}

void ASRSendMessage::startRecognition(SpeechRecognizer::Impl &impl) {
  std::string lm_uri, grammar_body, extra;
  std::string content_type;

  lm_uri = impl.lm_->getUri();
  grammar_body = impl.lm_->getGrammarBody();

  ASRMessageRequest request(Method::StartRecognition);
  request.set_header("Accept", "application/json");

  // if lm property uri and grammar body was not set, throw an error
  if (!lm_uri.empty()) {
    extra = lm_uri;
    content_type = "text/uri-list";
  } else if (!grammar_body.empty()) {
    extra = grammar_body;
    content_type = "application/srgs";

    // only one grammar is allowed, so the id is fixed by now
    request.set_header("Content-ID", "gram");
  } else {
    throw RecognitionException(RecognitionError::Code::FAILURE,
        std::string("lm uri and grammar body is emprty"));
  }

  request.set_header("Content-Type", content_type);
  request.set_extra(extra);
  request.set_header("Content-Length", std::to_string(extra.size()));

  std::string raw_message = request.raw();

  impl.logger_.write(websocketpp::log::elevel::info, "[SEND] " + raw_message);

  impl.sendMessage(raw_message);
}
