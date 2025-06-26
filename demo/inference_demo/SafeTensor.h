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

struct Shape2D {
    size_t width;
    size_t height;
};

struct Weight {
    size_t dataOffsets[2];
    std::string dtype;
    Shape2D shape;
};

class SafeTensor {
    std::shared_ptr<Config> config = nullptr;
    std::vector<std::vector<float> > embeddingMatrix;
    std::vector<std::vector<std::unordered_map<std::string, Weight> > > weights;

public:
    SafeTensor() = default;

    ~SafeTensor() = default;

    bool LoadBF16EmbeddingMatrix(const std::vector<char> &safeTensorData,
                                 nlohmann::json::const_reference embeddingMatrixObj);

    bool LoadFromFile(const std::string &tensorFilePath);

    std::vector<float> EmbeddingToken(int token);

    Weight GetLayerWeight(size_t layerIndex, const std::string &name);
};

#endif //SAFETENSORDATA_H
