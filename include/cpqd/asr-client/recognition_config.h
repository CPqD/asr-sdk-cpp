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

#ifndef INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_CONFIG_H_
#define INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_CONFIG_H_

#include <memory>

constexpr const char* CONFIDENCE_THRESHOLD = "CONFIDENCE_THRESHOLD";
constexpr const char* MAX_SENTENCES = "MAX_SENTENCES";
constexpr const char* NO_INPUT_TIMEOUT_MILLISECONDS = "NO_INPUT_TIMEOUT_MILLISECONDS";
constexpr const char* RECOG_TIMEOUT_SECONDS = "RECOG_TIMEOUT_SECONDS";
constexpr const char* NO_INPUT_TIMEOUT_ENABLED = "NO_INPUT_TIMEOUT_ENABLED";
constexpr const char* RECOG_TIMEOUT_ENABLED = "RECOG_TIMEOUT_ENABLED";
constexpr const char* INFER_AGE_ENABLED = "INFER_AGE_ENABLED";
constexpr const char* INFER_EMOTION_ENABLED = "INFER_GENDER_ENABLED";
constexpr const char* INFER_GENDER_ENABLED = "INFER_EMOTION_ENABLED";
constexpr const char* HEAD_MARGIN_MILLISECONDS = "HEAD_MARGIN_MILLISECONDS";
constexpr const char* TAIL_MARGIN_MILLISECONDS = "TAIL_MARGIN_MILLISECONDS";
constexpr const char* WAIT_END_MILLISECONDS = "WAIT_END_MILLISECONDS";
constexpr const char* CONTINUOUS_MODE = "CONTINUOUS_MODE";
constexpr const char* MAX_SEGMENT_DURATION = "MAX_SEGMENT_DURATION";
constexpr const char* START_INPUT_TIMERS = "START_INPUT_TIMERS";
constexpr const char* ENDPOINTER_AUTO_LEVEL_LEN = "ENDPOINTER_AUTO_LEVEL_LEN";
constexpr const char* ENDPOINTER_LEVEL_MODE = "ENDPOINTER_LEVEL_MODE";
constexpr const char* ENDPOINTER_LEVEL_THRESHOLD = "ENDPOINTER_LEVEL_THRESHOLD";
constexpr const char* VERIFY_BUFFER_UTTERANCE = "VERIFY_BUFFER_UTTERANCE";
constexpr const char* ACCOUNT_TAG = "ACCOUNT_TAG";
constexpr const char* CHANNEL_IDENTIFIER = "CHANNEL_IDENTIFIER";
constexpr const char* MEDIA_TYPE = "MEDIA_TYPE";

class RecognitionConfig {
 public:
  class Builder;

 private:

  class Properties {
   private:
    Properties() = default;

    unsigned int confidence_threshold_ = 0;
    unsigned int max_sentences_ = 1;
    unsigned int no_input_timeout_milliseconds_ = 0;
    unsigned int recog_timeout_seconds_ = 0;
    bool no_input_timeout_enabled_ = false;
    bool recog_timeout_enabled_ = false;
    bool infer_age_enabled_ = false;
    bool infer_gender_enabled_ = false;
    bool infer_emotion_enabled_ = false;
    unsigned int head_margin_milliseconds_ = 0;
    unsigned int tail_margin_milliseconds_ = 0;
    unsigned int wait_end_milliseconds_ = 0;
    bool continuous_mode_ = false;
    unsigned int max_segment_duration_ = 0;
    bool start_input_timers_ = false;
    unsigned int endpointer_auto_level_len_ = 0;
    unsigned int endpointer_level_mode_ = 0;
    unsigned int endpointer_level_threshold_ = 0;
    bool verify_buffer_utterance_ = false;
    std::string account_tag_ = "";
    std::string channel_identifier_ = "";
    std::string media_type_ = "";

    friend class RecognitionConfig;
    friend class RecognitionConfig::Builder;
  };

