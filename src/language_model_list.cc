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

std::unique_ptr<LanguageModelList> LanguageModelList::Builder::build() {
  return std::unique_ptr<LanguageModelList>(new LanguageModelList(properties_));
}

LanguageModelList::Builder& LanguageModelList::Builder::addFromURI(
    const std::string& uri) {
  properties_.uri_ = uri;
  return *this;
}

std::string LanguageModelList::getUri() { return properties_.uri_; }
