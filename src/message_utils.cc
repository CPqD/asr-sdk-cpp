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

#include "src/message_utils.h"
#include "src/constants.h"

std::string getString(Method c) {
  switch (c) {
  case Method::CreateSession:     return "CREATE_SESSION";
  case Method::SetParameters:     return "SET_PARAMETERS";
  case Method::GetParameters:     return "GET_PARAMETERS";
  case Method::StartRecognition:  return "START_RECOGNITION";
  case Method::StartInputTimers:  return "START_INPUT_TIMERS";
  case Method::SendAudio:         return "SEND_AUDIO";
  case Method::CancelRecognition: return "CANCEL_RECOGNITION";
  case Method::ReleaseSession:    return "RELEASE_SESSION";
  case Method::Response:          return "RESPONSE";
  case Method::StartOfSpeech:     return "START_OF_SPEECH";
  case Method::EndOfSpeech:       return "END_OF_SPEECH";
  case Method::RecognitionResult: return "RECOGNITION_RESULT";
  }
}

std::string startLine(Method c) {
  return asr_version + msg_separator + getString(c);
}
