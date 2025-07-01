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
    std::shared_ptr<MLEngine> mle = nullptr;

    std::vector<std::shared_ptr<TransformerBlock> > blocks;

    std::shared_ptr<Matrix> inputMatrix = nullptr;

    std::shared_ptr<Matrix> normMatrix = nullptr;

    std::shared_ptr<Matrix> outputMatrix = nullptr;

public:
    explicit Model(const std::shared_ptr<MLEngine> &mle,
                   const std::shared_ptr<Config> &config,
                   const std::shared_ptr<SafeTensor> &safeTensor);

    std::shared_ptr<Matrix> InitWeightMatrix(const std::shared_ptr<SafeTensor> &safeTensor, const Weight &weight) const;

    ~Model() = default;

    bool Init();

    void Dump() const;

    std::vector<float> Forward(const std::vector<float> &input, uint32_t tokenIndex, uint32_t tokenPos) const;
};


#endif //MODEL_H
