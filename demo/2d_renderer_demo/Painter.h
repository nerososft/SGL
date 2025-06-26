//
// Created by neo on 25-6-17.
//

#ifndef PAINTER_H
#define PAINTER_H
#include <vector>

#include "engine/renderer/renderer/Renderer.h"

struct Line {
    float x1, y1, x2, y2;
};

struct Point {
    float x, y;
};

struct Rectangle {
    float x1, y1, x2, y2;
};

class Painter {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;
    std::shared_ptr<SubComputeGraph> subComputeGraph = nullptr;

    std::shared_ptr<ImageToBufferCopyNode> offScreenCopyNode = nullptr;
    std::shared_ptr<VkGPUBuffer> offScreenBuffer = nullptr;

    std::shared_ptr<GraphicsRenderPassNode> mainRenderPassNode = nullptr;
    std::shared_ptr<GraphicsPipelineNode> graphicsPipelineNode = nullptr;
    std::shared_ptr<VkGPUFramebuffer> framebuffer = nullptr;

    uint32_t width = 768;
    uint32_t height = 768;

public:
    Painter() = default;

    ~Painter() = default;

    bool Init(uint32_t width, uint32_t height);

    bool CreateGraphicsPipelines();

    void drawRects(std::vector<Rectangle> rects);

    void drawLines(std::vector<Line> lines);

    void drawPoints(std::vector<Point> points);

    void toImage(const std::string &path) const;
};


#endif //PAINTER_H
