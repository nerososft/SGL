//
// Created by neo on 25-6-28.
//

#include "SplitOperator.h"

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

SplitOperator::SplitOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                             const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                             const std::vector<std::shared_ptr<VkGPUBuffer> > &outputBuffers): Operator(gpuCtx) {
    this->inputBuffer = inputBuffer;
    this->outputBuffers = outputBuffers;
}

SplitOperator::~SplitOperator() {
}

std::shared_ptr<IComputeGraphNode> SplitOperator::CreateComputeGraphNode() {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    for (size_t i = 0; i < this->params.nums; ++i) {
        descriptorSetLayoutBindings.push_back(
            VkGPUHelper::BuildDescriptorSetLayoutBinding(i + 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                         VK_SHADER_STAGE_COMPUTE_BIT));
    }

    const auto splitNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                                 "Split",
                                                                 SHADER(split.comp.glsl.spv),
                                                                 0,
                                                                 descriptorSetLayoutBindings,
                                                                 (this->params.dim + 255) / 256,
                                                                 1,
                                                                 1);
    const VkResult ret = splitNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating split node." << std::endl;
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
    for (size_t i = 0; i < this->params.nums; ++i) {
        buffers.push_back({
            .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE,
            .buf = {
                .bufferSize = this->outputBuffers[i]->GetBufferSize(),
                .buffer = this->outputBuffers[i]->GetBuffer(),
            }
        });
    }

    const PushConstantInfo pushConstantInfo{
        .size = sizeof(SplitOperatorParams),
        .data = &this->params,
    };
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };
    splitNode->AddComputeElement(computeElem);

    return splitNode;
}

void SplitOperator::Destroy() {
    Operator::Destroy();
}
