//
// Created by neo on 25-4-29.
//

#ifndef RENDERERCAMERA_H
#define RENDERERCAMERA_H
#include <glm/glm.hpp>

#include "runtime/gpu/VkGPUBuffer.h"
#include "runtime/gpu/compute_graph/IComputeGraphNode.h"

struct ViewProjectionMatrix {
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec4 position;
  glm::vec4 up;
};

class RendererCamera {
  ViewProjectionMatrix viewProjectionMatrix{};

  std::shared_ptr<VkGPUBuffer> viewProjectionBuffer = nullptr;

  float moveSpeed = 0.05;

  [[nodiscard]] glm::mat4 GetInitialViewMatrix() const;

  static glm::mat4 GetInitialProjectionMatrix(float aspectRatio);

public:
  RendererCamera(glm::vec3 position, glm::vec3 up);

  ~RendererCamera() = default;

  void SetPosition(const glm::vec3 position) {
    this->viewProjectionMatrix.position = glm::vec4(position, 1.0f);
  }

  [[nodiscard]] glm::vec3 GetPosition() const {
    return {this->viewProjectionMatrix.position.x,
            this->viewProjectionMatrix.position.y,
            this->viewProjectionMatrix.position.z};
  }

  [[nodiscard]] glm::mat4 GetViewMatrix() const;

  void SetViewMatrix(const glm::mat4 &viewMatrix);

  [[nodiscard]] glm::mat4 GetProjectionMatrix() const;

  void Destroy();

  bool CreateGPUCamera(const std::shared_ptr<VkGPUContext> &gpuCtx,
                       float aspectRatio);

  [[nodiscard]] PipelineNodeBuffer GetViewProjectionMatrixBufferNode() const;

  void MoveForward();

  void MoveBackward();

  void MoveLeft();

  void MoveRight();
};

#endif // RENDERERCAMERA_H
