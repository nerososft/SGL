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

std::shared_ptr<Matrix> Model::InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                                                const Weight &weight) const {
    std::shared_ptr<Matrix> weightMatrix = mle->CreateMatrix(weight.shape.width, weight.shape.height);
    const std::shared_ptr<VkGPUBuffer> matrixBuffer = weightMatrix->GetBuffer();
    if (matrixBuffer == nullptr) {
        Logger() << "matrixBuffer is null!";
        return nullptr;
    }
    const std::vector<float> weightData = safeTensor->GetLayerWeightData(weight);
    const VkResult result = matrixBuffer->UploadData(weightData.data(),
                                                     weightData.size() * sizeof(float));
    if (result != VK_SUCCESS) {
        Logger() << "matrixBuffer->UploadData failed!";
        return nullptr;
    }
    return weightMatrix;
}

bool Model::Init() {
    const uint64_t layerNums = this->config->GetHiddenLayerNums();
    Logger(Logger::DEBUG) << "layerNums: " << layerNums << std::endl;

    this->inputMatrix = mle->CreateMatrix(1024, 1); // FIXME: input shape should read from some config
    if (this->inputMatrix == nullptr) {
        Logger() << "Failed to create input matrix" << std::endl;
        return false;
    }

    const Weight normWeight = safeTensor->GetWeight("model.norm.weight");
    normMatrix = InitWeightMatrix(safeTensor, normWeight);
    assert(normMatrix != nullptr);

    this->outputMatrix = mle->CreateMatrix(1024, 1); // FIXME: input shape should read from some config
    if (this->outputMatrix == nullptr) {
        Logger() << "Failed to create output matrix" << std::endl;
        return false;
    }

    this->blocks.resize(layerNums);
    for (uint64_t i = 0; i < layerNums; i++) {
        blocks[i] = std::make_shared<TransformerBlock>(mle, i);
        if (i == 0) {
            blocks[i]->SetInputMatrix(this->inputMatrix);
        } else {
            blocks[i]->SetInputMatrix(blocks[i - 1]->GetOutputMatrix());
        }
        blocks[i]->Init(safeTensor, config);
    }

    const auto placeholderMatrix = mle->CreateMatrix(32, 32);
    mle->LayerNorm(blocks[layerNums - 1]->GetOutputMatrix(),
                   this->normMatrix,
                   placeholderMatrix,
                   1e-06,
                   true,
                   false,
                   this->outputMatrix);

    return true;
}

void Model::Dump() const {
    for (auto &block: this->blocks) {
        block->Dump();
    }
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
