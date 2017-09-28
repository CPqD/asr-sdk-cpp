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

#include "src/process_msg.h"
#include <mutex>

#include <cpqd/asr-client/recognition_exception.h>
#include <cpqd/asr-client/recognition_result.h>

#include <string>

#include "src/asr_message_request.h"
#include "src/message_utils.h"
#include "src/send_message.h"



void ASRProcessMsg::setNext(std::shared_ptr<ASRProcessMsg> &next) {
  next_ = next;
}

bool ASRProcessMsg::handle(SpeechRecognizer::Impl &impl,
                           ASRMessageResponse &response) {
  if (!next_) return false;
  return next_->handle(impl, response);
}

void ASRProcessMsg::add(std::shared_ptr<ASRProcessMsg> &next) {
  if (next_)
    next_->add(next);
  else
    next_ = next;
}

bool ASRProcessResponse::handle(SpeechRecognizer::Impl &impl,
                                ASRMessageResponse &response) {
  // TODO: Treat the version string
  std::string method_response = split(response.get_start_line(), ' ')[2];

  if (method_response != getMethodString(Method::Response)) {
    return ASRProcessMsg::handle(impl, response);
  }

  std::string status = getString(ResponseHeader::SessionStatus);
  std::string value = response.get_header(status);

  if (status == getString(SessionStatus::Idle))
    impl.session_status_ = SpeechRecognizer::Impl::SessionStatus::kIdle;
  else if (status == getString(SessionStatus::Listening)) {
    impl.session_status_ = SpeechRecognizer::Impl::SessionStatus::kListening;
    // invoking callback
    for (std::unique_ptr<RecognitionListener> &listener : impl.listener_) {
      listener->onListening();
    }
  } else if (status == getString(SessionStatus::Recognizing))
    impl.session_status_ = SpeechRecognizer::Impl::SessionStatus::kRecognizing;

  std::string method = getString(ResponseHeader::Method);
  value = response.get_header(method);

  if (value == getMethodString(Method::CreateSession)) {
    return createSession(impl, response);
  } else if (value == getMethodString(Method::SetParameters)) {
    return setParameters(impl, response);
  } else if (value == getMethodString(Method::StartRecognition)) {
    return startRecog(impl, response);
  } else if (value == getMethodString(Method::CancelRecognition)) {
    return cancelRecog(impl, response);
  }

  return true;
}

bool ASRProcessResponse::createSession(SpeechRecognizer::Impl &impl,
                                       ASRMessageResponse &response) {
  std::string key = getString(ResponseHeader::Result);
  std::string header = response.get_header(key);

  if (header != getString(ResultStatus::SUCCESS)) {
    generateError(impl, response);
    return false;
  }

  ASRSendMessage send_msg_;
  if (!impl.config_) {
    send_msg_.startRecognition(impl);
    return true;
  }

  send_msg_.setParameters(impl);
  return true;
}

bool ASRProcessResponse::setParameters(SpeechRecognizer::Impl &impl,
                                       ASRMessageResponse &response) {
  std::string key = getString(ResponseHeader::Result);
  std::string header = response.get_header(key);

  if (header != getString(ResultStatus::SUCCESS)) {
    generateError(impl, response);
    return false;
  }

  ASRSendMessage send_msg_;
  send_msg_.startRecognition(impl);

  return true;
}

bool ASRProcessResponse::startRecog(SpeechRecognizer::Impl &impl,
                                    ASRMessageResponse &response) {
  std::string key = getString(ResponseHeader::Result);
  std::string header = response.get_header(key);

  if (header != getString(ResultStatus::SUCCESS)) {
    generateError(impl, response);
    return false;
  }

  if (impl.audio_src_ == nullptr) return false;
//  impl.terminateSendMessageThread(); // Terminate any send message processes
  impl.sendAudioMessage_thread_ =
      std::thread(
        &ASRProcessResponse::sendAudioMessage,
        this, std::ref(impl)
        );
  return true;
}

