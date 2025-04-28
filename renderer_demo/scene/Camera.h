//
// Created by neo on 25-4-28.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

class Camera {
    glm::vec3 position{};
    glm::vec3 up{};

public:
    Camera(glm::vec3 position, glm::vec3 up);

    ~Camera() = default;

    void SetPosition(const glm::vec3 position) {
        this->position = position;
    }

    [[nodiscard]] glm::vec3 GetPosition() const { return position; }

    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    static glm::mat4 GetProjectionMatrix(float aspectRatio);
};


#endif //CAMERA_H
