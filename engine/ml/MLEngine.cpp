//
// Created by neo on 25-5-7.
//

#include "MLEngine.h"

#include "core/log/Log.h"
#include "operators/impl/GELUOperator.h"
#include "operators/impl/LayerNormOperator.h"
#include "operators/impl/MatMulOperator.h"
#include "operators/impl/ReLUOperator.h"
#include "operators/impl/RMSNormOperator.h"
#include "operators/impl/SigmoidOperator.h"
#include "operators/impl/SiLUOperator.h"
#include "operators/impl/SoftmaxOperator.h"
#include "operators/impl/TanhOperator.h"

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

// TODO: this should be CPU Compute Node
float MLEngine::SUM(const std::shared_ptr<Matrix> &vectorInput) {
    const auto inputBuffer = vectorInput->GetBuffer();
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        return 0.0f;
    }
    float sum = 0.0f;
    for (size_t i = 0; i < inputBuffer->GetBufferSize() / sizeof(float); i++) {
        sum += static_cast<float *>(inputBuffer->GetMappedAddr())[i];
    }
    inputBuffer->UnMapBuffer();
    return sum;
}

// TODO: this should be CPU Compute Node
float MLEngine::RMS(const std::shared_ptr<Matrix> &vectorInput, const float bias) {
    const auto inputBuffer = vectorInput->GetBuffer();
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        return 0.0f;
    }
    float sum = 0.0f;
    const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
    for (size_t i = 0; i < nums; i++) {
        sum += pow(static_cast<float *>(inputBuffer->GetMappedAddr())[i], 2.0f);
    }
    inputBuffer->UnMapBuffer();

    return sqrt(sum / nums + bias);
}

// TODO: this should be CPU Compute Node
float MLEngine::Avg(const std::shared_ptr<Matrix> &vectorInput) {
    const auto inputBuffer = vectorInput->GetBuffer();
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        return 0.0f;
    }
    float sum = 0.0f;
    const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
    for (size_t i = 0; i < nums; i++) {
        sum += static_cast<float *>(inputBuffer->GetMappedAddr())[i];
    }
    inputBuffer->UnMapBuffer();

    return sqrt(sum / nums);
}

// TODO: this should be CPU Compute Node
float MLEngine::Variance(const std::shared_ptr<Matrix> &vectorInput, const float avg) {
    const auto inputBuffer = vectorInput->GetBuffer();
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        return 0.0f;
    }
    float sum = 0.0f;
    const size_t nums = inputBuffer->GetBufferSize() / sizeof(float);
    for (size_t i = 0; i < nums; i++) {
        sum += pow(static_cast<float *>(inputBuffer->GetMappedAddr())[i] - avg, 2.0);
    }
    inputBuffer->UnMapBuffer();

    return sqrt(sum / nums);
}

