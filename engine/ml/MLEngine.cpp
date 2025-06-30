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
#include "operators/impl/gpu/DotProdOperator.h"
#include "operators/impl/gpu/ScaleOperator.h"
#include "operators/impl/gpu/SplitOperator.h"

bool MLEngine::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
    VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    Logger() << Logger::INFO << "Initialized Compute Engine, version: " << VERSION << std::endl;

    this->mainSubGraph = std::make_shared<SubComputeGraph>(this->gpuCtx);
    if (this->mainSubGraph == nullptr) {
        Logger() << Logger::ERROR << "Failed to initialize SubComputeGraph!" << std::endl;
        return false;
    }
    result = this->mainSubGraph->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize main SubComputeGraph!" << std::endl;
        return false;
    }

    this->graph = std::make_shared<ComputeGraph>(this->gpuCtx);
    if (this->graph == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph!" << std::endl;
        return false;
    }
    this->graph->AddSubGraph(this->mainSubGraph);

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

void MLEngine::ReLU(const std::shared_ptr<Matrix> &input,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}


void MLEngine::Sigmoid(const std::shared_ptr<Matrix> &input,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Tanh(const std::shared_ptr<Matrix> &input,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Softmax(const std::shared_ptr<Matrix> &input,
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
    softmaxOp->SetSum(sumOp->GetSum());
    const auto node = softmaxOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    node->AddDependenceNode(sumNode);
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::GELU(const std::shared_ptr<Matrix> &input,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::SiLU(const std::shared_ptr<Matrix> &input,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::MatMul(const std::shared_ptr<Matrix> &mat1,
                      const std::shared_ptr<Matrix> &mat2,
                      const std::shared_ptr<Matrix> &output) {
    if (mat1->GetWidth() != mat2->GetHeight()) {
        Logger() << "can not mul"
                << " mat with size (" << mat1->GetWidth() << "," << mat1->GetHeight() << ")"
                << " and mat with size (" << mat2->GetWidth() << "," << mat2->GetHeight() << ")!" << std::endl;
        return;
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::ScaledDotProductAttention(const std::shared_ptr<Matrix> &Q,
                                         const std::shared_ptr<Matrix> &K,
                                         const std::shared_ptr<Matrix> &qkDotProdOutput,
                                         const std::shared_ptr<Matrix> &qkDotProdScaleOutput,
                                         const std::shared_ptr<Matrix> &softmaxOutput,
                                         const std::shared_ptr<Matrix> &V,
                                         const std::shared_ptr<Matrix> &vMulOutput) {
    const auto qkDotProdOp = std::make_shared<DotProdOperator>(this->gpuCtx,
                                                               Q->GetBuffer(),
                                                               K->GetBuffer(),
                                                               qkDotProdOutput->GetBuffer());
    operators.push_back(qkDotProdOp);
    const auto qkDotProdNode = qkDotProdOp->CreateComputeGraphNode();
    if (qkDotProdNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    // Scale
    const auto scaleOp = std::make_shared<ScaleOperator>(this->gpuCtx,
                                                         qkDotProdOutput->GetBuffer(),
                                                         qkDotProdScaleOutput->GetBuffer());
    operators.push_back(scaleOp);
    scaleOp->SetDelta(K->GetHeight() * K->GetWidth());
    const auto scaleNode = scaleOp->CreateComputeGraphNode();
    if (qkDotProdNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    // Softmax
    const auto sumOp = std::make_shared<SumOperator>(qkDotProdScaleOutput->GetBuffer());
    sumOperators.push_back(sumOp);
    const auto sumNode = sumOp->CreateComputeGraphNode();
    if (sumNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto softmaxOp = std::make_shared<SoftmaxOperator>(this->gpuCtx,
                                                             qkDotProdScaleOutput->GetBuffer(),
                                                             softmaxOutput->GetBuffer());
    operators.push_back(softmaxOp);
    softmaxOp->SetSum(sumOp->GetSum());
    const auto softmaxNode = softmaxOp->CreateComputeGraphNode();
    if (softmaxNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto vMulOp = std::make_shared<MatMulOperator>(this->gpuCtx,
                                                         softmaxOutput->GetBuffer(),
                                                         V->GetBuffer(),
                                                         vMulOutput->GetBuffer());
    operators.push_back(vMulOp);
    vMulOp->SetMat1Size(softmaxOutput->GetWidth(), softmaxOutput->GetHeight());
    vMulOp->SetMat2Size(V->GetHeight(), V->GetWidth()); // Transpose
    const auto vMulNode = vMulOp->CreateComputeGraphNode();
    if (vMulNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    scaleNode->AddDependenceNode(qkDotProdNode);
    sumNode->AddDependenceNode(scaleNode);
    softmaxNode->AddDependenceNode(sumNode);
    vMulNode->AddDependenceNode(softmaxNode);
    this->mainSubGraph->AddComputeGraphNode(vMulNode);
}

void MLEngine::RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
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
    // avoid to optimize  out, because avg and variance will be use in layerNorm node precompute
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Compute() const {
    const VkResult result = this->graph->Compute();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to compute graph!" << std::endl;
    }
}

void MLEngine::Split(const std::shared_ptr<Matrix> &vectorInput,
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
    // avoid to optimize  out, because avg and variance will be use in layerNorm node precompute
    splitOp->SetDim(vectorInput->GetWidth() / nums);
    splitOp->SetNums(nums);
    const auto node = splitOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Concat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                      const std::shared_ptr<Matrix> &vectorOutput) {
    this->DupConcat(inputVectors, 1, vectorOutput);
}

void MLEngine::DupConcat(const std::vector<std::shared_ptr<Matrix> > &inputVectors,
                         const size_t dup,
                         const std::shared_ptr<Matrix> &vectorOutput) {
    std::vector<std::shared_ptr<VkGPUBuffer> > inputBufferVector;
    inputBufferVector.reserve(inputVectors.size());
    for (auto &mat: inputVectors) {
        inputBufferVector.push_back(mat->GetBuffer());
    }
    const auto concatOp = std::make_shared<ConcatOperator>(this->gpuCtx,
                                                           inputBufferVector,
                                                           vectorOutput->GetBuffer());
    this->operators.push_back(concatOp);
    // avoid to optimize  out, because avg and variance will be use in layerNorm node precompute
    concatOp->SetDim(inputVectors[0]->GetWidth());
    concatOp->SetNums(inputVectors.size());
    concatOp->SetDup(dup);
    const auto node = concatOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Add(const std::shared_ptr<Matrix> &inputVector1,
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
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::GatedSiLU(const std::shared_ptr<Matrix> &inputVector,
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

    const auto dotProdOp = std::make_shared<DotProdOperator>(this->gpuCtx,
                                                             inputVector->GetBuffer(),
                                                             gateSigmoidOutput->GetBuffer(),
                                                             outputVector->GetBuffer());
    operators.push_back(dotProdOp);
    const auto dotProdNode = dotProdOp->CreateComputeGraphNode();
    if (dotProdNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    dotProdNode->AddDependenceNode(node);
    this->mainSubGraph->AddComputeGraphNode(dotProdNode);
}
