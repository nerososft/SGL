//
// Created by neo on 25-4-29.
//

#ifndef RENDERERCAMERA_H
#define RENDERERCAMERA_H
#include <glm/glm.hpp>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/IComputeGraphNode.h"

struct ViewProjectionMatrix {
    glm::mat4 view;
    glm::mat4 projection;
};

class RendererCamera {
    glm::vec3 position{};
    glm::vec3 up{};

    ViewProjectionMatrix viewProjectionMatrix{};

    std::shared_ptr<VkGPUBuffer> viewProjectionBuffer = nullptr;

    glm::mat4 GetInitialViewMatrix() const;

    static glm::mat4 GetInitialProjectionMatrix(float aspectRatio);

public:
    RendererCamera(glm::vec3 position, glm::vec3 up);

    ~RendererCamera() = default;

    void SetPosition(const glm::vec3 position) {
        this->position = position;
    }

    [[nodiscard]] glm::vec3 GetPosition() const { return position; }

    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    void SetViewMatrix(const glm::mat4 &viewMatrix);

    glm::mat4 GetProjectionMatrix() const;

    void Destroy();

    bool CreateGPUCamera(const std::shared_ptr<VkGPUContext> &gpuCtx, float aspectRatio);

    PipelineNodeBuffer GetViewProjectionMatrixBufferNode() const;
};


#endif //RENDERERCAMERA_H
