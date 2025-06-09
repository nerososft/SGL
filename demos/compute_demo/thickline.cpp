//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <ostream>
#include <vector>
#include <core/gpu/compute_graph/ComputeGraph.h>

#include "GPUBezierThickLineGenerator.h"
#include "core/utils/TimeUtils.h"

int main(int argc, char *argv[]) {
    std::cout << "mindmaster_demo" << std::endl;

    std::vector<BezierLine> lines{};
    for (uint32_t i = 0; i < 4096; i++) {
        BezierLine line{
            .points = {
                {.x = 0.0, .y = 0.0f + 0.2f * static_cast<float>(i)},
                {.x = 300.0, .y = -100.0},
                {.x = 600.0, .y = 800.0},
                {.x = 1000.0, .y = 1100.0},
            },
            .beginWidth = 100.0f,
            .endWidth = 1.0f,
        };
        lines.push_back(line);
    }

    BezierParams bezierParams{};
    bezierParams.lineNums = 3581;
    bezierParams.numPoints = 1024;
    bezierParams.debugPixelMap = true;

    static GPUBezierThickLineGenerator utils;
    utils.SetParams(bezierParams);

    if (!utils.InitializeGPUPipeline()) {
        std::cerr << "Failed to initialize GPU pipeline." << std::endl;
        return EXIT_FAILURE;
    }

    const uint64_t now = TimeUtils::GetCurrentMonoMs();
    const Point2D *points = utils.GenerateThickLine(lines);
    const uint64_t last = TimeUtils::GetCurrentMonoMs();

    std::cout << "Totally Usage: " << last - now << " ms" << std::endl;
    for (uint32_t lineIdx = 0; lineIdx < bezierParams.lineNums; lineIdx++) {
        for (uint32_t pointIdx = 0; pointIdx < bezierParams.numPoints * 2; pointIdx++) {
            const uint32_t offset = lineIdx * bezierParams.numPoints * 2;
            const auto [x, y] = points[offset + pointIdx];
            // std::cout << "(" << x << "," << y << ")";
        }
        // std::cout << std::endl;
    }
    utils.GeneratePixelMap("../../../demos/compute_demo/line.png");

    return 0;
}
