//
// Created by neo on 25-5-25.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <memory>

#include "core/gpu/VkGPUBuffer.h"


class Matrix {
    uint32_t width = 0;
    uint32_t height = 0;
    std::shared_ptr<VkGPUBuffer> buffer = nullptr;

public:
    Matrix(uint32_t width, uint32_t height);

    Matrix(uint32_t width, uint32_t height, const std::shared_ptr<VkGPUBuffer> &buffer);

    ~Matrix();

    bool CreateMatrix(std::shared_ptr<VkGPUContext> &ctx);

    [[nodiscard]] bool SetData(const std::vector<float> &data) const;

    std::shared_ptr<VkGPUBuffer> GetBuffer();

    [[nodiscard]] void *GetDataAddr() const;

    [[nodiscard]] uint32_t GetWidth() const { return this->width; }

    [[nodiscard]] uint32_t GetHeight() const { return this->height; }

    void Print() const;
};


#endif //MATRIX_H
