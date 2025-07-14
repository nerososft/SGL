//
// Created by neo on 25-7-9.
//

#include "LogSoftmaxOperator.h"

#include <cassert>

#include "core/config.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

LogSoftmaxOperator::LogSoftmaxOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                       const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                                       const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

LogSoftmaxOperator::~LogSoftmaxOperator() = default;

std::shared_ptr<IComputeGraphNode> LogSoftmaxOperator::CreateComputeGraphNode() {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto logSoftmaxNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                                "LogSoftmax",
                                                                SHADER(logsoftmax.comp.glsl.spv),
                                                                sizeof(LogSoftmaxOperatorParams),
                                                                descriptorSetLayoutBindings,
                                                                (nums + 255) / 256,
                                                                1,
                                                                1);
    const VkResult ret = logSoftmaxNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating softmax node." << std::endl;
        return nullptr;
    }
    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .buf{
            .bufferSize = this->inputBuffer->GetBufferSize(),
            .buffer = this->inputBuffer->GetBuffer(),
        }
    });
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
        .buf = {
            .bufferSize = this->outputBuffer->GetBufferSize(),
            .buffer = this->outputBuffer->GetBuffer(),
        }
    });

    const PushConstantInfo pushConstantInfo{
        .size = sizeof(LogSoftmaxOperatorParams),
        .data = &this->params
    };
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
        .preCompute = [this] {
            assert(this->sum != nullptr);
            this->params.sum = *this->sum;
        }
    };
    logSoftmaxNode->AddComputeElement(computeElem);
    return logSoftmaxNode;
}

void LogSoftmaxOperator::Destroy() {
    UnaryOperator::Destroy();
}
