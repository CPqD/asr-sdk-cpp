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

// Backported from C++14
namespace internal {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace internal

SpeechRecognizer::Builder::Builder() {
  std::unique_ptr<SpeechRecognizer::Properties> tmp(
      new SpeechRecognizer::Properties);
  properties_ = std::move(tmp);
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::serverUrl(
    const std::string &url) {
  if (url.empty())
    throw std::logic_error("Empty url");

  bool found_scheme = (url.find("ws://") == 0) || (url.find("wss://") == 0);
  if (!found_scheme)
    throw std::invalid_argument("invalid url");

  properties_->url_ = url;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::credentials(
    const std::string &user, const std::string &passwd) {
  properties_->user_ = user;
  properties_->passwd_ = passwd;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::recogConfig(
    std::unique_ptr<RecognitionConfig> config) {
  properties_->recog_config_ = std::move(config);
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::userAgent(
    const std::string &value) {
  properties_->user_agent_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::addListener(
    std::unique_ptr<RecognitionListener> listener) {
  properties_->listener_.push_back(std::move(listener));
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::audioSampleRate(
    unsigned int value) {
  properties_->audio_sample_rate_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::audioEncoding(
    AudioEncoding value) {
  properties_->audio_encoding_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::maxWaitSeconds(
    unsigned int value) {
  properties_->max_wait_seconds_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::connectOnRecognize(
    bool value) {
  properties_->connect_on_recognize_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::autoClose(
    bool value) {
  properties_->auto_close_ = value;
  return *this;
}

SpeechRecognizer::Builder &SpeechRecognizer::Builder::logPath(
  std::string value) {
  properties_->log_path_ = value;
  return *this;
}

std::unique_ptr<SpeechRecognizer> SpeechRecognizer::Builder::build() {
  SpeechRecognizer *tmp = new SpeechRecognizer(std::move(properties_));
  return std::unique_ptr<SpeechRecognizer>(tmp);
}
