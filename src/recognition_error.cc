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

#include <cpqd/asr-client/recognition_error.h>

RecognitionError::RecognitionError(RecognitionError::Code code,
                                   std::string message)
    : code_(code), message_(message) {}

std::string RecognitionError::getString() const {
  std::string msg;
  switch (code_) {
    case Code::FAILURE:
      msg = "FAILURE";
      break;
    case Code::SESSION_TIMEOUT:
      msg = "SESSION_TIMEOUT";
      break;
    case Code::CONNECTION_FAILURE:
      msg = "CONNECTION_FAILURE";
      break;
    case Code::ACTIVE_RECOGNITION:
      msg = "ACTIVE_RECOGNITION";
      break;  
  }

  if (!message_.empty()) msg += ": " + message_;

  return msg;
}

RecognitionError::Code RecognitionError::getCode() const { return code_; }
