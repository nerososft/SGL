//
// Created by neo on 25-4-13.
//

#ifndef IMAGECOPYNODE_H
#define IMAGECOPYNODE_H
#include <vulkan/vulkan_core.h>

#include "IComputeGraphNode.h"
#include "gpu_engine/gpu/VkGPUContext.h"

typedef struct {
    VkDeviceSize bufferSize;
    VkBuffer buffer;
} ImageToCopyNodeBufferInfo;

typedef struct {
    uint32_t rowLength;
    uint32_t height;
    uint32_t width;
    VkImage image;
} ImageToCopyNodeImageInfo;

class ImageToBufferCopyNode final : public IComputeGraphNode {
    ImageToCopyNodeImageInfo srcImage{};
    ImageToCopyNodeBufferInfo dstBuffer{};
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;

public:
    ImageToBufferCopyNode(const std::shared_ptr<VkGPUContext> &gpuCtx,
                          const std::string &name,
                          ImageToCopyNodeImageInfo srcImage,
                          ImageToCopyNodeBufferInfo dstBuffer);

    ~ImageToBufferCopyNode() override = default;

    VkResult CreateComputeGraphNode() override;

    void Compute(VkCommandBuffer commandBuffer) override;

    void Destroy() override;
};


#endif //IMAGECOPYNODE_H
