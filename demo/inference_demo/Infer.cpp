//
// Created by neo on 25-6-24.
//

#include "Infer.h"

#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"

bool Infer::Init() {
    Logger(Logger::DEBUG) << "Infer Init......" << std::endl;
    const uint64_t initTimeStart = TimeUtils::GetCurrentMonoMs();
    mle = std::make_shared<MLEngine>();
    if (!mle->Init()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return false;
    }

    this->tokenizer = std::make_shared<Tokenizer>();
    bool ok = tokenizer->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/tokenizer.json");
    if (!ok) {
        Logger() << "Failed to load tokenizer.json";
        return false;
    }

    this->config = std::make_shared<Config>();
    ok = config->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/config.json");
    if (!ok) {
        Logger() << "Failed to load config.json";
        return false;
    }

    this->safeTensor = std::make_shared<SafeTensor>(this->config);
    ok = safeTensor->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/model.safetensors");
    if (!ok) {
        Logger() << "Failed to load safetensors";
        return false;
    }

    this->model = std::make_shared<Model>(this->mle, this->config, this->safeTensor);
    ok = model->Init();
    if (!ok) {
        Logger() << "Failed to init model";
        return false;
    }
    const uint64_t initTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger(Logger::DEBUG) << "Infer Init done! time: " << initTimeEnd - initTimeStart << std::endl << std::endl;
    return true;
}

void Infer::Run(const std::string &prompt) const {
    const uint64_t decodeTimeStart = TimeUtils::GetCurrentMonoMs();
    const auto result = tokenizer->Encode(prompt);
    const uint64_t decodeTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Decode time: " << decodeTimeEnd - decodeTimeStart << std::endl;

    const uint64_t inferTimeStart = TimeUtils::GetCurrentMonoMs();
    for (size_t pos = 0; pos < result.size(); ++pos) {
        const std::vector<float> embedding = this->safeTensor->EmbeddingToken(result[pos]);
        Logger() << "Token: " << result[pos] << std::endl;
        Logger() << "Embedding(" << embedding.size() << "): [";
        for (const float e: embedding) {
            std::cout << e << " ";
        }
        std::cout << "]" << std::endl;

        // Let's transform
        const std::vector<float> output = model->Forward(embedding, result[pos], pos);

        Logger() << "Output(" << output.size() << "): [";
        for (const float o: output) {
            std::cout << o << " ";
        }
        std::cout << "]" << std::endl;
    }
    const uint64_t inferTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Infer time: " << inferTimeEnd - inferTimeStart << std::endl << std::endl;

    this->model->Dump();
}
