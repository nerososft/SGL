//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <ostream>
#include <vector>
#include <gpu_engine/gpu/compute_graph/ComputeGraph.h>

#include "GPUBezierThickLineGenerator.h"

int main(int argc, char *argv[]) {
    std::cout << "mindmaster_demo" << std::endl;

    std::vector<BezierLine> lines{};
    for (uint32_t i = 0; i < 100; i++) {
        BezierLine line{
            .points = {
                {.x = 0.0, .y = 100.0f + 8.0f * static_cast<float>(i)},
                {.x = 600.0, .y = -100.0},
                {.x = 400.0, .y = 1100.0},
                {.x = 1000.0, .y = 900.0},
            },
            .beginWidth = 100.0f,
            .endWidth = 10.0f,
        };
        lines.push_back(line);
    }

    BezierParams bezierParams{};
    bezierParams.lineNums = 100;
    bezierParams.numPoints = 1024;
    bezierParams.debugPixelMap = true;

    GPUBezierThickLineGenerator utils;
    utils.SetParams(bezierParams);

    if (!utils.InitializeGPUPipeline()) {
        std::cerr << "Failed to initialize GPU pipeline." << std::endl;
        return EXIT_FAILURE;
    }

    const Point2D *points = utils.GenerateThickLine(lines);
    for (uint32_t lineIdx = 0; lineIdx < bezierParams.lineNums; lineIdx++) {
        for (uint32_t pointIdx = 0; pointIdx < bezierParams.numPoints; pointIdx++) {
            const uint32_t offset = lineIdx * bezierParams.numPoints * 2;
            Point2D up = points[offset + pointIdx];
            Point2D down = points[offset * bezierParams.numPoints + pointIdx];
        }
    }
    return 0;
}
