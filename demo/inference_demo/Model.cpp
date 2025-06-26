//
// Created by neo on 25-6-24.
//

#include "Model.h"

#include <iostream>

#include "core/log/Log.h"

Model::Model() {
}

bool Model::Init(const std::shared_ptr<MLEngine> &mle,
                 const std::shared_ptr<Config> &config,
                 const std::shared_ptr<SafeTensor> &safeTensor) {
    this->safeTensor = safeTensor;
    this->config = config;

    const uint64_t layerNums = this->config->GetHiddenLayerNums();
    Logger(Logger::DEBUG) << "layerNums: " << layerNums << std::endl;

    this->blocks.resize(layerNums);
    for (uint64_t i = 0; i < layerNums; i++) {
        blocks[i] = std::make_shared<TransformerBlock>(mle, i);
        blocks[i]->Init(safeTensor);
    }

    return true;
}

std::vector<float> Model::Forward(const std::vector<float> &input) {
    // TODO: do compute
    return {};
}
