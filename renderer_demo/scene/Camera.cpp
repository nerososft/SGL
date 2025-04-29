//
// Created by neo on 25-4-28.
//

#include "Camera.h"
#include  <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3 position, const glm::vec3 up) {
    this->position = position;
    this->up = up;
}

glm::mat4 Camera::GetViewMatrix() const {
    const glm::mat4 view = glm::lookAt(this->position, glm::vec3(0, 0, 0), this->up);
    return view;
}

glm::mat4 Camera::GetProjectionMatrix(const float aspectRatio) {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}
