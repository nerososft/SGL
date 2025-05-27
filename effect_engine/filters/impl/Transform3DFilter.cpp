//
// Created by 1234 on 2025/3/6.
//

#include "Transform3DFilter.h"

#include "gpu_engine/config.h"
#include "gpu_engine/gpu/VkGPUHelper.h"
#include "gpu_engine/log/Log.h"

PipelineNodeBuffer Transform3DFilter::GetVertexBufferNode() const {
    PipelineNodeBuffer vertexBufferNode = {};
    vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
    vertexBufferNode.buf.bufferSize = vertexBuffer->GetBufferSize();
    vertexBufferNode.buf.buffer = vertexBuffer->GetBuffer();
    return vertexBufferNode;
}

PipelineNodeBuffer Transform3DFilter::GetIndicesBufferNode() const {
    PipelineNodeBuffer indicesBufferNode = {};
    indicesBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
    indicesBufferNode.buf.buffer = indicesBuffer->GetBuffer();
    indicesBufferNode.buf.bufferSize = indicesBuffer->GetBufferSize();
    return indicesBufferNode;
}

PipelineNodeBuffer Transform3DFilter::GetTransformMatrixBufferNode() const {
    PipelineNodeBuffer transformMatrixBufferNode = {};
    transformMatrixBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
    transformMatrixBufferNode.buf.buffer = transformMatrixBuffer->GetBuffer();
    transformMatrixBufferNode.buf.bufferSize = transformMatrixBuffer->GetBufferSize();
    return transformMatrixBufferNode;
}

VkResult Transform3DFilter::AddDrawElement(const std::vector<Vertex> &vertexData,
                                           const std::vector<uint32_t> &indicesData,
                                           const glm::mat4 &transform) {
    vertexBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (vertexBuffer == nullptr) {
        Logger() << "vertexBuffer is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    const VkDeviceSize vertexBufferSize = vertexData.size() * sizeof(Vertex);
    VkResult ret = vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_VERTEX, vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer allocate and bind failed" << std::endl;
        return ret;
    }
    ret = vertexBuffer->UploadData(vertexData.data(), vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer upload failed" << std::endl;
        return ret;
    }

    /*
     * Indices upload
     */
    indicesBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (indicesBuffer == nullptr) {
        Logger() << "indexBuffer is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const VkDeviceSize indicesBufferSize = indicesData.size() * sizeof(uint32_t);
    ret = indicesBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX, indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer allocate and bind failed" << std::endl;
        return ret;
    }
    ret = indicesBuffer->UploadData(indicesData.data(), indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer upload failed" << std::endl;
        return ret;
    }

    /*
    * Transform matrix
    */
    transformMatrixBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (transformMatrixBuffer == nullptr) {
        Logger() << "transform matrix is null" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    constexpr VkDeviceSize transformMatrixBufferSize = sizeof(glm::mat4);
    ret = transformMatrixBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, transformMatrixBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "transform matrix buffer allocate and bind failed" << std::endl;
        return ret;
    }

    ret = transformMatrixBuffer->UploadData(&transform, transformMatrixBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "transform matrix buffer upload failed" << std::endl;
        return ret;
    }

    /*
     * Texture
     */
    const std::vector<uint32_t> queueFamilies = {0};
    ret = VkGPUHelper::CreateImage(this->gpuCtx->GetCurrentDevice(),
                                   static_cast<float>(this->width),
                                   static_cast<float>(this->height),
                                   VK_IMAGE_TYPE_2D,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                   VK_SHARING_MODE_CONCURRENT,
                                   queueFamilies,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   &this->textureImage);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create texture image" << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateImageView(this->gpuCtx->GetCurrentDevice(),
                                       this->textureImage,
                                       VK_IMAGE_VIEW_TYPE_2D,
                                       VK_FORMAT_R8G8B8A8_SRGB,
                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                       &this->textureImageView);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create texture imageview" << std::endl;
        return ret;
    }

    ret = VkGPUHelper::CreateSampler(this->gpuCtx->GetCurrentDevice(), &this->textureSampler);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create texture sampler" << std::endl;
        return ret;
    }

    std::vector<PipelineNodeBuffer> buffers;
    buffers.push_back(GetVertexBufferNode());
    buffers.push_back(GetIndicesBufferNode());
    buffers.push_back(GetTransformMatrixBufferNode()); // uniform 0
    const GraphicsElement element{
        .pushConstantInfo = {},
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };

    this->graphicsPipelineNode->AddGraphicsElement(element);
    return VK_SUCCESS;
}


