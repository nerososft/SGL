//
// Created by neo on 25-6-23.
//

#include "TransformUtils.h"

#include <glm/matrix.hpp>

bool isSingular(const glm::mat4 &mat, const double epsilon = 1e-6) {
    const double det = glm::determinant(mat);
    return std::fabs(det) < epsilon;
}

glm::mat4 TransformUtils::Transform(const std::vector<glm::vec3> &from, const std::vector<glm::vec3> &to) {
    assert(from.size() == to.size() && "X and Y must have the same number of samples");

    glm::mat4 X_m(0.0f);
    glm::mat4 Y_m(0.0f);
    for (int i = 0; i < from.size(); i++) {
        X_m[0][i] = from[i].x;
        X_m[1][i] = from[i].y;
        X_m[2][i] = from[i].z;
        X_m[3][i] = 1.0f;
        Y_m[0][i] = to[i].x;
        Y_m[1][i] = to[i].y;
        Y_m[2][i] = to[i].z;
        Y_m[3][i] = 1.0f;
    }

    const glm::mat4 X_mat = glm::transpose(X_m);
    const glm::mat4 Y_mat = glm::transpose(Y_m);

    glm::mat4 XXT = X_mat * glm::transpose(X_mat);
    if (isSingular(XXT)) {
        constexpr float lambda = 1e-6f;
        XXT = XXT + lambda * glm::mat4(1.0f);
    }
    const glm::mat4 XXT_inv = glm::inverse(XXT);
    const glm::mat4 A = Y_mat * glm::transpose(X_mat);
    const glm::mat4 trans = A * XXT_inv;
    const glm::mat4 transform = glm::transpose(trans);

    return transform;
}
