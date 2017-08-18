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

#ifndef SRC_PROCESSRESULT_H_
#define SRC_PROCESSRESULT_H_

#include <cpqd/asr-client/recognition_result.h>
#include "src/process_msg.h"

#include <string>

class ASRProcessResult : public ASRProcessMsg {
 public:

   typedef RecognitionResult::Code ResultStatus;

   enum class Header {
    Handle,
    ResultStatus,
    SessionStatus,
    ContentLength,
    ContentType,
  };

  enum class SessionStatus { IDLE };

 public:
  bool handle(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);

 private:
  std::string getString(Header hdr);
  std::string getString(SessionStatus st);
};

#endif  // SRC_PROCESSRESULT_H_
