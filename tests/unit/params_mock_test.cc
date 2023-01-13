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

//#include <env.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <string>
#include <thread>

#include <cpqd/asr-client/buffer_audio_source.h>
#include <cpqd/asr-client/file_audio_source.h>
#include <cpqd/asr-client/recognition_config.h>
#include <cpqd/asr-client/recognition_exception.h>
#include <cpqd/asr-client/speech_recog.h>

#ifndef NO_INPUT_TIMEOUT_MS
#define NO_INPUT_TIMEOUT_MS 1000
#endif

constexpr char kChannelIdentifier[] = "Channel-Identifier";

class RecognizerMockTest : public testing::Test, public testing::WithParamInterface<std::string> {
   public:
    void SetUp() {
        std::cout << "Starting server\n";
        url_ = GetParam();
        if (url_.find("wss") != std::string::npos) {
            setenv("TLS_ON", "1", true);
            setenv("WS_PORT", "9003", true);
        }
        auto ret = std::system("cd ../ws_mock && ../build/ws_mock/cpqd-ws-mock > /dev/null &");
        ASSERT_EQ(0, ret);
        sleep(2);
    }

    void TearDown() {
        std::cout << "Stoping server\n";
        sleep(1);
        auto ret = std::system("pkill cpqd-ws-mock");
        ASSERT_EQ(0, ret);
        unsetenv("TLS_ON");
        unsetenv("WS_PORT");
    }

    static bool VerifyMessage(const std::string path, const std::string& message) {
        // std::cout << message << '\n';
        if (message.empty()) return true;
        std::ifstream input_file(path);
        std::istringstream msg(message);
        std::string file_line, msg_line;
        if (!input_file.is_open()) return false;

        while (std::getline(input_file, file_line)) {
            // std::cout << "ACTUAL: " << file_line << '\n';
            std::getline(msg, msg_line);
            // std::cout << "EXPCTD: " << msg_line << '\n';
            if (msg_line.find(kChannelIdentifier) != std::string::npos) {
                msg_line = msg_line.substr(0, sizeof(kChannelIdentifier));
                file_line = file_line.substr(0, sizeof(kChannelIdentifier));
            }
            EXPECT_EQ(msg_line, file_line);
        }
        input_file.close();
        return true;
    }

    /* Most common recognizer configuration */
    std::unique_ptr<SpeechRecognizer> defaultBuild(bool continuous = false) {
        std::unique_ptr<RecognitionConfig> config = RecognitionConfig::Builder()
                                                        //.maxSentences(3)
                                                        .continuousMode(continuous)
                                                        .maxSegmentDuration(3750)
                                                        .build();
        config->getConfigFromEvironment();
        return SpeechRecognizer::Builder()
            .serverUrl(url_)
            .recogConfig(std::move(config))
            .credentials(test::username, test::password)
            .maxWaitSeconds(30)
            .build();
    }

   private:
    std::string url_ = "";
};

TEST_P(RecognizerMockTest, params) {
    setenv("ACCOUNT_TAG", "other-tag", true);
    setenv("CHANNEL_IDENTIFIER", "my-channel-1", true);
    setenv("MEDIA_TYPE", "audio/raw", true);
    setenv("CONFIDENCE_THRESHOLD", "30", true);
    setenv("MAX_SENTENCES", "3", true);
    setenv("RECOG_TIMEOUT_SECONDS", "30", true);
    setenv("RECOG_TIMEOUT_ENABLED", "true", true);
    setenv("RECOG_TIMEOUT_ENABLED", "true", true);
    setenv("NO_INPUT_TIMEOUT_MILLISECONDS", "2000", true);
    setenv("INFER_AGE_ENABLED", "true", true);
    setenv("INFER_EMOTION_ENABLED", "true", true);
    setenv("INFER_GENDER_ENABLED", "true", true);
    setenv("HEAD_MARGIN_MILLISECONDS", "2000", true);
    setenv("TAIL_MARGIN_MILLISECONDS", "2000", true);
    setenv("WAIT_END_MILLISECONDS", "2000", true);
    setenv("MAX_SEGMENT_DURATION", "5000", true);
    setenv("CONTINUOUS_MODE", "true", true);
    setenv("START_INPUT_TIMERS", "true", true);
    setenv("VERIFY_BUFFER_UTTERANCE", "true", true);

    std::shared_ptr<AudioSource> audio = std::make_shared<FileAudioSource>(test::previsao_tempo_8k);
    std::unique_ptr<LanguageModelList> lm =
        LanguageModelList::Builder().addFromURI(test::slm_uri).build();
    std::unique_ptr<SpeechRecognizer> asr = defaultBuild();

    asr->recognize(audio, std::move(lm));
    std::vector<RecognitionResult> result = asr->waitRecognitionResult();
    asr->close();

    ASSERT_LT(0, result.size());
    bool at_least_one_high_confidence = false;
    for (RecognitionResult& res : result) {
        if (res.getCode() != RecognitionResult::Code::RECOGNIZED) continue;
        at_least_one_high_confidence = true;
    }

    ASSERT_EQ(true, at_least_one_high_confidence) << "No results with high confidence!";

    std::string messages =
        "ASR 2.4 CREATE_SESSION\r\n"
        "User-Agent:ASR Client\r\n\r\n"
        "ASR 2.4 SET_PARAMETERS\r\n"
        "Channel-Identifier:my-channel-1\r\n"
        "Infer-age-enabled:true\r\n"
        "Infer-emotion-enabled:true\r\n"
        "Infer-gender-enabled:true\r\n"
        "Media-Type:audio/raw\r\n"
        "Ver-Buffer-Utterance:true\r\n"
        "decoder.confidenceThreshold:30\r\n"
        "decoder.continuousMode:true\r\n"
        "decoder.maxSentences:3\r\n"
        "decoder.startInputTimers:1\r\n"
        "endpointer.headMargin:2000\r\n"
        "endpointer.maxSegmentDuration:5000\r\n"
        "endpointer.tailMargin:2000\r\n"
        "endpointer.waitEnd:2000\r\n"
        "licenseManager.accountTag:other-tag\r\n"
        "recognitionTimeout.enabled:1\r\n"
        "recognitionTimeout.value:30\r\n\r\n"
        "ASR 2.4 START_RECOGNITION\r\n"
        "Accept:application/json\r\n"
        "Content-Length:19\r\n"
        "Content-Type:text/uri-list\r\n\r\n"
        "builtin:slm/general\r\n";
    EXPECT_TRUE(VerifyMessage("/tmp/message", messages));
}

INSTANTIATE_TEST_CASE_P(, RecognizerMockTest, testing::Values(test::mock_url, test::tls_mock_url));