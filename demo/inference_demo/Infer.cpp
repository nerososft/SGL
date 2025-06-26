//
// Created by neo on 25-6-24.
//

#include "Infer.h"

#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"

bool Infer::Init() {
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

    this->model = std::make_shared<Model>();
    ok = model->Init(this->mle, this->config, this->safeTensor);
    if (!ok) {
        Logger() << "Failed to init model";
        return false;
    }
    return true;
}

void Infer::Run(const std::string &prompt) const {
    const uint64_t decodeTimeStart = TimeUtils::GetCurrentMonoMs();
    const auto result = tokenizer->Encode(prompt);
    const uint64_t decodeTimeEnd = TimeUtils::GetCurrentMonoMs();
    Logger(Logger::DEBUG) << "Decode time: " << decodeTimeEnd - decodeTimeStart << std::endl;

    for (const auto &token: result) {
        std::vector<float> embedding = this->safeTensor->EmbeddingToken(token);
        Logger(Logger::DEBUG) << "Token: " << token << std::endl;
        Logger(Logger::DEBUG) << "Embedding(" << embedding.size() << "): [";
        for (int i = 0; i < embedding.size(); i++) {
            std::cout << embedding[i] << " ";
        }
        std::cout << "]" << std::endl;

        std::vector<float> output = model->Forward(embedding);
        Logger(Logger::DEBUG) << "Output(" << output.size() << "): [";
        for (int i = 0; i < output.size(); i++) {
            std::cout << output[i] << " ";
        }
        std::cout << "]" << std::endl << std::endl;
    }

    // TODO: let's transform

    Logger(Logger::DEBUG) << std::endl;
}
