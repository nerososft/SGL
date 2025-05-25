//
// Created by neo on 25-5-7.
//

#ifndef MLENGINE_H
#define MLENGINE_H
#include <memory>

#include "Matrix.h"
#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"

class MLEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> graph = nullptr;
    std::shared_ptr<SubComputeGraph> mainSubGraph = nullptr;

    std::vector<std::shared_ptr<VkGPUBuffer> > buffers;

public:
    MLEngine() = default;

    bool Init();

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width, uint32_t height);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width, uint32_t height, std::shared_ptr<VkGPUBuffer> &buffer);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width, uint32_t height, const std::vector<float> &data);

    void ReLU(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void Sigmoid(const std::shared_ptr<Matrix> &input,
                 const std::shared_ptr<Matrix> &output);

    void Tanh(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void MatMul(const std::shared_ptr<Matrix> &mat1,
                const std::shared_ptr<Matrix> &mat2,
                const std::shared_ptr<Matrix> &output);


    ~MLEngine() = default;

    void Compute() const;
};


#endif //MLENGINE_H
