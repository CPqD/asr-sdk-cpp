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

#include <cpqd/asr-client/file_audio_source.h>
#include <cpqd/asr-client/buffer_audio_source.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_exception.h>
#include <cpqd/asr-client/speech_recog.h>

#ifndef NO_INPUT_TIMEOUT_MS
#define NO_INPUT_TIMEOUT_MS 1000
#endif

/*
 * ok:       basicGrammar
 * ok:       basicGrammarClearVoice
 * ok:       recognizeNoSpeech (NO_MATCH)
 * ok:       recognizeNoInput
 * ok:       recognizeBufferAudioSource
 * ok:       recognizeBufferBlockRead
 * ok:       recognizeMaxWaitSeconds
 * ok:       closeWhileRecognize
 * ok:       closeWithoutRecognize
 * ok:       cancelWhileRecognize
 * ok:       cancelNoRecognize
 * ok:       waitRecognitionResult
 * ok:       waitRecognitionResultDuplicateTest
 * ok:       duplicateRecognize
 * ok:       multipleRecognize
 * ok-ish:   connectOnRecognize (doesn't test connection per se, only the object status)
 * ok-ish:   multipleAutoClose  (doesn't test connection per se, only the object status)
 * ok:       sessionTimeout
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


/*Class for simulating noInputTimeout*/
class DelayedFileAudioSource: public FileAudioSource {
public:
  DelayedFileAudioSource(const std::string& file_name,
                         AudioFormat fmt = AudioFormat())
    : FileAudioSource(file_name, fmt)
  {}
  int read(std::vector<char>& buffer) {
    std::this_thread::sleep_for(
          std::chrono::milliseconds(NO_INPUT_TIMEOUT_MS + 10)
          );
    return FileAudioSource::read(buffer);
  }
};


TEST(RecognizerTest, basicGrammar) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_phone_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::grammar_phone_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_LT(0, result.size());
  for (RecognitionResult& res : result) {
    ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());

    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      std::cout << "alt.getConfidence(): " << alt.getConfidence() << std::endl;
      ASSERT_GE(alt.getConfidence(), 90);
    }
  }
}

TEST(NoGrammarTest, basicGrammarClearVoice) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_GT(result.size(), 0);

  for (RecognitionResult& res : result) {
    ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());

    for (RecognitionResult::Alternative& alt : res.getAlternatives()) {
      ASSERT_GE(alt.getConfidence(), 90);
    }
  }
}

TEST(NoGrammarTest, recognizeNoSpeech) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_silence_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->recognize(audio, std::move(lm));

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  EXPECT_EQ(result[0].getCode(), RecognitionResult::Code::NO_SPEECH);
  asr->close();
}

TEST(RecognizerTest, recognizeNoInput) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder()
      .noInputTimeoutEnabled(true)
      .noInputTimeoutMilliseconds(NO_INPUT_TIMEOUT_MS)
      .startInputTimers(true)
      .build();

  // Making a buffer with delayed read to simulate no input timeout
  std::shared_ptr<DelayedFileAudioSource> audio =
      std::make_shared<DelayedFileAudioSource>(test::audio_silence_8k);

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::grammar_phone_uri).build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials(test::username,
                   test::password)
      .maxWaitSeconds(30)
      .build();
  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> res = asr->waitRecognitionResult();
  EXPECT_EQ(res[0].getCode(), RecognitionResult::Code::NO_INPUT_TIMEOUT);
  asr->close();
}

TEST(RecognizerTest, recognizeBufferAudioSource) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder()
      .noInputTimeoutEnabled(false)
      .build();

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials(test::username,
                   test::password)
      .maxWaitSeconds(30)
      .build();
  static long buffer_size = 200000; // Longer than audio_phone_8k
  std::ifstream ifs;
  ifs.open(test::audio_phone_8k_raw);

  // Reading file content
  std::vector<char> audio_content;
  if (!ifs.eof() && !ifs.fail())
  {
    ifs.seekg(0, std::ios_base::end);
    std::streampos fileSize = ifs.tellg();
    audio_content.resize(fileSize);
    ifs.seekg(0, std::ios_base::beg);
    ifs.read(&audio_content[0], fileSize);
  }

  // Assert that file content is smaller than buffer size
  ASSERT_LT(audio_content.size(), buffer_size);

  // Writing content smaller than buffer_size
  AudioFormat fmt;
  fmt.fileFormat = AudioFileFormat::RAW;
  fmt.bits_per_sample_ = 16;
  std::shared_ptr<BufferAudioSource> audio =
      std::make_shared<BufferAudioSource>(fmt, buffer_size);

  asr->recognize(audio, std::move(lm));
  audio->write(audio_content);
  audio->finish();

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();
  // Assert that there's at least one valid result
  ASSERT_GT(result.size(), 0);
}

