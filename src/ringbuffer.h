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

#ifndef SRC_RINGBUFFER_H_
#define SRC_RINGBUFFER_H_

#include <vector>
#include <atomic>

class RingBuffer {
 public:

  RingBuffer(unsigned int sizeBytes);

  ~RingBuffer();

  unsigned int Read(char* dataPtr, unsigned int numBytes);

  unsigned int ReadAll(std::vector<char>& data);

  // Write to the ring buffer. Do not overwrite data that has not yet
  // been read.
  unsigned int Write(const char* dataPtr, unsigned int numBytes);

  // Set all data to 0 and flag buffer as empty.
  bool Empty(void);

  unsigned int GetSize();

  unsigned int GetWriteAvail();

  unsigned int GetReadAvail();

 private:
  char* data_;
  unsigned int size_;
  std::atomic<unsigned int> readPtr_;
  std::atomic<unsigned int> writePtr_;
};

#endif  // SRC_RINGBUFFER_H_
