//
// Created by neo on 25-5-7.
//

#include "MLEngine.h"

#include "core/log/Log.h"
#include "operators/impl/gpu/GELUOperator.h"
#include "operators/impl/gpu/LayerNormOperator.h"
#include "operators/impl/gpu/MatMulOperator.h"
#include "operators/impl/gpu/ReLUOperator.h"
#include "operators/impl/gpu/RMSNormOperator.h"
#include "operators/impl/gpu/SigmoidOperator.h"
#include "operators/impl/gpu/SiLUOperator.h"
#include "operators/impl/gpu/SoftmaxOperator.h"
#include "operators/impl/gpu/TanhOperator.h"
#include "operators/impl/cpu/AvgOperator.h"
#include "operators/impl/cpu/RMSOperator.h"
#include "operators/impl/cpu/SumOperator.h"
#include "operators/impl/cpu/VarianceOperator.h"
#include "operators/impl/gpu/AddOperator.h"
#include "operators/impl/gpu/ConcatOperator.h"
#include "operators/impl/gpu/MulOperator.h"
#include "operators/impl/gpu/RoPEMulOperator.h"
#include "operators/impl/gpu/RoPEOperator.h"
#include "operators/impl/gpu/ScaleOperator.h"
#include "operators/impl/gpu/SplitOperator.h"
#include "operators/impl/gpu/TransposeOperator.h"

std::shared_ptr<Sequence> MLEngine::Seq() {
    auto seq = std::make_shared<Sequence>(this->gpuCtx);
    seq->Init();
    return seq;
}

bool MLEngine::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
    const VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    Logger() << Logger::INFO << "Initialized Compute Engine, version: " << VERSION << std::endl;

    return true;
}

std::shared_ptr<Matrix> MLEngine::CreateMatrix(uint32_t width,
                                               uint32_t height) {
    auto matrix = std::make_shared<Matrix>(width, height);
    if (matrix == nullptr) {
        Logger() << Logger::ERROR << "Failed to allocate matrix!" << std::endl;
        return nullptr;
    }
    if (!matrix->CreateMatrix(this->gpuCtx)) {
        Logger() << Logger::ERROR << "Failed to create matrix!" << std::endl;
        return nullptr;
    }
    return matrix;
}

std::shared_ptr<Matrix> MLEngine::CreateMatrix(uint32_t width,
                                               uint32_t height,
                                               std::shared_ptr<VkGPUBuffer> &buffer) {
    auto matrix = std::make_shared<Matrix>(width, height, buffer);
    if (matrix == nullptr) {
        Logger() << Logger::ERROR << "Failed to allocate matrix!" << std::endl;
        return nullptr;
    }
    if (!matrix->CreateMatrix(this->gpuCtx)) {
        Logger() << Logger::ERROR << "Failed to create matrix!" << std::endl;
        return nullptr;
    }
    return matrix;
}

std::shared_ptr<Matrix> MLEngine::CreateMatrix(uint32_t width,
                                               uint32_t height,
                                               const std::vector<float> &data) {
    auto matrix = std::make_shared<Matrix>(width, height);
    if (matrix == nullptr) {
        Logger() << Logger::ERROR << "Failed to allocate matrix!" << std::endl;
        return nullptr;
    }
    if (!matrix->CreateMatrix(this->gpuCtx)) {
        Logger() << Logger::ERROR << "Failed to create matrix!" << std::endl;
        return nullptr;
    }
    if (!matrix->SetData(data)) {
        Logger() << Logger::ERROR << "Failed to set matrix data!" << std::endl;
        return nullptr;
    }
    return matrix;
}

std::shared_ptr<IComputeGraphNode> MLEngine::ReLU(const std::shared_ptr<Matrix> &input,
                                                  const std::shared_ptr<Matrix> &output) {
    const auto reluOp = std::make_shared<ReLUOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
    operators.push_back(reluOp);
    const auto node = reluOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}


