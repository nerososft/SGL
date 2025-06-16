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
    for (uint32_t i = 0; i < 1; i++) {
        BezierLine line{
            .points = {
                {.x = 500, .y = 500},
               {.x = 300.0, .y = 100.0},
               {.x = 800.0, .y = 100.0},
               {.x = 900.0, .y = 100.0},
            },
            .beginWidth = 100.0f,
            .endWidth = 1.0f,
        };
        lines.push_back(line);
    }

    BezierParams bezierParams{};
    bezierParams.bodyPointsNums = 1024;
    bezierParams.assPointsNums = 30;
    bezierParams.headPointsNums = 10;

    static GPUBezierThickLineGenerator utils;

    if (!utils.InitializeGPUPipeline(bezierParams)) {
        std::cerr << "Failed to initialize GPU pipeline." << std::endl;
        return EXIT_FAILURE;
    }

    const uint64_t now = TimeUtils::GetCurrentMonoMs();
    const Point2D *points = utils.GenerateThickLine(lines);
    const uint64_t last = TimeUtils::GetCurrentMonoMs();

    const size_t pointsNums = bezierParams.bodyPointsNums + bezierParams.assPointsNums + bezierParams.headPointsNums;
    std::cout << "Totally Usage: " << last - now << " ms" << std::endl;
    for (uint32_t lineIdx = 0; lineIdx < bezierParams.lineNums; lineIdx++) {
        for (uint32_t pointIdx = 0; pointIdx < pointsNums * 2; pointIdx++) {
            const uint32_t offset = lineIdx * pointsNums * 2;
            const auto [x, y] = points[offset + pointIdx];
            // std::cout << "(" << x << "," << y << ")";
        }
        // std::cout << std::endl;
    }

    return 0;
}
