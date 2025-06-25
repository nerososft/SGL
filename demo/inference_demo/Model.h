//
// Created by neo on 25-6-24.
//

#ifndef MODEL_H
#define MODEL_H
#include <vector>

#include "SafeTensor.h"
#include "TransformerBlock.h"

class Model {
    std::vector<std::shared_ptr<TransformerBlock> > blocks;
    std::shared_ptr<Config> config = nullptr;
    std::shared_ptr<SafeTensor> safeTensor = nullptr;

    // normal

public:
    explicit Model();

    ~Model() = default;

    bool Init(const std::shared_ptr<Config> &config, const std::shared_ptr<SafeTensor> &safeTensor);
};


#endif //MODEL_H
