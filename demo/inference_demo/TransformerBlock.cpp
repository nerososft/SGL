//
// Created by neo on 25-6-24.
//

#include "TransformerBlock.h"

TransformerBlock::TransformerBlock(uint64_t layerIdx) {
    this->layerIndex = layerIdx;
}

bool TransformerBlock::Init(const std::shared_ptr<SafeTensor> &safeTensor) {

}
