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

#ifndef INCLUDE_CPQD_ASR_CLIENT_AUDIO_SOURCE_H_
#define INCLUDE_CPQD_ASR_CLIENT_AUDIO_SOURCE_H_

#include <vector>

enum class AudioFileFormat {
  RAW,
  WAV
};

struct AudioFormat {
    AudioFileFormat fileFormat = AudioFileFormat::WAV;
    unsigned int bits_per_sample_ = 16;
    unsigned int sample_rate_ = 8000;
};

class AudioSource {
 public:
  AudioSource();

  AudioSource(AudioFormat fmt);

  virtual ~AudioSource() = default;

  virtual int read(std::vector<char>& buffer) = 0;

  virtual void close() = 0;

  virtual void finish() = 0;

  AudioFormat getAudioFormat() const;

  protected:
  AudioFormat fmt_;
};

#endif  // INCLUDE_CPQD_ASR_CLIENT_AUDIO_SOURCE_H_
