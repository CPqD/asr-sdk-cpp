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

#ifndef INCLUDE_CPQD_ASR_CLIENT_MIC_AUDIO_SOURCE_H_
#define INCLUDE_CPQD_ASR_CLIENT_MIC_AUDIO_SOURCE_H_

// Defined as such for divisibility by 2, 3 and 4 (16, 24 and 32 bit)
#ifndef MIC_BUFFER_SIZE_BYTES
#define MIC_BUFFER_SIZE_BYTES 1536
#endif

#include <cpqd/asr-client/audio_source.h>

#include <atomic>
#include <thread>
#include <fstream>
#include <string>
#include <deque>
#include <mutex>

class MicAudioSource : public AudioSource {
 public:
  explicit MicAudioSource(AudioFormat fmt = AudioFormat(), size_t buffer_size = 4096);

  ~MicAudioSource();

  int read(std::vector<char>& buffer);

  void close();

  void finish();

 private:
  typedef void PaStream;
  typedef struct PaStreamParameters PaStreamParameters;

  void open();
  void enqueueThread();
  void threadJoin();
  void threadWaitJoin();

  std::atomic<bool> thread_join_;
  std::atomic<bool> finish_;
  std::atomic<bool> is_open_;
  std::thread thread_;
  std::mutex mtx_;

  /* Low-level buffer for writing data from microphone*/
  char mic_buffer_[MIC_BUFFER_SIZE_BYTES];        // Hard-coded, in bytes
  size_t byteIndex_;

  /* High-level buffer for enqueing data and passing to the ASR client */
  std::deque<char> data_;
  size_t buffer_size_;

  PaStream* stream_;
  std::unique_ptr<PaStreamParameters> params_;
  int err_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_MIC_AUDIO_SOURCE_H_