  void setConfidenceThresholdFromEnv(bool replace_empty = false);
  void setMaxSentencesFromEnv(bool replace_empty = false);
  void setNoInputTimeoutMillisecondsFromEnv(bool replace_empty = false);
  void setRecognitionTimeoutSecondsFromEnv(bool replace_empty = false);
  void setNoInputTimeoutEnabledFromEnv(bool replace_empty = false);
  void setRecognitionTimeoutEnabledFromEnv(bool replace_empty = false);
  void setInferAgeEnabledFromEnv(bool replace_empty = false);
  void setInferEmotionEnabledFromEnv(bool replace_empty = false);
  void setInferGenderEnabledFromEnv(bool replace_empty = false);
  void setHeadMarginMillisecondsFromEnv(bool replace_empty = false);
  void setTailMarginMillisecondsFromEnv(bool replace_empty = false);
  void setWaitEndMillisecondsFromEnv(bool replace_empty = false);
  void setContinuousModeFromEnv(bool replace_empty = false);
  void setMaxSegmentDurationFromEnv(bool replace_empty = false);
  void setStartInputTimersFromEnv(bool replace_empty = false);
  void setEndpointerAutoLevelLenFromEnv(bool replace_empty = false);
  void setEndpointerLevelModeFromEnv(bool replace_empty = false);
  void setEndpointerLevelThresholdFromEnv(bool replace_empty = false);
  void setVerifyBufferUtteranceFromEnv(bool replace_empty = false);
  void setAccountTagFromEnv(bool replace_empty = false);
  void setChannelIdentifierFromEnv(bool replace_empty = false);
  void setMediaTypeFromEnv(bool replace_empty = false);

 public:
  RecognitionConfig() = default;

  RecognitionConfig(const RecognitionConfig& config);

  RecognitionConfig& operator=(RecognitionConfig const& config);

  void getConfigFromEvironment(bool replace_empty = false);

  unsigned int confidenceThreshold();
  unsigned int maxSentences();
  unsigned int noInputTimeoutMilliseconds();
  unsigned int recognitionTimeoutSeconds();
  bool noInputTimeoutEnabled();
  bool recognitionTimeoutEnabled();
  bool inferAgeEnabled();
  bool inferEmotionEnabled();
  bool inferGenderEnabled();
  unsigned int headMarginMilliseconds();
  unsigned int tailMarginMilliseconds();
  unsigned int waitEndMilliseconds();
  bool continuousMode();
  unsigned int maxSegmentDuration();
  bool startInputTimers();
  unsigned int endpointerAutoLevelLen();
  unsigned int endpointerLevelMode();
  unsigned int endpointerLevelThreshold();
  bool verifyBufferUtterance();
  std::string accountTag();
  std::string channelIdentifier();
  std::string mediaType();

 private:
  explicit RecognitionConfig(const Properties& properties)
      : properties_(properties) {}

  Properties properties_;
};

class RecognitionConfig::Builder {
 public:
  Builder() = default;

  // Retorna a lista de modelos da língua construída.
  std::unique_ptr<RecognitionConfig> build();

  RecognitionConfig::Builder& confidenceThreshold(unsigned int value);
  RecognitionConfig::Builder& maxSentences(unsigned int value);
  RecognitionConfig::Builder& noInputTimeoutMilliseconds(unsigned int value);
  RecognitionConfig::Builder& recognitionTimeoutSeconds(unsigned int value);
  RecognitionConfig::Builder& noInputTimeoutEnabled(bool value);
  RecognitionConfig::Builder& recognitionTimeoutEnabled(bool value);
  RecognitionConfig::Builder& inferAgeEnabled(bool value);
  RecognitionConfig::Builder& inferEmotionEnabled(bool value);
  RecognitionConfig::Builder& inferGenderEnabled(bool value);
  RecognitionConfig::Builder& headMarginMilliseconds(unsigned int value);
  RecognitionConfig::Builder& tailMarginMilliseconds(unsigned int value);
  RecognitionConfig::Builder& waitEndMilliseconds(unsigned int value);
  RecognitionConfig::Builder& continuousMode(bool value);
  RecognitionConfig::Builder& maxSegmentDuration(unsigned int value);
  RecognitionConfig::Builder& startInputTimers(bool value);
  RecognitionConfig::Builder& endpointerAutoLevelLen(unsigned int value);
  RecognitionConfig::Builder& endpointerLevelMode(unsigned int value);
  RecognitionConfig::Builder& endpointerLevelThreshold(unsigned int value);
  RecognitionConfig::Builder& verifyBufferUtterance(bool value);
  RecognitionConfig::Builder& channelIdentifier(const std::string& value);
  RecognitionConfig::Builder& mediaType(const std::string& value);
  RecognitionConfig::Builder& accountTag(const std::string& value);

 private:
  RecognitionConfig::Properties properties_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_CONFIG_H_
