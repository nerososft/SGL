//
// Created by 1234 on 2025/3/6.
//

#ifndef TRANSFORMFILTER_H
#define TRANSFORMFILTER_H
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "effect_engine/filters/BasicFilter.h"
#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/VkGPUContext.h"
#include "gpu_engine/gpu/compute_graph/GraphicsPipelineNode.h"
#include "gpu_engine/gpu/compute_graph/GraphicsRenderPassNode.h"
#include "gpu_engine/gpu/compute_graph/ImageToBufferCopyNode.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoords;
};

struct TransformFilterParams {
    BasicFilterParam imageSize;
    glm::mat4 transformMatrix;
};

class Transform3DFilter final : public BasicFilter {
    TransformFilterParams transformFilterParams{};

    uint32_t width = 768;
    uint32_t height = 768;

    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;

    std::shared_ptr<ImageToBufferCopyNode> offScreenCopyNode = nullptr;

    std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
    std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;
    std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

    std::shared_ptr<VkGPUBuffer> vertexBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> indicesBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> transformMatrixBuffer = nullptr;

    VkImage textureImage = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

public:
    Transform3DFilter() = default;

    ~Transform3DFilter() override = default;

    [[nodiscard]] PipelineNodeBuffer GetVertexBufferNode() const;

    [[nodiscard]] PipelineNodeBuffer GetIndicesBufferNode() const;

    [[nodiscard]] PipelineNodeBuffer GetTransformMatrixBufferNode() const;

    PipelineNodeBuffer GetTextureBufferNode() const;

    VkResult ConstructMainGraphicsPipeline();

    VkResult AddDrawElement(const std::vector<Vertex> &vertexData,
                            const std::vector<uint32_t> &indicesData,
                            const glm::mat4 &transform);

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   const std::vector<FilterImageInfo> &inputImageInfo,
                   const std::vector<FilterImageInfo> &outputImageInfo) override;

    void SetTransformMatrix(const glm::mat4 &matrix) { this->transformFilterParams.transformMatrix = matrix; }

    void Destroy() override;
};


#endif //TRANSFORMFILTER_H
