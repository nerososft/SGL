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
    this->config = config;

    this->inputLayerNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "input_layernorm");
    this->selfAttnKNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.k_norm");
    this->selfAttnKProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.k_proj");
    this->selfAttnOProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.o_proj");
    this->selfAttnQNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.q_norm");
    this->selfAttnQProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.q_proj");
    this->selfAttnVProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "self_attn.v_proj");
    this->postAttentionLayerNormWeight = safeTensor->GetLayerWeight(this->layerIndex, "post_attention_layernorm");
    this->mlpUpProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.up_proj");
    this->mlpGateProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.gate_proj");
    this->mlpDownProjWeight = safeTensor->GetLayerWeight(this->layerIndex, "mlp.down_proj");

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

    inputLayerNorm = InitWeightMatrix(safeTensor, inputLayerNormWeight);
    assert(inputLayerNorm != nullptr);

    biasMatrix = mle->CreateMatrix(32, 32);

    return true;
}

void TransformerBlock::SetInputsMatrix(const std::vector<std::shared_ptr<Matrix> > &inputs) {
    this->inputsMatrix = inputs;
}

void TransformerBlock::SetOutputsMatrix(const std::vector<std::shared_ptr<Matrix> > &outputs) {
    this->outputsMatrix = outputs;
}

