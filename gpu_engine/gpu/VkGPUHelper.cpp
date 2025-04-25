//
// Created by 1234 on 2025/3/6.
//

#include "VkGPUHelper.h"

#include <iostream>
#include <ostream>
#ifdef OS_OPEN_HARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif

#include "log/Log.h"
#include "utils/IOUtils.h"

VkResult VkGPUHelper::CreateImageView(const VkDevice device,
                                      const VkImage image,
                                      const VkImageViewType viewType,
                                      const VkFormat format,
                                      const VkImageAspectFlags aspectFlags,
                                      VkImageView *imageView) {
    VkResult result = VK_SUCCESS;
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.flags = 0;
    viewInfo.pNext = nullptr;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    result = vkCreateImageView(device, &viewInfo, nullptr, imageView);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create image view, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

auto VkGPUHelper::CreateImage(const VkDevice device,
                              const float width,
                              const float height,
                              const VkImageType imageType,
                              const VkFormat format,
                              const VkImageUsageFlags usage,
                              const VkSharingMode sharingMode,
                              const std::vector<uint32_t> &queueFamilies,
                              const VkImageLayout initialLayout,
                              VkImage *image) -> VkResult {
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = 0;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = imageType;
    imageCreateInfo.format = format;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = sharingMode;
    imageCreateInfo.queueFamilyIndexCount = queueFamilies.size();
    imageCreateInfo.pQueueFamilyIndices = queueFamilies.data();
    imageCreateInfo.initialLayout = initialLayout;
    const VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, image);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create image, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateFramebuffer(const VkDevice device,
                                        const uint32_t width,
                                        const uint32_t height,
                                        const std::vector<VkImageView> &attachments,
                                        const VkRenderPass renderPass,
                                        VkFramebuffer *framebuffer) {
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.flags = 0;
    framebufferInfo.pNext = nullptr;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.renderPass = renderPass;
    const VkResult ret = vkCreateFramebuffer(device, &framebufferInfo, nullptr, framebuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create framebuffer, err=" << string_VkResult(ret) << std::endl;
    }
    return ret;
}

VkResult VkGPUHelper::CreateSemaphore(const VkDevice device,
                                      VkSemaphore *semaphore) {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    const VkResult ret = vkCreateSemaphore(device, &semaphoreInfo, nullptr, semaphore);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create semaphore, err=" << string_VkResult(ret) << std::endl;
    }
    return ret;
}

PFN_vkCmdPipelineBarrier2KHR VkGPUHelper::GetVkCmdPipelineBarrier2Fn(const VkDevice device) {
    PFN_vkCmdPipelineBarrier2KHR func = nullptr;
    func = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR"));
    return func;
}

void VkGPUHelper::GPUCmdPipelineMemBarrier(const VkCommandBuffer commandBuffer,
                                           const VkPipelineStageFlags srcStageMask,
                                           const VkPipelineStageFlags dstStageMask,
                                           const VkDependencyFlags dependencyFlags,
                                           const std::vector<VkMemoryBarrier> &memoryBarriers) {
    const std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;
    const std::vector<VkImageMemoryBarrier> imageMemoryBarrier;
    GPUCmdPipelineBarrier(commandBuffer,
                          srcStageMask,
                          dstStageMask,
                          dependencyFlags,
                          memoryBarriers,
                          bufferMemoryBarriers,
                          imageMemoryBarrier);
}

VkMemoryBarrier VkGPUHelper::BuildMemoryBarrier(const VkAccessFlagBits srcAccessMask,
                                                const VkAccessFlagBits dstAccessMask) {
    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    return memoryBarrier;
}

