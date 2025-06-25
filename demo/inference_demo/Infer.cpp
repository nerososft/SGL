//
// Created by neo on 25-6-24.
//

#include "Infer.h"

#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"

void Infer::Init() {
    this->tokenizer = std::make_shared<Tokenizer>();
    bool ok = tokenizer->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/tokenizer.json");
    if (!ok) {
        Logger() << "Failed to load tokenizer.json";
        return;
    }

    this->config = std::make_shared<Config>();
    ok = config->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/config.json");
    if (!ok) {
        Logger() << "Failed to load config.json";
        return;
    }

    this->safeTensor = std::make_shared<SafeTensor>();
    ok = safeTensor->LoadFromFile("../../../demo/inference_demo/models/Qwen3_0_6B/model.safetensors");
    if (!ok) {
        Logger() << "Failed to load safetensors";
        return;
    }

    this->model = std::make_shared<Model>();
    ok = model->Init(this->config, this->safeTensor);
    if (!ok) {
        Logger() << "Failed to init model";
        return;
    }
}

void Infer::Run(const std::string &prompt) const {
    const uint64_t decodeTimeStart = TimeUtils::GetCurrentMonoMs();
    const auto result = tokenizer->Encode(prompt);
    const uint64_t decodeTimeEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Decode time: " << decodeTimeEnd - decodeTimeStart << std::endl;

    for (const auto &token: result) {
        std::vector<float> embedding = this->safeTensor->EmbeddingToken(token);
        std::cout << "Token: " << token << std::endl;
        std::cout << "Embedding(" << embedding.size() << "): [";
        for (int i = 0; i < embedding.size(); i++) {
            std::cout << embedding[i] << " ";
        }
        std::cout << "]" << std::endl;
    }

    std::cout << std::endl;
}