void TransformerBlock::MultiHead(const size_t tokenPos) {
    const size_t seqLen = inputsMatrix.size();
    inputLayerNormOutput.resize(seqLen); // seq_len, 1024
    qProjOutput.resize(seqLen); // seq_len, 2048
    qHeads.resize(seqLen); // seq_len, 16, 128
    qHeadLayerNormOutputs.resize(seqLen); // seq_len, 16, 128
    kProjOutput.resize(seqLen); //seq_len, 1024
    kHeads.resize(seqLen); // seq_len, 8, 128
    kHeadLayerNormOutputs.resize(seqLen); // seq_len, 8, 128
    vProjOutput.resize(seqLen); //seq_len, 1024
    vHeads.resize(seqLen); // seq_len, 8, 128
    vHeadLayerNormOutputs.resize(seqLen); // seq_len, 8, 128

    if (inputLayerNormOutput[tokenPos] == nullptr) {
        inputLayerNormOutput[tokenPos] = InitMatrix(inputLayerNormWeight);
        assert(inputLayerNorm != nullptr);
    }
    if (qProjOutput[tokenPos] == nullptr) {
        qProjOutput[tokenPos] = mle->CreateMatrix(selfAttnQProjWeight.shape.width, 1);
        assert(qProjOutput[tokenPos] != nullptr);
    }
    if (kProjOutput[tokenPos] == nullptr) {
        kProjOutput[tokenPos] = mle->CreateMatrix(selfAttnKProjWeight.shape.width, 1);
        assert(kProjOutput[tokenPos] != nullptr);
    }
    if (vProjOutput[tokenPos] == nullptr) {
        vProjOutput[tokenPos] = mle->CreateMatrix(selfAttnVProjWeight.shape.width, 1);
        assert(vProjOutput[tokenPos] != nullptr);
    }
    const size_t queryHeadNums = qProjOutput[tokenPos]->GetWidth() / config->GetHeadDim();
    qHeads[tokenPos].resize(queryHeadNums);
    qHeadLayerNormOutputs[tokenPos].resize(queryHeadNums);
    for (int i = 0; i < queryHeadNums; i++) {
        if (qHeads[tokenPos][i] == nullptr) {
            auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (mat == nullptr) {
                Logger() << "failed to create qHead matrix" << std::endl;
                throw std::runtime_error("failed to create qHead matrix");
            }
            qHeads[tokenPos][i] = mat;
        }

        if (qHeadLayerNormOutputs[tokenPos][i] == nullptr) {
            auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (matLayerNorm == nullptr) {
                Logger() << "failed to create qHead layerNorm matrix" << std::endl;
                throw std::runtime_error("failed to create qHead layerNorm matrix");
            }
            qHeadLayerNormOutputs[tokenPos][i] = matLayerNorm;
        }
    }

    const size_t keyHeadNums = kProjOutput[tokenPos]->GetWidth() / config->GetHeadDim();
    kHeads[tokenPos].resize(keyHeadNums);
    kHeadLayerNormOutputs[tokenPos].resize(keyHeadNums);
    for (int i = 0; i < keyHeadNums; i++) {
        if (kHeads[tokenPos][i] == nullptr) {
            auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (mat == nullptr) {
                Logger() << "failed to create kHead matrix" << std::endl;
                throw std::runtime_error("failed to create kHead matrix");
            }
            kHeads[tokenPos][i] = mat;
        }

        if (kHeadLayerNormOutputs[tokenPos][i] == nullptr) {
            auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (matLayerNorm == nullptr) {
                Logger() << "failed to create kHead layerNorm matrix" << std::endl;
                throw std::runtime_error("failed to create kHead layerNorm matrix");
            }
            kHeadLayerNormOutputs[tokenPos][i] = matLayerNorm;
        }
    }

    const size_t valueHeadNums = vProjOutput[tokenPos]->GetWidth() / config->GetHeadDim();
    vHeads[tokenPos].resize(valueHeadNums);
    vHeadLayerNormOutputs[tokenPos].resize(valueHeadNums);
    for (int i = 0; i < valueHeadNums; i++) {
        if (vHeads[tokenPos][i] == nullptr) {
            auto mat = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (mat == nullptr) {
                Logger() << "failed to create vHead matrix" << std::endl;
                throw std::runtime_error("failed to create vHead matrix");
            }
            vHeads[tokenPos][i] = mat;
        }

        if (vHeadLayerNormOutputs[tokenPos][i] == nullptr) {
            auto matLayerNorm = mle->CreateMatrix(config->GetHeadDim(), 1);
            if (matLayerNorm == nullptr) {
                Logger() << "failed to create vHead layerNorm matrix" << std::endl;
                throw std::runtime_error("failed to create vHead layerNorm matrix");
            }
            vHeadLayerNormOutputs[tokenPos][i] = matLayerNorm;
        }
    }

    const std::shared_ptr<Sequence> seq = mle->Seq()
            ->Record(mle->LayerNorm(inputsMatrix[tokenPos],
                                    this->inputLayerNorm,
                                    biasMatrix,
                                    1e-06,
                                    true,
                                    false,
                                    inputLayerNormOutput[tokenPos]))
            ->Record(mle->MatMul(inputLayerNormOutput[tokenPos], selfAttnQProj, qProjOutput[tokenPos]))
            ->Record(mle->MatMul(inputLayerNormOutput[tokenPos], selfAttnKProj, kProjOutput[tokenPos]))
            ->Record(mle->MatMul(inputLayerNormOutput[tokenPos], selfAttnVProj, vProjOutput[tokenPos]))
            ->Record(mle->Split(qProjOutput[tokenPos], queryHeadNums, qHeads[tokenPos]));
    for (int i = 0; i < queryHeadNums; i++) {
        seq->Record(mle->LayerNorm(qHeads[tokenPos][i],
                                   selfAttnQNorm,
                                   biasMatrix,
                                   1e-06,
                                   true,
                                   false,
                                   qHeadLayerNormOutputs[tokenPos][i]));
    }
    seq->Record(mle->Split(kProjOutput[tokenPos], keyHeadNums, kHeads[tokenPos]));
    for (int i = 0; i < keyHeadNums; i++) {
        seq->Record(mle->LayerNorm(kHeads[tokenPos][i],
                                   selfAttnKNorm,
                                   biasMatrix,
                                   1e-06,
                                   true,
                                   false,
                                   kHeadLayerNormOutputs[tokenPos][i]));
    }
    seq->Record(mle->Split(vProjOutput[tokenPos], valueHeadNums, vHeads[tokenPos]));
    for (int i = 0; i < valueHeadNums; i++) {
        seq->Record(mle->LayerNorm(vHeads[tokenPos][i],
                                   selfAttnKNorm,
                                   biasMatrix,
                                   1e-06,
                                   true,
                                   false,
                                   vHeadLayerNormOutputs[tokenPos][i]));
    }
    seq->Eval()->Destroy();
}

