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

#include <gtest/gtest.h>

#include "test_config.h"

#include <string>

#include <cpqd/asr-client/file_audio_source.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/speech_recog.h>
#include <cpqd/asr-client/recognition_listener.h>

class TestListener : public RecognitionListener {
public:
  TestListener() {}
  ~TestListener() {}

  void onListening() { std::cout << "listening..." << std::endl; }

  void onSpeechStart(int time) { std::cout << "speech start..." << std::endl; }

  void onSpeechStop(int time) { std::cout << "speech stop..." << std::endl; }

  void onPartialRecognition(PartialRecognition& partial){
    std::cout << partial.text_ << std::endl;
  }

  void onRecognitionResult(RecognitionResult& res) {
    if (res.getCode() == RecognitionResult::Code::NO_MATCH) {
      std::cout << "NO_MATCH" << std::endl;
      return;
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

  void onError(RecognitionError& error) {
    std::cout << "ERROR: " << error.getString() << std::endl;
  }
};

TEST(RecognizerBuildTest, urlNull) {
  std::string url = "";

  ASSERT_THROW(SpeechRecognizer::Builder().serverUrl(url).build(),
               std::logic_error);
}

TEST(RecognizerBuildTest, urlInvalid) {
  std::string url = "ws://xpto";

  ASSERT_THROW(SpeechRecognizer::Builder().serverUrl(url).build(),
               std::invalid_argument);
}

TEST(RecognizerBuildTest, addListener) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder().build();

  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  auto listener = std::unique_ptr<TestListener>(new TestListener);

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .addListener(std::move(listener))
      .credentials(test::username, test::password)
      .maxWaitSeconds(30)
      .build();
  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();

  ASSERT_GT(result.size(), 0);
}
