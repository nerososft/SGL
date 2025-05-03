//
// Created by neo on 25-4-29.
//

#include "RendererLight.h"

#include "gpu_engine/log/Log.h"

bool RendererLight::CreateGPULight(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    lightBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (lightBuffer == nullptr) {
        Logger() << "lightBuffer is null" << std::endl;
        return false;
    }
    const VkDeviceSize lightBufferSize = sizeof(Light);
    VkResult ret = lightBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, lightBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Light buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = lightBuffer->UploadData(&light, lightBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Light buffer upload failed" << std::endl;
        return false;
    }
    return true;
}

PipelineNodeBuffer RendererLight::GetLightBufferNode() const {
    PipelineNodeBuffer lightBufferNode = {};
    lightBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
    lightBufferNode.buffer = lightBuffer->GetBuffer();
    lightBufferNode.bufferSize = lightBuffer->GetBufferSize();
    return lightBufferNode;
}

void RendererLight::Destroy() {
    if (lightBuffer != nullptr) {
        lightBuffer->Destroy();
        lightBuffer = nullptr;
    }
}

void RendererLight::SetLightPosition(const glm::vec4 &position) {
    this->light.position = position;
    this->lightBuffer->UploadData(&light, sizeof(Light));
}

void RendererLight::SetLightColor(const glm::vec4 color) {
    this->light.color = color;
    this->lightBuffer->UploadData(&light, sizeof(Light));
}
