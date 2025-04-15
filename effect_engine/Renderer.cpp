//
// Created by neo on 25-4-11.
//

#include "Renderer.h"

#include <queue>

#include "gpu/VkGPUBuffer.h"
#include "gpu/VkGPUHelper.h"
#include "gpu/compute_graph/ImageToBufferCopyNode.h"
#include "log/Log.h"
#include "utils/ImageUtils.h"

bool Renderer::ConstructMainGraphicsPipeline() {
    std::vector<PipelineNodeBuffer> buffers;

    vertexBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (vertexBuffer == nullptr) {
        Logger() << "vertexBuffer is null" << std::endl;
        return false;
    }
    const std::vector<Vertex> vertices = {
        {
            .position = {-0.5f, -0.5f, 0.0f},
        },
        {
            .position = {0.5f, -0.5f, 0.0f},
        },
        {
            .position = {-0.5f, 0.5f, 0.0f},
        },
        {
            .position = {-0.5f, 0.5f, 0.0f},
        },
        {
            .position = {0.5f, -0.5f, 0.0f},
        },
        {
            .position = {0.5f, 0.5f, 0.0f},
        },
    };
    VkResult ret = vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_VERTEX, vertices.size() * sizeof(float));
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = vertexBuffer->UploadData(vertices.data(), sizeof(float) * vertices.size());
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer upload failed" << std::endl;
        return false;
    }

    PipelineNodeBuffer vertexBufferNode = {};
    vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
    vertexBufferNode.bufferSize = sizeof(float) * vertices.size();
    vertexBufferNode.buffer = vertexBuffer->GetBuffer();

    indicesBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (indicesBuffer == nullptr) {
        Logger() << "indexBuffer is null" << std::endl;
        return false;
    }
    const std::vector indices = {0, 1, 2, 3, 4, 5};
    ret = indicesBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX, indices.size() * sizeof(uint32_t));
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = indicesBuffer->UploadData(indices.data(), sizeof(uint32_t) * indices.size());
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer upload failed" << std::endl;
        return false;
    }

    PipelineNodeBuffer indexBufferNode = {};
    indexBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
    indexBufferNode.buffer = indicesBuffer->GetBuffer();
    indexBufferNode.bufferSize = sizeof(uint32_t) * indices.size();

    buffers.push_back(vertexBufferNode);
    buffers.push_back(indexBufferNode);
    const GraphicsElement element{
        .buffers = buffers,
        .pushConstantInfo = {
            .size = sizeof(FrameInfo),
            .data = &this->frameInfo
        },
        .customDrawFunc = nullptr,
    };

    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = {
        {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0,
        }
    };

    this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(this->gpuCtx,
                                                                        "mainGraphicsPipeline",
                                                                        this->mainRenderPassNode->GetRenderPass(),
                                                                        SHADER(rect.vert.glsl.spv),
                                                                        SHADER(rect.frag.glsl.spv),
                                                                        vertexInputBindingDescriptions,
                                                                        vertexInputAttributeDescriptions,
                                                                        this->width,
                                                                        this->height);
    if (this->graphicsPipelineNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return false;
    }

    this->graphicsPipelineNode->AddGraphicsElement(element);

    ret = this->graphicsPipelineNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return false;
    }

    this->mainRenderPassNode->AddDependenceNode(this->graphicsPipelineNode);

    this->swapChain = std::make_shared<VkGPUSwapChain>(this->gpuCtx);
    if (this->swapChain == nullptr) {
        Logger() << Logger::ERROR << "Failed to create swap chain!" << std::endl;
        return false;
    }

    return true;
}

bool Renderer::Init() {
    std::vector<const char *> requiredExtensions;
    this->gpuCtx = std::make_shared<VkGPUContext>(requiredExtensions);
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
        .color = {1.0f, 1.0f, 0.0f, 0.0f}
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

    const std::vector<uint32_t> queueFamilies = {0}; // FIXME: depends on multi queue or single queue
    result = this->framebuffer->CreateFramebuffer(queueFamilies);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return false;
    }

    mainRenderPassNode->SetFramebuffer(framebuffer);
    Logger() << Logger::INFO << "Renderer Initialized!" << std::endl;

    if (!ConstructMainGraphicsPipeline()) {
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
    srcCopyInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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

VkResult Renderer::RenderFrame() const {
    const VkResult ret = this->computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to render compute graph!" << std::endl;
    }
    return ret;
}

void Renderer::RenderFrameOffScreen(const std::string &path) const {
    VkResult ret = this->RenderFrame();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to render frame!" << std::endl;
        return;
    }

    const VkDeviceSize offScreenBufferSize = this->width * this->height * 4;
    ret = offScreenBuffer->MapBuffer(offScreenBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map output storage buffer!" << std::endl;
        return;
    }

    ImageUtils::WritePngFile(path,
                             this->width,
                             this->height,
                             4,
                             offScreenBuffer->GetMappedAddr());
}