void VkGPUHelper::GPUCmdEndRenderPass(const VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void VkGPUHelper::GPUCmdBeginRenderPass(const VkCommandBuffer commandBuffer,
                                        const VkRenderPass renderPass,
                                        const VkFramebuffer framebuffer,
                                        const VkRect2D renderArea,
                                        const std::vector<VkClearValue> &clearValues) {
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea = renderArea;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

VkResult VkGPUHelper::CreateRenderPass(const VkDevice device,
                                       const std::vector<VkAttachmentDescription> &attachments,
                                       const std::vector<VkSubpassDependency> &dependencies,
                                       const std::vector<VkSubpassDescription> &subPasses,
                                       VkRenderPass *renderPass) {
    VkResult ret = VK_SUCCESS;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.flags = 0;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = subPasses.size();
    renderPassInfo.pSubpasses = subPasses.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();
    ret = vkCreateRenderPass(device, &renderPassInfo, nullptr, renderPass);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create render pass, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateGraphicsPipeline(const VkDevice device,
                                             const VkPipelineCache pipelineCache,
                                             const VkPipelineLayout pipelineLayout,
                                             const VkShaderModule vertexShaderModule,
                                             const VkShaderModule fragmentShaderModule,
                                             const VkRenderPass renderPass,
                                             std::vector<VkVertexInputBindingDescription>
                                             vertexInputBindingDescriptions,
                                             std::vector<VkVertexInputAttributeDescription>
                                             vertexInputAttributeDescriptions,
                                             VkPrimitiveTopology inputAssemblyTopology,
                                             VkBool32 primitiveRestartEnable,
                                             std::vector<VkViewport> viewports,
                                             std::vector<VkRect2D> viewportScissors,
                                             VkPolygonMode polygonMode,
                                             float lineWidth,
                                             std::vector<VkPipelineColorBlendAttachmentState>
                                             colorBlendStateCreateInfos,
                                             std::vector<VkDynamicState> dynamicStates,
                                             VkPipeline *pipeline) {
    VkResult ret = VK_SUCCESS;
    std::vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.flags = 0;
    vertexShaderStageCreateInfo.pNext = nullptr;
    vertexShaderStageCreateInfo.pName = "main";
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pSpecializationInfo = nullptr;
    shaderStages.push_back(vertexShaderStageCreateInfo);

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
    fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.flags = 0;
    fragmentShaderStageCreateInfo.pNext = nullptr;
    fragmentShaderStageCreateInfo.pName = "main";
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;
    shaderStages.push_back(fragmentShaderStageCreateInfo);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexInputBindingDescriptions.size();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.flags = 0;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.topology = inputAssemblyTopology;
    inputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;

    VkPipelineTessellationStateCreateInfo tessellationState = {};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.flags = 0;
    tessellationState.pNext = nullptr;
    tessellationState.patchControlPoints = 0;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.flags = 0;
    viewportState.pNext = nullptr;
    viewportState.scissorCount = viewportScissors.size();
    viewportState.pScissors = viewportScissors.data();
    viewportState.viewportCount = viewports.size();
    viewportState.pViewports = viewports.data();

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.pNext = nullptr;
    rasterizationState.depthClampEnable = VK_TRUE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = polygonMode;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0.0f;
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasSlopeFactor = 0.0f;
    rasterizationState.lineWidth = lineWidth;

    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.flags = 0;
    multisampleState.pNext = nullptr;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.minSampleShading = 0.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = VK_FALSE;
    multisampleState.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.flags = 0;
    depthStencilState.pNext = nullptr;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;

    VkStencilOpState frontStencilState = {};
    frontStencilState.failOp = VK_STENCIL_OP_KEEP;
    frontStencilState.passOp = VK_STENCIL_OP_KEEP;
    frontStencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    frontStencilState.compareOp = VK_COMPARE_OP_LESS;
    frontStencilState.compareMask = 0;
    frontStencilState.writeMask = 0;
    frontStencilState.reference = 0;
    depthStencilState.front = frontStencilState;

    VkStencilOpState backStencilState = {};
    backStencilState.failOp = VK_STENCIL_OP_KEEP;
    backStencilState.passOp = VK_STENCIL_OP_KEEP;
    backStencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    backStencilState.compareOp = VK_COMPARE_OP_LESS;
    backStencilState.compareMask = 0;
    backStencilState.writeMask = 0;
    backStencilState.reference = 0;
    depthStencilState.front = backStencilState;
    depthStencilState.maxDepthBounds = 0;
    depthStencilState.maxDepthBounds = 1;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.flags = 0;
    colorBlendState.pNext = nullptr;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = colorBlendStateCreateInfos.size();
    colorBlendState.pAttachments = colorBlendStateCreateInfos.data();
    colorBlendState.blendConstants[0] = 0.0f;
    colorBlendState.blendConstants[1] = 0.0f;
    colorBlendState.blendConstants[2] = 0.0f;
    colorBlendState.blendConstants[3] = 0.0f;

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.flags = 0;
    dynamicState.pNext = nullptr;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfos.push_back(pipelineCreateInfo);

    ret = vkCreateGraphicsPipelines(device,
                                    pipelineCache,
                                    pipelineCreateInfos.size(),
                                    pipelineCreateInfos.data(),
                                    nullptr,
                                    pipeline);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create graphics pipeline." << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateUniformBufferAndUploadData(const VkDevice device,
                                                       const std::vector<uint32_t> &queueFamilyIndices,
                                                       const VkPhysicalDeviceMemoryProperties *memoryProperties,
                                                       const VkDeviceSize bufferSize,
                                                       VkBuffer *buffer,
                                                       VkDeviceMemory *bufferMemory,
                                                       const void *uploadData) {
    VkResult ret = CreateStorageBufferAndBindMem(device,
                                                 bufferSize,
                                                 queueFamilyIndices,
                                                 memoryProperties,
                                                 buffer,
                                                 bufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create input storage buffer!" << std::endl;
        return ret;
    }

    void *data = nullptr;
    ret = vkMapMemory(device, *bufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map input storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    memcpy(data, uploadData, bufferSize);
    vkUnmapMemory(device, *bufferMemory);
    return ret;
}

VkResult VkGPUHelper::CreateStorageBufferAndUploadData(const VkDevice device,
                                                       const std::vector<uint32_t> &queueFamilyIndices,
                                                       const VkPhysicalDeviceMemoryProperties *memoryProperties,
                                                       const VkDeviceSize bufferSize,
                                                       VkBuffer *buffer,
                                                       VkDeviceMemory *bufferMemory,
                                                       const void *uploadData) {
    VkResult ret = CreateStorageBufferAndBindMem(device,
                                                 bufferSize,
                                                 queueFamilyIndices,
                                                 memoryProperties,
                                                 buffer,
                                                 bufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to create input storage buffer!" << std::endl;
        return ret;
    }

    void *data = nullptr;
    ret = vkMapMemory(device, *bufferMemory, 0, bufferSize, 0, &data);
    if (ret != VK_SUCCESS) {
        Logger() << "Failed to map input storage buffer memory, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    memcpy(data, uploadData, bufferSize);
    vkUnmapMemory(device, *bufferMemory);
    return ret;
}

VkSubmitInfo VkGPUHelper::BuildSubmitInfo(const VkFlags *submitWaitDstStageMask,
                                          const std::vector<VkCommandBuffer> &submitCommandBuffers,
                                          const std::vector<VkSemaphore> &submitSignalSemaphores,
                                          const std::vector<VkSemaphore> &submitWaitSemaphores) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = submitCommandBuffers.size();
    submitInfo.pCommandBuffers = submitCommandBuffers.data();
    submitInfo.signalSemaphoreCount = submitSignalSemaphores.size();
    submitInfo.pSignalSemaphores = submitSignalSemaphores.data();
    submitInfo.waitSemaphoreCount = submitWaitSemaphores.size();
    submitInfo.pWaitSemaphores = submitWaitSemaphores.data();
    submitInfo.pWaitDstStageMask = submitWaitDstStageMask;
    return submitInfo;
}

VkBufferMemoryBarrier VkGPUHelper::BuildBufferMemoryBarrier(const VkAccessFlagBits srcAccessMask,
                                                            const VkAccessFlagBits dstAccessMask,
                                                            const VkBuffer buffer,
                                                            const VkDeviceSize size) {
    VkBufferMemoryBarrier bufferMemoryBarrier = {};
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufferMemoryBarrier.pNext = nullptr;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.buffer = buffer;
    bufferMemoryBarrier.offset = 0;
    bufferMemoryBarrier.size = size;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return bufferMemoryBarrier;
}

VkImageMemoryBarrier VkGPUHelper::BuildImageMemoryBarrier(const VkAccessFlagBits srcAccessMask,
                                                          const VkAccessFlagBits dstAccessMask,
                                                          const VkImage image,
                                                          const VkImageLayout oldLayout,
                                                          const VkImageLayout newLayout) {
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return imageMemoryBarrier;
}

void VkGPUHelper::GPUCmdPipelineBufferMemBarrier(const VkCommandBuffer commandBuffer,
                                                 const VkPipelineStageFlags srcStageMask,
                                                 const VkPipelineStageFlags dstStageMask,
                                                 const VkDependencyFlags dependencyFlags,
                                                 const std::vector<VkBufferMemoryBarrier> &bufferMemoryBarriers) {
    const std::vector<VkMemoryBarrier> memoryBarriers;
    const std::vector<VkImageMemoryBarrier> imageMemoryBarrier;
    GPUCmdPipelineBarrier(commandBuffer,
                          srcStageMask,
                          dstStageMask,
                          dependencyFlags,
                          memoryBarriers,
                          bufferMemoryBarriers,
                          imageMemoryBarrier);
}

void VkGPUHelper::GPUCmdPipelineImageMemBarrier(const VkCommandBuffer commandBuffer,
                                                const VkPipelineStageFlags srcStageMask,
                                                const VkPipelineStageFlags dstStageMask,
                                                const VkDependencyFlags dependencyFlags,
                                                const std::vector<VkImageMemoryBarrier> &imageMemoryBarriers) {
    const std::vector<VkMemoryBarrier> memoryBarriers;
    const std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;
    GPUCmdPipelineBarrier(commandBuffer,
                          srcStageMask,
                          dstStageMask,
                          dependencyFlags,
                          memoryBarriers,
                          bufferMemoryBarriers,
                          imageMemoryBarriers);
}

void VkGPUHelper::GPUCmdPipelineBarrier(const VkCommandBuffer commandBuffer,
                                        const VkPipelineStageFlags srcStageMask,
                                        const VkPipelineStageFlags dstStageMask,
                                        const VkDependencyFlags dependencyFlags,
                                        const std::vector<VkMemoryBarrier> &memoryBarriers,
                                        const std::vector<VkBufferMemoryBarrier> &bufferMemoryBarriers,
                                        const std::vector<VkImageMemoryBarrier> &imageMemoryBarriers
) {
    vkCmdPipelineBarrier(commandBuffer,
                         srcStageMask,
                         dstStageMask,
                         dependencyFlags,
                         memoryBarriers.size(),
                         memoryBarriers.data(),
                         bufferMemoryBarriers.size(),
                         bufferMemoryBarriers.data(),
                         imageMemoryBarriers.size(),
                         imageMemoryBarriers.data());
}

VkResult VkGPUHelper::GPUQueueSubmit(const VkQueue queue,
                                     const std::vector<VkSubmitInfo> &submitInfos,
                                     const VkFence fence) {
    const VkResult ret = vkQueueSubmit(queue, submitInfos.size(), submitInfos.data(), fence);
    if (ret != VK_SUCCESS) {
        Logger() << "vkQueueSubmit failed, err=" << string_VkResult(ret) << std::endl;
    }
    return ret;
}

void VkGPUHelper::GPUCmdDispatch(const VkCommandBuffer commandBuffer,
                                 const uint32_t groupCountX,
                                 const uint32_t groupCountY,
                                 const uint32_t groupCountZ) {
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VkGPUHelper::GPUCmdPushConstant(const VkCommandBuffer commandBuffer,
                                     const VkPipelineLayout pipelineLayout,
                                     const VkShaderStageFlagBits shaderStageFlag,
                                     const uint32_t offset,
                                     const size_t pushConstantSize,
                                     const void *pushConstantData) {
    vkCmdPushConstants(commandBuffer,
                       pipelineLayout,
                       shaderStageFlag,
                       offset,
                       pushConstantSize,
                       pushConstantData);
}

void VkGPUHelper::GPUEndCommandBuffer(const VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);
}

void VkGPUHelper::GPUBeginCommandBuffer(const VkCommandBuffer commandBuffer) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pNext = nullptr;
    beginInfo.pInheritanceInfo = nullptr;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

VkResult VkGPUHelper::AllocateCommandBuffers(const VkDevice device,
                                             const VkCommandPool commandPool,
                                             const uint32_t commandBufferCount,
                                             VkCommandBuffer *commandBuffers) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBufferCount;

    const VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to allocate command buffer, err = " << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateFence(const VkDevice device, VkFence *fence) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    info.pNext = nullptr;
    const VkResult result = vkCreateFence(device, &info, nullptr, fence);
    if (result != VK_SUCCESS) {
        Logger() << "vkCreateFence failed, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkWriteDescriptorSet VkGPUHelper::BuildWriteDescriptorSet(const VkDescriptorSet descriptorSet,
                                                          const uint32_t dtsBinding,
                                                          const VkDescriptorType type,
                                                          const VkDescriptorImageInfo *imageInfo,
                                                          const VkDescriptorBufferInfo *bufferInfo,
                                                          const VkBufferView *texelBufferView) {
    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.dstBinding = dtsBinding;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = type;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = bufferInfo;
    writeDescriptorSet.pImageInfo = imageInfo;
    writeDescriptorSet.pTexelBufferView = texelBufferView;
    return writeDescriptorSet;
}

VkWriteDescriptorSet VkGPUHelper::BuildWriteStorageBufferDescriptorSet(const VkDescriptorSet descriptorSet,
                                                                       const uint32_t dstBinding,
                                                                       const VkDescriptorBufferInfo *
                                                                       descriptorBufferInfo) {
    return BuildWriteDescriptorSet(descriptorSet,
                                   dstBinding,
                                   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   nullptr,
                                   descriptorBufferInfo,
                                   nullptr);
}

VkResult VkGPUHelper::AllocateDescriptorSets(const VkDevice device,
                                             const VkDescriptorPool descriptorPool,
                                             const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                             VkDescriptorSet *descriptorSets) {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = descriptorSetLayouts.size();
    descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

    const VkResult result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets);
    if (result != VK_SUCCESS) {
        Logger() << "failed to allocate descriptor sets, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateShaderModule(const VkDevice device,
                                         const size_t shaderCodeSize,
                                         const uint32_t *shaderSpvCode,
                                         VkShaderModule *shaderModule) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.codeSize = shaderCodeSize;
    createInfo.pCode = shaderSpvCode;

    const VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, shaderModule);
    if (result != VK_SUCCESS) {
        Logger() << "failed to create shader module, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateShaderModuleFromPath(const VkDevice device,
                                                 const std::string &shaderFilePath,
                                                 VkShaderModule *shaderModule) {
    std::vector<char> shaderSpvCode = IOUtils::ReadFile(shaderFilePath);
    if (shaderSpvCode.empty()) {
        return VK_ERROR_INVALID_SHADER_NV;
    }
    const VkResult result = CreateShaderModule(device,
                                               shaderSpvCode.size(),
                                               reinterpret_cast<uint32_t *>(shaderSpvCode.data()),
                                               shaderModule);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create shader module, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreateComputePipeline(const VkDevice device,
                                            const VkPipelineCache pipelineCache,
                                            const VkPipelineLayout pipelineLayout,
                                            const VkShaderModule shaderModule,
                                            VkPipeline *pipeline) {
    VkPipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.flags = 0;
    shaderStageInfo.pNext = nullptr;
    shaderStageInfo.pName = "main";
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pSpecializationInfo = nullptr;

    std::vector<VkComputePipelineCreateInfo> pipelineCreateInfos;
    VkComputePipelineCreateInfo computePipelineCreateInfo;
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.pNext = nullptr;
    computePipelineCreateInfo.basePipelineIndex = 0;
    computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineCreateInfo.layout = pipelineLayout;
    computePipelineCreateInfo.stage = shaderStageInfo;
    pipelineCreateInfos.push_back(computePipelineCreateInfo);
    const VkResult result = vkCreateComputePipelines(device,
                                                     pipelineCache,
                                                     pipelineCreateInfos.size(),
                                                     pipelineCreateInfos.data(),
                                                     nullptr,
                                                     pipeline);
    if (result != VK_SUCCESS) {
        Logger() << "failed to create compute pipeline, err=" << string_VkResult(result) << std::endl;
    }
    return result;
}

VkResult VkGPUHelper::CreatePipelineLayout(const VkDevice device,
                                           const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts,
                                           const std::vector<VkPushConstantRange> &pushConstantRanges,
                                           VkPipelineLayout *pipelineLayout) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

    const VkResult ret = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipelineLayout);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create pipeline layout, " << string_VkResult(ret) << std::endl;
    }
    return ret;
}

VkResult VkGPUHelper::CreateDescriptorSetLayout(const VkDevice device,
                                                const std::vector<VkDescriptorSetLayoutBinding> &
                                                descriptorSetLayoutBindings,
                                                VkDescriptorSetLayout *descriptorSetLayout) {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindings.size();
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

    const VkResult ret = vkCreateDescriptorSetLayout(device,
                                                     &descriptorSetLayoutCreateInfo,
                                                     nullptr,
                                                     descriptorSetLayout);
    if (ret != VK_SUCCESS) {
        Logger() << "failed to create descriptor set layout, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateBuffer(const VkDevice device,
                                   const VkDeviceSize size,
                                   const VkBufferUsageFlags usage,
                                   const VkSharingMode sharingMode,
                                   const std::vector<uint32_t> &queueFamilyIndices,
                                   VkBuffer *buffer) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pNext = nullptr;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.queueFamilyIndexCount = queueFamilyIndices.size();
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    const VkResult ret = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
    if (ret != VK_SUCCESS) {
        Logger() << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
    }
    return ret;
}

uint32_t VkGPUHelper::GetRequiredMemTypeIndex(const VkPhysicalDeviceMemoryProperties *memProps,
                                              const VkMemoryRequirements &memRequirements,
                                              const uint32_t memoryPropertyFlagsBits) {
    for (uint32_t i = 0; i < memProps->memoryTypeCount; i++) {
        if (memRequirements.memoryTypeBits & (1 << i)) {
            if ((memoryPropertyFlagsBits & memProps->memoryTypes[i].propertyFlags) == memoryPropertyFlagsBits) {
                return i;
            }
        }
    }
    return 0;
}

VkResult VkGPUHelper::CreateStorageBufferAndBindMem(const VkDevice device,
                                                    const VkDeviceSize size,
                                                    const std::vector<uint32_t> &queueFamilyIndices,
                                                    const VkPhysicalDeviceMemoryProperties *memProps,
                                                    const uint32_t memoryPropertyFlagsBits,
                                                    VkBuffer *storageBuffer,
                                                    VkDeviceMemory *storageBufferMemory) {
    const VkBufferUsageFlags flag = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VkResult ret = CreateBuffer(device,
                                size,
                                flag,
                                VK_SHARING_MODE_EXCLUSIVE,
                                queueFamilyIndices,
                                storageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *storageBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps,
                                                        memRequirements,
                                                        memoryPropertyFlagsBits);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, storageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    Logger() << "VkMemory Allocate " << allocInfo.allocationSize << " bytes, need: " << size << " bytes, align " <<
            memRequirements.alignment << std::endl;
    ret = vkBindBufferMemory(device, *storageBuffer, *storageBufferMemory, 0);
    if (ret != VK_SUCCESS) {
        Logger() << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateImageAndBindMem(const VkDevice device,
                                            const float width,
                                            const float height,
                                            const VkImageType imageType,
                                            const VkFormat format,
                                            const VkImageUsageFlags usage,
                                            const VkSharingMode sharingMode,
                                            const VkImageLayout initialLayout,
                                            const VkPhysicalDeviceMemoryProperties *memProps,
                                            const std::vector<uint32_t> &queueFamilies,
                                            VkImage *image,
                                            VkDeviceMemory *imageMemory) {
    VkResult ret = CreateImage(device,
                               width,
                               height,
                               imageType,
                               format,
                               usage,
                               sharingMode,
                               queueFamilies,
                               initialLayout,
                               image);
    if (ret != VK_SUCCESS) {
        Logger() << Logger::ERROR << "failed to create color image!" << std::endl;
        return ret;
    }
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps,
                                                        memRequirements,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, imageMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    Logger() << "VkMemory Allocate " << allocInfo.allocationSize
            << " bytes, need: " << static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * 4
            << " bytes, align " << memRequirements.alignment << std::endl;
    ret = vkBindImageMemory(device, *image, *imageMemory, 0);
    if (ret != VK_SUCCESS) {
        Logger() << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateStorageBufferAndBindMem(const VkDevice device,
                                                    const VkDeviceSize size,
                                                    const std::vector<uint32_t> &queueFamilyIndices,
                                                    const VkPhysicalDeviceMemoryProperties *memProps,
                                                    VkBuffer *storageBuffer,
                                                    VkDeviceMemory *storageBufferMemory) {
    return CreateStorageBufferAndBindMem(device, size,
                                         queueFamilyIndices, memProps,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                         storageBuffer, storageBufferMemory);
}


VkResult VkGPUHelper::CreateUniformBufferAndBindMem(const VkDevice device,
                                                    const VkDeviceSize size,
                                                    const std::vector<uint32_t> &queueFamilyIndices,
                                                    const VkPhysicalDeviceMemoryProperties *memProps,
                                                    VkBuffer *storageBuffer,
                                                    VkDeviceMemory *storageBufferMemory) {
    VkResult ret = CreateBuffer(device,
                                size,
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_SHARING_MODE_EXCLUSIVE,
                                queueFamilyIndices,
                                storageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *storageBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps,
                                                        memRequirements,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, storageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    ret = vkBindBufferMemory(device, *storageBuffer, *storageBufferMemory, 0);
    if (ret != VK_SUCCESS) {
        Logger() << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateVertexBufferAndBindMem(const VkDevice device,
                                                   const VkDeviceSize size,
                                                   const std::vector<uint32_t> &queueFamilyIndices,
                                                   const VkPhysicalDeviceMemoryProperties *memProps,
                                                   VkBuffer *storageBuffer,
                                                   VkDeviceMemory *storageBufferMemory) {
    VkResult ret = CreateBuffer(device,
                                size,
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_SHARING_MODE_EXCLUSIVE,
                                queueFamilyIndices,
                                storageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *storageBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps,
                                                        memRequirements,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, storageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    ret = vkBindBufferMemory(device, *storageBuffer, *storageBufferMemory, 0);
    if (ret != VK_SUCCESS) {
        Logger() << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}

VkResult VkGPUHelper::CreateIndexBufferAndBindMem(const VkDevice device,
                                                  const VkDeviceSize size,
                                                  const std::vector<uint32_t> &queueFamilyIndices,
                                                  const VkPhysicalDeviceMemoryProperties *memProps,
                                                  VkBuffer *storageBuffer,
                                                  VkDeviceMemory *storageBufferMemory) {
    VkResult ret = CreateBuffer(device,
                                size,
                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                VK_SHARING_MODE_EXCLUSIVE,
                                queueFamilyIndices,
                                storageBuffer);
    if (ret != VK_SUCCESS) {
        Logger() << "vkCreateBuffer failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *storageBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.pNext = nullptr;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GetRequiredMemTypeIndex(memProps,
                                                        memRequirements,
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ret = vkAllocateMemory(device, &allocInfo, nullptr, storageBufferMemory);
    if (ret != VK_SUCCESS) {
        Logger() << "vkAllocateMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    ret = vkBindBufferMemory(device, *storageBuffer, *storageBufferMemory, 0);
    if (ret != VK_SUCCESS) {
        Logger() << "vkBindBufferMemory failed, err=" << string_VkResult(ret) << std::endl;
        return ret;
    }
    return ret;
}