VkResult Transform3DFilter::ConstructMainGraphicsPipeline() {
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
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texCoords),
        },
    };

    this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(this->gpuCtx,
                                                                        "mainGraphicsPipeline",
                                                                        this->mainRenderPassNode->GetRenderPass(),
                                                                        SHADER(transform3d.vert.glsl.spv),
                                                                        SHADER(transform3d.frag.glsl.spv),
                                                                        vertexInputBindingDescriptions,
                                                                        vertexInputAttributeDescriptions,
                                                                        this->width,
                                                                        this->height);
    if (this->graphicsPipelineNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const std::vector<Vertex> vertices = {
        {
            .position = {-0.5f, -0.5f, 0.0f},
            .color = {0.0f, 0.0f, 1.0f},
        },
        {
            .position = {0.5f, -0.5f, 0.0f},
            .color = {1.0f, 0.0f, 0.0f},
        },
        {
            .position = {-0.5f, 0.5f, 0.0f},
            .color = {0.0f, 1.0f, 0.0f},
        },
        {
            .position = {-0.5f, 0.5f, 0.0f},
            .color = {1.0f, 1.0f, 0.0f},
        },
        {
            .position = {0.5f, -0.5f, 0.0f},
            .color = {0.0f, 0.0f, 1.0f},
        },
        {
            .position = {0.5f, 0.5f, 0.0f},
            .color = {1.0f, 0.0f, 0.0f},
        },
    };
    const std::vector<uint32_t> indices = {0, 1, 2, 3, 4, 5};
    constexpr auto transform = glm::mat4(1.0f);

    VkResult ret = this->AddDrawElement(vertices, indices, transform);
    if (ret != VK_SUCCESS) {
        Logger() << "draw mash add failed" << std::endl;
        return ret;
    }

    ret = this->graphicsPipelineNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return ret;
    }

    this->mainRenderPassNode->AddDependenceNode(this->graphicsPipelineNode);

    return VK_SUCCESS;
}

VkResult Transform3DFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                  const std::vector<FilterImageInfo> &inputImageInfo,
                                  const std::vector<FilterImageInfo> &outputImageInfo) {
    this->gpuCtx = gpuCtx;
    this->width = inputImageInfo[0].width;
    this->height = inputImageInfo[0].height;

    const std::vector<uint32_t> queueFamilies = {0};
    this->computeGraph = std::make_shared<ComputeGraph>(this->gpuCtx);
    if (!this->computeGraph) {
        Logger() << Logger::ERROR << "Failed to create compute graph!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    this->subComputeGraph = std::make_shared<SubComputeGraph>(this->gpuCtx);
    if (!this->subComputeGraph) {
        Logger() << Logger::ERROR << "Failed to create sub compute graph!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = this->subComputeGraph->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize sub compute graph!" << std::endl;
        return result;
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
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    result = mainRenderPassNode->CreateComputeGraphNode();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create render pass compute graph node!" << std::endl;
        return result;
    }

    this->framebuffer = std::make_shared<VkGPUFramebuffer>(this->gpuCtx,
                                                           this->width,
                                                           this->height,
                                                           this->mainRenderPassNode->GetRenderPass());
    if (this->framebuffer == nullptr) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    result = this->framebuffer->CreateFramebuffer(queueFamilies);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create framebuffer!" << std::endl;
        return result;
    }

    mainRenderPassNode->SetFramebuffer(framebuffer);
    Logger() << Logger::INFO << "Renderer Initialized!" << std::endl;

    if (ConstructMainGraphicsPipeline() != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create main graphics pipeline!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    ImageToCopyNodeImageInfo srcCopyInfo;
    srcCopyInfo.height = this->height;
    srcCopyInfo.width = this->width;
    srcCopyInfo.rowLength = this->width;
    srcCopyInfo.image = this->framebuffer->GetColorImage();
    ImageToCopyNodeBufferInfo dstCopyInfo;
    dstCopyInfo.buffer = outputImageInfo[0].storageBuffer;
    dstCopyInfo.bufferSize = this->width * this->height * 4;

    offScreenCopyNode = std::make_shared<ImageToBufferCopyNode>(this->gpuCtx,
                                                                "offScreenCopy",
                                                                srcCopyInfo,
                                                                dstCopyInfo);
    if (offScreenCopyNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create offscreen copy node!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    offScreenCopyNode->CreateComputeGraphNode();
    offScreenCopyNode->AddDependenceNode(this->mainRenderPassNode);

    this->subComputeGraph->AddComputeGraphNode(offScreenCopyNode);
    this->computeGraph->AddSubGraph(this->subComputeGraph);

    return computeGraph->Compute();
}

void Transform3DFilter::Destroy() {
    BasicFilter::Destroy();
}
