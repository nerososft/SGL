//
// Created by neo on 25-6-3.
//

#ifndef GAUSSIAN_SPLATTING_3D_H
#define GAUSSIAN_SPLATTING_3D_H
#include <vector>
#include <glm/glm.hpp>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"

struct GaussianPoint3D {
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec4 scale; // Covariance Matrix
    glm::vec4 rotate; // Covariance Matrix
    glm::vec4 opacity;
};

struct GaussianSplatting3DParams {
    glm::mat4 view;
    glm::mat4 proj;
    uint32_t numPoints;
    uint32_t width;
    uint32_t height;
};

class GaussianSplatting3DRenderer {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<ComputePipelineNode> bezierNode = nullptr;

    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> pixelMapBuffer = nullptr;

    GaussianSplatting3DParams params{};

public:
    GaussianSplatting3DRenderer() = default;

    ~GaussianSplatting3DRenderer();

    bool InitializeGPUPipeline(size_t maxPoints);

    void Render(const std::vector<GaussianPoint3D> &points) const;

    void GeneratePixelMap(const std::string &path) const;

    void SetParams(const GaussianSplatting3DParams &params) {
        this->params = params;
    }
};

#endif //GAUSSIAN_SPLATTING_3D_H
