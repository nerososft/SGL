//
// Created by neo on 25-6-24.
//

#include "Model.h"

#include <iostream>

#include "core/log/Log.h"

Model::Model(const std::shared_ptr<sgl::compute::ComputeEngine> &ce,
             const std::shared_ptr<Config> &config,
             const std::shared_ptr<SafeTensor> &safeTensor) {
  this->safeTensor = safeTensor;
  this->config = config;
  this->ce = ce;
}

std::shared_ptr<sgl::compute::Matrix>
Model::InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                        const Weight &weight) const {
  std::shared_ptr<sgl::compute::Matrix> weightMatrix =
      ce->CreateMatrix(weight.shape.width, weight.shape.height);
  const std::shared_ptr<VkGPUBuffer> matrixBuffer = weightMatrix->GetBuffer();
  if (matrixBuffer == nullptr) {
    Logger() << "matrixBuffer is null!";
    return nullptr;
  }
  const std::vector<float> weightData = safeTensor->GetLayerWeightData(weight);
  const VkResult result = matrixBuffer->UploadData(
      weightData.data(), weightData.size() * sizeof(float));
  if (result != VK_SUCCESS) {
    Logger() << "matrixBuffer->UploadData failed!";
    return nullptr;
  }
  return weightMatrix;
}

bool Model::Init() {
  const uint64_t layerNums = this->config->GetHiddenLayerNums();
  Logger(Logger::DEBUG) << "layerNums: " << layerNums << std::endl;
  this->outputsMatrix.resize(layerNums);

  const Weight normWeight = safeTensor->GetWeight("model.norm.weight");
  normMatrix = InitWeightMatrix(safeTensor, normWeight);
  assert(normMatrix != nullptr);

  const Weight lmHeadWeight = safeTensor->GetWeight("lm_head.weight");
  lmHeadMatrix = InitWeightMatrix(safeTensor, lmHeadWeight);
  assert(lmHeadMatrix != nullptr);

  result = ce->CreateMatrix(this->lmHeadMatrix->GetWidth(), 1);
  assert(result != nullptr);
  softmaxResult = ce->CreateMatrix(this->lmHeadMatrix->GetWidth(), 1);
  assert(softmaxResult != nullptr);

  this->blocks.resize(layerNums);

  biasMatrix = ce->CreateMatrix(32, 32);
  return true;
}

void Model::Dump() const {
  // blocks[1]->Dump();
  // for (auto &block: this->blocks) {
  // block->Dump();
  // }
}

std::vector<float>
Model::Forward(const std::vector<std::vector<float>> &inputs) {
  inputsMatrix.resize(inputs.size());
  for (size_t tokenPos = 0; tokenPos < inputs.size(); tokenPos++) {
    if (inputsMatrix[tokenPos] == nullptr) {
      const auto inputMatrix =
          ce->CreateMatrix(this->config->GetHiddenSize(), 1);
      if (inputMatrix == nullptr) {
        Logger() << "Failed to create input matrix" << std::endl;
        return {};
      }
      const auto inputBuffer = inputMatrix->GetBuffer();
      const VkResult result = inputBuffer->UploadData(
          inputs[tokenPos].data(), inputs[tokenPos].size() * sizeof(float));
      if (result != VK_SUCCESS) {
        Logger() << "Failed to upload input vector" << std::endl;
        return {};
      }

      this->inputsMatrix[tokenPos] = inputMatrix;
    }
  }

  for (uint64_t layerIdx = 0; layerIdx < this->config->GetHiddenLayerNums();
       layerIdx++) {
    this->outputsMatrix[layerIdx].resize(inputs.size());
    for (size_t tokenPos = 0; tokenPos < inputs.size(); tokenPos++) {
      if (this->outputsMatrix[layerIdx][tokenPos] == nullptr) {
        const auto outputMatrix =
            ce->CreateMatrix(this->config->GetHiddenSize(), 1);
        if (outputMatrix == nullptr) {
          Logger() << "Failed to create input matrix" << std::endl;
          return {};
        }
        this->outputsMatrix[layerIdx][tokenPos] = outputMatrix;
      }
    }
  }

  for (uint64_t layerIdx = 0; layerIdx < this->config->GetHiddenLayerNums();
       layerIdx++) {
    blocks[layerIdx] = std::make_shared<TransformerBlock>(ce, layerIdx);
    blocks[layerIdx]->SetOutputsMatrix(this->outputsMatrix[layerIdx]);
    if (layerIdx == 0) {
      blocks[layerIdx]->SetInputsMatrix(this->inputsMatrix);
    } else {
      blocks[layerIdx]->SetInputsMatrix(this->outputsMatrix[layerIdx - 1]);
    }
    blocks[layerIdx]->Init(safeTensor, config);
  }

  for (uint64_t layerIdx = 0; layerIdx < this->config->GetHiddenLayerNums();
       layerIdx++) {
    for (size_t tokenPos = 0; tokenPos < inputs.size(); tokenPos++) {
      blocks[layerIdx]->MultiHead(tokenPos);
    }
    blocks[layerIdx]->Attention();
    for (size_t tokenPos = 0; tokenPos < inputs.size(); tokenPos++) {
      blocks[layerIdx]->MLP(tokenPos);
    }
  }

  const auto output = blocks[this->config->GetHiddenLayerNums() - 1]
                          ->GetOutputsMatrix()[inputs.size() - 1];
  ce->Seq()
      ->Record(ce->MatMul(output, lmHeadMatrix, result))
      ->Record(ce->LogSoftmax(result, softmaxResult))
      ->Eval()
      ->Destroy();
  result->Destroy();

  std::vector<float> re(this->lmHeadMatrix->GetWidth());
  softmaxResult->GetBuffer()->DownloadData(re.data(),
                                           re.size() * sizeof(float));
  std::ranges::sort(re, std::greater<float>());
  for (size_t i = 0; i < 100; i++) {
    std::cout << re[i] << ", ";
  }

  return {};
}
