//
// Created by neo on 25-6-3.
//

#ifndef BEZIERTHICKLINEUTILS_H
#define BEZIERTHICKLINEUTILS_H
#include <map>
#include <vector>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"
#include "gpu_engine/gpu/compute_graph/ComputePipelineNode.h"

struct Point2D {
    float x;
    float y;
};

struct BezierLine {
    Point2D points[4];
    float beginWidth;
    float endWidth;
};

struct GPUBezierLine {
    BezierLine bezierLine;
    std::shared_ptr<VkGPUBuffer> buffer;
};

struct BezierParams {
    uint lineNums;

    uint numPoints; // 生成的点数量
    float tStart; // 参数t的起始值
    float tEnd; // 参数t的结束值

    uint width;
    uint height;
};

class BezierThickLineUtils {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> computeSubGraph = nullptr;
    std::shared_ptr<ComputePipelineNode> bezierNode = nullptr;

    BezierParams params{};
    std::map<BezierLine, GPUBezierLine> lines;

public:
    bool InitializeGPUPipeline();

    std::vector<std::vector<Point2D> > GenerateThickLine(const std::vector<BezierLine> &lines);

    void FreeGPUThinkLine(BezierLine line);

    void SetParams(const BezierParams &params) {
        this->params = params;
    };
};


#endif //BEZIERTHICKLINEUTILS_H
