//
// Created by neo on 25-6-24.
//

#ifndef TRANSFORMERBLOCK_H
#define TRANSFORMERBLOCK_H
#include <cstdint>
#include <memory>

#include "engine/ml/Matrix.h"


class TransformerBlock {
    uint64_t layerIndex = 0;

    // Multi-head attention
    const std::shared_ptr<Matrix> Q = nullptr;
    const std::shared_ptr<Matrix> K = nullptr;
    const std::shared_ptr<Matrix> qkMulOutput = nullptr;
    const std::shared_ptr<Matrix> softmaxOutput = nullptr;
    const std::shared_ptr<Matrix> V = nullptr;
    const std::shared_ptr<Matrix> vMulOutput = nullptr;

    const std::shared_ptr<Matrix> add1Output = nullptr; // multi head attention 残差连接

    const std::shared_ptr<Matrix> layerNormal1Output = nullptr;

    // ffn
    const std::shared_ptr<Matrix> linearLayer1Output = nullptr;
    const std::shared_ptr<Matrix> siluOutput = nullptr;
    const std::shared_ptr<Matrix> linearLayer2Output = nullptr;

    const std::shared_ptr<Matrix> add2Output = nullptr; // FFN 残差连接

    const std::shared_ptr<Matrix> layerNormal2Output = nullptr;

public:
    explicit TransformerBlock(uint64_t layerIdx);

    ~TransformerBlock() = default;
};


#endif //TRANSFORMERBLOCK_H
