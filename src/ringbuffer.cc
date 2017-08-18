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

#include "src/ringbuffer.h"

#include <cstring>

RingBuffer::RingBuffer(unsigned int sizeBytes) {
  data_ = new char[sizeBytes];
  memset(data_, 0, sizeBytes);
  size_ = sizeBytes;
  readPtr_ = 0;
  writePtr_ = 0;
}

RingBuffer::~RingBuffer() { delete[] data_; }

bool RingBuffer::Empty(void) {
  memset(data_, 0, size_);
  readPtr_ = 0;
  writePtr_ = 0;
  return true;
}

unsigned int RingBuffer::Read(char *dataPtr, unsigned int numBytes) {
  unsigned int readBytesAvail = GetReadAvail();

  // If there's nothing to read or no data available, then we can't read
  // anything.
  if (dataPtr == NULL || numBytes <= 0 || readBytesAvail == 0) {
    return 0;
  }

  // Cap our read at the number of bytes available to be read.
  unsigned int numBytesToRead = numBytes < readBytesAvail ? numBytes : readBytesAvail;

  // Make sure we read an even number of bytes. 
  // It is important when converting from bytes to int!
  if (numBytesToRead % 2 != 0) numBytesToRead--;

  // Simultaneously keep track of how many bytes we've read and our position in
  // the outgoing buffer
  if (numBytesToRead > size_ - readPtr_) {
    int len = size_ - readPtr_;
    memcpy(dataPtr, data_ + readPtr_, len);
    memcpy(dataPtr + len, data_, numBytesToRead - len);
  } else {
    memcpy(dataPtr, data_ + readPtr_, numBytesToRead);
  }

  readPtr_ = (readPtr_ + numBytesToRead) % size_;

  return numBytesToRead;
}

unsigned int RingBuffer::ReadAll(std::vector<char>& data) {
  unsigned int readBytesAvail = GetReadAvail();

  // If there's no data available, then we can't read anything.
  if (readBytesAvail == 0) {
    return 0;
  }

  data.clear();

  // Simultaneously keep track of how many bytes we've read and our position in
  // the outgoing buffer
  if (readBytesAvail > size_ - readPtr_) {
    size_t len = size_ - readPtr_;
    data.insert(data.begin(), &data_[readPtr_], &data_[readPtr_ + len]);
    data.insert(data.begin() + len, data_, &data_[readBytesAvail - len]);
  } else {
    data.insert(data.begin(), &data_[readPtr_],
                &data_[readPtr_ + readBytesAvail]);
  }

  readPtr_ = (readPtr_ + readBytesAvail) % size_;

  return readBytesAvail;
}

unsigned int RingBuffer::Write(const char *dataPtr, unsigned int numBytes) {
  unsigned int writeBytesAvail = GetWriteAvail();

  // If there's nothing to write or no room available, we can't write anything.
  if (dataPtr == NULL || numBytes <= 0 || writeBytesAvail == 0) {
    return 0;
  }

  // Cap our write at the number of bytes available to be written.
  unsigned int numBytesToWrite = numBytes < writeBytesAvail ? numBytes : writeBytesAvail;

  // Simultaneously keep track of how many bytes we've written and our position
  // in the incoming buffer
  if (numBytesToWrite > size_ - writePtr_) {
    size_t len = size_ - writePtr_;
    memcpy(data_ + writePtr_, dataPtr, len);
    memcpy(data_, dataPtr + len, numBytesToWrite - len);
  } else {
    memcpy(data_ + writePtr_, dataPtr, numBytesToWrite);
  }

  writePtr_ = (writePtr_ + numBytesToWrite) % size_;

  return numBytesToWrite;
}

unsigned int RingBuffer::GetSize(void) { return size_; }

unsigned int RingBuffer::GetWriteAvail(void) {
  if (writePtr_ == readPtr_) return size_ - 1;

  if (writePtr_ > readPtr_) return size_ - 1 - (writePtr_ - readPtr_);

  return readPtr_ - writePtr_ - 1;
}

unsigned int RingBuffer::GetReadAvail(void) {
  if (writePtr_ == readPtr_) return 0;

  if (writePtr_ > readPtr_) return writePtr_ - readPtr_;

  return size_ - (readPtr_ - writePtr_);
}
