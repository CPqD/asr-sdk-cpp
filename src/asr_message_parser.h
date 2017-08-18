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

#ifndef SRC_ASR_MESSAGE_PARSER_H_
#define SRC_ASR_MESSAGE_PARSER_H_

#include <map>
#include <string>

#include "src/constants.h"

/// Base ASR Message parser
/**
 * Includes methods and data elements common to all types of ASR messages such
 * as headers, initial_line, extra.
 */
class ASRMessageParser {
public:
  ASRMessageParser() = default;

  /// Generate and return the ASR Message headers as a string
  std::string raw() const;

  /// Get the value of an ASR Message header
  /**
   * @todo Make this method case insensitive.
   *
   * @param [in] key The name/key of the header to get.
   * @return The value associated with the given ASR Message header key.
   */
  const std::string& get_header(const std::string& key) const;

  /// Get ASR Message body
  /**
   * Gets the body of the ASR Message object
   *
   * @return The body of the ASR Message message.
   */
  const std::string& get_extra() const { return extra_; }

  /// Get ASR Message start line
  /**
   * Gets the start line of the ASR Message object
   *
   * @return The start line of the ASR Message message.
   */
  std::string get_start_line() const { return start_line_; }

protected:
  std::string start_line_;
  std::map<std::string, std::string> headers_;
  std::string extra_;
};

#endif  // SRC_ASR_MESSAGE_PARSER_H_
