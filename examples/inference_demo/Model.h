//
// Created by neo on 25-6-24.
//

#ifndef MODEL_H
#define MODEL_H
#include <vector>

#include "SafeTensor.h"
#include "TransformerBlock.h"
#include "core/compute/ComputeEngine.h"

class Model {
  std::shared_ptr<Config> config = nullptr;
  std::shared_ptr<SafeTensor> safeTensor = nullptr;

  std::shared_ptr<sgl::compute::ComputeEngine> ce = nullptr;

  std::vector<std::shared_ptr<TransformerBlock>> blocks;

  std::vector<std::shared_ptr<sgl::compute::Matrix>> inputsMatrix; // seq_len, 1024
  std::vector<std::vector<std::shared_ptr<sgl::compute::Matrix>>>
      outputsMatrix; // hidden_layers, seq_len, 1024

  std::shared_ptr<sgl::compute::Matrix> biasMatrix = nullptr;
  std::shared_ptr<sgl::compute::Matrix> normMatrix = nullptr;
  std::shared_ptr<sgl::compute::Matrix> lmHeadMatrix = nullptr;

  std::shared_ptr<sgl::compute::Matrix> result = nullptr;
  std::shared_ptr<sgl::compute::Matrix> softmaxResult = nullptr;

public:
  explicit Model(const std::shared_ptr<sgl::compute::ComputeEngine> &ce,
                 const std::shared_ptr<Config> &config,
                 const std::shared_ptr<SafeTensor> &safeTensor);

  [[nodiscard]] std::shared_ptr<sgl::compute::Matrix>
  InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                   const Weight &weight) const;

  ~Model() = default;

  bool Init();

  void Dump() const;

  [[nodiscard]] std::vector<float>
  Forward(const std::vector<std::vector<float>> &inputs);
};

#endif // MODEL_H
