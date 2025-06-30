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
    biasMatrix = mle->CreateMatrix(32, 32);
    mle->LayerNorm(this->inputMatrix,
                   this->inputLayerNorm,
                   biasMatrix,
                   1e-06,
                   true,
                   false,
                   inputLayerNormOutput);

    qProjOutput = mle->CreateMatrix(selfAttnQProjWeight.shape.width, 1);
    assert(qProjOutput != nullptr);
    mle->MatMul(inputLayerNormOutput, selfAttnQProj, qProjOutput);

    kProjOutput = mle->CreateMatrix(selfAttnKProjWeight.shape.width, 1);
    assert(kProjOutput != nullptr);
    mle->MatMul(inputLayerNormOutput, selfAttnKProj, kProjOutput);

    vProjOutput = mle->CreateMatrix(selfAttnVProjWeight.shape.width, 1);
    assert(vProjOutput != nullptr);
    mle->MatMul(inputLayerNormOutput, selfAttnVProj, vProjOutput);

    // Q
    size_t queryHeadNums = qProjOutput->GetWidth() / config->GetHeadDim();
    qHeads.resize(queryHeadNums);
    qHeadLayerNormOutputs.resize(queryHeadNums);
    for (int i = 0; i < queryHeadNums; i++) {
        auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (mat == nullptr) {
            Logger() << "failed to create qHead matrix" << std::endl;
            return false;
        }
        qHeads[i] = mat;

        auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (matLayerNorm == nullptr) {
            Logger() << "failed to create qHead layerNorm matrix" << std::endl;
            return false;
        }
        qHeadLayerNormOutputs[i] = matLayerNorm;
    }
    mle->Split(qProjOutput, queryHeadNums, qHeads);
    for (int i = 0; i < queryHeadNums; i++) {
        mle->LayerNorm(qHeads[i], selfAttnQNorm, biasMatrix, 1e-06,
                       true,
                       false,
                       qHeadLayerNormOutputs[i]);
    }

    // K
    size_t keyHeadNums = kProjOutput->GetWidth() / config->GetHeadDim();
    kHeads.resize(keyHeadNums);
    kHeadLayerNormOutputs.resize(keyHeadNums);
    for (int i = 0; i < keyHeadNums; i++) {
        auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (mat == nullptr) {
            Logger() << "failed to create kHead matrix" << std::endl;
            return false;
        }
        kHeads[i] = mat;

        auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (matLayerNorm == nullptr) {
            Logger() << "failed to create kHead layerNorm matrix" << std::endl;
            return false;
        }
        kHeadLayerNormOutputs[i] = matLayerNorm;
    }
    mle->Split(kProjOutput, keyHeadNums, kHeads);
    for (int i = 0; i < keyHeadNums; i++) {
        mle->LayerNorm(kHeads[i], selfAttnKNorm, biasMatrix, 1e-06,
                       true,
                       false,
                       kHeadLayerNormOutputs[i]);
    }

    // V
    size_t valueHeadNums = vProjOutput->GetWidth() / config->GetHeadDim();
    vHeads.resize(valueHeadNums);
    vHeadLayerNormOutputs.resize(valueHeadNums);
    for (int i = 0; i < valueHeadNums; i++) {
        auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (mat == nullptr) {
            Logger() << "failed to create vHead matrix" << std::endl;
            return false;
        }
        vHeads[i] = mat;

        auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
        if (matLayerNorm == nullptr) {
            Logger() << "failed to create vHead layerNorm matrix" << std::endl;
            return false;
        }
        vHeadLayerNormOutputs[i] = matLayerNorm;
    }
    mle->Split(vProjOutput, valueHeadNums, vHeads);
    for (int i = 0; i < valueHeadNums; i++) {
        mle->LayerNorm(vHeads[i], selfAttnKNorm, biasMatrix, 1e-06,
                       true,
                       false,
                       vHeadLayerNormOutputs[i]);
    }

    // Scaled Dot-Product Attention
    qkvAttentionConcatOutput = mle->CreateMatrix(selfAttnQProjWeight.shape.width, selfAttnQProjWeight.shape.width);
    assert(qkvAttentionConcatOutput != nullptr);
    qkDotProdOutputs.resize(queryHeadNums);
    qkDotProdScaleOutputs.resize(queryHeadNums);
    qkDotProdScaleSoftmaxOutputs.resize(queryHeadNums);
    qkvAttentionOutput.resize(queryHeadNums);
    for (int i = 0; i < queryHeadNums; i++) {
        qkDotProdOutputs[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(qkDotProdOutputs[i] != nullptr);
        qkDotProdScaleOutputs[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(qkDotProdScaleOutputs[i] != nullptr);
        qkDotProdScaleSoftmaxOutputs[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(qkDotProdScaleSoftmaxOutputs[i] != nullptr);
        qkvAttentionOutput[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(qkvAttentionOutput[i] != nullptr);
        mle->ScaledDotProductAttention(qHeadLayerNormOutputs[i],
                                       kHeadLayerNormOutputs[i / 2],
                                       qkDotProdOutputs[i],
                                       qkDotProdScaleOutputs[i],
                                       qkDotProdScaleSoftmaxOutputs[i],
                                       vHeadLayerNormOutputs[i / 2],
                                       qkvAttentionOutput[i]);
    }

    mle->Concat(qkvAttentionOutput, qkvAttentionConcatOutput);

    // TODO: MLP/FFN
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
    Logger() << "qHeadLayerNormOutputs: ";
    for (int i = 0; i < 16; i++) {
        qHeadLayerNormOutputs[i]->Print();
    }
    Logger() << "kHeadLayerNormOutputs: ";
    for (int i = 0; i < 8; i++) {
        kHeadLayerNormOutputs[i]->Print();
    }
    Logger() << "vHeadLayerNormOutputs: ";
    for (int i = 0; i < 8; i++) {
        vHeadLayerNormOutputs[i]->Print();
    }
    Logger() << "qkvAttentionOutput: ";
    for (int i = 0; i < 16; i++) {
        qkvAttentionOutput[i]->Print();
    }
}
