//
// Created by neo on 25-7-8.
//

#include "TransposeOperator.h"

#include "core/gpu/VkGPUHelper.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"
#include "core/log/Log.h"


TransposeOperator::TransposeOperator(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                     const std::shared_ptr<VkGPUBuffer> &inputBuffer,
                                     const std::shared_ptr<VkGPUBuffer> &outputBuffer)
    : UnaryOperator(gpuCtx, inputBuffer, outputBuffer) {
}

std::shared_ptr<IComputeGraphNode> TransposeOperator::CreateComputeGraphNode() {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                                     VK_SHADER_STAGE_COMPUTE_BIT));

    const auto tNode = std::make_shared<ComputePipelineNode>(this->gpuCtx,
                                                             "Transpose",
                                                             SHADER(transpose.comp.glsl.spv),
                                                             sizeof(TransposeOperatorParams),
                                                             descriptorSetLayoutBindings,
                                                             (this->params.height + 31) / 32,
                                                             (this->params.width + 31) / 32,
                                                             1);
    const VkResult ret = tNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Error creating gemm node." << std::endl;
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
        .size = sizeof(TransposeOperatorParams),
        .data = &this->params,
    };
    const ComputeElement computeElem{
        .pushConstantInfo = pushConstantInfo,
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };
    tNode->AddComputeElement(computeElem);
    return tNode;
}

TransposeOperator::~TransposeOperator() {
    this->Destroy();
}

void TransposeOperator::Destroy() {
    UnaryOperator::Destroy();
}
