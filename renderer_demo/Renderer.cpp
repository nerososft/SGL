//
// Created by neo on 25-4-11.
//

#include "Renderer.h"

#include <queue>
#include <vulkan/vk_enum_string_helper.h>

#include "../gpu_engine/gpu/VkGPUBuffer.h"
#include "../gpu_engine/gpu/VkGPUHelper.h"
#include "../gpu_engine/gpu/compute_graph/ImageToBufferCopyNode.h"
#include "../gpu_engine/log/Log.h"
#include "../gpu_engine/utils/ImageUtils.h"

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
    const VkDeviceSize vertexBufferSize = vertices.size() * sizeof(Vertex);
    VkResult ret = vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_VERTEX, vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = vertexBuffer->UploadData(vertices.data(), vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer upload failed" << std::endl;
        return false;
    }

    PipelineNodeBuffer vertexBufferNode = {};
    vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
    vertexBufferNode.bufferSize = vertexBufferSize;
    vertexBufferNode.buffer = vertexBuffer->GetBuffer();

    indicesBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (indicesBuffer == nullptr) {
        Logger() << "indexBuffer is null" << std::endl;
        return false;
    }
    const std::vector indices = {0, 1, 2, 3, 4, 5};
    const VkDeviceSize indicesBufferSize = indices.size() * sizeof(uint32_t);
    ret = indicesBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX, indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = indicesBuffer->UploadData(indices.data(), indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer upload failed" << std::endl;
        return false;
    }

    PipelineNodeBuffer indexBufferNode = {};
    indexBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
    indexBufferNode.buffer = indicesBuffer->GetBuffer();
    indexBufferNode.bufferSize = indicesBufferSize;

    buffers.push_back(vertexBufferNode);
    buffers.push_back(indexBufferNode);
    const GraphicsElement element{
        .pushConstantInfo = {
            .size = sizeof(FrameInfo),
            .data = &this->frameInfo
        },
        .buffers = buffers,
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

    return true;
}

bool Renderer::Init(const std::vector<const char *> &requiredExtensions,
                    VkSurfaceKHR (*GetSurface)(VkInstance instance)) {
    std::vector<const char *> extensions;
    for (auto extension: requiredExtensions) {
        Logger() << "Require extension: " << extension << std::endl;
        extensions.push_back(extension);
    }
    this->gpuCtx = std::make_shared<VkGPUContext>(extensions);
    this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_validation");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_LUNARG_api_dump");
    // this->gpuCtx->AddInstanceEnableLayer("VK_LAYER_KHRONOS_synchronization2");
    // this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_synchronization2");
    this->gpuCtx->AddDeviceEnabledExtension("VK_KHR_swapchain");
    VkResult result = this->gpuCtx->Init();
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to initialize Vulkan GPU context!" << std::endl;
        return false;
    }
    Logger() << Logger::INFO << "Initialized Renderer, version: " << VERSION << std::endl;

    this->swapChain = std::make_shared<VkGPUSwapChain>(this->gpuCtx);
    if (this->swapChain == nullptr) {
        Logger() << Logger::ERROR << "Failed to create swap chain!" << std::endl;
        return false;
    }

    std::vector<uint32_t> queueFamilies = {0};
    result = this->swapChain->CreateSwapChain(GetSurface(this->gpuCtx->GetInstance()),
                                              this->width,
                                              this->height,
                                              queueFamilies);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create swap chain!" << std::endl;
        return false;
    }

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
        .color = {1.0f, 1.0f, 1.0f, 0.0f}
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


    result = VkGPUHelper::CreateFence(this->gpuCtx->GetCurrentDevice(), &this->renderFinishedFence);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create fence!" << std::endl;
        return false;
    }
    result = VkGPUHelper::CreateSemaphore(this->gpuCtx->GetCurrentDevice(), &this->imageAvailableSemaphore);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create semaphore!" << std::endl;
        return false;
    }
    result = VkGPUHelper::CreateSemaphore(this->gpuCtx->GetCurrentDevice(), &this->renderFinishedSemaphore);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to create semaphore!" << std::endl;
        return false;
    }
    result = VkGPUHelper::AllocateCommandBuffers(this->gpuCtx->GetCurrentDevice(),
                                                 this->gpuCtx->GetCommandPool(0),
                                                 1,
                                                 &presentCmdBuffer);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to allocate command buffers!" << std::endl;
        return false;
    }
    return true;
}

VkResult Renderer::RenderFrame() {
    this->frameInfo.frameIndex++;
    const VkResult ret = this->computeGraph->Compute();
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to render compute graph!" << std::endl;
    }
    return ret;
}

VkResult Renderer::Present() const {
    uint32_t imageIndex = 0;
    const std::vector fences = {renderFinishedFence};
    vkResetFences(this->gpuCtx->GetCurrentDevice(), fences.size(), fences.data());
    const VkResult result = vkAcquireNextImageKHR(this->gpuCtx->GetCurrentDevice(),
                                                  this->swapChain->GetSwapChain(),
                                                  UINT64_MAX,
                                                  this->imageAvailableSemaphore,
                                                  this->renderFinishedFence,
                                                  &imageIndex);
    if (result != VK_SUCCESS) {
        Logger() << Logger::ERROR << "Failed to acquire swap chain image, err=" << string_VkResult(result) << std::endl;
        return result;
    }

    VkGPUHelper::GPUBeginCommandBuffer(this->presentCmdBuffer);

    const VkImageCopy region{
        .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .extent = {width, height, 1}
    };
    vkCmdCopyImage(this->presentCmdBuffer,
                   this->framebuffer->GetColorImage(),
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   this->swapChain->GetSwapChainImg(imageIndex),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &region);

    vkEndCommandBuffer(this->presentCmdBuffer);

    std::vector submitWaitSemaphores = {imageAvailableSemaphore};
    for (VkSemaphore waitSemaphore: this->computeGraph->GetComputeDoneSemaphores()) {
        submitWaitSemaphores.push_back(waitSemaphore);
    }
    const std::vector submitSignalSemaphores = {renderFinishedSemaphore};
    const std::vector submitCommandBuffers = {this->presentCmdBuffer};
    const std::vector<VkPipelineStageFlags> waitDstStageMasks = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = submitWaitSemaphores.size();
    submitInfo.pWaitSemaphores = submitWaitSemaphores.data();
    submitInfo.signalSemaphoreCount = submitSignalSemaphores.size();
    submitInfo.pSignalSemaphores = submitSignalSemaphores.data();
    submitInfo.commandBufferCount = submitCommandBuffers.size();
    submitInfo.pCommandBuffers = submitCommandBuffers.data();
    submitInfo.pWaitDstStageMask = waitDstStageMasks.data();
    const std::vector submitInfos = {submitInfo};
    VkGPUHelper::GPUQueueSubmit(
        this->gpuCtx->DispatchQueue(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT).queue,
        submitInfos,
        renderFinishedFence);

    vkWaitForFences(this->gpuCtx->GetCurrentDevice(),
                    fences.size(),
                    fences.data(),
                    VK_TRUE,
                    UINT64_MAX);

    const std::vector presentSwapChains = {this->swapChain->GetSwapChain()};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pWaitSemaphores = nullptr;
    presentInfo.swapchainCount = presentSwapChains.size();
    presentInfo.pSwapchains = presentSwapChains.data();
    presentInfo.pResults = nullptr;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(this->gpuCtx->DispatchQueue(VK_QUEUE_GRAPHICS_BIT).queue, &presentInfo);
    return VK_SUCCESS;
}

void Renderer::RenderFrameOffScreen(const std::string &path) {
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
