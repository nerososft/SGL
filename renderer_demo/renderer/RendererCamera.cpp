//
// Created by neo on 25-4-29.
//

#include "RendererCamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

RendererCamera::RendererCamera(const glm::vec3 position, const glm::vec3 up) {
    this->position = position;
    this->up = up;
}

glm::mat4 RendererCamera::GetViewMatrix() const {
    const glm::mat4 view = glm::lookAt(this->position, glm::vec3(0, 0, 0), this->up);
    return view;
}

glm::mat4 RendererCamera::GetProjectionMatrix(const float aspectRatio) {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}
