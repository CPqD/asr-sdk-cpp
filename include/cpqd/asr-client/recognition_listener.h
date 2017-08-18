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

#ifndef INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_LISTENER_H_
#define INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_LISTENER_H_

#include <cpqd/asr-client/recognition_result.h>
#include <cpqd/asr-client/recognition_error.h>

#include <string>

class RecognitionListener {
 public:
  RecognitionListener() = default;

  virtual ~RecognitionListener() = default;

  virtual void onListening() = 0;

  virtual void onSpeechStart(int time) = 0;

  virtual void onSpeechStop(int time) = 0;

  virtual void onPartialRecognition(PartialRecognition &partial) = 0;

  virtual void onRecognitionResult(RecognitionResult &result) = 0;

  virtual void onError(RecognitionError& error) = 0;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_RECOGNITION_LISTENER_H_
