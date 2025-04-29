//
// Created by neo on 25-4-29.
//

#ifndef RENDERERLIGHT_H
#define RENDERERLIGHT_H
#include <glm/vec4.hpp>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/IComputeGraphNode.h"

struct Light {
    glm::vec4 position;
    glm::vec4 color;
    glm::vec4 direction;
};

class RendererLight {
    Light light{};

    std::shared_ptr<VkGPUBuffer> lightBuffer = nullptr;

public:
    RendererLight() = default;

    ~RendererLight() = default;

    void Destroy();

    glm::vec4 GetLightPosition() const { return light.position; }

    void SetLightPosition(const glm::vec4 &position);

    bool CreateGPULight(const std::shared_ptr<VkGPUContext> &gpuCtx);

    [[nodiscard]] PipelineNodeBuffer GetLightBufferNode() const;
};


#endif //RENDERERLIGHT_H