TEST(RecognizerTest, recognizeBufferBlockRead) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder()
      .noInputTimeoutEnabled(false)
      .build();

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials(test::username,
                   test::password)
      .maxWaitSeconds(30)
      .build();
  char buffer[4000]; // Shorter than previsao-tempo-8k.raw

  // Audio instance
  AudioFormat fmt;
  fmt.fileFormat = AudioFileFormat::RAW;
  fmt.bits_per_sample_ = 16;
  std::shared_ptr<BufferAudioSource> audio =
      std::make_shared<BufferAudioSource>(fmt, 4000);
  asr->recognize(audio, std::move(lm));

  // Writing and blocking audio. For each 2000 samples (250ms), wait 300ms
  std::ifstream ifs;
  ifs.open(test::previsao_tempo_8k_raw);
  ifs.read(buffer, 4000);
  do {
    audio->write(buffer, 4000);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    ifs.read(buffer, 4000);
  } while (ifs.gcount() != 0);
  audio->finish();

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  // Assert that there's at least one valid result
  ASSERT_GT(result.size(), 0);
}

TEST(RecognizerTest, recognizeMaxWaitSeconds) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder().build();

  std::shared_ptr<AudioSource> audio =
      std::make_shared<DelayedFileAudioSource>(test::previsao_tempo_8k);

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials(test::username,
                   test::password)
      .maxWaitSeconds(1)
      .build();
  asr->recognize(audio, std::move(lm));
  try {
    asr->waitRecognitionResult();
    FAIL() << "Expected RecognitionException";
  } catch (RecognitionException& err) {
    EXPECT_EQ(err.getCode(), RecognitionError::Code::FAILURE);
  } catch(...) {
    FAIL() << "Expected RecognitionException";
  }
}


TEST(RecognizerTest, closeWhileRecognize) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  asr->recognize(audio, std::move(lm));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  asr->close();

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();

  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest, closeWithoutRecognize) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->close();

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();

  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest,cancelNoRecognize) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->cancelRecognition();
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest,cancelWhileRecognize) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->recognize(audio, std::move(lm));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  asr->cancelRecognition();
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest, waitRecognitionResultNoRecognizerTest) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();
  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest, waitRecognitionResultDuplicateTest) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::slm_uri).build();
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();

  ASSERT_GT(result.size(), 0);
  result = asr->waitRecognitionResult();
  ASSERT_EQ(result.size(), 0);
}

TEST(RecognizerTest, duplicateRecognize) {
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_phone_8k);

  auto lm = LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  asr->recognize(audio, std::move(lm));

  std::shared_ptr<AudioSource> audio2 =
      std::make_shared<FileAudioSource>(test::previsao_tempo_8k);

  auto lm2 = LanguageModelList::Builder().addFromURI(test::slm_uri).build();

  std::this_thread::sleep_for(std::chrono::milliseconds(120));

  ASSERT_THROW(asr->recognize(audio2, std::move(lm2)), RecognitionException);

  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  ASSERT_GT(result.size(), 0);
}

TEST(RecognizerTest, multipleRecognize) {
  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

  for (auto i = 0; i < 3; ++i) {
    std::unique_ptr<LanguageModelList> lm =
        LanguageModelList::Builder()
        .addFromURI(test::grammar_phone_uri)
        .build();
    std::shared_ptr<AudioSource> audio =
        std::make_shared<FileAudioSource>(test::audio_phone_8k);
    asr->recognize(audio, std::move(lm));
    std::vector<RecognitionResult> result = asr->waitRecognitionResult();

    for (RecognitionResult& res : result) {
      ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());
    }
  }
}

TEST(RecognizerTest, multipleConnectOnRecognize) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder().build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials("asrdev", "inova")
      .maxWaitSeconds(30)
      .connectOnRecognize(true)
      .build();
  ASSERT_EQ(false, asr->isOpen()) <<
                                     "Connection should stay closed until 1st recognition!";

  int wait_secs[] = {2, 2, 6};
  for (auto i = 0; i < 3; ++i) {
    std::unique_ptr<LanguageModelList> lm =
        LanguageModelList::Builder()
        .addFromURI(test::grammar_phone_uri)
        .build();
    std::shared_ptr<AudioSource> audio =
        std::make_shared<FileAudioSource>(test::audio_phone_8k);
    asr->recognize(audio, std::move(lm));
    std::vector<RecognitionResult> result = asr->waitRecognitionResult();

    ASSERT_EQ(true, asr->isOpen()) <<
                                      "Connection should stay open after 1st recognition!";;
    for (RecognitionResult& res : result) {
      ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());
    }

    std::this_thread::sleep_for(std::chrono::seconds(wait_secs[i]));
  }
}


