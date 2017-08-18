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

#ifndef SRC_PROCESS_MSG_H_
#define SRC_PROCESS_MSG_H_

#include <memory>
#include <string>

#include "src/asr_message_response.h"
#include "src/speech_recog_impl.h"

class ASRProcessMsg {
 public:
  ASRProcessMsg() : next_(nullptr) {}

  virtual ~ASRProcessMsg() = default;

  virtual bool handle(SpeechRecognizer::Impl& impl,
                      ASRMessageResponse& response);

  void setNext(std::shared_ptr<ASRProcessMsg>& next);

  void add(std::shared_ptr<ASRProcessMsg>& next);

 protected:
  std::shared_ptr<ASRProcessMsg> next_;
};

class ASRProcessResponse : public ASRProcessMsg {
 public:
  enum class ResponseHeader {
    Handle,
    Method,
    Expires,
    Result,
    SessionStatus,
    ErrorCode,
    Message,
  };

  enum class ResultStatus { SUCCESS, FAILURE, INVALID_ACTION };

  enum class SessionStatus { Idle, Listening, Recognizing };

  bool handle(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);

 private:
  bool createSession(SpeechRecognizer::Impl& impl,
                     ASRMessageResponse& response);

  bool setParameters(SpeechRecognizer::Impl& impl,
                     ASRMessageResponse& response);

  bool startRecog(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);

  bool cancelRecog(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);

  bool sendAudioMessage(SpeechRecognizer::Impl& impl);

  void generateError(SpeechRecognizer::Impl& impl,
                     ASRMessageResponse& response);

  std::string getString(ResponseHeader rh);
  std::string getString(SessionStatus st);
  std::string getString(ResultStatus value);
};

class ASRProcessStartSpeech : public ASRProcessMsg {
  bool handle(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);
};

class ASRProcessEndSpeech : public ASRProcessMsg {
  bool handle(SpeechRecognizer::Impl& impl, ASRMessageResponse& response);
};

#endif  // SRC_PROCESS_MSG_H_
