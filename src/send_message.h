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

#ifndef SRC_SENDMESSAGE_H_
#define SRC_SENDMESSAGE_H_

#include "src/asr_message_response.h"
#include "src/speech_recog_impl.h"

class ASRSendMessage {
 public:
  ASRSendMessage();

  void createSession(SpeechRecognizer::Impl& impl);

  void setParameters(SpeechRecognizer::Impl& impl);

  void startRecognition(SpeechRecognizer::Impl& impl);
};

#endif  // SRC_SENDMESSAGE_H_
