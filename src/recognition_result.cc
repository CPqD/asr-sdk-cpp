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

#include <cpqd/asr-client/recognition_result.h>

RecognitionResult::RecognitionResult()
  : result_status_(Code::NO_MATCH)
{}

RecognitionResult::RecognitionResult(Code result_status)
  : result_status_(result_status)
{}

RecognitionResult::RecognitionResult(std::string status_string){
  if(status_string == "PROCESSING")
    result_status_ = Code::NO_MATCH;
  else if (status_string == "RECOGNIZED")
    result_status_ = Code::RECOGNIZED;
  else if (status_string == "NO_MATCH")
    result_status_ = Code::NO_MATCH;
  else if (status_string == "NO_INPUT_TIMEOUT")
    result_status_ = Code::NO_INPUT_TIMEOUT;
  else if (status_string == "MAX_SPEECH")
    result_status_ = Code::MAX_SPEECH;
  else if (status_string == "EARLY_SPEECH")
    result_status_ = Code::EARLY_SPEECH;
  else if (status_string == "RECOGNITION_TIMEOUT")
    result_status_ = Code::RECOGNITION_TIMEOUT;
  else if (status_string == "NO_SPEECH")
    result_status_ = Code::NO_SPEECH;
  else if (status_string == "CANCELED")
    result_status_ = Code::CANCELED;
  else if (status_string == "FAILURE")
    result_status_ = Code::FAILURE;
}

int RecognitionResult::Alternative::getConfidence() { return confidence_; }

std::string RecognitionResult::Alternative::getText() { return text_; }

std::vector<Interpretation>
RecognitionResult::Alternative::getInterpretations() {
  return interpretations_;
}

std::vector<Word>
RecognitionResult::Alternative::getWords() {
  return words_;
}

RecognitionResult::Alternative &RecognitionResult::Alternative::languageModel(
    std::string lang_model) {
  lang_model_ = lang_model;
  return *this;
}

RecognitionResult::Alternative &RecognitionResult::Alternative::text(
    std::string text) {
  text_ = text;
  return *this;
}

RecognitionResult::Alternative &
RecognitionResult::Alternative::addInterpretation(std::string text,
                                                  int confidence) {
  interpretations_.push_back({text, confidence});
  return *this;
}

RecognitionResult::Alternative &
RecognitionResult::Alternative::addWord(Word word) {
  words_.push_back(word);
  return *this;
}

RecognitionResult::Alternative &
RecognitionResult::Alternative::addWord(std::string text, int confidence,
                                        float start_time, float end_time) {
  words_.push_back({text, confidence, start_time, end_time});
  return *this;
}

RecognitionResult::Alternative &RecognitionResult::Alternative::confidence(
    int confidence) {
  confidence_ = confidence;
  return *this;
}

RecognitionResult &RecognitionResult::addAlternatives(const Alternative &alt) {
  if (result_status_ == Code::NO_MATCH)
    result_status_ = Code::RECOGNIZED;

  alternatives_.push_back(alt);
  return *this;
}

std::string RecognitionResult::getString(RecognitionResult::Code st) {
  switch (st) {
  case Code::PROCESSING:
    return "PROCESSING";
  case Code::RECOGNIZED:
    return "RECOGNIZED";
  case Code::NO_MATCH:
    return "NO_MATCH";
  case Code::NO_INPUT_TIMEOUT:
    return "NO_INPUT_TIMEOUT";
  case Code::MAX_SPEECH:
    return "MAX_SPEECH";
  case Code::EARLY_SPEECH:
    return "EARLY_SPEECH";
  case Code::RECOGNITION_TIMEOUT:
    return "RECOGNITION_TIMEOUT";
  case Code::NO_SPEECH:
    return "NO_SPEECH";
  case Code::CANCELED:
    return "CANCELED";
  case Code::FAILURE:
    return "FAILURE";
  }
}

std::string RecognitionResult::Classifier::getAge() {
  return age_;
}

std::string RecognitionResult::Classifier::getEmotion() {
  return emotion_;
}

std::string RecognitionResult::Classifier::getGender() {
  return gender_;
}

void RecognitionResult::Classifier::setAge(std::string& age) {
  age_ = age;
}

void RecognitionResult::Classifier::setEmotion(std::string& emotion) {
  emotion_ = emotion;
}

void RecognitionResult::Classifier::setGender(std::string& gender) {
  gender_ = gender;
}

bool RecognitionResult::Classifier::hasAge() {
  return !age_.empty();
}

bool RecognitionResult::Classifier::hasEmotion() {
  return !emotion_.empty();
}

bool RecognitionResult::Classifier::hasGender() {
  return !gender_.empty();
}
