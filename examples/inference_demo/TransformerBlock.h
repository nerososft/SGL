//
// Created by neo on 25-6-24.
//

#ifndef TRANSFORMERBLOCK_H
#define TRANSFORMERBLOCK_H
#include <cstdint>
#include <memory>

#include "SafeTensor.h"
#include "core/compute/ComputeEngine.h"
#include "core/compute/Matrix.h"

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

  std::shared_ptr<sgl::compute::ComputeEngine> ce = nullptr;
  std::shared_ptr<Config> config = nullptr;

  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      inputsMatrix; // seq_len, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      outputsMatrix; // seq_len, 1024

  std::shared_ptr<sgl::compute::Matrix> biasMatrix = nullptr;

  Weight inputLayerNormWeight{};
  Weight selfAttnKNormWeight{};
  Weight selfAttnKProjWeight{};
  Weight selfAttnOProjWeight{};
  Weight selfAttnQNormWeight{};
  Weight selfAttnQProjWeight{};
  Weight selfAttnVProjWeight{};
  Weight postAttentionLayerNormWeight{};
  Weight mlpUpProjWeight{};
  Weight mlpGateProjWeight{};
  Weight mlpDownProjWeight{};

  // KQV
  std::shared_ptr<sgl::compute::Matrix> inputLayerNorm = nullptr; // 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      inputLayerNormOutput; // seq_len, 1024

  std::shared_ptr<sgl::compute::Matrix> selfAttnQProj = nullptr; // 2048, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      qProjOutput; // seq_len, 2048
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      qHeads; // seq_len, 16, 128
  std::shared_ptr<sgl::compute::Matrix> selfAttnQNorm = nullptr; // 128
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      qHeadLayerNormOutputs; // seq_len, 16, 128

  std::shared_ptr<sgl::compute::Matrix> selfAttnKProj = nullptr; // 1024, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      kProjOutput; // seq_len, 1024
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      kHeads; // seq_len, 8, 128
  std::shared_ptr<sgl::compute::Matrix> selfAttnKNorm = nullptr; // 128
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      kHeadLayerNormOutputs; // seq_len, 8, 128

  std::shared_ptr<sgl::compute::Matrix> selfAttnVProj = nullptr; // 1024, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      vProjOutput; // seq_len, 1024
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      vHeads; // seq_len, 8, 128
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      vHeadLayerNormOutputs; // seq_len, 8, 128

  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      qkvAttentionOutputs; // 16, seq_Len, 128
  std::shared_ptr<sgl::compute::Matrix>
      qkvAttentionConcatOutput; // seq_len, 2048

  std::shared_ptr<sgl::compute::Matrix> selfAttnOProj = nullptr; // 1024, 2048
  std::shared_ptr<sgl::compute::Matrix> selfAttnOProjOutput; // seq_len, 1024

  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      add1Outputs; // seq_len, 1024
  std::shared_ptr<sgl::compute::Matrix> postAttentionLayerNorm =
      nullptr; // 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      postAttentionLayerNormOutputs; // seq_len, 1024

  std::shared_ptr<sgl::compute::Matrix> mlpUpProj = nullptr; // 3072, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      mlpUpProjOutputs;                                        // seq_len, 3072
  std::shared_ptr<sgl::compute::Matrix> mlpGateProj = nullptr; // 3072, 1024
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      mlpGateProjOutputs; // seq_len, 3072
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      mlpGateSigmoidOutputs; // seq_len, 3072
  std::vector<std::shared_ptr<sgl::compute::Matrix>> mlpGateOutputs; // 3072
  std::shared_ptr<sgl::compute::Matrix> mlpDownProj = nullptr; // 1024,3072
  std::vector<std::shared_ptr<sgl::compute::Matrix>>
      mlpOutputs; // seq_len, 1024

public:
  explicit TransformerBlock(
      const std::shared_ptr<sgl::compute::ComputeEngine> &ce,
      uint64_t layerIdx);

  [[nodiscard]] std::shared_ptr<sgl::compute::Matrix>
  InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                   const Weight &weight) const;

  [[nodiscard]] std::shared_ptr<sgl::compute::Matrix>
  InitMatrix(const Weight &weight) const;

  bool Init(const std::shared_ptr<SafeTensor> &safeTensor,
            const std::shared_ptr<Config> &config);

  ~TransformerBlock() = default;

  void SetInputsMatrix(
      const std::vector<std::shared_ptr<sgl::compute::Matrix>> &inputs);

  void SetOutputsMatrix(
      const std::vector<std::shared_ptr<sgl::compute::Matrix>> &outputs);

  std::vector<std::shared_ptr<sgl::compute::Matrix>> GetOutputsMatrix() {
    return outputsMatrix;
  }

  void MultiHead(size_t tokenPos);

  void Attention();

  void MLP(size_t tokenPos);

  void Dump() const;
};

#endif // TRANSFORMERBLOCK_H
