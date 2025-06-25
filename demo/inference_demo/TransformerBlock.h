//
// Created by neo on 25-6-24.
//

#ifndef TRANSFORMERBLOCK_H
#define TRANSFORMERBLOCK_H
#include <cstdint>
#include <memory>

#include "SafeTensor.h"
#include "engine/ml/Matrix.h"

// "model.layers.0.input_layernorm.weight": 1024
// "model.layers.0.self_attn.k_norm.weight": 128
// "model.layers.0.self_attn.k_proj.weight": 1024, 1024
// "model.layers.0.self_attn.o_proj.weight": 1024, 2048
// "model.layers.0.self_attn.q_norm.weight": 128
// "model.layers.0.self_attn.q_proj.weight": 2048, 1024
// "model.layers.0.self_attn.v_proj.weight": 1024, 1024
// "model.layers.0.post_attention_layernorm.weight": 1024
// "model.layers.0.mlp.up_proj.weight": 3072, 1024
// "model.layers.0.mlp.gate_proj.weight": 3072, 1024
// "model.layers.0.mlp.down_proj.weight": 1024,3072

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

    // Multi-head attention
    const std::shared_ptr<Matrix> Q = nullptr;
    const std::shared_ptr<Matrix> K = nullptr;
    const std::shared_ptr<Matrix> qkMulOutput = nullptr;
    const std::shared_ptr<Matrix> softmaxOutput = nullptr;
    const std::shared_ptr<Matrix> V = nullptr;
    const std::shared_ptr<Matrix> vMulOutput = nullptr;

    const std::shared_ptr<Matrix> add1Output = nullptr; // multi head attention 残差连接

    const std::shared_ptr<Matrix> layerNormal1Output = nullptr;

    // ffn
    const std::shared_ptr<Matrix> linearLayer1Output = nullptr;
    const std::shared_ptr<Matrix> siluOutput = nullptr;
    const std::shared_ptr<Matrix> linearLayer2Output = nullptr;

    const std::shared_ptr<Matrix> add2Output = nullptr; // FFN 残差连接

    const std::shared_ptr<Matrix> layerNormal2Output = nullptr;

public:
    explicit TransformerBlock(uint64_t layerIdx);

    bool Init(const std::shared_ptr<SafeTensor> &safeTensor);

    ~TransformerBlock() = default;
};


#endif //TRANSFORMERBLOCK_H
