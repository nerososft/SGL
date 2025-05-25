//
// Created by neo on 25-5-25.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <memory>

#include "gpu_engine/gpu/VkGPUBuffer.h"


class Matrix {
    uint32_t width = 0;
    uint32_t height = 0;
    std::shared_ptr<VkGPUBuffer> buffer = nullptr;

public:
    Matrix(uint32_t width, uint32_t height);

    Matrix(uint32_t width, uint32_t height, const std::shared_ptr<VkGPUBuffer> &buffer);

    ~Matrix();

    bool CreateMatrix(std::shared_ptr<VkGPUContext> &ctx);

    bool SetData(const std::vector<float> &data) const;

    std::shared_ptr<VkGPUBuffer> GetBuffer();

    void *GetDataAddr() const;
};


#endif //MATRIX_H
