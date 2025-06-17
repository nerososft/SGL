//
// Created by neo on 25-6-17.
//

#include "Painter.h"

#include <ostream>
#include <glm/ext/matrix_transform.hpp>

#include "core/gpu/VkGPUHelper.h"
#include "core/log/Log.h"
#include "core/utils/ImageUtils.h"

bool Painter::Init(uint32_t width, uint32_t height) {
    std::vector<const char *> extensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(extensions);
    this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
    VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    Logger() << Logger::INFO << "Initialized Renderer, version: " << VERSION << std::endl;

    std::vector<uint32_t> queueFamilies = {0};
    this->computeGraph = std::make_shared<ComputeGraph>(this->gpuCtx);
    if (!this->computeGraph) {
        Logger() << Logger::ERROR << "Failed to create compute graph!" << std::endl;
        return false;
    }
    this->subComputeGraph = std::make_shared<SubComputeGraph>(this->gpuCtx);
    if (!this->subComputeGraph) {
        Logger() << Logger::ERROR << "Failed to create sub compute graph!" << std::endl;
        return false;
    }
    result = this->subComputeGraph->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize sub compute graph!" << std::endl;
        return false;
    }

    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency> dependencies;
    std::vector<VkSubpassDescription> subPasses;
    VkAttachmentDescription colorAttachment;
    colorAttachment.flags = 0;
    colorAttachment.format = VK_FORMAT_R8G8B8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments.push_back(colorAttachment);

    VkAttachmentDescription depthAttachment;
    depthAttachment.flags = 0;
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments.push_back(depthAttachment);

    const std::vector<VkAttachmentReference> colorAttachments = {
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        }
    };

    const std::vector<VkAttachmentReference> depthAttachments = {
        {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        }
    };

    VkSubpassDescription subPassDescription;
    subPassDescription.flags = 0;
    subPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPassDescription.colorAttachmentCount = colorAttachments.size();
    subPassDescription.pColorAttachments = colorAttachments.data();
    subPassDescription.inputAttachmentCount = 0;
    subPassDescription.pInputAttachments = nullptr;
    subPassDescription.preserveAttachmentCount = 0;
    subPassDescription.pPreserveAttachments = nullptr;
    subPassDescription.pResolveAttachments = nullptr;
    subPassDescription.pDepthStencilAttachment = depthAttachments.data();
    subPasses.push_back(subPassDescription);

    std::vector<VkClearValue> clearValues;
    clearValues.push_back({
        .color = {0.0f, 0.0f, 0.0f, 0.0f}
    });
    clearValues.push_back({
        .depthStencil = {1.0f, 0}
    });
    mainRenderPassNode = std::make_shared<GraphicsRenderPassNode>(this->gpuCtx,
                                                                  "mainRenderPass",
                                                                  attachments,
                                                                  dependencies,
                                                                  subPasses,
                                                                  this->width,
                                                                  this->height,
                                                                  clearValues);
    if (mainRenderPassNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create graphics render pass node!" << std::endl;
        return false;
    }
    result = mainRenderPassNode->CreateComputeGraphNode();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create render pass compute graph node!" << std::endl;
        return false;
    }

    this->framebuffer = std::make_shared<VkGPUFramebuffer>(this->gpuCtx,
                                                           this->width,
                                                           this->height,
                                                           this->mainRenderPassNode->GetRenderPass());
    if (this->framebuffer == nullptr) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return false;
    }

    result = this->framebuffer->CreateFramebuffer(queueFamilies);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return false;
    }

    mainRenderPassNode->SetFramebuffer(framebuffer);
    Logger() << Logger::INFO << "Renderer Initialized!" << std::endl;

    if (!CreateGraphicsPipelines()) {
        Logger() << Logger::ERROR << "Failed to create main graphics pipeline!" << std::endl;
        return false;
    }

    offScreenBuffer = std::make_shared<VkGPUBuffer>(this->gpuCtx);
    if (offScreenBuffer == nullptr) {
        Logger() << Logger::ERROR << "Failed to create offscreen buffer!" << std::endl;
        return false;
    }
    result = offScreenBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, this->width * this->height * 4);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to allocate offscreen buffer!" << std::endl;
        return false;
    }

    ImageToCopyNodeImageInfo srcCopyInfo;
    srcCopyInfo.height = this->height;
    srcCopyInfo.width = this->width;
    srcCopyInfo.rowLength = this->width;
    srcCopyInfo.image = this->framebuffer->GetColorImage();
    ImageToCopyNodeBufferInfo dstCopyInfo;
    dstCopyInfo.buffer = this->offScreenBuffer->GetBuffer();
    dstCopyInfo.bufferSize = this->width * this->height * 4;

    offScreenCopyNode = std::make_shared<ImageToBufferCopyNode>(this->gpuCtx,
                                                                "offScreenCopy",
                                                                srcCopyInfo,
                                                                dstCopyInfo);
    if (offScreenCopyNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create offscreen copy node!" << std::endl;
        return false;
    }
    offScreenCopyNode->CreateComputeGraphNode();
    offScreenCopyNode->AddDependenceNode(this->mainRenderPassNode);

    this->subComputeGraph->AddComputeGraphNode(offScreenCopyNode);
    this->computeGraph->AddSubGraph(this->subComputeGraph);

    return true;
}

bool Painter::CreateGraphicsPipelines() {
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = {
        {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, color),
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, normal),
        },
        {
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texCoords),
        },
    };

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    // descriptorSetLayoutBindings.push_back(
        // VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                                                     // VK_SHADER_STAGE_ALL_GRAPHICS));
    this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(this->gpuCtx,
                                                                        "mainGraphicsPipeline",
                                                                        this->mainRenderPassNode->GetRenderPass(),
                                                                        SHADER(rect.vert.glsl.spv),
                                                                        SHADER(rect.frag.glsl.spv),
                                                                        sizeof(FrameInfo),
                                                                        descriptorSetLayoutBindings,
                                                                        vertexInputBindingDescriptions,
                                                                        vertexInputAttributeDescriptions,
                                                                        this->width,
                                                                        this->height);
    if (this->graphicsPipelineNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return false;
    }

    const VkResult ret = this->graphicsPipelineNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return false;
    }

    // TODO: vertex data,  draw elemenet add

    this->mainRenderPassNode->AddDependenceNode(this->graphicsPipelineNode);

    return true;
}

void Painter::toImage(const std::string &path) const {
    VkResult ret = this->computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to render compute graph!" << std::endl;
    }

    const VkDeviceSize offScreenBufferSize = this->width * this->height * 4;

    std::vector<char> imgData(offScreenBufferSize);
    ret = offScreenBuffer->DownloadData(imgData.data(), offScreenBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer!" << std::endl;
        return;
    }

    ImageUtils::WritePngFile(path,
                             this->width,
                             this->height,
                             4,
                             imgData.data());
}