std::shared_ptr<IComputeGraphNode> MLEngine::Sigmoid(const std::shared_ptr<Matrix> &input,
                                                     const std::shared_ptr<Matrix> &output) {
    const auto sigmoidOp = std::make_shared<SigmoidOperator>(this->gpuCtx,
                                                             input->GetBuffer(),
                                                             output->GetBuffer());
    operators.push_back(sigmoidOp);
    const auto node = sigmoidOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Tanh(const std::shared_ptr<Matrix> &input,
                                                  const std::shared_ptr<Matrix> &output) {
    const auto tanhOp = std::make_shared<TanhOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
    operators.push_back(tanhOp);
    const auto node = tanhOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Softmax(const std::shared_ptr<Matrix> &input,
                                                     const std::shared_ptr<Matrix> &output) {
    const auto sumOp = std::make_shared<SumOperator>(input->GetBuffer());
    sumOperators.push_back(sumOp);
    const auto sumNode = sumOp->CreateComputeGraphNode();
    if (sumNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto softmaxOp = std::make_shared<SoftmaxOperator>(this->gpuCtx,
                                                             input->GetBuffer(),
                                                             output->GetBuffer());
    this->operators.push_back(softmaxOp);
    softmaxOp->SetSum(sumOp->GetInnerSum());
    const auto node = softmaxOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    node->AddDependenceNode(sumNode);
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::GELU(const std::shared_ptr<Matrix> &input,
                                                  const std::shared_ptr<Matrix> &output) {
    const auto geluOp = std::make_shared<GELUOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
    operators.push_back(geluOp);
    const auto node = geluOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::SiLU(const std::shared_ptr<Matrix> &input,
                                                  const std::shared_ptr<Matrix> &output) {
    const auto siluOp = std::make_shared<SiLUOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
    operators.push_back(siluOp);
    const auto node = siluOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::MatMul(const std::shared_ptr<Matrix> &mat1,
                                                    const std::shared_ptr<Matrix> &mat2,
                                                    const std::shared_ptr<Matrix> &output) {
    if (mat1->GetWidth() != mat2->GetHeight()) {
        Logger() << "can not mul"
                << " mat with size (" << mat1->GetWidth() << "," << mat1->GetHeight() << ")"
                << " and mat with size (" << mat2->GetWidth() << "," << mat2->GetHeight() << ")!" << std::endl;
        throw std::runtime_error("can not mul");
    }
    const auto matMulOp = std::make_shared<MatMulOperator>(this->gpuCtx, mat1->GetBuffer(),
                                                           mat2->GetBuffer(),
                                                           output->GetBuffer());
    operators.push_back(matMulOp);
    matMulOp->SetMat1Size(mat1->GetWidth(), mat1->GetHeight());
    matMulOp->SetMat2Size(mat2->GetWidth(), mat2->GetHeight());
    const auto node = matMulOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::RoPE(const uint32_t ropeTheta,
                                                  uint32_t m,
                                                  const std::shared_ptr<Matrix> &vectorInput,
                                                  const std::shared_ptr<Matrix> &vectorOutput) {
    const auto ropeOp = std::make_shared<RoPEOperator>(this->gpuCtx,
                                                       vectorInput->GetBuffer(),
                                                       vectorOutput->GetBuffer());
    operators.push_back(ropeOp);
    ropeOp->SetRopeTheta(ropeTheta);
    ropeOp->SetM(m);
    auto node = ropeOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::ScaledDotProductAttention(const std::shared_ptr<Matrix> &Q,
                                                                       const std::shared_ptr<Matrix> &K,
                                                                       const uint64_t ropeTheta,
                                                                       const uint32_t m,
                                                                       const uint32_t n,
                                                                       const std::shared_ptr<Matrix> &qRoPEOutput,
                                                                       const std::shared_ptr<Matrix> &kRoPEOutput,
                                                                       const std::shared_ptr<Matrix> &qkDotProdOutput,
                                                                       const std::shared_ptr<Matrix> &
                                                                       qkDotProdScaleOutput,
                                                                       const std::shared_ptr<Matrix> &softmaxOutput,
                                                                       const std::shared_ptr<Matrix> &V,
                                                                       const std::shared_ptr<Matrix> &vMulOutput) {
    // TODO: IMPL ME
    return nullptr;
}

std::shared_ptr<IComputeGraphNode> MLEngine::RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
                                                     const float scale,
                                                     const float bias,
                                                     const float epsilon,
                                                     const std::shared_ptr<Matrix> &vectorOutput) {
    const auto rmsOp = std::make_shared<RMSOperator>(vectorInput->GetBuffer());
    rmsOperators.push_back(rmsOp);
    rmsOp->SetBias(bias);
    const auto rmsNode = rmsOp->CreateComputeGraphNode();
    if (rmsNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto rmsNormOp = std::make_shared<RMSNormOperator>(this->gpuCtx,
                                                             vectorInput->GetBuffer(),
                                                             vectorOutput->GetBuffer());
    operators.push_back(rmsNormOp);
    rmsNormOp->SetRMS(rmsOp->GetRms());
    rmsNormOp->SetScale(scale);
    rmsNormOp->SetEpsilon(epsilon);
    const auto node = rmsNormOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    node->AddDependenceNode(rmsNode);
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
                                                       const std::shared_ptr<Matrix> &weightInput,
                                                       const std::shared_ptr<Matrix> &biasInput,
                                                       const float epsilon,
                                                       const bool weightEnable,
                                                       const bool biasEnable,
                                                       const std::shared_ptr<Matrix> &vectorOutput) {
    const auto avgOp = std::make_shared<AvgOperator>(vectorInput->GetBuffer());
    avgOperators.push_back(avgOp);
    const auto avgNode = avgOp->CreateComputeGraphNode();
    if (avgNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto varianceOp = std::make_shared<VarianceOperator>(vectorInput->GetBuffer(), avgOp->GetAvg());
    variancesOperators.push_back(varianceOp);
    const auto varianceNode = varianceOp->CreateComputeGraphNode();
    if (varianceNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto layerNormOp = std::make_shared<LayerNormOperator>(this->gpuCtx,
                                                                 vectorInput->GetBuffer(),
                                                                 weightInput->GetBuffer(),
                                                                 biasInput->GetBuffer(),
                                                                 vectorOutput->GetBuffer());
    this->operators.push_back(layerNormOp);
    layerNormOp->SetAvg(avgOp->GetAvg());
    layerNormOp->SetVariance(varianceOp->GetVariance());

    layerNormOp->SetBiasEnable(biasEnable);
    layerNormOp->SetEpsilon(epsilon);
    layerNormOp->SetWeightEnable(weightEnable);
    const auto node = layerNormOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    varianceNode->AddDependenceNode(avgNode);
    node->AddDependenceNode(varianceNode);
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Split(const std::shared_ptr<Matrix> &vectorInput,
                                                   const uint64_t nums,
                                                   const std::vector<std::shared_ptr<Matrix> > &results) {
    std::vector<std::shared_ptr<VkGPUBuffer> > resultsVector;
    resultsVector.reserve(results.size());
    for (auto &mat: results) {
        resultsVector.push_back(mat->GetBuffer());
    }
    const auto splitOp = std::make_shared<SplitOperator>(this->gpuCtx,
                                                         vectorInput->GetBuffer(),
                                                         resultsVector);
    this->operators.push_back(splitOp);
    splitOp->SetDim(vectorInput->GetWidth() / nums);
    splitOp->SetNums(nums);
    const auto node = splitOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Concat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                                                    const std::shared_ptr<Matrix> &vectorOutput) {
    if (inputVectors.empty()) {
        Logger() << Logger::ERROR << "Empty input!" << std::endl;
        return nullptr;
    }
    if (inputVectors.size() * inputVectors[0]->GetWidth() != vectorOutput->GetWidth()) {
        Logger() << Logger::ERROR << "Input vector width mismatch(" << inputVectors.size() << " * " << inputVectors[0]->
                GetWidth() << ", " << vectorOutput->GetWidth() << ")!" << std::endl;
        return nullptr;
    }
    std::vector<std::shared_ptr<VkGPUBuffer> > inputBufferVector;
    inputBufferVector.reserve(inputVectors.size());
    for (auto &mat: inputVectors) {
        inputBufferVector.push_back(mat->GetBuffer());
    }
    const auto concatOp = std::make_shared<ConcatOperator>(this->gpuCtx,
                                                           inputBufferVector,
                                                           vectorOutput->GetBuffer());
    this->operators.push_back(concatOp);
    concatOp->SetBlockWidth(inputVectors[0]->GetWidth());
    concatOp->SetBlockHeight(inputVectors[0]->GetHeight());
    concatOp->SetWidth(vectorOutput->GetWidth());
    concatOp->SetHeight(vectorOutput->GetHeight());
    concatOp->SetNums(inputVectors.size());
    const auto node = concatOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Add(const std::shared_ptr<Matrix> &inputVector1,
                                                 const std::shared_ptr<Matrix> &inputVector2,
                                                 const std::shared_ptr<Matrix> &outputVector) {
    const auto addOp = std::make_shared<AddOperator>(this->gpuCtx,
                                                     inputVector1->GetBuffer(),
                                                     inputVector2->GetBuffer(),
                                                     outputVector->GetBuffer());
    operators.push_back(addOp);
    const auto node = addOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Sum(const std::shared_ptr<Matrix> &inputVector1) {
    const auto sumOp = std::make_shared<SumOperator>(inputVector1->GetBuffer());
    sumOperators.push_back(sumOp);
    const auto sumNode = sumOp->CreateComputeGraphNode();
    if (sumNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return sumNode;
}


std::shared_ptr<IComputeGraphNode> MLEngine::Sum(const std::shared_ptr<Matrix> &inputVector1, float *output) {
    const auto sumOp = std::make_shared<SumOperator>(inputVector1->GetBuffer());
    sumOperators.push_back(sumOp);
    sumOp->SetSum(output);
    const auto sumNode = sumOp->CreateComputeGraphNode();
    if (sumNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return sumNode;
}

std::shared_ptr<IComputeGraphNode> MLEngine::GatedSiLU(const std::shared_ptr<Matrix> &inputVector,
                                                       const std::shared_ptr<Matrix> &gateVector,
                                                       const std::shared_ptr<Matrix> &gateSigmoidOutput,
                                                       const std::shared_ptr<Matrix> &outputVector) {
    const auto sigmoidOp = std::make_shared<SigmoidOperator>(this->gpuCtx,
                                                             gateVector->GetBuffer(),
                                                             gateSigmoidOutput->GetBuffer());
    operators.push_back(sigmoidOp);
    const auto node = sigmoidOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto elementWiseMulOp = std::make_shared<MulOperator>(this->gpuCtx,
                                                                inputVector->GetBuffer(),
                                                                gateSigmoidOutput->GetBuffer(),
                                                                outputVector->GetBuffer());
    operators.push_back(elementWiseMulOp);
    const auto elementWiseMulNode = elementWiseMulOp->CreateComputeGraphNode();
    if (elementWiseMulNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    elementWiseMulNode->AddDependenceNode(node);
    return elementWiseMulNode;
}

std::shared_ptr<IComputeGraphNode> MLEngine::RoPEAndMul(const uint32_t ropeTheta,
                                                        const uint32_t m,
                                                        const uint32_t n,
                                                        const std::shared_ptr<Matrix> &Q,
                                                        const std::shared_ptr<Matrix> &K,
                                                        const std::shared_ptr<Matrix> &dotProdOutput) {
    assert(Q != nullptr);
    assert(K != nullptr);
    assert(dotProdOutput != nullptr);
    const auto ropeMulOp = std::make_shared<RoPEMulOperator>(this->gpuCtx,
                                                             Q->GetBuffer(),
                                                             K->GetBuffer(),
                                                             dotProdOutput->GetBuffer());
    operators.push_back(ropeMulOp);
    ropeMulOp->SetRopeTheta(ropeTheta);
    ropeMulOp->SetM(m);
    ropeMulOp->SetN(n);
    auto node = ropeMulOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}

std::shared_ptr<IComputeGraphNode> MLEngine::RoPEDotProduct(const uint32_t ropeTheta,
                                                            const uint32_t m,
                                                            const uint32_t n,
                                                            const std::shared_ptr<Matrix> &Q,
                                                            const std::shared_ptr<Matrix> &K,
                                                            const std::shared_ptr<Matrix> &dotProdOutput,
                                                            float *output) {
    const auto ropeDotProdMulNode = RoPEAndMul(ropeTheta, m, n, Q, K, dotProdOutput);
    if (ropeDotProdMulNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto sumOp = std::make_shared<SumOperator>(dotProdOutput->GetBuffer());
    sumOp->SetSum(output);
    sumOperators.push_back(sumOp);
    const auto sumNode = sumOp->CreateComputeGraphNode();
    if (sumNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    sumNode->AddDependenceNode(ropeDotProdMulNode);
    return sumNode;
}

std::shared_ptr<IComputeGraphNode> MLEngine::Transpose(const std::shared_ptr<Matrix> &inputMatrix,
                                                       const std::shared_ptr<Matrix> &outputMatrix) {
    const auto transposeOp = std::make_shared<TransposeOperator>(this->gpuCtx, inputMatrix->GetBuffer(),
                                                                 outputMatrix->GetBuffer());
    operators.push_back(transposeOp);
    transposeOp->SetMatSize(inputMatrix->GetWidth(), inputMatrix->GetHeight());
    const auto node = transposeOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    return node;
}
