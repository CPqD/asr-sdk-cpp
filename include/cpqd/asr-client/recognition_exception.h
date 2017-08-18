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

#ifndef INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_EXCEPTION_H_
#define INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_EXCEPTION_H_

#include <cpqd/asr-client/recognition_error.h>

#include <exception>
#include <string>

#ifdef _MSC_VER
#define noexcept
#endif

class RecognitionException : public std::exception {
 public:
  RecognitionException() = delete;

  explicit RecognitionException(RecognitionError::Code code,
                                std::string message = std::string())
      : error_(code, message) {
    message_ = error_.getString();
  }

  const char* what() const noexcept { return message_.c_str(); }

  RecognitionError::Code getCode() const { return error_.getCode(); }

 private:
  RecognitionError error_;

  std::string message_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_EXCEPTION_H_
