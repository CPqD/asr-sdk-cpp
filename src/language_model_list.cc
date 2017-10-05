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

#include <cpqd/asr-client/language_model_list.h>
#include <cpqd/asr-client/recognition_exception.h>

std::unique_ptr<LanguageModelList> LanguageModelList::Builder::build() {
  return std::unique_ptr<LanguageModelList>(new LanguageModelList(properties_));
}

LanguageModelList::Builder& LanguageModelList::Builder::addFromURI(
    const std::string& uri) {
  // at the moment support only one uri or inline grammar
  if (!properties_.uri_.empty() || !properties_.grammar_body_.empty()) {
    throw RecognitionException(RecognitionError::Code::FAILURE,
        std::string("Only one lm is supported at the moment"));
  }

  properties_.uri_ = uri;
  return *this;
}

LanguageModelList::Builder& LanguageModelList::Builder::addInlineGrammar(
    const std::string& grammar_body) {
  // at the moment support only one uri or inline grammar
  if (!properties_.uri_.empty() || !properties_.grammar_body_.empty()) {
    throw RecognitionException(RecognitionError::Code::FAILURE,
        std::string("Only one lm is supported at the moment"));
  }

  properties_.grammar_body_ = grammar_body;
  return *this;
}

std::string LanguageModelList::getUri() { return properties_.uri_; }

std::string LanguageModelList::getGrammarBody() {
  return properties_.grammar_body_;
}
