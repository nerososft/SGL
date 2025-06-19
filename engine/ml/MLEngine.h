//
// Created by neo on 25-5-7.
//

#ifndef MLENGINE_H
#define MLENGINE_H
#include <memory>

#include "Matrix.h"
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "core/gpu/compute_graph/ComputeGraph.h"

class MLEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> graph = nullptr;
    std::shared_ptr<SubComputeGraph> mainSubGraph = nullptr;

    std::vector<std::shared_ptr<VkGPUBuffer> > buffers;

public:
    MLEngine() = default;

    bool Init();

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         std::shared_ptr<VkGPUBuffer> &buffer);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         const std::vector<float> &data);

    float SUM(const std::shared_ptr<Matrix> &vectorInput);

    static float RMS(const std::shared_ptr<Matrix> &vectorInput,
                     float bias);

    float Avg(const std::shared_ptr<Matrix> &vectorInput);

    float Variance(const std::shared_ptr<Matrix> &vectorInput,
                   float avg);

    void ReLU(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void Sigmoid(const std::shared_ptr<Matrix> &input,
                 const std::shared_ptr<Matrix> &output);

    void Tanh(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void Softmax(const std::shared_ptr<Matrix> &input,
                 const std::shared_ptr<Matrix> &output);

    void GELU(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void MatMul(const std::shared_ptr<Matrix> &mat1,
                const std::shared_ptr<Matrix> &mat2,
                const std::shared_ptr<Matrix> &output);

    void SelfAttention(const std::shared_ptr<Matrix> &Q,
                       const std::shared_ptr<Matrix> &K,
                       const std::shared_ptr<Matrix> &qkMulOutput,
                       const std::shared_ptr<Matrix> &softmaxOutput,
                       const std::shared_ptr<Matrix> &V,
                       const std::shared_ptr<Matrix> &vMulOutput);

    void RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
                 float scale,
                 float epsilon,
                 float bias,
                 const std::shared_ptr<Matrix> &vectorOutput);

    void LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
                   float scale,
                   float epsilon,
                   float bias,
                   const std::shared_ptr<Matrix> &vectorOutput);

    ~MLEngine() = default;

    void Compute() const;
};


#endif //MLENGINE_H
