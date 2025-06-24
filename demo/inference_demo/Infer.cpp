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

    const uint64_t decodeTimeStart = TimeUtils::GetCurrentMonoMs();
    const auto result = tokenizer->Encode("hello world");
    const uint64_t decodeTimeEnd = TimeUtils::GetCurrentMonoMs();
    std::cout << "Decode time: " << decodeTimeEnd - decodeTimeStart << std::endl;

    for (const auto &token: result) {
        std::cout << token << " ";
    }
    std::cout << std::endl;

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

    return;
}
