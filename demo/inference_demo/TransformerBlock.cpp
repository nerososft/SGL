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
    qkvAttentionConcatOutput = mle->CreateMatrix(selfAttnQProjWeight.shape.width, 1);
    assert(qkvAttentionConcatOutput != nullptr);
    qkDotProdOutputs.resize(queryHeadNums);
    qRoPEOutput.resize(queryHeadNums);
    kRoPEOutput.resize(queryHeadNums);
    qkDotProdScaleOutputs.resize(queryHeadNums);
    qkDotProdScaleSoftmaxOutputs.resize(queryHeadNums);
    qkvAttentionOutput.resize(queryHeadNums);
    for (int i = 0; i < queryHeadNums; i++) {
        qRoPEOutput[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(qRoPEOutput[i] != nullptr);
        kRoPEOutput[i] = mle->CreateMatrix(config->GetHeadDim(), 1);
        assert(kRoPEOutput[i] != nullptr);
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
                                       config->GetRoPETheta(),
                                       &tokenIndex,
                                       &tokenPos,
                                       qRoPEOutput[i],
                                       kRoPEOutput[i],
                                       qkDotProdOutputs[i],
                                       qkDotProdScaleOutputs[i],
                                       qkDotProdScaleSoftmaxOutputs[i],
                                       vHeadLayerNormOutputs[i / 2],
                                       qkvAttentionOutput[i]);
    }

    mle->Concat(qkvAttentionOutput, qkvAttentionConcatOutput);

    selfAttnOProjOutput = mle->CreateMatrix(selfAttnOProj->GetWidth(), 1);
    assert(selfAttnOProjOutput != nullptr);
    mle->MatMul(qkvAttentionConcatOutput, selfAttnOProj, selfAttnOProjOutput);

    // add
    add1Output = mle->CreateMatrix(selfAttnOProj->GetWidth(), 1);
    assert(add1Output != nullptr);
    mle->Add(selfAttnOProjOutput, inputMatrix, add1Output);

    postAttentionLayerNormOutput = mle->CreateMatrix(postAttentionLayerNorm->GetWidth(), 1);
    assert(postAttentionLayerNormOutput != nullptr);
    mle->LayerNorm(add1Output, postAttentionLayerNorm, biasMatrix, 1e-06,
                   true,
                   false,
                   postAttentionLayerNormOutput);

    // MLP/FFN
    mlpUpProjOutput = mle->CreateMatrix(mlpUpProj->GetWidth(), 1);
    assert(mlpUpProjOutput != nullptr);
    mle->MatMul(postAttentionLayerNormOutput, mlpUpProj, mlpUpProjOutput);

    mlpGateProjOutput = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
    assert(mlpGateProjOutput != nullptr);
    mle->MatMul(postAttentionLayerNormOutput, mlpGateProj, mlpGateProjOutput);

    mlpGateSigmoidOutput = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
    assert(mlpGateSigmoidOutput != nullptr);
    mlpGateOutput = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
    assert(mlpGateOutput != nullptr);
    mle->GatedSiLU(mlpUpProjOutput, mlpGateProjOutput, mlpGateSigmoidOutput, mlpGateOutput);

    mlpOutput = mle->CreateMatrix(mlpGateProj->GetHeight(), 1);
    assert(mlpOutput != nullptr);
    mle->MatMul(mlpGateOutput, mlpDownProj, mlpOutput);

    mle->LayerNorm(mlpOutput, postAttentionLayerNormOutput, biasMatrix, 1e-06,
                   true,
                   false,
                   outputMatrix);

    return true;
}

void TransformerBlock::SetInputMatrix(const std::shared_ptr<Matrix> &input) {
    this->inputMatrix = input;
}

std::shared_ptr<Matrix> &TransformerBlock::GetOutputMatrix() {
    return this->outputMatrix;
}

void TransformerBlock::Dump() const {
    Logger() << "TransformerLayer:" << this->layerIndex << std::endl;
    Logger() << "inputMatrix: ";
    inputMatrix->Print();
    Logger() << "inputLayerNorm: ";
    inputLayerNorm->Print();
    Logger() << "inputLayerNormOutput: ";
    inputLayerNormOutput->Print();
    Logger() << "qProjOutput: ";
    qProjOutput->Print();

    for (int i = 0; i < 16; i++) {
        Logger() << "qHeadLayerNormOutputs(" << i << "): ";
        qHeadLayerNormOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "qRoPEOutput(" << i << "): ";
        qRoPEOutput[i]->Print();
    }
    for (int i = 0; i < 8; i++) {
        Logger() << "kHeadLayerNormOutputs(" << i << "): ";
        kHeadLayerNormOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "kRoPEOutput(" << i << "): ";
        kRoPEOutput[i]->Print();
    }
    for (int i = 0; i < 8; i++) {
        Logger() << "vHeadLayerNormOutputs(" << i << "): ";
        vHeadLayerNormOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "qkDotProdOutputs(" << i << "): ";
        qkDotProdOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "qkDotProdScaleOutputs(" << i << "): ";
        qkDotProdScaleOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "qkDotProdScaleSoftmaxOutputs(" << i << "): ";
        qkDotProdScaleSoftmaxOutputs[i]->Print();
    }
    for (int i = 0; i < 16; i++) {
        Logger() << "qkvAttentionOutput(" << i << "): ";
        qkvAttentionOutput[i]->Print();
    }
    Logger() << "qkvAttentionConcatOutput: ";
    qkvAttentionConcatOutput->Print();
    Logger() << "selfAttnOProjOutput: ";
    selfAttnOProjOutput->Print();
    Logger() << "add1Output: ";
    add1Output->Print();
    Logger() << "postAttentionLayerNormOutput: ";
    postAttentionLayerNormOutput->Print();
    Logger() << "mlpUpProjOutput: ";
    mlpUpProjOutput->Print();
    Logger() << "mlpGateProjOutput: ";
    mlpGateProjOutput->Print();
    Logger() << "mlpGateSigmoidOutput: ";
    mlpGateSigmoidOutput->Print();
    Logger() << "mlpGateOutput: ";
    mlpGateOutput->Print();
    Logger() << "mlpOutput: ";
    mlpOutput->Print();
    Logger() << "Output: ";
    outputMatrix->Print();
    Logger() << std::endl;
}
