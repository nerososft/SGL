//
// Created by neo on 25-6-24.
//

#include "TransformerBlock.h"

#include "core/log/Log.h"

TransformerBlock::TransformerBlock(const std::shared_ptr<MLEngine> &mle,
                                   const uint64_t layerIdx) {
    this->mle = mle;
    this->layerIndex = layerIdx;
}

std::shared_ptr<Matrix> TransformerBlock::InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
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

std::shared_ptr<Matrix> TransformerBlock::InitMatrix(const Weight &weight) const {
    return mle->CreateMatrix(weight.shape.width, weight.shape.height);
}

bool TransformerBlock::Init(const std::shared_ptr<SafeTensor> &safeTensor,
                            const std::shared_ptr<Config> &config) {
    Weight inputLayerNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "input_layernorm");
    Weight selfAttnKNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.k_norm");
    Weight selfAttnKProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.k_proj");
    Weight selfAttnOProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.o_proj");
    Weight selfAttnQNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.q_norm");
    Weight selfAttnQProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.q_proj");
    Weight selfAttnVProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.v_proj");
    Weight postAttentionLayerNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "post_attention_layernorm");
    Weight mlpUpProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.up_proj");
    Weight mlpGateProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.gate_proj");
    Weight mlpDownProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.down_proj");

    selfAttnKNorm = InitWeightMatrix(safeTensor, selfAttnKNormWeight);
    assert(selfAttnKNorm != nullptr);
    selfAttnKProj = InitWeightMatrix(safeTensor, selfAttnKProjWeight);
    assert(selfAttnKProj != nullptr);
    selfAttnOProj = InitWeightMatrix(safeTensor, selfAttnOProjWeight);
    assert(selfAttnOProj != nullptr);
    selfAttnQNorm = InitWeightMatrix(safeTensor, selfAttnQNormWeight);
    assert(selfAttnQNorm != nullptr);
    selfAttnQProj = InitWeightMatrix(safeTensor, selfAttnQProjWeight);
    assert(selfAttnQProj != nullptr);
    selfAttnVProj = InitWeightMatrix(safeTensor, selfAttnVProjWeight);
    assert(selfAttnVProj != nullptr);
    postAttentionLayerNorm = InitWeightMatrix(safeTensor, postAttentionLayerNormWeight);
    assert(postAttentionLayerNorm != nullptr);
    mlpUpProj = InitWeightMatrix(safeTensor, mlpUpProjWeight);
    assert(mlpUpProj != nullptr);
    mlpGateProj = InitWeightMatrix(safeTensor, mlpGateProjWeight);
    assert(mlpGateProj != nullptr);
    mlpDownProj = InitWeightMatrix(safeTensor, mlpDownProjWeight);
    assert(mlpDownProj != nullptr);

    outputMatrix = InitMatrix(inputLayerNormWeight);
    assert(outputMatrix != nullptr);

    inputLayerNorm = InitWeightMatrix(safeTensor, inputLayerNormWeight);
    assert(inputLayerNorm != nullptr);

    inputLayerNormOutput = InitMatrix(inputLayerNormWeight);
    assert(inputLayerNorm != nullptr);
    auto placeholderMatrix = mle->CreateMatrix(32, 32);
    mle->LayerNorm(this->inputMatrix,
                   this->inputLayerNorm,
                   placeholderMatrix,
                   1e-06,
                   true,
                   false,
                   inputLayerNormOutput);

    qProjOutput = mle->CreateMatrix(selfAttnQProjWeight.shape.width, 1);
    mle->MatMul(inputLayerNormOutput, selfAttnQProj, qProjOutput);

    Logger() << "dimHead:" << config->GetHeadDim() << std::endl;

    qHeads.resize(config->GetHeadDim());
    for (int i = 0; i < config->GetHeadDim(); i++) {
        auto mat = mle->CreateMatrix(qProjOutput->GetWidth() / config->GetHeadDim(), 1);
        if (mat == nullptr) {
            Logger() << "failed to create qHead matrix" << std::endl;
            return false;
        }
        qHeads[i] = mat;
    }

    // do MultiHead split for Q
    mle->Split(qProjOutput, config->GetHeadDim(), qHeads);

    // KQV multi-head normalize

    // TODO: construct transformer block compute graph
    return true;
}

void TransformerBlock::SetInputMatrix(const std::shared_ptr<Matrix> &input) {
    this->inputMatrix = input;
}

std::shared_ptr<Matrix> &TransformerBlock::GetOutputMatrix() {
    return this->outputMatrix;
}

void TransformerBlock::Dump() const {
    Logger() << "inputMatrix: ";
    inputMatrix->Print();
    Logger() << "inputLayerNorm: ";
    inputLayerNorm->Print();
    Logger() << "inputLayerNormOutput: ";
    inputLayerNormOutput->Print();
    Logger() << "qProjOutput: ";
    qProjOutput->Print();
}
