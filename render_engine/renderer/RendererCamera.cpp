//
// Created by neo on 25-4-29.
//

#include "RendererCamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "gpu_engine/log/Log.h"

RendererCamera::RendererCamera(const glm::vec3 position, const glm::vec3 up) {
    this->viewProjectionMatrix.position = glm::vec4(position, 1.0f);
    this->viewProjectionMatrix.up = glm::vec4(up, 1.0f);
}

glm::mat4 RendererCamera::GetInitialViewMatrix() const {
    const glm::mat4 view = glm::lookAt(glm::vec3(this->viewProjectionMatrix.position.x,
                                                 this->viewProjectionMatrix.position.y,
                                                 this->viewProjectionMatrix.position.z),
                                       glm::vec3(0, 0, 0),
                                       glm::vec3(this->viewProjectionMatrix.up.x,
                                                 this->viewProjectionMatrix.up.y,
                                                 this->viewProjectionMatrix.up.z));
    return view;
}

glm::mat4 RendererCamera::GetInitialProjectionMatrix(const float aspectRatio) {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
}

glm::mat4 RendererCamera::GetViewMatrix() const {
    return viewProjectionMatrix.view;
}

void RendererCamera::SetViewMatrix(const glm::mat4 &viewMatrix) {
    this->viewProjectionMatrix.view = viewMatrix;
    this->viewProjectionBuffer->UploadData(&viewProjectionMatrix, sizeof(ViewProjectionMatrix));
}

glm::mat4 RendererCamera::GetProjectionMatrix() const {
    return viewProjectionMatrix.projection;
}

void RendererCamera::Destroy() {
    if (viewProjectionBuffer != nullptr) {
        viewProjectionBuffer->Destroy();
        viewProjectionBuffer = nullptr;
    }
}

bool RendererCamera::CreateGPUCamera(const std::shared_ptr<VkGPUContext> &gpuCtx, const float aspectRatio) {
    viewProjectionBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (viewProjectionBuffer == nullptr) {
        Logger() << "vp is null" << std::endl;
        return false;
    }
    const VkDeviceSize vpBufferSize = sizeof(ViewProjectionMatrix);
    VkResult ret = viewProjectionBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, vpBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "VP buffer allocate and bind failed" << std::endl;
        return false;
    }
    this->viewProjectionMatrix.view = GetInitialViewMatrix();
    this->viewProjectionMatrix.projection = GetInitialProjectionMatrix(aspectRatio);
    ret = viewProjectionBuffer->UploadData(&viewProjectionMatrix, vpBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "VP buffer upload failed" << std::endl;
        return false;
    }
    return true;
}

PipelineNodeBuffer RendererCamera::GetViewProjectionMatrixBufferNode() const {
    PipelineNodeBuffer vpBufferNode = {};
    vpBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
    vpBufferNode.buffer = viewProjectionBuffer->GetBuffer();
    vpBufferNode.bufferSize = viewProjectionBuffer->GetBufferSize();
    return vpBufferNode;
}
