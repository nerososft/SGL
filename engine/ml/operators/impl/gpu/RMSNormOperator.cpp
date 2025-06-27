//
// Created by neo on 25-6-19.
//

#include "RMSNormOperator.h"

#include <cassert>

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

RMSNormOperator::RMSNormOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                                 const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

RMSNormOperator::~RMSNormOperator() {
}

std::shared_ptr<IComputeGraphNode> RMSNormOperator::CreateComputeGraphNode() {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto rmsNormNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                             "RSMNorm",
                                                             SHADER(rms_norm.comp.glsl.spv),
                                                             0,
                                                             descriptorSetLayoutBindings,
                                                             (nums + 255) / 256,
                                                             1,
                                                             1);
    const VkResult ret = rmsNormNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating RSMNorm node." << std::endl;
        return nullptr;
    }
    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back({
        .type = PIPELINE_NODE_BUFFER_STORAGE_READ,
        .buf = {
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
        .size = sizeof(RMSNormOperatorParams),
        .data = &this->params
    };
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
        .preCompute = [=] {
            assert(this != nullptr); // if null, means optimized out
            assert(this->rms != nullptr);
            this->params.rms = *this->rms;
        }
    };
    rmsNormNode->AddComputeElement(computeElem);

    return rmsNormNode;
}

void RMSNormOperator::Destroy() {
    UnaryOperator::Destroy();
}
