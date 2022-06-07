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
#include <string>

#include <cpqd/asr-client/file_audio_source.h>
#include <cpqd/asr-client/language_model_list.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_result.h>
#include <cpqd/asr-client/speech_recog.h>

int main(int argc, char* argv[]) {
  if(argc < 4 || argc > 7){
    std::cerr << "Usage: " << argv[0] << " <ws_url> <lang_uri> <wav_path> [ <user> <password> ]" << std::endl;
    std::cerr << "   eg: " << argv[0] << " ws://127.0.0.1:8025/asr-server/asr builtin:grammar/samples/phone audio/phone-1937050211-8k.wav" << std::endl;
    std::cerr << "  eg2: " << argv[0] << " wss://contact/cpqd/and/request/a/key/ builtin:slm/general /path/to/audio.wav myusername mypassword" << std::endl;
    std::cerr << "  eg3: " << argv[0] << " ws://127.0.0.1:8000/ws/v1/recognize/ builtin:slm/general ../../asr-sdk-python/audio/audio_8k_pizza_mensagem_calendario_cpqd.wav --classifiers" << std::endl;
    return -1;
  }

  bool classifiers = false;

  if ((argc == 5 || argc == 7) && (std::string(argv[argc - 1]) == "--classifiers")) {
      classifiers = true;
  }

  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder()
      .maxSentences(3)
      .confidenceThreshold(50)
      .noInputTimeoutEnabled(true)
      .noInputTimeoutMilliseconds(5000)
      .startInputTimers(true)
      .inferAgeEnabled(classifiers)
      .inferEmotionEnabled(classifiers)
      .inferGenderEnabled(classifiers)
      .build();

  std::unique_ptr<LanguageModelList> lm;

  std::shared_ptr<AudioSource> audio;
  std::string audio_file(argv[3]);
  std::string url(argv[1]);
  std::string username("");
  std::string password("");
  if(argc == 6){
    username = std::string(argv[4]);
    password = std::string(argv[5]);
  }

  char input = 'r';
  char enter;
  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(url)
      .recogConfig(std::move(config))
      .credentials(username.c_str(), password.c_str())
      .connectOnRecognize(false)
      .autoClose(false)
      .maxWaitSeconds(1000)
      .build();

  audio = std::make_shared<FileAudioSource>(audio_file);
  lm = LanguageModelList::Builder().addFromURI(argv[2]).build();

  asr->recognize(audio, std::move(lm));
  std::cout << "Recognition wait" << std::endl;
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  std::cout << "Recognition end" << std::endl;

  int j = 0;
  for (RecognitionResult& res : result) {
    if (res.getCode() == RecognitionResult::Code::NO_MATCH) {
      std::cout << "NO_MATCH" << std::endl;
      continue;
    }

    int i = 0;
    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      std::cout << "Res [" << j
                << "] Alt [" << ++i
                << "] (score = " << alt.getConfidence()
                << "): " << alt.getText() << std::endl;
      int j = 0;
      for (Interpretation& interpretation : alt.getInterpretations()) {
        std::cout << "\t Interpretacao [" << ++j
                  << "]: " << interpretation.text_ << std::endl;
      }
    }
    j++;
    if (res.getClassifiers().hasAge()) {
      std::cout << "Age Result: " << res.getClassifiers().getAge() << std::endl;
    }
    if (res.getClassifiers().hasEmotion()) {
      std::cout << "Emotion Result: " << res.getClassifiers().getEmotion() << std::endl;
    }
    if (res.getClassifiers().hasGender()) {
      std::cout << "Gender Result: " << res.getClassifiers().getGender() << std::endl;
    }
  }
  //   } catch (std::exception& e) {
  //     std::cout << e.what() << std::endl;
  //   } catch (...) {
  //     std::cout << "error" << std::endl;
  //   }

  return 0;
}
