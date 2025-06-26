//
// Created by neo on 25-6-24.
//

#ifndef MODEL_H
#define MODEL_H
#include <vector>

#include "SafeTensor.h"
#include "TransformerBlock.h"
#include "engine/ml/MLEngine.h"

class Model {
    std::shared_ptr<Config> config = nullptr;
    std::shared_ptr<SafeTensor> safeTensor = nullptr;

    std::vector<std::shared_ptr<TransformerBlock> > blocks;
    // normal

public:
    explicit Model(const std::shared_ptr<Config> &config,
                   const std::shared_ptr<SafeTensor> &safeTensor);

    ~Model() = default;

    bool Init(const std::shared_ptr<MLEngine> &mle);

    std::vector<float> Forward(const std::vector<float> &input);
};


#endif //MODEL_H
