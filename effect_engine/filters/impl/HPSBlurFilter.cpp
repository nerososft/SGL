//
// Created by neo on 25-4-2.
//

#include "HPSBlurFilter.h"
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include "effect_engine/gpu/compute_graph/GraphicsPipelineNode.h"
#include "effect_engine/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "effect_engine/log/Log.h"

VkResult HPSBlurFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                              const std::vector<FilterImageInfo> &inputImageInfo,
                              const std::vector<FilterImageInfo> &outputImageInfo) {
    this->computeGraph = std::make_shared<ComputeGraph>(gpuCtx);
    this->computeSubGraph = std::make_shared<SubComputeGraph>(gpuCtx);
    VkResult ret = this->computeSubGraph->Init();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create compute graph, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    // TODO:
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency> dependencies;
    std::vector<VkSubpassDescription> subPasses;
    const auto renderPassNode = std::make_shared<GraphicsRenderPassNode>(gpuCtx,
                                                                         "HPSBlurRenderPass",
                                                                         attachments,
                                                                         dependencies,
                                                                         subPasses,
                                                                         inputImageInfo[0].width,
                                                                         inputImageInfo[0].height);
    ret = renderPassNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create graphics renderpass node, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    // TODO:
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    const auto graphicsNode = std::make_shared<GraphicsPipelineNode>(gpuCtx,
                                                                     "HPSBlurPipeline",
                                                                     renderPassNode->GetRenderPass(),
                                                                     SHADER(rect.vert.glsl.spv),
                                                                     SHADER(rect.frag.glsl.spv),
                                                                     vertexInputBindingDescriptions,
                                                                     vertexInputAttributeDescriptions,
                                                                     inputImageInfo[0].width, inputImageInfo[0].height);

    // TODO:
    graphicsNode->AddGraphicsElement({});

    ret = graphicsNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create graphics pipeline node, err =" << string_VkResult(ret) << std::endl;
        return ret;
    }

    renderPassNode->AddDependenceNode(graphicsNode);
    this->computeSubGraph->AddComputeGraphNode(renderPassNode);
    this->computeGraph->AddSubGraph(computeSubGraph);
    return this->computeGraph->Compute();
}

void HPSBlurFilter::Destroy() {
}
