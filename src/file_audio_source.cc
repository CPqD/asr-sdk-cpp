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

#include <cpqd/asr-client/file_audio_source.h>

#include <cstdint>
#include <iostream>

typedef struct {
  uint8_t riff[4];
  uint32_t chunk_size;
  uint8_t wave[4];

  uint8_t fmtchunk_id[4];
  uint32_t fmtchunk_size;
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bps;

  uint8_t datachunk_id[4];
  uint32_t datachunk_size;
} WavHeader;

FileAudioSource::FileAudioSource(const std::string& file_name, AudioFormat fmt)
    : AudioSource (fmt), file_name_(file_name) {

  ifs_.open(file_name_, std::ifstream::in | std::ifstream::binary);

  if(file_name.substr(file_name.find_last_of(".") + 1) == "wav") {
    WavHeader header;

    ifs_.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    datachunk_size_ = header.datachunk_size;
    fmt_.sample_rate_ = header.sample_rate;
    fmt_.bits_per_sample_ = header.bps;
    return;
  }

  fmt_.fileFormat = AudioFileFormat::RAW;
  datachunk_size_ = ifs_.tellg();
}

FileAudioSource::~FileAudioSource() {
  ifs_.close();
}

int FileAudioSource::read(std::vector<char>& buffer) {
  char tmp[block_size_];
  ifs_.read(tmp, block_size_);

  if (ifs_.gcount() == 0)
    return -1;

  buffer.assign(tmp, tmp + ifs_.gcount());

  return ifs_.gcount();
}

void FileAudioSource::close() {}

void FileAudioSource::finish() {}
