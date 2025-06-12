//
// Created by neo on 25-6-3.
//

#ifndef GAUSSIAN_SPLATTING_3D_H
#define GAUSSIAN_SPLATTING_3D_H
#include <vector>
#include <glm/glm.hpp>

#include "GuassianModel.h"
#include "core/gpu/VkGPUBuffer.h"
#include "core/gpu/compute_graph/ComputeGraph.h"
#include "core/gpu/compute_graph/ComputePipelineNode.h"

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
    std::shared_ptr<VkGPUBuffer> depthMapBuffer = nullptr;

    GaussianSplatting3DParams params{};

public:
    GaussianSplatting3DRenderer() = default;

    ~GaussianSplatting3DRenderer();

    bool InitializeGPUPipeline(size_t maxPoints);

    void Render(const std::vector<GaussianPoint> &points) const;

    void GeneratePixelMap(const std::string &path) const;

    void SetParams(const GaussianSplatting3DParams &params) {
        this->params = params;
    }
};

#endif //GAUSSIAN_SPLATTING_3D_H
