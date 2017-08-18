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

#include "src/asr_message_parser.h"

#include <sstream>

std::string ASRMessageParser::raw() const {
  std::stringstream raw;

  if (!start_line_.empty()) raw << start_line_ << msg_delimiter;

  std::map<std::string, std::string>::const_iterator it;
  for (it = headers_.begin(); it != headers_.end(); it++) {
    raw << it->first << hdr_separator << it->second << msg_delimiter;
  }

  raw << msg_delimiter;

  if (!extra_.empty()) raw << extra_ << msg_delimiter;

  return raw.str();
}

const std::string &ASRMessageParser::get_header(const std::string &key) const {
  std::map<std::string, std::string>::const_iterator h = headers_.find(key);

  if (h == headers_.end()) {
    return empty_header;
  } else {
    return h->second;
  }
}
