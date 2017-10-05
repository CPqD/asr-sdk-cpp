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

#ifndef INCLUDE_CPQD_ASR_CLIENT_LANGUAGE_MODEL_LIST_H_
#define INCLUDE_CPQD_ASR_CLIENT_LANGUAGE_MODEL_LIST_H_

#include <memory>
#include <string>

class LanguageModelList {
 public:
  class Builder;

 private:
  class Properties {
   private:
    Properties() = default;

    std::string uri_;

    // inline grammar body
    std::string grammar_body_;

    friend class LanguageModelList;
    friend class LanguageModelList::Builder;
  };

 public:
  LanguageModelList() = delete;

  std::string getUri();

  std::string getGrammarBody();

 private:
  explicit LanguageModelList(const Properties& properties)
      : properties_(properties) {}

  Properties properties_;
};

class LanguageModelList::Builder {
 public:
  Builder() = default;

  std::unique_ptr<LanguageModelList> build();

  // Add a language model (free speech, pre-compiled grammar or dynamic grammar)
  // from its URI. URIs are allowed with protocols: "http", "file", "builtin".
  // At the moment, we only support "file" e "builtin".
  Builder& addFromURI(const std::string& uri);

  // Add a grammar from its body, inferring the type of grammar by its content
  // Allowed types are SRGS ABNF or XML
  Builder& addInlineGrammar(const std::string& addInlineGrammar);

 private:
  LanguageModelList::Properties properties_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_LANGUAGE_MODEL_LIST_H_
