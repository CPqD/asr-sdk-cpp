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

#include <cpqd/asr-client/recognition_config.h>

std::unique_ptr<RecognitionConfig> RecognitionConfig::Builder::build() {
  return std::unique_ptr<RecognitionConfig>(new RecognitionConfig(properties_));
}

RecognitionConfig::Builder &RecognitionConfig::Builder::confidenceThreshold(
    unsigned int value) {
  properties_.confidence_threshold_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::maxSentences(
    unsigned int value) {
  properties_.max_sentences_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::noInputTimeoutMilliseconds(unsigned int value) {
  properties_.no_input_timeout_milliseconds_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::recognitionTimeoutSeconds(unsigned int value) {
  properties_.recog_timeout_seconds_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::noInputTimeoutEnabled(
    bool value) {
  properties_.no_input_timeout_enabled_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::recognitionTimeoutEnabled(bool value) {
  properties_.recog_timeout_enabled_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::headMarginMilliseconds(
    unsigned int value) {
  properties_.head_margin_milliseconds_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::tailMarginMilliseconds(
    unsigned int value) {
  properties_.tail_margin_milliseconds_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::waitEndMilliseconds(
    unsigned int value) {
  properties_.wait_end_milliseconds_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::startInputTimers(
    bool value) {
  properties_.start_input_timers_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::endpointerAutoLevelLen(
    unsigned int value) {
  properties_.endpointer_auto_level_len_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::endpointerLevelMode(
    unsigned int value) {
  properties_.endpointer_level_mode_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::endpointerLevelThreshold(unsigned int value) {
  properties_.endpointer_level_threshold_ = value;
  return *this;
}

RecognitionConfig::RecognitionConfig(const RecognitionConfig &config) {
  properties_ = config.properties_;
}

RecognitionConfig &RecognitionConfig::operator=(
    const RecognitionConfig &config) {
  properties_ = config.properties_;
  return *this;
}

unsigned int RecognitionConfig::confidenceThreshold() {
  return properties_.confidence_threshold_;
}

unsigned int RecognitionConfig::maxSentences() {
  return properties_.max_sentences_;
}

unsigned int RecognitionConfig::noInputTimeoutMilliseconds() {
  return properties_.no_input_timeout_milliseconds_;
}

unsigned int RecognitionConfig::recognitionTimeoutSeconds() {
  return properties_.recog_timeout_seconds_;
}

bool RecognitionConfig::noInputTimeoutEnabled() {
  return properties_.no_input_timeout_enabled_;
}

bool RecognitionConfig::recognitionTimeoutEnabled() {
  return properties_.recog_timeout_enabled_;
}

unsigned int RecognitionConfig::headMarginMilliseconds() {
  return properties_.head_margin_milliseconds_;
}

unsigned int RecognitionConfig::tailMarginMilliseconds() {
  return properties_.tail_margin_milliseconds_;
}

unsigned int RecognitionConfig::waitEndMilliseconds() {
  return properties_.wait_end_milliseconds_;
}

bool RecognitionConfig::startInputTimers() {
  return properties_.start_input_timers_;
}

unsigned int RecognitionConfig::endpointerAutoLevelLen() {
  return properties_.endpointer_auto_level_len_;
}

unsigned int RecognitionConfig::endpointerLevelMode() {
  return properties_.endpointer_level_mode_;
}

unsigned int RecognitionConfig::endpointerLevelThreshold() {
  return properties_.endpointer_level_threshold_;
}
