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
    unsigned int head_margin_milliseconds_ = 0;
    unsigned int tail_margin_milliseconds_ = 0;
    unsigned int wait_end_milliseconds_ = 0;
    bool start_input_timers_ = false;
    unsigned int endpointer_auto_level_len_ = 0;
    unsigned int endpointer_level_mode_ = 0;
    unsigned int endpointer_level_threshold_ = 0;

    friend class RecognitionConfig;
    friend class RecognitionConfig::Builder;
  };

 public:
  RecognitionConfig() = default;

  RecognitionConfig(const RecognitionConfig& config);

  RecognitionConfig& operator=(RecognitionConfig const& config);

  unsigned int confidenceThreshold();
  unsigned int maxSentences();
  unsigned int noInputTimeoutMilliseconds();
  unsigned int recognitionTimeoutSeconds();
  bool noInputTimeoutEnabled();
  bool recognitionTimeoutEnabled();
  unsigned int headMarginMilliseconds();
  unsigned int tailMarginMilliseconds();
  unsigned int waitEndMilliseconds();
  bool startInputTimers();
  unsigned int endpointerAutoLevelLen();
  unsigned int endpointerLevelMode();
  unsigned int endpointerLevelThreshold();

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
  RecognitionConfig::Builder& headMarginMilliseconds(unsigned int value);
  RecognitionConfig::Builder& tailMarginMilliseconds(unsigned int value);
  RecognitionConfig::Builder& waitEndMilliseconds(unsigned int value);
  RecognitionConfig::Builder& startInputTimers(bool value);
  RecognitionConfig::Builder& endpointerAutoLevelLen(unsigned int value);
  RecognitionConfig::Builder& endpointerLevelMode(unsigned int value);
  RecognitionConfig::Builder& endpointerLevelThreshold(unsigned int value);

 private:
  RecognitionConfig::Properties properties_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_CONFIG_H_
