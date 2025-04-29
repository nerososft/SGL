//
// Created by neo on 25-4-29.
//

#ifndef RENDERERCAMERA_H
#define RENDERERCAMERA_H
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>


class RendererCamera {
    glm::vec3 position{};
    glm::vec3 up{};

public:
    RendererCamera(glm::vec3 position, glm::vec3 up);

    ~RendererCamera() = default;

    void SetPosition(const glm::vec3 position) {
        this->position = position;
    }

    [[nodiscard]] glm::vec3 GetPosition() const { return position; }

    [[nodiscard]] glm::mat4 GetViewMatrix() const;

    static glm::mat4 GetProjectionMatrix(float aspectRatio);
};


#endif //RENDERERCAMERA_H