TEST(RecognizerTest, multipleAutoClose) {
  std::unique_ptr<RecognitionConfig> config =
      RecognitionConfig::Builder().build();

  std::unique_ptr<SpeechRecognizer> asr = SpeechRecognizer::Builder()
      .serverUrl(test::server_url)
      .recogConfig(std::move(config))
      .credentials("asrdev", "inova")
      .maxWaitSeconds(30)
      .connectOnRecognize(true)
      .autoClose(true)
      .build();
  ASSERT_EQ(false, asr->isOpen()) <<
                                     "Connection should stay closed until recognition!";

  int wait_secs[] = {2, 2, 6};
  for (auto i = 0; i < 3; ++i) {
    std::unique_ptr<LanguageModelList> lm =
        LanguageModelList::Builder()
        .addFromURI(test::grammar_phone_uri)
        .build();
    std::shared_ptr<AudioSource> audio =
        std::make_shared<FileAudioSource>(test::audio_phone_8k);
    asr->recognize(audio, std::move(lm));
    std::vector<RecognitionResult> result = asr->waitRecognitionResult();

    ASSERT_EQ(false, asr->isOpen()) <<
                                       "Connection should close after every recognition!";;
    for (RecognitionResult& res : result) {
      ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());
    }

    std::this_thread::sleep_for(std::chrono::seconds(wait_secs[i]));
  }
}


TEST(RecognizerTest, sessionTimeout) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_phone_8k);

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder().addFromURI(test::grammar_phone_uri).build();

  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->recognize(audio, std::move(lm));
  std::this_thread::sleep_for(std::chrono::seconds(65));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_LT(0, result.size());
  for (RecognitionResult& res : result) {
    ASSERT_EQ(RecognitionResult::Code::RECOGNIZED, res.getCode());
  }
}

TEST(GrammarInline, Recog) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_yes_16k);

  std::string str_gram =  "#ABNF 1.0 UTF-8;\n"
                          "language pt-BR;\n"
                          "tag-format <semantics/1.0>;\n"
                          "mode voice;\n"
                          "root $root;\n"
                          "$root = sim | não;\n";

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder()
      .addInlineGrammar(str_gram)
      .build();

  std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
  asr->recognize(audio, std::move(lm));
  std::vector<RecognitionResult> result = asr->waitRecognitionResult();
  asr->close();

  ASSERT_GT(result.size(), 0);
}

TEST(GrammarInline, WrongGrammar) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_yes_16k);

  std::string str_gram =  "#ABNF 1.0 UTF-8;\n"
                          "language pt-BR;\n"
                          "tag-format <semantics/1.0>;\n"
                          "mode voice;\n"
                          "root $rodfot;\n"
                          "$root = sim | não;\n";

  std::unique_ptr<LanguageModelList> lm =
      LanguageModelList::Builder()
      .addInlineGrammar(str_gram)
      .build();

  try {
    std::unique_ptr<SpeechRecognizer> asr = defaultBuild();
    asr->recognize(audio, std::move(lm));

    std::vector<RecognitionResult> result = asr->waitRecognitionResult();
    asr->close();
  } catch (RecognitionException& e) {
  } catch (...) {
    FAIL() << "Expected error on start recognition";
  }
}

TEST(GrammarInline, TwoGrammars) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_yes_16k);
  std::unique_ptr<LanguageModelList> lm;

  try {
  std::string str_gram =  "#ABNF 1.0 UTF-8;\n"
                          "language pt-BR;\n"
                          "tag-format <semantics/1.0>;\n"
                          "mode voice;\n"
                          "root $root;\n"
                          "$root = sim | não;\n";

   lm = LanguageModelList::Builder()
        .addInlineGrammar(str_gram)
        .addInlineGrammar(str_gram)
        .build();
  } catch (RecognitionException& e) {
    return;
  } catch (...) {
    FAIL() << "Expected error on LanguageModelList";
  }
}

TEST(GrammarInline, InlineAndUri) {
  std::shared_ptr<AudioSource> audio =
      std::make_shared<FileAudioSource>(test::audio_yes_16k);
  std::unique_ptr<LanguageModelList> lm;

  try {
  std::string str_gram =  "#ABNF 1.0 UTF-8;\n"
                          "language pt-BR;\n"
                          "tag-format <semantics/1.0>;\n"
                          "mode voice;\n"
                          "root $root;\n"
                          "$root = sim | não;\n";

   lm = LanguageModelList::Builder()
        .addInlineGrammar(str_gram)
        .addFromURI(test::grammar_phone_uri)
        .build();
  } catch (RecognitionException& e) {
    return;
  } catch (...) {
    FAIL() << "Expected error on LanguageModelList";
  }
}