void TransformerBlock::Attention(const size_t tokenPos) {
    const size_t seqLen = inputsMatrix.size();
    qkvAttentionOutputs.resize(seqLen);
    qkvAttentionConcatOutputs.resize(seqLen);
    if (qkvAttentionConcatOutputs[tokenPos] == nullptr) {
        qkvAttentionConcatOutputs[tokenPos] = mle->CreateMatrix(selfAttnQProjWeight.shape.width, 1);
        assert(qkvAttentionConcatOutputs[tokenPos] != nullptr);
    }

    const size_t queryHeadNums = qProjOutput[tokenPos]->GetWidth() / config->GetHeadDim();
    qkvAttentionOutputs[tokenPos].resize(queryHeadNums);
    kqRoPEMulOutputs[tokenPos].resize(queryHeadNums);
    for (int headIdx = 0; headIdx < queryHeadNums; headIdx++) {
        for (int tokenIdx = 0; tokenIdx < seqLen; tokenIdx++) {
            if (kqRoPEMulOutputs[tokenPos][headIdx] == nullptr) {
                kqRoPEMulOutputs[tokenPos][headIdx] = mle->CreateMatrix(config->GetHeadDim(), 1);
                assert(kqRoPEMulOutputs[tokenPos][headIdx] != nullptr);
            }
            if (qkvAttentionOutputs[tokenPos][headIdx] == nullptr) {
                qkvAttentionOutputs[tokenPos][headIdx] = mle->CreateMatrix(config->GetHeadDim(), 1);
                assert(qkvAttentionOutputs[tokenPos][headIdx] != nullptr);
            }
        }
    }

    if (selfAttnOProjOutputs[tokenPos] == nullptr) {
        selfAttnOProjOutputs[tokenPos] = mle->CreateMatrix(selfAttnOProj->GetWidth(), 1);
        assert(selfAttnOProjOutputs[tokenPos] != nullptr);
    }
    if (add1Outputs[tokenPos] == nullptr) {
        add1Outputs[tokenPos] = mle->CreateMatrix(selfAttnOProj->GetWidth(), 1);
        assert(add1Outputs[tokenPos] != nullptr);
    }

    qkSoftmaxOutputs.resize(queryHeadNums);
    for (int headIdx = 0; headIdx < queryHeadNums; headIdx++) {
        qkSoftmaxOutputs[headIdx].resize(seqLen);
        const auto seq = mle->Seq();
        std::vector<float> qkDotProdScale(seqLen);
        for (int tokenIdx = 0; tokenIdx < seqLen; tokenIdx++) {
            seq->Record(mle->RoPEAndMul(
                        config->GetRoPETheta(),
                        tokenPos,
                        tokenIdx,
                        qHeadLayerNormOutputs[tokenPos][headIdx],
                        kHeadLayerNormOutputs[tokenPos][headIdx / 2],
                        kqRoPEMulOutputs[tokenPos][headIdx]))
                    ->Record(mle->Sum(kqRoPEMulOutputs[tokenPos][headIdx],
                                      &qkDotProdScale[tokenIdx]));
        }
        auto softmax = mle->CreateMatrix(seqLen, 1, qkDotProdScale);
        assert(softmax != nullptr);
        seq->Record(mle->Softmax(softmax,
                                 qkSoftmaxOutputs[headIdx][tokenPos]))
                ->Eval()
                ->Destroy();
        // (RoPE(Q) · RoPE(K)) / sqrt(dim) * V
        // TODO:
    }

    const auto seq = mle->Seq();
    seq->Record(mle->Concat(qkvAttentionOutputs[tokenPos], qkvAttentionConcatOutputs[tokenPos]))
            ->Record(
                mle->MatMul(qkvAttentionConcatOutputs[tokenPos],
                            selfAttnOProj,
                            selfAttnOProjOutputs[tokenPos]))
            ->Record(mle->Add(selfAttnOProjOutputs[tokenPos],
                              inputsMatrix[tokenPos],
                              add1Outputs[tokenPos]))
            ->Record(mle->LayerNorm(add1Outputs[tokenPos],
                                    postAttentionLayerNorm,
                                    biasMatrix,
                                    1e-06,
                                    true,
                                    false,
                                    postAttentionLayerNormOutputs[tokenPos]))
            ->Eval()
            ->Destroy();
}