bool ASRProcessResponse::cancelRecog(SpeechRecognizer::Impl &impl,
                                     ASRMessageResponse &response) {
  std::string key = getString(ResponseHeader::Result);
  std::string header = response.get_header(key);

  // Invalid action treated as successful since cancel recog shouldn't throw
  // errors if server is not recognizing anything
  if (header != getString(ResultStatus::SUCCESS) &&
      header != getString(ResultStatus::INVALID_ACTION)) {
    generateError(impl, response);
    return false;
  }

  // discard any pending result
  impl.result_.clear();

  impl.recognizing_ = false;
  impl.cv_.notify_one();
  return true;
}

bool ASRProcessResponse::sendAudioMessage(SpeechRecognizer::Impl &impl) {
  int ret = 1;

  do {
    std::this_thread::sleep_for(
          std::chrono::milliseconds(100)
          );
    ASRMessageRequest request(Method::SendAudio);

    std::vector<char> buffer;
    ret = impl.audio_src_->read(buffer);

    if (impl.sendAudioMessage_terminate_) return true;
    if (buffer.empty() && ret != -1) continue;

    std::string extra(buffer.begin(), buffer.end());
    request.set_extra(extra);
    request.set_header("Content-Length", std::to_string(extra.size()));
    request.set_header("Content-Type", "application/octet-stream");

    if (ret == -1) {
      request.set_header("LastPacket", "true");
    } else
      request.set_header("LastPacket", "false");

    std::string raw_message = request.raw();

    {
      std::unique_lock<std::mutex> l(impl.lock_);
      impl.logger_.write(
            websocketpp::log::elevel::info,
            "[SEND] " + request.get_start_line() + "\nContent-Length: " +
            request.get_header("Content-Length") + "\nLastPacket: " +
            request.get_header("LastPacket") + "\n");
    }

    impl.sendMessage(raw_message);
  } while (ret != -1);

  return true;
}

void ASRProcessResponse::generateError(SpeechRecognizer::Impl &impl,
                                       ASRMessageResponse &response) {
  std::string key = getString(ResponseHeader::ErrorCode);
  std::string error_msg = response.get_header(key);

  key = getString(ResponseHeader::Message);
  std::string header = response.get_header(key);

  if (!header.empty())
    error_msg += ": " + header;

  key = getString(ResponseHeader::Method);
  header = response.get_header(key);

  impl.recognitionError(RecognitionError::Code::FAILURE,
                        "[" + header + "]: " + error_msg);
}

std::string ASRProcessResponse::getString(ResponseHeader rh) {
  switch (rh) {
  case ResponseHeader::Handle:
    return "Handle";
  case ResponseHeader::Method:
    return "Method";
  case ResponseHeader::Expires:
    return "Expires";
  case ResponseHeader::Result:
    return "Result";
  case ResponseHeader::SessionStatus:
    return "Session-Status";
  case ResponseHeader::ErrorCode:
    return "Error-Code";
  case ResponseHeader::Message:
    return "Message";
  }
}

std::string ASRProcessResponse::getString(SessionStatus st) {
  switch (st) {
  case SessionStatus::Idle:
    return "IDLE";
  case SessionStatus::Listening:
    return "LISTENING";
  case SessionStatus::Recognizing:
    return "RECOGNIZING";
  }
}

std::string ASRProcessResponse::getString(ResultStatus value) {
  switch (value) {
  case ResultStatus::SUCCESS:
    return "SUCCESS";
  case ResultStatus::FAILURE:
    return "FAILURE";
  case ResultStatus::INVALID_ACTION:
    return "INVALID_ACTION";
  }
}

bool ASRProcessStartSpeech::handle(SpeechRecognizer::Impl &impl,
                                   ASRMessageResponse &response) {
  std::string method = split(response.get_start_line(), ' ')[2];
  if (method != getMethodString(Method::StartOfSpeech)) {
    return ASRProcessMsg::handle(impl, response);
  }

  // invoking callback
  for (std::unique_ptr<RecognitionListener> &listener : impl.listener_) {
    listener->onSpeechStart(0);
  }

  return true;
}

bool ASRProcessEndSpeech::handle(SpeechRecognizer::Impl &impl,
                                 ASRMessageResponse &response) {
  std::string method = split(response.get_start_line(), ' ')[2];
  if (method != getMethodString(Method::EndOfSpeech)) {
    return ASRProcessMsg::handle(impl, response);
  }

  // invoking callback
  for (std::unique_ptr<RecognitionListener> &listener : impl.listener_) {
    listener->onSpeechStop(0);
  }

  return true;
}
