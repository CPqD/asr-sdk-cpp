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

#ifndef SRC_ASR_MESSAGE_REQUEST_H_
#define SRC_ASR_MESSAGE_REQUEST_H_

#include <string>

#include "src/asr_message_parser.h"
#include "src/message_utils.h"

class ASRMessageRequest : public ASRMessageParser {
 public:
  explicit ASRMessageRequest(Method c);

  /// Set a value for an ASR Message header, replacing an existing value
  /**
   * This method will set the value of the ASR Message header `key` with the
   * indicated value. If a header with the name `key` already exists, `val`
   * will replace the existing value.
   *
   * @param [in] key The name/key of the header to append to.
   * @param [in] val The value to append.
   */
  void set_header(const std::string& key, const std::string& val) {
    headers_[key] = val;
  }

  /// Remove a header from the parser
  /**
   * Removes the header entirely from the parser
   *
   * @param [in] key The name/key of the header to remove.
   */
  void remove_header(const std::string& key) { headers_.erase(key); }

  /// Set ASR Message extra content
  /**
   * @param [in] value The value to set the ASR Message extra content.
   */
  void set_extra(std::string const& value) { extra_ = value; }

  /// Set ASR Message start line
  /**
   * @param [in] value The value to set the ASR Message start line.
   */
  void set_start_line(std::string const& value) { start_line_ = value; }
};

#endif  // SRC_ASR_MESSAGE_REQUEST_H_
