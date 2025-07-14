//
// Created by neo on 25-4-29.
//

#ifndef RENDERERLIGHT_H
#define RENDERERLIGHT_H
#include <glm/vec4.hpp>

#include "examples/3d_renderer_demo/scene/Scene.h"
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/compute_graph/IComputeGraphNode.h"

class RendererLight {
    Light light{};

    std::shared_ptr<VkGPUBuffer> lightBuffer = nullptr;

public:
    RendererLight() = default;

    ~RendererLight() = default;

    void Destroy();

    glm::vec4 GetLightPosition() const { return light.position; }

    void SetLightPosition(const glm::vec4 &position);

    void SetLightColor(glm::vec4 color);

    bool CreateGPULight(const std::shared_ptr<VkGPUContext> &gpuCtx);

    [[nodiscard]] PipelineNodeBuffer GetLightBufferNode() const;
};


#endif //RENDERERLIGHT_H
