//
// Created by neo on 25-6-24.
//

#include "TransformerBlock.h"

TransformerBlock::TransformerBlock(const std::shared_ptr<MLEngine> &mle,
                                   uint64_t layerIdx) {
    this->mle = mle;
    this->layerIndex = layerIdx;
}

bool TransformerBlock::Init(const std::shared_ptr<SafeTensor> &safeTensor) {
    Weight inputLayerNormWeight = safeTensor->GetLayerWeight(0, "input_layernorm");
    Weight selfAttnKNormWeight = safeTensor->GetLayerWeight(0, "self_attn.k_norm");
    Weight selfAttnKProjWeight = safeTensor->GetLayerWeight(0, "self_attn.k_proj");
    Weight selfAttnOProjWeight = safeTensor->GetLayerWeight(0, "self_attn.o_proj");
    Weight selfAttnQNormWeight = safeTensor->GetLayerWeight(0, "self_attn.q_norm");
    Weight selfAttnQProjWeight = safeTensor->GetLayerWeight(0, "self_attn.q_proj");
    Weight selfAttnVProjWeight = safeTensor->GetLayerWeight(0, "self_attn.v_proj");
    Weight postAttentionLayerNormWeight = safeTensor->GetLayerWeight(0, "post_attention_layernorm");
    Weight mlpUpProjWeight = safeTensor->GetLayerWeight(0, "mlp.up_proj");
    Weight mlpGateProjWeight = safeTensor->GetLayerWeight(0, "mlp.gate_proj");
    Weight mlpDownProjWeight = safeTensor->GetLayerWeight(0, "mlp.down_proj");

    inputLayerNorm = mle->CreateMatrix(inputLayerNormWeight.shape.width, inputLayerNormWeight.shape.height); // 1024
    selfAttnKNorm = mle->CreateMatrix(selfAttnKNormWeight.shape.width, selfAttnKNormWeight.shape.height); // 128
    selfAttnKProj = mle->CreateMatrix(selfAttnKProjWeight.shape.width, selfAttnKProjWeight.shape.height); // 1024, 1024
    selfAttnOProj = mle->CreateMatrix(selfAttnOProjWeight.shape.width, selfAttnOProjWeight.shape.height); // 1024, 2048
    selfAttnQNorm = mle->CreateMatrix(selfAttnQNormWeight.shape.width, selfAttnQNormWeight.shape.height); // 128
    selfAttnQProj = mle->CreateMatrix(selfAttnQProjWeight.shape.width, selfAttnQProjWeight.shape.height); // 2048, 1024
    selfAttnVProj = mle->CreateMatrix(selfAttnVProjWeight.shape.width, selfAttnVProjWeight.shape.height); // 1024, 1024
    postAttentionLayerNorm = mle->CreateMatrix(postAttentionLayerNormWeight.shape.width,
                                               postAttentionLayerNormWeight.shape.height); // 1024
    mlpUpProj = mle->CreateMatrix(mlpUpProjWeight.shape.width, mlpUpProjWeight.shape.height); // 3072, 1024
    mlpGateProj = mle->CreateMatrix(mlpGateProjWeight.shape.width, mlpGateProjWeight.shape.height); // 3072, 1024
    mlpDownProj = mle->CreateMatrix(mlpDownProjWeight.shape.width, mlpDownProjWeight.shape.height); // 1024,3072

    // TODO: Upload Data from Weight
}
