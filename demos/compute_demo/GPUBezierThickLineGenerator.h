//
// Created by neo on 25-6-3.
//

#ifndef BEZIER_THICK_LINE_UTILS_H
#define BEZIER_THICK_LINE_UTILS_H
#include <vector>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"

#define MAX_LINE_NUMS (1024)

struct Point2D {
    float x;
    float y;
};

struct BezierLine {
    Point2D points[4];
    float beginWidth;
    float endWidth;
};

struct BezierParams {
    uint lineNums;
    uint numPoints;
    bool debugPixelMap;
};

class GPUBezierThickLineGenerator {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<ComputePipelineNode> bezierNode = nullptr;

    std::shared_ptr<VkGPUBuffer> inputBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> outputBuffer = nullptr;
    std::shared_ptr<VkGPUBuffer> pixelMapBuffer = nullptr;

    BezierParams params{};

public:
    GPUBezierThickLineGenerator() = default;

    ~GPUBezierThickLineGenerator();

    bool InitializeGPUPipeline();

    [[nodiscard]] Point2D *GenerateThickLine(const std::vector<BezierLine> &lines) const;

    void GeneratePixelMap(const std::string &path) const;

    void SetParams(const BezierParams &params) {
        this->params = params;
    }

    void UnMapOutputBuffer() const {
        if (outputBuffer != nullptr) {
            outputBuffer->UnMapBuffer();
        }
    }
};

#endif //BEZIER_THICK_LINE_UTILS_H
