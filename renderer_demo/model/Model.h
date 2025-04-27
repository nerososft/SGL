//
// Created by neo on 25-4-27.
//

#ifndef MODEL_H
#define MODEL_H
#include <memory>

#include "gpu_engine/gpu/VkGPUBuffer.h"


class Model {
    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> indexBuffer = nullptr;

public:
    Model() = default;

    ~Model() = default;
};


#endif //MODEL_H