void MLEngine::ReLU(const std::shared_ptr<Matrix> &input,
                    const std::shared_ptr<Matrix> &output) {
    const auto reluOp = std::make_shared<ReLUOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
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
    const auto node = tanhOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Softmax(const std::shared_ptr<Matrix> &input,
                       const std::shared_ptr<Matrix> &output) {
    const auto softmaxOp = std::make_shared<SoftmaxOperator>(this->gpuCtx,
                                                             input->GetBuffer(),
                                                             output->GetBuffer());
    const float sum = SUM(input);
    softmaxOp->SetSum(sum);
    const auto node = softmaxOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::GELU(const std::shared_ptr<Matrix> &input,
                    const std::shared_ptr<Matrix> &output) {
    const auto geluOp = std::make_shared<GELUOperator>(this->gpuCtx,
                                                       input->GetBuffer(),
                                                       output->GetBuffer());
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
    matMulOp->SetMat1Size(mat1->GetWidth(), mat1->GetHeight());
    matMulOp->SetMat2Size(mat2->GetWidth(), mat2->GetHeight());
    const auto node = matMulOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::SelfAttention(const std::shared_ptr<Matrix> &Q,
                             const std::shared_ptr<Matrix> &K,
                             const std::shared_ptr<Matrix> &qkMulOutput,
                             const std::shared_ptr<Matrix> &softmaxOutput,
                             const std::shared_ptr<Matrix> &V,
                             const std::shared_ptr<Matrix> &vMulOutput) {
    if (Q->GetWidth() != V->GetHeight()) {
        Logger() << "can not mul"
                << " mat with size (" << Q->GetWidth() << "," << Q->GetHeight() << ")"
                << " and mat with size (" << V->GetWidth() << "," << V->GetHeight() << ")!" << std::endl;
        return;
    }
    if (qkMulOutput == nullptr) {
        Logger() << Logger::ERROR << "Failed to create Q K mul result matrix!" << std::endl;
        return;
    }

    const auto qkMulOp = std::make_shared<MatMulOperator>(this->gpuCtx,
                                                          Q->GetBuffer(),
                                                          K->GetBuffer(),
                                                          qkMulOutput->GetBuffer());
    qkMulOp->SetMat1Size(Q->GetWidth(), Q->GetHeight());
    qkMulOp->SetMat2Size(K->GetWidth(), K->GetHeight());
    const auto qkMulNode = qkMulOp->CreateComputeGraphNode();
    if (qkMulNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    // TODO: scale qkMatMulOutput
    // TODO: mask qkMatMulOutput
    const auto softmaxOp = std::make_shared<SoftmaxOperator>(this->gpuCtx,
                                                             qkMulOutput->GetBuffer(),
                                                             softmaxOutput->GetBuffer());
    const float sum = SUM(qkMulOutput);
    softmaxOp->SetSum(sum);
    const auto softmaxNode = softmaxOp->CreateComputeGraphNode();
    if (softmaxNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    const auto vMulOp = std::make_shared<MatMulOperator>(this->gpuCtx,
                                                         softmaxOutput->GetBuffer(),
                                                         V->GetBuffer(),
                                                         vMulOutput->GetBuffer());
    vMulOp->SetMat1Size(softmaxOutput->GetWidth(), softmaxOutput->GetHeight());
    vMulOp->SetMat2Size(V->GetWidth(), V->GetHeight());
    const auto vMulNode = vMulOp->CreateComputeGraphNode();
    if (vMulNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }

    softmaxNode->AddDependenceNode(qkMulNode);
    // TODO: scale and mask node
    vMulNode->AddDependenceNode(softmaxNode);
    this->mainSubGraph->AddComputeGraphNode(vMulNode);
}

void MLEngine::RMSNorm(const std::shared_ptr<Matrix> &vectorInput,
                       const float scale,
                       const float bias,
                       const float epsilon,
                       const std::shared_ptr<Matrix> &vectorOutput) {
    const float rms = RMS(vectorInput, bias);
    const auto rmsNormOp = std::make_shared<RMSNormOperator>(this->gpuCtx,
                                                             vectorInput->GetBuffer(),
                                                             vectorOutput->GetBuffer());
    rmsNormOp->SetRMS(rms);
    rmsNormOp->SetScale(scale);
    rmsNormOp->SetEpsilon(epsilon);
    const auto node = rmsNormOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::LayerNorm(const std::shared_ptr<Matrix> &vectorInput,
                         float scale,
                         float epsilon,
                         float bias,
                         const std::shared_ptr<Matrix> &vectorOutput) {
    float avg = Avg(vectorInput);
    float variance = Variance(vectorInput, avg);
    const auto layerNormOp = std::make_shared<LayerNormOperator>(this->gpuCtx,
                                                                 vectorInput->GetBuffer(),
                                                                 vectorOutput->GetBuffer());
    layerNormOp->SetAvg(avg);
    layerNormOp->SetVariance(variance);
    layerNormOp->SetScale(scale);
    layerNormOp->SetEpsilon(epsilon);
    layerNormOp->SetBias(bias);
    const auto node = layerNormOp->CreateComputeGraphNode();
    if (node == nullptr) {
        Logger() << Logger::ERROR << "Failed to create compute graph node!" << std::endl;
        throw std::runtime_error("Failed to create compute graph node!");
    }
    this->mainSubGraph->AddComputeGraphNode(node);
}

void MLEngine::Compute() const {
    const VkResult result = this->graph->Compute();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to compute graph!" << std::endl;
    }
}
