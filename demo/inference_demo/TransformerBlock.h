//
// Created by neo on 25-6-24.
//

#ifndef TRANSFORMERBLOCK_H
#define TRANSFORMERBLOCK_H
#include <cstdint>
#include <memory>

#include "SafeTensor.h"
#include "engine/ml/Matrix.h"
#include "engine/ml/MLEngine.h"

// "model.layers.0.input_layernorm.weight": 1024
// "model.layers.0.self_attn.q_proj.weight": 2048, 1024
// "model.layers.0.self_attn.q_norm.weight": 128

// "model.layers.0.self_attn.k_proj.weight": 1024, 1024
// "model.layers.0.self_attn.v_proj.weight": 1024, 1024
// "model.layers.0.self_attn.k_norm.weight": 128

// "model.layers.0.self_attn.o_proj.weight": 1024, 2048

// "model.layers.0.mlp.up_proj.weight": 3072, 1024
// "model.layers.0.mlp.gate_proj.weight": 3072, 1024
// "model.layers.0.mlp.down_proj.weight": 1024,3072

// "model.layers.0.post_attention_layernorm.weight": 1024

// x [batch, seq_len, 1024]
//   ↓
// LayerNorm
//   ↓
// Self-Attention:
//   q_proj: [1024→2048] → 分割为16个头 [128]
//   k_proj: [1024→1024] → 分割为16个头 [128]
//   v_proj: [1024→1024] → 分割为16个头 [128]
//   注意力计算 → 合并头 [2048]
//   o_proj: [2048→1024]
//   残差连接 → [1024]
//   ↓
// LayerNorm
//   ↓
// Gated MLP:
//   gate_proj: [1024→3072]
//   up_proj: [1024→3072]
//   门控乘法 → [3072]
//   down_proj: [3072→1024]
//   残差连接 → [1024]
//   ↓
// output [batch, seq_len, 1024]

class TransformerBlock {
    uint64_t layerIndex = 0;
    std::shared_ptr<MLEngine> mle = nullptr;

    std::shared_ptr<Matrix> inputMatrix = nullptr;
    std::shared_ptr<Matrix> outputMatrix = nullptr;

    std::shared_ptr<Matrix> biasMatrix = nullptr;

    std::shared_ptr<Matrix> inputLayerNorm = nullptr; // 1024
    std::shared_ptr<Matrix> inputLayerNormOutput = nullptr; // 1024

    std::shared_ptr<Matrix> selfAttnQProj = nullptr; // 2048, 1024
    std::shared_ptr<Matrix> qProjOutput = nullptr; //2048
    std::vector<std::shared_ptr<Matrix> > qHeads;
    std::shared_ptr<Matrix> selfAttnQNorm = nullptr; // 128
    std::vector<std::shared_ptr<Matrix> > qHeadLayerNormOutputs;

    std::shared_ptr<Matrix> selfAttnKProj = nullptr; // 1024, 1024
    std::shared_ptr<Matrix> kProjOutput = nullptr; //1024
    std::vector<std::shared_ptr<Matrix> > kHeads;
    std::shared_ptr<Matrix> selfAttnKNorm = nullptr; // 128
    std::vector<std::shared_ptr<Matrix> > kHeadLayerNormOutputs;

    std::shared_ptr<Matrix> selfAttnVProj = nullptr; // 1024, 1024
    std::shared_ptr<Matrix> vProjOutput = nullptr; //1024
    std::vector<std::shared_ptr<Matrix> > vHeads;
    std::vector<std::shared_ptr<Matrix> > vHeadLayerNormOutputs;

    std::vector<std::shared_ptr<Matrix> > qkDotProdOutputs;
    std::vector<std::shared_ptr<Matrix> > qkDotProdScaleOutputs;
    std::vector<std::shared_ptr<Matrix> > qkDotProdScaleSoftmaxOutputs;
    std::vector<std::shared_ptr<Matrix> > qkvAttentionOutput;

    std::shared_ptr<Matrix> qkvAttentionConcatOutput = nullptr; // 2048
    std::shared_ptr<Matrix> selfAttnOProj = nullptr; // 1024, 2048

    const std::shared_ptr<Matrix> add1Output = nullptr; // multi head attention 残差连接
    std::shared_ptr<Matrix> postAttentionLayerNorm = nullptr; // 1024

    std::shared_ptr<Matrix> mlpUpProj = nullptr; // 3072, 1024
    std::shared_ptr<Matrix> mlpGateProj = nullptr; // 3072, 1024
    std::shared_ptr<Matrix> mlpDownProj = nullptr; // 1024,3072

public:
    explicit TransformerBlock(const std::shared_ptr<MLEngine> &mle,
                              uint64_t layerIdx);

    [[nodiscard]] std::shared_ptr<Matrix> InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                                                           const Weight &weight) const;

    [[nodiscard]] std::shared_ptr<Matrix> InitMatrix(const Weight &weight) const;

    bool Init(const std::shared_ptr<SafeTensor> &safeTensor,
              const std::shared_ptr<Config> &config);

    ~TransformerBlock() = default;

    void SetInputMatrix(const std::shared_ptr<Matrix> &input);

    std::shared_ptr<Matrix> &GetOutputMatrix();

    void Dump() const;
};

#endif //TRANSFORMERBLOCK_H
