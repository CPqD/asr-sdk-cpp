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

#ifndef INCLUDE_CPQD_ASR_CLIENT_BUFFER_AUDIO_SOURCE_H_
#define INCLUDE_CPQD_ASR_CLIENT_BUFFER_AUDIO_SOURCE_H_

#include <cpqd/asr-client/audio_source.h>

#include <memory>
#include <mutex>

class BufferAudioSource : public AudioSource {
 public:
  class Impl;

  BufferAudioSource(AudioFormat fmt = AudioFormat(),
                    size_t buffer_size = 50000);

  ~BufferAudioSource();

  int read(std::vector<char>& buffer);

  bool write(std::vector<char>& buffer);
  
  bool write(char* buffer, size_t size);

  void close();

  void finish();

 private:
  std::shared_ptr<Impl> impl_ = nullptr;

  std::mutex mtx_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_BUFFER_AUDIO_SOURCE_H_
