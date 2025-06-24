//
// Created by neo on 25-6-24.
//

#ifndef MODEL_H
#define MODEL_H
#include "SafeTensor.h"
#include "TransformerBlock.h"


class Model {
    std::vector<std::shared_ptr<TransformerBlock> > blocks;

public:
    explicit Model(std::shared_ptr<SafeTensor> safeTensor);

    ~Model() = default;

    void Forward();
};


#endif //MODEL_H
