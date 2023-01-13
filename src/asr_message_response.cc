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

#include "src/asr_message_response.h"

#include <algorithm>
#include <cctype>
#include <iostream>

#include "ws_parser.h"

namespace internal {

void removeExtraSpaces(std::string& input) {
  bool prev_is_space = true;
  input.erase(std::remove_if(input.begin(), input.end(),
                             [&prev_is_space](char curr) {
                               bool r = std::isspace(curr) && prev_is_space;
                               prev_is_space = std::isspace(curr);
                               return r;
                             }),
              input.end());
}

}  // namespace internal

void ASRMessageResponse::consume(const std::string& payload) {
  std::string msg_body = payload;
  std::string msg_extra;

  cpqd::WsParser parser(payload);
  headers_ = parser.GetParams();
  extra_ = parser.GetBody();
  start_line_ = "ASR 2.4 " + parser.GetCmd();
  auto start = 0U;
  auto end = payload.find(hdr_delimiter);

  // search for empty line
  if (end != std::string::npos) {
    msg_body = payload.substr(start, end - start);
    msg_extra = payload.substr(end + hdr_delimiter.length());
  }
}
