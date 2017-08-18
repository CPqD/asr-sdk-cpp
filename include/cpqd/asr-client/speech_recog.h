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

#ifndef INCLUDE_CPQD_ASR_CLIENT_SPEECH_RECOG_H_
#define INCLUDE_CPQD_ASR_CLIENT_SPEECH_RECOG_H_

#include <cpqd/asr-client/audio_source.h>
#include <cpqd/asr-client/language_model_list.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_listener.h>
#include <cpqd/asr-client/recognition_result.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

enum class AudioEncoding {
  LINEAR16,  // PCM linear de 16 bits por amostra
  ALAW,
  ULAW,
};

/** @brief SpeechRecognizer class represents an interface between ASR client and
 * server
 *
 * O áudio a ser reconhecido é representado pela interface AudioSource e o
 * modelo da língua usado,
 * pela classe LanguageModel. Os parâmetros que configuram o reconhecimento são
 * definidos
 * através da classe RecognitionConfig.
 * Funções de callback de reconhecimento podem ser definidas através da classe
 * ASRListener.
 */
class SpeechRecognizer {
 public:
  class Builder;
  class Impl;

 private:
  class Properties {
   private:
    Properties() = default;

    Properties(const Properties& prop) = delete;

    std::string url_;
    std::string user_;
    std::string passwd_;
    std::string user_agent_;
    unsigned int audio_sample_rate_;
    AudioEncoding audio_encoding_;
    std::unique_ptr<RecognitionConfig> recog_config_ = nullptr;
    std::vector<std::unique_ptr<RecognitionListener>> listener_;

    // Parameters with default values
    unsigned int max_wait_seconds_ = 30;
    bool connect_on_recognize_ = false;
    bool auto_close_ = false;
    std::string log_path_ = "log.txt";

    friend class SpeechRecognizer;
    friend class SpeechRecognizer::Builder;
  };

 public:
  SpeechRecognizer() = delete;

  ~SpeechRecognizer();

  void resetImpl();

  void close();

  void cancelRecognition();

  void recognize(const std::shared_ptr<AudioSource>& audio_src,
                      std::unique_ptr<LanguageModelList> lm);

  std::vector<RecognitionResult> waitRecognitionResult();

  bool isOpen(); // For testing purposes

 private:
  explicit SpeechRecognizer(std::unique_ptr<Properties> properties);

  std::shared_ptr<Impl> impl_ = nullptr;

  std::unique_ptr<Properties> properties_ = nullptr;

  std::chrono::time_point<std::chrono::system_clock> start_;
};

/** @brief This class creates new SpeechRecognizer objects
 *
 * This class creates new SpeechRecognizer objects applying the Builder Pattern
*/
class SpeechRecognizer::Builder {
 public:
  Builder();

  std::unique_ptr<SpeechRecognizer> build();

  SpeechRecognizer::Builder& serverUrl(const std::string& url);
  SpeechRecognizer::Builder& credentials(const std::string& user,
                                         const std::string& passwd);
  SpeechRecognizer::Builder& recogConfig(
      std::unique_ptr<RecognitionConfig> config);
  SpeechRecognizer::Builder& userAgent(const std::string& value);
  SpeechRecognizer::Builder& addListener(
      std::unique_ptr<RecognitionListener> listener);
  SpeechRecognizer::Builder& audioSampleRate(unsigned int value);
  SpeechRecognizer::Builder& audioEncoding(AudioEncoding value);
  SpeechRecognizer::Builder& maxWaitSeconds(unsigned int value);
  SpeechRecognizer::Builder& connectOnRecognize(bool value);
  SpeechRecognizer::Builder& autoClose(bool value);
  SpeechRecognizer::Builder& logPath(std::string value);

 private:
  std::unique_ptr<SpeechRecognizer::Properties> properties_ = nullptr;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_SPEECH_RECOG_H_
