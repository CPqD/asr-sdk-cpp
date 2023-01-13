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

#include <algorithm>

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

RecognitionConfig::Builder &
RecognitionConfig::Builder::inferAgeEnabled(bool value) {
  properties_.infer_age_enabled_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::inferEmotionEnabled(bool value) {
  properties_.infer_emotion_enabled_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::inferGenderEnabled(bool value) {
  properties_.infer_gender_enabled_ = value;
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

RecognitionConfig::Builder &
RecognitionConfig::Builder::continuousMode(bool value) {
  properties_.continuous_mode_ = value;
  return *this;
}

RecognitionConfig::Builder &RecognitionConfig::Builder::maxSegmentDuration(
    unsigned int value) {
  properties_.max_segment_duration_ = value;
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

RecognitionConfig::Builder &
RecognitionConfig::Builder::verifyBufferUtterance(bool value) {
  properties_.verify_buffer_utterance_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::accountTag(const std::string& value) {
  properties_.account_tag_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::channelIdentifier(const std::string& value) {
  properties_.channel_identifier_ = value;
  return *this;
}

RecognitionConfig::Builder &
RecognitionConfig::Builder::mediaType(const std::string& value) {
  properties_.media_type_ = value;
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

bool RecognitionConfig::inferAgeEnabled() {
  return properties_.infer_age_enabled_;
}

bool RecognitionConfig::inferEmotionEnabled() {
  return properties_.infer_emotion_enabled_;
}

bool RecognitionConfig::inferGenderEnabled() {
  return properties_.infer_gender_enabled_;
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

bool RecognitionConfig::continuousMode() {
  return properties_.continuous_mode_;
}

unsigned int RecognitionConfig::maxSegmentDuration() {
  return properties_.max_segment_duration_;
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

bool RecognitionConfig::verifyBufferUtterance() {
  return properties_.verify_buffer_utterance_;
}

std::string  RecognitionConfig::accountTag() {
  return properties_.account_tag_;
}

std::string  RecognitionConfig::channelIdentifier() {
  return properties_.channel_identifier_;
}

std::string  RecognitionConfig::mediaType() {
  return properties_.media_type_;
}

void setInteger(const char* par_name, bool replace_empty, unsigned int& par) {
  const char* env_p;
  if ((env_p = std::getenv(par_name))) {
    try {
      par = std::stoul(env_p);
    } catch (...) {
      par = 0;
    }
  } else if (replace_empty) {
    par = 0;
  }
}

void setBool(const char* par_name, bool replace_empty, bool& par) {
  const char* env_p;
  if ((env_p = std::getenv(par_name))) {
    std::string p(env_p);
    std::transform(p.begin(), p.end(), p.begin(), ::toupper);
    par = (p == "TRUE");
  } else if (replace_empty) {
    par = false;
  }
}

void setString(const char* par_name, bool replace_empty, std::string& par) {
  const char* env_p;
  if ((env_p = std::getenv(par_name))) {
    par = std::string(env_p);
  } else if (replace_empty) {
    par = "";
  }
}

void RecognitionConfig::setConfidenceThresholdFromEnv(bool replace_empty) {
  setInteger(CONFIDENCE_THRESHOLD, replace_empty, properties_.confidence_threshold_);
}

void RecognitionConfig::setMaxSentencesFromEnv(bool replace_empty) {
  setInteger(MAX_SENTENCES, replace_empty, properties_.max_sentences_);
}

void RecognitionConfig::setNoInputTimeoutMillisecondsFromEnv(bool replace_empty) {
  setInteger(NO_INPUT_TIMEOUT_MILLISECONDS, replace_empty, properties_.no_input_timeout_milliseconds_);
}

void RecognitionConfig::setRecognitionTimeoutSecondsFromEnv(bool replace_empty) {
  setInteger(RECOG_TIMEOUT_SECONDS, replace_empty, properties_.recog_timeout_seconds_);
}

void RecognitionConfig::setNoInputTimeoutEnabledFromEnv(bool replace_empty) {
  setBool(NO_INPUT_TIMEOUT_ENABLED, replace_empty, properties_.no_input_timeout_enabled_);
}

void RecognitionConfig::setRecognitionTimeoutEnabledFromEnv(bool replace_empty) {
  setBool(RECOG_TIMEOUT_ENABLED, replace_empty, properties_.recog_timeout_enabled_);
}

void RecognitionConfig::setInferAgeEnabledFromEnv(bool replace_empty) {
  setBool(INFER_AGE_ENABLED, replace_empty, properties_.infer_age_enabled_);
}

void RecognitionConfig::setInferEmotionEnabledFromEnv(bool replace_empty) {
  setBool(INFER_EMOTION_ENABLED, replace_empty, properties_.infer_emotion_enabled_);
}
void RecognitionConfig::setInferGenderEnabledFromEnv(bool replace_empty) {
  setBool(INFER_GENDER_ENABLED, replace_empty, properties_.infer_gender_enabled_);
}

void RecognitionConfig::setHeadMarginMillisecondsFromEnv(bool replace_empty) {
  setInteger(HEAD_MARGIN_MILLISECONDS, replace_empty, properties_.head_margin_milliseconds_);
}

void RecognitionConfig::setTailMarginMillisecondsFromEnv(bool replace_empty) {
  setInteger(TAIL_MARGIN_MILLISECONDS, replace_empty, properties_.tail_margin_milliseconds_);
}

void RecognitionConfig::setWaitEndMillisecondsFromEnv(bool replace_empty) {
  setInteger(WAIT_END_MILLISECONDS, replace_empty, properties_.wait_end_milliseconds_);
}

void RecognitionConfig::setContinuousModeFromEnv(bool replace_empty) {
  setBool(INFER_AGE_ENABLED, replace_empty, properties_.continuous_mode_);
}

void RecognitionConfig::setMaxSegmentDurationFromEnv(bool replace_empty) {
  setInteger(MAX_SEGMENT_DURATION, replace_empty, properties_.max_segment_duration_);
}

void RecognitionConfig::setStartInputTimersFromEnv(bool replace_empty) {
  setBool(START_INPUT_TIMERS, replace_empty, properties_.start_input_timers_);
}

void RecognitionConfig::setEndpointerAutoLevelLenFromEnv(bool replace_empty) {
  setInteger(ENDPOINTER_AUTO_LEVEL_LEN, replace_empty, properties_.endpointer_auto_level_len_);
}

void RecognitionConfig::setEndpointerLevelModeFromEnv(bool replace_empty) {
  setInteger(ENDPOINTER_LEVEL_MODE, replace_empty, properties_.endpointer_level_mode_);
}

void RecognitionConfig::setEndpointerLevelThresholdFromEnv(bool replace_empty) {
  setInteger(ENDPOINTER_LEVEL_THRESHOLD, replace_empty, properties_.endpointer_level_threshold_);
}

void RecognitionConfig::setVerifyBufferUtteranceFromEnv(bool replace_empty) {
  setBool(VERIFY_BUFFER_UTTERANCE, replace_empty, properties_.verify_buffer_utterance_);
}

void RecognitionConfig::setAccountTagFromEnv(bool replace_empty) {
  setString(ACCOUNT_TAG, replace_empty, properties_.account_tag_);
}

void RecognitionConfig::setChannelIdentifierFromEnv(bool replace_empty) {
  setString(CHANNEL_IDENTIFIER, replace_empty, properties_.channel_identifier_);
}

void RecognitionConfig::setMediaTypeFromEnv(bool replace_empty) {
  setString(MEDIA_TYPE, replace_empty, properties_.media_type_);
}

void RecognitionConfig::getConfigFromEvironment(bool replace_empty) {
  setConfidenceThresholdFromEnv(replace_empty);
  setMaxSentencesFromEnv(replace_empty);
  setNoInputTimeoutMillisecondsFromEnv(replace_empty);
  setRecognitionTimeoutSecondsFromEnv(replace_empty);
  setNoInputTimeoutEnabledFromEnv(replace_empty);
  setRecognitionTimeoutEnabledFromEnv(replace_empty);
  setInferAgeEnabledFromEnv(replace_empty);
  setInferEmotionEnabledFromEnv(replace_empty);
  setInferGenderEnabledFromEnv(replace_empty);
  setHeadMarginMillisecondsFromEnv(replace_empty);
  setTailMarginMillisecondsFromEnv(replace_empty);
  setWaitEndMillisecondsFromEnv(replace_empty);
  setContinuousModeFromEnv(replace_empty);
  setMaxSegmentDurationFromEnv(replace_empty);
  setStartInputTimersFromEnv(replace_empty);
  setEndpointerAutoLevelLenFromEnv(replace_empty);
  setEndpointerLevelModeFromEnv(replace_empty);
  setEndpointerLevelThresholdFromEnv(replace_empty);
  setVerifyBufferUtteranceFromEnv(replace_empty);
  setAccountTagFromEnv(replace_empty);
  setChannelIdentifierFromEnv(replace_empty);
  setMediaTypeFromEnv(replace_empty);
}