void TransformerBlock::MLP(const size_t tokenPos) {
    if (postAttentionLayerNormOutputs[tokenPos] == nullptr) {
        postAttentionLayerNormOutputs[tokenPos] = mle->CreateMatrix(postAttentionLayerNorm->GetWidth(), 1);
        assert(postAttentionLayerNormOutputs[tokenPos] != nullptr);
    }
    if (mlpUpProjOutputs[tokenPos] == nullptr) {
        mlpUpProjOutputs[tokenPos] = mle->CreateMatrix(mlpUpProj->GetWidth(), 1);
        assert(mlpUpProjOutputs[tokenPos] != nullptr);
    }
    if (mlpGateProjOutputs[tokenPos] == nullptr) {
        mlpGateProjOutputs[tokenPos] = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
        assert(mlpGateProjOutputs[tokenPos] != nullptr);
    }
    if (mlpGateSigmoidOutputs[tokenPos] == nullptr) {
        mlpGateSigmoidOutputs[tokenPos] = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
        assert(mlpGateSigmoidOutputs[tokenPos] != nullptr);
    }
    if (mlpGateOutputs[tokenPos] == nullptr) {
        mlpGateOutputs[tokenPos] = mle->CreateMatrix(mlpGateProj->GetWidth(), 1);
        assert(mlpGateOutputs[tokenPos] != nullptr);
    }
    if (mlpOutputs[tokenPos] == nullptr) {
        mlpOutputs[tokenPos] = mle->CreateMatrix(mlpGateProj->GetHeight(), 1);
        assert(mlpOutputs[tokenPos] != nullptr);
    }

    // MLP/FFN
    const auto seq = mle->Seq()
            ->Record(mle->MatMul(postAttentionLayerNormOutputs[tokenPos],
                                 mlpUpProj,
                                 mlpUpProjOutputs[tokenPos]))
            ->Record(mle->MatMul(postAttentionLayerNormOutputs[tokenPos],
                                 mlpGateProj, mlpGateProjOutputs[tokenPos]))
            ->Record(mle->GatedSiLU(mlpUpProjOutputs[tokenPos],
                                    mlpGateProjOutputs[tokenPos],
                                    mlpGateSigmoidOutputs[tokenPos],
                                    mlpGateOutputs[tokenPos]))
            ->Record(mle->MatMul(mlpGateOutputs[tokenPos],
                                 mlpDownProj,
                                 mlpOutputs[tokenPos]))
            ->Record(mle->LayerNorm(mlpOutputs[tokenPos],
                                    postAttentionLayerNormOutputs[tokenPos],
                                    biasMatrix,
                                    1e-06,
                                    true,
                                    false,
                                    outputsMatrix[tokenPos]))
            ->Eval()
            ->Destroy();
}

void TransformerBlock::Dump() const {
    const size_t seqLen = inputsMatrix.size();
    Logger() << "TransformerLayer:" << this->layerIndex << std::endl;
    for (int tokenIdx = 0; tokenIdx < seqLen; tokenIdx++) {
        Logger() << "inputMatrix: ";
        inputsMatrix[tokenIdx]->Print();
        Logger() << "inputLayerNorm: ";
        inputLayerNorm->Print();
        Logger() << "inputLayerNormOutput: ";
        inputLayerNormOutput[tokenIdx]->Print();
        Logger() << "qProjOutput: ";
        qProjOutput[tokenIdx]->Print();

        for (int i = 0; i < 16; i++) {
            Logger() << "qHeadLayerNormOutputs(" << i << "): ";
            qHeadLayerNormOutputs[tokenIdx][i]->Print();
        }
        for (int i = 0; i < 8; i++) {
            Logger() << "kHeadLayerNormOutputs(" << i << "): ";
            kHeadLayerNormOutputs[tokenIdx][i]->Print();
        }
        for (int i = 0; i < 8; i++) {
            Logger() << "vHeadLayerNormOutputs(" << i << "): ";
            vHeadLayerNormOutputs[tokenIdx][i]->Print();
        }
        for (int i = 0; i < 16; i++) {
            Logger() << "qkvAttentionOutput(" << i << "): ";
            qkvAttentionOutputs[tokenIdx][i]->Print();
        }
        Logger() << "qkvAttentionConcatOutput: ";
        qkvAttentionConcatOutputs[tokenIdx]->Print();
        Logger() << "selfAttnOProjOutput: ";
        selfAttnOProjOutputs[tokenIdx]->Print();
        Logger() << "add1Output: ";
        add1Outputs[tokenIdx]->Print();
        Logger() << "postAttentionLayerNormOutput: ";
        postAttentionLayerNormOutputs[tokenIdx]->Print();
        Logger() << "mlpUpProjOutput: ";
        mlpUpProjOutputs[tokenIdx]->Print();
        Logger() << "mlpGateProjOutput: ";
        mlpGateProjOutputs[tokenIdx]->Print();
        Logger() << "mlpGateSigmoidOutput: ";
        mlpGateSigmoidOutputs[tokenIdx]->Print();
        Logger() << "mlpGateOutput: ";
        mlpGateOutputs[tokenIdx]->Print();
        Logger() << "mlpOutput: ";
        mlpOutputs[tokenIdx]->Print();
        Logger() << "Output: ";
        outputsMatrix[tokenIdx]->Print();
    }
    Logger() << std::endl;
}
