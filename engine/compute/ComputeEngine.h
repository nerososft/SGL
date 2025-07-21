//
// Created by neo on 25-5-7.
//

#ifndef COMPUTE_ENGINE_H
#define COMPUTE_ENGINE_H
#include "Matrix.h"
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/VkGPUContext.h"
#include "core/gpu/compute_graph/ComputeGraph.h"
#include "core/gpu/compute_graph/Sequence.h"
#include "operators/IOperator.h"
#include "operators/impl/cpu/AvgOperator.h"
#include "operators/impl/cpu/ExpSumOperator.h"
#include "operators/impl/cpu/MaxOperator.h"
#include "operators/impl/cpu/RMSOperator.h"
#include "operators/impl/cpu/SumOperator.h"
#include "operators/impl/cpu/VarianceOperator.h"
#include <memory>

class ComputeEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

    std::vector<std::shared_ptr<VkGPUBuffer>> buffers;

    // for lifecycle control
    std::vector<std::shared_ptr<IOperator> > operators;
    std::vector<std::shared_ptr<AvgOperator> > avgOperators;
    std::vector<std::shared_ptr<VarianceOperator> > variancesOperators;
    std::vector<std::shared_ptr<SumOperator> > sumOperators;
    std::vector<std::shared_ptr<MaxOperator> > maxOperators;
    std::vector<std::shared_ptr<ExpSumOperator> > expSumOperators;
    std::vector<std::shared_ptr<RMSOperator> > rmsOperators;

public:
    ComputeEngine() = default;

    ~ComputeEngine() = default;

    std::shared_ptr<Sequence> Seq();

    bool Init();

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         std::shared_ptr<VkGPUBuffer> &buffer);

    std::shared_ptr<Matrix> CreateMatrix(uint32_t width,
                                         uint32_t height,
                                         const std::vector<float> &data);

    std::shared_ptr<IComputeGraphNode> ReLU(const std::shared_ptr<Matrix> &input,
                                            const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> Sigmoid(const std::shared_ptr<Matrix> &input,
                                               const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> Tanh(const std::shared_ptr<Matrix> &input,
                                            const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> Softmax(const std::shared_ptr<Matrix> &input,
                                               const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> GELU(const std::shared_ptr<Matrix> &input,
                                            const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> SiLU(const std::shared_ptr<Matrix> &input,
                                            const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> MatMul(const std::shared_ptr<Matrix> &mat1,
                                              const std::shared_ptr<Matrix> &mat2,
                                              const std::shared_ptr<Matrix> &output);

    std::shared_ptr<IComputeGraphNode> RoPE(uint32_t ropeTheta,
                                            uint32_t m,
                                            const std::shared_ptr<Matrix> &vectorInput,
                                            const std::shared_ptr<Matrix> &vectorOutput);

    std::shared_ptr<IComputeGraphNode> RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
                                               float scale,
                                               float bias,
                                               float epsilon,
                                               const std::shared_ptr<Matrix> &vectorOutput);

    std::shared_ptr<IComputeGraphNode> LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
                                                 const std::shared_ptr<Matrix> &weightInput,
                                                 const std::shared_ptr<Matrix> &biasInput,
                                                 float epsilon,
                                                 bool weightEnable,
                                                 bool biasEnable,
                                                 const std::shared_ptr<Matrix> &vectorOutput);

    std::shared_ptr<IComputeGraphNode> Split(const std::shared_ptr<Matrix> &vectorInput,
                                             uint64_t nums,
                                             const std::vector<std::shared_ptr<Matrix> > &results);

    std::shared_ptr<IComputeGraphNode> Concat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                                              const std::shared_ptr<Matrix> &vectorOutput);

    std::shared_ptr<IComputeGraphNode> Add(const std::shared_ptr<Matrix> &inputVector1,
                                           const std::shared_ptr<Matrix> &inputVector2,
                                           const std::shared_ptr<Matrix> &outputVector);

    std::shared_ptr<IComputeGraphNode> Sum(const std::shared_ptr<Matrix> &inputVector1);

    std::shared_ptr<IComputeGraphNode> Sum(const std::shared_ptr<Matrix> &inputVector1,
                                           float *output);

    /*
     * GatedSiLU(up,gate)=up⋅sigmod(gate)
     */
    std::shared_ptr<IComputeGraphNode> GatedSiLU(const std::shared_ptr<Matrix> &inputVector,
                                                 const std::shared_ptr<Matrix> &gateVector,
                                                 const std::shared_ptr<Matrix> &gateSigmoidOutput,
                                                 const std::shared_ptr<Matrix> &outputVector);

    std::shared_ptr<IComputeGraphNode> RoPEAndMul(uint32_t ropeTheta,
                                                  uint32_t m,
                                                  uint32_t n,
                                                  const std::shared_ptr<Matrix> &Q,
                                                  const std::shared_ptr<Matrix> &K,
                                                  const std::shared_ptr<Matrix> &dotProdOutput);

    std::shared_ptr<IComputeGraphNode> RoPEDotProduct(uint32_t ropeTheta,
                                                      uint32_t m,
                                                      uint32_t n,
                                                      const std::shared_ptr<Matrix> &Q,
                                                      const std::shared_ptr<Matrix> &K,
                                                      const std::shared_ptr<Matrix> &dotProdOutput,
                                                      float *output);

    std::shared_ptr<IComputeGraphNode> Transpose(const std::shared_ptr<Matrix> &inputMatrix,
                                                 const std::shared_ptr<Matrix> &outputMatrix);

    std::shared_ptr<IComputeGraphNode> LogSoftmax(const std::shared_ptr<Matrix> &input,
                                                  const std::shared_ptr<Matrix> &output);

    /*
     * Math operatprs
     */
    std::shared_ptr<IComputeGraphNode> Sin(const std::shared_ptr<Matrix>& input, const std::shared_ptr<Matrix>& output);
    std::shared_ptr<IComputeGraphNode> Cos(const std::shared_ptr<Matrix>& input, const std::shared_ptr<Matrix>& output);
    std::shared_ptr<IComputeGraphNode> Exp(const std::shared_ptr<Matrix>& input, const std::shared_ptr<Matrix>& output);
};


#endif //COMPUTE_ENGINE_H
