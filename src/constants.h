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

#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#include <string>

/// ASR message protocol version
static const std::string asr_version = "ASR 2.4";

/// Literal value of the ASR message delimiter
static const std::string msg_delimiter = "\r\n";

/// Literal value of the ASR message header delimiter
static const std::string hdr_delimiter = "\r\n\r\n";

/// Literal value of the ASR message separator
static const std::string msg_separator = " ";

/// Literal value of the ASR message header separator
static const std::string hdr_separator = ":";

/// Literal value of an empty header
static const std::string empty_header;

#endif  // SRC_CONSTANTS_H_
