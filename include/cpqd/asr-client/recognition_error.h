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

#ifndef INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_ERROR_H_
#define INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_ERROR_H_

#include <string>

class RecognitionError {
 public:
  enum class Code {
    FAILURE,
    
    // Session was inactive for a long period of time and was finalized
    SESSION_TIMEOUT,

    // Connection failure on ASR Server
    CONNECTION_FAILURE,

    // Another recognition is currently running
    ACTIVE_RECOGNITION,
  };

  RecognitionError(Code code, std::string message = std::string());

  std::string getString() const;

  Code getCode() const;

 private:
  Code code_ = Code::FAILURE;

  std::string message_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_ERROR_H_
