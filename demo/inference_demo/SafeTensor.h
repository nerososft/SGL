//
// Created by neo on 25-6-24.
//

#ifndef SAFETENSORDATA_H
#define SAFETENSORDATA_H

#include "Config.h"
#include "vendor/json.hpp"

class SafeTensor {
    std::shared_ptr<Config> config = nullptr;
    std::vector<std::vector<float> > embeddingMatrix;

public:
    SafeTensor() = default;

    ~SafeTensor() = default;

    bool LoadBF16EmbeddingMatrix(const std::vector<char> &safeTensorData,
                                 nlohmann::json::const_reference embeddingMatrixObj);

    bool LoadFromFile(const std::string &tensorFilePath);

    std::vector<float> EmbeddingToken(int token);
};

#endif //SAFETENSORDATA_H
