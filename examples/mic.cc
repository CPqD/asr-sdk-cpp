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


#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include <cpqd/asr-client/mic_audio_source.h>
#include <cpqd/asr-client/language_model_list.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_result.h>
#include <cpqd/asr-client/speech_recog.h>

int main(int argc, char* argv[]) {
  if(argc != 3 && argc != 5){
    std::cerr << "Usage: " << argv[0] << " <ws_url> <lang_uri> [ <user> <password> ]" << std::endl;
    std::cerr << "   eg: " << argv[0] << " ws://127.0.0.1:8025/asr-server/asr builtin:grammar/samples/phone " << std::endl;
    std::cerr << "  eg2: " << argv[0] << " wss://contact/cpqd/and/request/a/key/ builtin:slm/general /path/to/audio.wav myusername mypassword" << std::endl;
    return -1;
  }
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder()
      .maxSentences(3)
      .confidenceThreshold(50)
      .noInputTimeoutEnabled(true)
      .noInputTimeoutMilliseconds(5000)
      .startInputTimers(true)
      .build();

  std::unique_ptr<LanguageModelList> lm;

  std::shared_ptr<AudioSource> audio;
  std::string url(argv[1]);
  std::string username("");
  std::string password("");
  if(argc == 5){
    username = std::string(argv[3]);
    password = std::string(argv[4]);
  }

  char input = 'r';
  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(url)
      .recogConfig(std::move(config))
      .credentials(username, password)
      .connectOnRecognize(false)
      .autoClose(false)
      .build();
  while(1) {

    audio = std::make_shared<MicAudioSource>();
    lm = LanguageModelList::Builder().addFromURI(argv[2]).build();

    std::cout << "Press R to recognize, or anything else to exit" << std::endl;
    std::cin >> input;
    if(!((input=='r' || input == 'R'))){
      break;
    }

    asr->recognize(audio, std::move(lm));
    std::cout << "Recognition wait" << std::endl;
    std::vector<RecognitionResult> result = asr->waitRecognitionResult();
    std::cout << "Recognition end" << std::endl;

    for (RecognitionResult& res : result) {
      if (res.getCode() == RecognitionResult::Code::NO_MATCH) {
        std::cout << "NO_MATCH" << std::endl;
        continue;
      }

      int i = 0;
      for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
        std::cout << "Alternativa [" << ++i
                  << "] (score = " << alt.getConfidence()
                  << "): " << alt.getText() << std::endl;
        int j = 0;
        for (Interpretation& interpretation : alt.getInterpretations()) {
          std::cout << "\t Interpretacao [" << ++j
                    << "]: " << interpretation.text_ << std::endl;
        }
      }
    }
  }


  return 0;
}
