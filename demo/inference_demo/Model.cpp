//
// Created by neo on 25-6-24.
//

#include "Model.h"

#include <iostream>

Model::Model() {
}

bool Model::Init(const std::shared_ptr<Config> &config, const std::shared_ptr<SafeTensor> &safeTensor) {
    this->safeTensor = safeTensor;
    this->config = config;

    const uint64_t layerNums = this->config->GetHiddenLayerNums();
    std::cout << "layerNums: " << layerNums << std::endl;

    // TODO: init transformer blocks

    return true;
}
