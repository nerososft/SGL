//
// Created by neo on 25-5-25.
//

#include "SoftmaxOperator.h"

#include <cassert>

#include "core/config.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"

SoftmaxOperator::SoftmaxOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                 const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                                 const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

SoftmaxOperator::~SoftmaxOperator() = default;

std::shared_ptr<IComputeGraphNode> SoftmaxOperator::CreateComputeGraphNode() {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    const size_t nums = outputBuffer->GetBufferSize() / sizeof(float);
    auto softmaxNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                             "Softmax",
                                                             SHADER(softmax.comp.glsl.spv),
                                                             sizeof(SoftmaxOperatorParams),
                                                             descriptorSetLayoutBindings,
                                                             (nums + 255) / 256,
                                                             1,
                                                             1);
    const VkResult ret = softmaxNode->CreateComputeGraphNode();
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
        .size = sizeof(SoftmaxOperatorParams),
        .data = &this->params
    };
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
        .preCompute = [=] {
            assert(this != nullptr); // if null, means optimized out
            assert(this->sum != nullptr);
            Logger() << "Sum: " << *this->sum << std::endl;
            this->params.sum = *this->sum;
        }
    };
    softmaxNode->AddComputeElement(computeElem);
    return softmaxNode;
}

void SoftmaxOperator::Destroy() {
    UnaryOperator::Destroy();
}
