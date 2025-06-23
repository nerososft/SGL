//
// Created by neo on 25-6-23.
//

#ifndef TRANSFORMUTILS_H
#define TRANSFORMUTILS_H
#include <vector>
#include <glm/fwd.hpp>


class TransformUtils {
public:
    TransformUtils() = default;

    ~TransformUtils() = default;

    static glm::mat4 Transform(const std::vector<glm::vec3> &from, const std::vector<glm::vec3> &to);
};

#endif //TRANSFORMUTILS_H
