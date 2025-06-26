//
// Created by neo on 25-6-24.
//

#include "Model.h"

#include <iostream>

#include "core/log/Log.h"

Model::Model(const std::shared_ptr<MLEngine> &mle,
             const std::shared_ptr<Config> &config,
             const std::shared_ptr<SafeTensor> &safeTensor) {
    this->safeTensor = safeTensor;
    this->config = config;
    this->mle = mle;
}

bool Model::Init() {
    const uint64_t layerNums = this->config->GetHiddenLayerNums();
    Logger(Logger::DEBUG) << "layerNums: " << layerNums << std::endl;

    this->inputMatrix = mle->CreateMatrix(1024, 1); // FIXME: input shape should read from some config
    if (this->inputMatrix == nullptr) {
        Logger() << "Failed to create input matrix" << std::endl;
        return false;
    }

    this->outputMatrix = mle->CreateMatrix(1024, 1); // FIXME: input shape should read from some config
    if (this->outputMatrix == nullptr) {
        Logger() << "Failed to create output matrix" << std::endl;
        return false;
    }

    this->blocks.resize(layerNums);
    for (uint64_t i = 0; i < layerNums; i++) {
        blocks[i] = std::make_shared<TransformerBlock>(mle, i);
        blocks[i]->Init(safeTensor);
        if (i == 0) {
            blocks[i]->SetInputMatrix(this->inputMatrix);
        } else {
            blocks[i]->SetInputMatrix(blocks[i - 1]->GetOutputMatrix());
        }
    }

    mle->LayerNorm(blocks[layerNums - 1]->GetOutputMatrix(), 0, 0, 0, this->outputMatrix);

    return true;
}

std::vector<float> Model::Forward(const std::vector<float> &input) const {
    const auto inputBuffer = this->inputMatrix->GetBuffer();
    const VkResult result = inputBuffer->UploadData(input.data(), input.size() * sizeof(float));
    if (result != VK_SUCCESS) {
        Logger() << "Failed to upload input vector" << std::endl;
        return {};
    }

    mle->Compute();

    std::vector<float> output;
    output.resize(1024); // TODO: read from config

    float *data = output.data();
    this->outputMatrix->GetBuffer()->DownloadData(data, 1024 * sizeof(float));
    for (size_t i = 0; i < 1024; i++) {
        output[i] = data[i];
    }

    return output;
}
