//
// Created by neo on 25-6-18.
//

#include "TransformFilter.h"

#include "core/config.h"

bool isSingular(const glm::mat4 &mat, const double epsilon = 1e-6) {
    const double det = glm::determinant(mat);
    return std::fabs(det) < epsilon;
}

void TransformFilter::SetTransform(const std::vector<glm::vec3> &from, const std::vector<glm::vec3> &to) {
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

    this->transformFilterParams.transformMatrix = transform;
}

VkResult TransformFilter::Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                const std::vector<FilterImageInfo> &inputImageInfo,
                                const std::vector<FilterImageInfo> &outputImageInfo) {
    BasicFilterParams params;
    this->transformFilterParams.imageSize.width = inputImageInfo[0].width;;
    this->transformFilterParams.imageSize.height = inputImageInfo[0].height;;
    this->transformFilterParams.imageSize.channels = 4;
    this->transformFilterParams.imageSize.bytesPerLine = this->transformFilterParams.imageSize.width * 4;
    params.paramsSize = sizeof(TransformFilterParams);
    params.paramsData = &this->transformFilterParams;
    params.shaderPath = SHADER(transform.comp.glsl.spv);

    return BasicFilter::Apply(gpuCtx,
                              "Transform",
                              inputImageInfo[0].bufferSize,
                              inputImageInfo[0].storageBuffer,
                              outputImageInfo[0].storageBuffer,
                              params,
                              (outputImageInfo[0].width + 31) / 32,
                              (outputImageInfo[0].height + 31) / 32,
                              1);
}

void TransformFilter::Destroy() {
    BasicFilter::Destroy();
}
