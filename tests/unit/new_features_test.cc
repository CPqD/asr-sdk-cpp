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

#include <chrono>
#include <string>
#include <thread>
#include <fstream>

#include <cpqd/asr-client/file_audio_source.h>
#include <cpqd/asr-client/buffer_audio_source.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_exception.h>
#include <cpqd/asr-client/speech_recog.h>

#ifndef NO_INPUT_TIMEOUT_MS
#define NO_INPUT_TIMEOUT_MS 1000
#endif

/*
 * These tests are only expected to pass on engine >= 3.0 and server >= 2.5
 *
 * ok:         wordsSlm
 * ok:         wordsGrammar
 * TODO:       multipleSegments
 */


/* Most common recognizer configuration */
std::unique_ptr<SpeechRecognizer> defaultBuild(){
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder().build();
  return SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials(test::username, test::password)
      .maxWaitSeconds(30)
      .build();
}


TEST(RecognizerTest, wordsSlm) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_LT(0, result.size());
  bool at_least_one_high_confidence = false;
  for (RecognitionResult& res : result) {
    if(res.getCode() != RecognitionResult::Code::RECOGNIZED) continue;
    at_least_one_high_confidence = true;

    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      ASSERT_GE(alt.getConfidence(), 90);
      auto words = alt.getWords();
      ASSERT_EQ(6, words.size());
      EXPECT_EQ("previsão", words[0].text_);
      EXPECT_EQ("do", words[1].text_);
      EXPECT_EQ("tempo", words[2].text_);
      EXPECT_EQ("em", words[3].text_);
      EXPECT_EQ("são", words[4].text_);
      EXPECT_EQ("paulo", words[5].text_);
      float last = 0;
      for(Word w : words){
          EXPECT_GE(w.confidence_, 90);
          EXPECT_GE(w.start_time_, last);
          EXPECT_GT(w.end_time_, w.start_time_);
          last = w.end_time_;
      }
      EXPECT_GT(last, 0);
    }
  }
  ASSERT_EQ(true, at_least_one_high_confidence) << "No results with high confidence!";
}


TEST(RecognizerTest, wordsGrammar) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_phone_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::grammar_phone_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_LT(0, result.size());
  bool at_least_one_high_confidence = false;
  for (RecognitionResult& res : result) {
    if(res.getCode() != RecognitionResult::Code::RECOGNIZED) continue;
    at_least_one_high_confidence = true;

    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      std::cout << "alt.getConfidence(): " << alt.getConfidence() << std::endl;
      ASSERT_GE(alt.getConfidence(), 90);
      auto words = alt.getWords();
      ASSERT_EQ(8, words.size());
      EXPECT_EQ("dezenove", words[0].text_);
      EXPECT_EQ("três", words[1].text_);
      EXPECT_EQ("sete", words[2].text_);
      EXPECT_EQ("zero", words[3].text_);
      EXPECT_EQ("cinco", words[4].text_);
      EXPECT_EQ("zero", words[5].text_);
      EXPECT_EQ("dois", words[6].text_);
      EXPECT_EQ("onze", words[7].text_);
      float last = 0;
      for(Word w : words){
          EXPECT_GE(w.confidence_, 90);
          EXPECT_GE(w.start_time_, last);
          EXPECT_GT(w.end_time_, w.start_time_);
          last = w.end_time_;
      }
      EXPECT_GT(last, 0);
    }
  }
  ASSERT_EQ(true, at_least_one_high_confidence) << "No results with high confidence!";
}

TEST(RecognizerTest, dynamicGrammarMulti) {
  std::ifstream fs_gram(test::grammar_pizza_path);
  std::stringstream buffer;
  buffer << fs_gram.rdbuf();
  std::string str_gram = buffer.str();
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addInlineGrammar(str_gram).build();
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_pizza_multi_8k);
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  
  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_LT(0, result.size());
  int num_res = 0;
  for (RecognitionResult& res : result) {
    if(res.getCode() != RecognitionResult::Code::RECOGNIZED){
      continue;
    }
    num_res++;

    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      std::cout << "alt.getConfidence(): " << alt.getConfidence() << std::endl;
      std::cout << alt.getText() << std::endl;
    }
  }
  ASSERT_EQ(3, num_res) << "Number of results (segments) should be 3!";
}

