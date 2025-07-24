//
// Created by neo on 25-6-24.
//

#ifndef MODEL_H
#define MODEL_H
#include <vector>

#include "SafeTensor.h"
#include "TransformerBlock.h"
#include "engine/compute/ComputeEngine.h"

class Model {
  std::shared_ptr<Config> config = nullptr;
  std::shared_ptr<SafeTensor> safeTensor = nullptr;

  std::shared_ptr<ComputeEngine> ce = nullptr;

  std::vector<std::shared_ptr<TransformerBlock>> blocks;

  std::vector<std::shared_ptr<Matrix>> inputsMatrix; // seq_len, 1024
  std::vector<std::vector<std::shared_ptr<Matrix>>>
      outputsMatrix; // hidden_layers, seq_len, 1024

  std::shared_ptr<Matrix> biasMatrix = nullptr;
  std::shared_ptr<Matrix> normMatrix = nullptr;
  std::shared_ptr<Matrix> lmHeadMatrix = nullptr;

  std::shared_ptr<Matrix> result = nullptr;
  std::shared_ptr<Matrix> softmaxResult = nullptr;

public:
  explicit Model(const std::shared_ptr<ComputeEngine> &ce,
                 const std::shared_ptr<Config> &config,
                 const std::shared_ptr<SafeTensor> &safeTensor);

  [[nodiscard]] std::shared_ptr<Matrix>
  InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor,
                   const Weight &weight) const;

  ~Model() = default;

  bool Init();

  void Dump() const;

  [[nodiscard]] std::vector<float>
  Forward(const std::vector<std::vector<float>> &inputs);
};

#endif // MODEL_H
