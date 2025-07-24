//
// Created by neo on 25-6-24.
//

#ifndef SAFETENSORDATA_H
#define SAFETENSORDATA_H

#include "Config.h"
#include "vendor/json.hpp"

enum DataType {
  FLOAT,
  BF16,
};

struct DataOffset {
  size_t start;
  size_t end;
};

struct Shape2D {
  size_t width;
  size_t height;
};

struct Weight {
  DataOffset dataOffsets;
  std::string dtype;
  Shape2D shape;
};

class SafeTensor {
  std::shared_ptr<Config> config = nullptr;

  std::vector<char> dataBytes;

  std::vector<std::vector<float>> embeddingMatrix;
  std::unordered_map<std::string, Weight> weights;

public:
  explicit SafeTensor(const std::shared_ptr<Config> &config);

  ~SafeTensor() = default;

  bool LoadWeight(const std::string &weightName,
                  nlohmann::json::const_reference weightData);

  bool LoadWeights(const std::vector<char> &safeTensorData,
                   nlohmann::json::const_reference header);

  bool LoadBF16EmbeddingMatrix(const std::vector<char> &safeTensorData,
                               nlohmann::json::const_reference header);

  bool LoadFromFile(const std::string &tensorFilePath);

  std::vector<float> EmbeddingToken(int token);

  Weight GetWeight(const std::string &name);

  Weight GetLayerWeight(size_t layerIndex, const std::string &name);

  std::vector<float> GetLayerWeightData(const Weight &weight);
};

#endif // SAFETENSORDATA_H
