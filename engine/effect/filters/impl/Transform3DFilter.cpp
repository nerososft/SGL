//
// Created by 1234 on 2025/3/6.
//

#include "Transform3DFilter.h"

#include <glm/ext/matrix_clip_space.hpp>

#include "core/config.h"
#include "core/gpu/VkGPUHelper.h"
#include "core/log/Log.h"

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

PipelineNodeBuffer Transform3DFilter::GetTextureBufferNode(const VkBuffer buffer,
                                                           const VkDeviceMemory memory) const {
    PipelineNodeBuffer textureBufferNode = {};
    textureBufferNode.type = PIPELINE_NODE_SAMPLER;
    textureBufferNode.sampler.image = this->textureImage;
    textureBufferNode.sampler.imageView = this->textureImageView;
    textureBufferNode.sampler.imageBuffer = buffer;
    textureBufferNode.sampler.imageBufferMemory = memory;
    textureBufferNode.sampler.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    textureBufferNode.sampler.sampler = this->textureSampler;
    return textureBufferNode;
}

VkResult Transform3DFilter::AddDrawElement(const std::vector<Vertex> &vertexData,
                                           const std::vector<uint32_t> &indicesData,
                                           const glm::mat4 &transform,
                                           const FilterImageInfo &imageInfo) {
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
                                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   VK_SHARING_MODE_EXCLUSIVE,
                                   queueFamilies,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   &this->textureImage);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create texture image" << std::endl;
        return ret;
    }

    imageBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    ret = imageBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_LOCAL, imageInfo.bufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to allocate image buffer" << std::endl;
    }

    ret = vkBindImageMemory(this->gpuCtx->GetCurrentDevice(),
                            this->textureImage,
                            imageBuffer->GetDeviceMemory(),
                            0);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to bind image memory" << std::endl;
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
    buffers.push_back(GetTextureBufferNode(imageInfo.storageBuffer, imageInfo.storageBufferMemory)); // sampler 1
    const GraphicsElement element{
        .pushConstantInfo = {},
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };

    this->graphicsPipelineNode->AddGraphicsElement(element);
    return VK_SUCCESS;
}


VkResult Transform3DFilter::ConstructMainGraphicsPipeline(const FilterImageInfo &imageInfo) {
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

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                                                     VK_SHADER_STAGE_ALL_GRAPHICS));
    descriptorSetLayoutBindings.push_back(
        VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                                     VK_SHADER_STAGE_ALL_GRAPHICS));

    this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(this->gpuCtx,
                                                                        "mainGraphicsPipeline",
                                                                        this->mainRenderPassNode->GetRenderPass(),
                                                                        SHADER(transform3d.vert.glsl.spv),
                                                                        SHADER(transform3d.frag.glsl.spv),
                                                                        64,
                                                                        descriptorSetLayoutBindings,
                                                                        vertexInputBindingDescriptions,
                                                                        vertexInputAttributeDescriptions,
                                                                        this->width,
                                                                        this->height);
    if (this->graphicsPipelineNode == nullptr) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult ret = this->graphicsPipelineNode->CreateComputeGraphNode();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create graphics pipeline node!" << std::endl;
        return ret;
    }

    const std::vector<Vertex> vertices = {
        {
            .position = {-1.0f, -1.0f, 0.0f},
            .color = {0.0f, 0.0f, 1.0f},
            .texCoords = {0.0f, 0.0f}
        },
        {
            .position = {1.0f, -1.0f, 0.0f},
            .color = {1.0f, 0.0f, 0.0f},
            .texCoords = {1.0f, 0.0f}
        },
        {
            .position = {-1.0f, 1.0f, 0.0f},
            .color = {0.0f, 1.0f, 0.0f},
            .texCoords = {0.0f, 1.0f}
        },
        {
            .position = {-1.0f, 1.0f, 0.0f},
            .color = {1.0f, 1.0f, 0.0f},
            .texCoords = {0.0f, 1.0f}
        },
        {
            .position = {1.0f, -1.0f, 0.0f},
            .color = {0.0f, 0.0f, 1.0f},
            .texCoords = {1.0f, 0.0f}
        },
        {
            .position = {1.0f, 1.0f, 0.0f},
            .color = {1.0f, 0.0f, 0.0f},
            .texCoords = {1.0f, 1.0f}
        },
    };
    const std::vector<uint32_t> indices = {0, 1, 2, 3, 4, 5};

    ret = this->AddDrawElement(vertices, indices, this->transformFilterParams.transformMatrix, imageInfo);
    if (ret != VK_SUCCESS) {
        Logger() << "draw mash add failed" << std::endl;
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

    if (ConstructMainGraphicsPipeline(inputImageInfo[0]) != VK_SUCCESS) {
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
