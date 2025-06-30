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
#include "operators/IOperator.h"

class MLEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> graph = nullptr;
    std::shared_ptr<SubComputeGraph> mainSubGraph = nullptr;

    std::vector<std::shared_ptr<VkGPUBuffer> > buffers;
    std::vector<std::shared_ptr<IOperator> > operators;

public:
    MLEngine() = default;

    ~MLEngine() = default;

    bool Init();

    void Compute() const;

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         std::shared_ptr<VkGPUBuffer> &buffer);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         const std::vector<float> &data);

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

    void SiLU(const std::shared_ptr<Matrix> &input,
              const std::shared_ptr<Matrix> &output);

    void MatMul(const std::shared_ptr<Matrix> &mat1,
                const std::shared_ptr<Matrix> &mat2,
                const std::shared_ptr<Matrix> &output);

    /*
     * Attention(Q, K, V) = softmax(Q·K^T/√d)·V
     */
    void ScaledDotProductAttention(const std::shared_ptr<Matrix> &Q,
                                   const std::shared_ptr<Matrix> &K,
                                   const std::shared_ptr<Matrix> &qkDotProdOutput,
                                   const std::shared_ptr<Matrix> &qkDotProdScaleOutput,
                                   const std::shared_ptr<Matrix> &softmaxOutput,
                                   const std::shared_ptr<Matrix> &V,
                                   const std::shared_ptr<Matrix> &vMulOutput);

    void RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
                 float scale,
                 float bias,
                 float epsilon,
                 const std::shared_ptr<Matrix> &vectorOutput);

    void LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
                   const std::shared_ptr<Matrix> &weightInput,
                   const std::shared_ptr<Matrix> &biasInput,
                   float epsilon,
                   bool weightEnable,
                   bool biasEnable,
                   const std::shared_ptr<Matrix> &vectorOutput);

    void Split(const std::shared_ptr<Matrix> &vectorInput,
               uint64_t nums,
               const std::vector<std::shared_ptr<Matrix> > &results);

    void Concat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                const std::shared_ptr<Matrix> &vectorOutput);

    void DupConcat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                   size_t dup,
                   const std::shared_ptr<Matrix> &vectorOutput);
};


#endif //MLENGINE_H
