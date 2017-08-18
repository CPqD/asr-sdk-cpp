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

#ifndef INCLUDE_CPQD_ASR_CLIENT_FILE_AUDIO_SOURCE_H_
#define INCLUDE_CPQD_ASR_CLIENT_FILE_AUDIO_SOURCE_H_

#include <cpqd/asr-client/audio_source.h>

#include <fstream>
#include <string>
#include <vector>

class FileAudioSource : public AudioSource {
 public:
  explicit FileAudioSource(const std::string& file_name,
                           AudioFormat fmt = AudioFormat());

  ~FileAudioSource();

  int read(std::vector<char>& buffer);

  void close();

  void finish();

 private:
  std::string file_name_;
  std::ifstream ifs_;
  static const size_t block_size_ = 10000;
  size_t datachunk_size_ = 0;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_FILE_AUDIO_SOURCE_H_
