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

#include <cpqd/asr-client/buffer_audio_source.h>

#include <atomic>

#include "src/ringbuffer.h"

class BufferAudioSource::Impl {
  public:
    Impl(unsigned int sizeBytes) :
      ring_buffer_(sizeBytes) {}

    RingBuffer ring_buffer_;

    std::atomic<bool> finished_{false};
};

BufferAudioSource::BufferAudioSource(AudioFormat fmt, size_t buffer_size) :
  AudioSource (fmt) {
  impl_ = std::make_shared<Impl>(buffer_size);
}

BufferAudioSource::~BufferAudioSource() {}

int BufferAudioSource::read(std::vector<char> &buffer) {
  int ret = impl_->ring_buffer_.ReadAll(buffer);

  if (impl_->finished_)
    return -1;

  return ret;
}

bool BufferAudioSource::write(std::vector<char> &buffer) {
  if (impl_->finished_)
    return false;

  impl_->ring_buffer_.Write(buffer.data(), buffer.size());
  return true;
}

bool BufferAudioSource::write(char* buffer, size_t size) {
  if (impl_->finished_)
    return false;

  impl_->ring_buffer_.Write(buffer, size);
  return true;
}

void BufferAudioSource::close() {}

void BufferAudioSource::finish() {
  impl_->finished_ = true;
}
