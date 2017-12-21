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

#include <cpqd/asr-client/mic_audio_source.h>

#include <cstdint>
#include <iostream>
#include <cpqd/asr-client/recognition_exception.h>
extern "C"{
#include "portaudio.h"
}

MicAudioSource::MicAudioSource(AudioFormat fmt, size_t buffer_size)
  : AudioSource (fmt) {

  buffer_size_ = buffer_size;
  params_ = std::unique_ptr<PaStreamParameters>(new PaStreamParameters());

  params_->channelCount = 1; // ASR works only with monaural audio
  switch(fmt_.bits_per_sample_) {
  case 8:
    params_->sampleFormat = paInt8; break;
  case 16:
    params_->sampleFormat = paInt16; break;
  case 24:
    params_->sampleFormat = paInt24; break;
  case 32:
    params_->sampleFormat = paInt32; break;
  default:
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Invalid bits per sample in microphone usage ") +
                               std::to_string(fmt_.bits_per_sample_));
  }
  finish_ = false;
  thread_join_ = true; // Start with closed thread
  is_open_ = false;
}

void MicAudioSource::open(){
  if(is_open_){
    close();
  }
  err_ = Pa_Initialize();
  if(err_ != paNoError){
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error in microphone PortAudio initialization: ") +
                               std::string(Pa_GetErrorText(err_)));
  }
  if( Pa_GetDeviceCount() <= 0 )
  {
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error in microphone PortAudio initialization: ") +
                               std::string("No available input devices."));
  }
  params_->device = Pa_GetDefaultInputDevice();
// TODO: deal with fancy microphones (USB and whatnot)i
//  for (int i = 0, end = Pa_GetDeviceCount(); i != end; ++i) {
//      PaDeviceInfo const* info = Pa_GetDeviceInfo(i);
//      if (!info) continue;
//      std::cout << i << ": " << info->name << std::endl;
//  }
  int framesPerBuffer = MIC_BUFFER_SIZE_BYTES * 8 / fmt_.bits_per_sample_;
  err_ = Pa_OpenStream(&stream_,
                       params_.get(),
                       nullptr,
                       fmt_.sample_rate_,
                       framesPerBuffer,
                       0,
                       nullptr,
                       this);

  if(err_ != paNoError){
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error in opening PortAudio stream: ") +
                               std::string(Pa_GetErrorText(err_)));
  }
  thread_join_ = false;
  thread_ = std::thread(&MicAudioSource::enqueueThread, this);
  is_open_ = true;
}

void MicAudioSource::enqueueThread(){
  // Roughly the time in milisseconds for a buffer write cycle from the microphone
  int framesPerBuffer = MIC_BUFFER_SIZE_BYTES * 8 / fmt_.bits_per_sample_;
  err_ = Pa_StartStream(stream_);
  if(err_ != paNoError){
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error in starting PortAudio stream: ") +
                               std::string(Pa_GetErrorText(err_)));
  }
  while(!thread_join_){
    err_ = Pa_ReadStream(stream_, mic_buffer_, framesPerBuffer);
    if(err_ < 0){
      throw RecognitionException(RecognitionError::Code::FAILURE,
                                 std::string("Error in PortAudio stream activity: ") +
                                 std::string(Pa_GetErrorText(err_)));
    }
    /* Write on output high-level buffer */
    {
      std::unique_lock<std::mutex> l(mtx_);
      for(int i=0; i<MIC_BUFFER_SIZE_BYTES; i++){
        data_.push_back(mic_buffer_[i]);
      }
    }
  }
  if(Pa_IsStreamActive(stream_)){
    err_ = Pa_StopStream(stream_);
    if(err_ != paNoError){
      throw RecognitionException(RecognitionError::Code::FAILURE,
                                 std::string("Error in stopping PortAudio stream: ") +
                                 std::string(Pa_GetErrorText(err_)));
    }
  }
  err_ = Pa_CloseStream(stream_);
  if(err_ != paNoError){
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error in PortAudio stream closure: ") +
                               std::string(Pa_GetErrorText(err_)));
  }
  stream_ = nullptr;
}

MicAudioSource::~MicAudioSource() {
  close();
  Pa_Terminate();
}

int MicAudioSource::read(std::vector<char>& buffer) {
  /* Write on output high-level buffer */
  size_t i = 0;
  if(finish_){
    finish_ = false;
    data_.clear();
    buffer.clear();
    return -1;
  }
  if(!is_open_)
    this->open();
  buffer.clear(); // Clear any contents of the buffer
  while(i<buffer_size_)
  {
    {
      std::unique_lock<std::mutex> l(mtx_);
      for(; i<buffer_size_ && !data_.empty(); i++){
        buffer.push_back(data_[0]);
        data_.pop_front();
      }
    }
    // Give the enqueing thread some room for populating the buffer
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return i;
}

void MicAudioSource::threadJoin() {
  thread_join_ = true;
  mtx_.lock();
  if(thread_.joinable()){
    mtx_.unlock();
    thread_.join();
  } else mtx_.unlock();
}

void MicAudioSource::threadWaitJoin() {
  mtx_.lock();
  while(thread_.joinable()){
    mtx_.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    mtx_.lock();
  }
  mtx_.unlock();
}

void MicAudioSource::close() {
  threadJoin();
  threadWaitJoin();
  if(stream_ != nullptr){
    throw RecognitionException(RecognitionError::Code::FAILURE,
                               std::string("Error on closing PortAudio stream."));
  }
  data_.clear();
  is_open_ = false;
}

void MicAudioSource::finish() {
  finish_ = true;
}
