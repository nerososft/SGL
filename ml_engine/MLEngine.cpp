//
// Created by neo on 25-5-7.
//

#include "MLEngine.h"

#include "gpu_engine/log/Log.h"
#include "operators/impl/GELUOperator.h"
#include "operators/impl/MatMulOperator.h"
#include "operators/impl/ReLUOperator.h"
#include "operators/impl/SigmoidOperator.h"
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
    const auto inputBuffer = input->GetBuffer();
    if (inputBuffer->MapBuffer(inputBuffer->GetBufferSize()) != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to map buffer!" << std::endl;
        return;
    }
    float sum = 0.0f;
    for (size_t i = 0; i < inputBuffer->GetBufferSize() / sizeof(float); i++) {
        sum += static_cast<float *>(inputBuffer->GetMappedAddr())[i];
    }
    inputBuffer->UnMapBuffer();
    const auto softmaxOp = std::make_shared<SoftmaxOperator>(this->gpuCtx,
                                                             input->GetBuffer(),
                                                             output->GetBuffer());
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

void MLEngine::Compute() const {
    const VkResult result = this->graph->Compute();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to compute graph!" << std::endl;
    }
}
