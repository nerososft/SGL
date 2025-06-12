//
// Created by neo on 25-5-20.
//

#include <iostream>
#include <vector>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <core/gpu/compute_graph/ComputeGraph.h>

#include "GaussianSplatting3DRenderer.h"
#include "SpzModel.h"
#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"

int main(int argc, char *argv[]) {
    std::cout << "3dgs_demo" << std::endl;
    // SpzModel model;
    // bool loaded = model.loadModel("../../../demos/3dgs/assets/builtin.models/butterfly");
    //
    // return 0;

    std::vector<GaussianPoint> points{};

    float x = 512;
    float y = 512;
    float z = 0;

    float r = 1.0f;
    float g = 0;
    float b = 1.0f;

    float sx = 100;
    float sy = 100;
    float sz = 100;

    float rx = 45;
    float ry = 45;
    float rz = 45;

    const GaussianPoint point{
        .position = glm::vec4(x, y, z, 1.0f),
        .color = glm::vec4(r, g, b, 1.0f),
        .scale = glm::vec4(sx, sy, sz, 0),
        .rotate = glm::vec4(rx, ry, rz, 1),
        .opacity = glm::vec4(1.0f, 0, 0, 0),
    };
    points.push_back(point);

    GaussianSplatting3DParams gaussianSplatting3dParams{};
    gaussianSplatting3dParams.numPoints = points.size();
    gaussianSplatting3dParams.width = 1024;
    gaussianSplatting3dParams.height = 1024;

    gaussianSplatting3dParams.view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    gaussianSplatting3dParams.proj = glm::perspective(
        glm::radians(60.0f),
        static_cast<float>(gaussianSplatting3dParams.width) / static_cast<float>(gaussianSplatting3dParams.height),
        0.1f, 10.0f
    );

    gaussianSplatting3dParams.proj[1][1] *= -1;

    static GaussianSplatting3DRenderer utils;
    utils.SetParams(gaussianSplatting3dParams);
    if (!utils.InitializeGPUPipeline(10000000)) {
        std::cerr << "Failed to initialize GPU pipeline." << std::endl;
        return EXIT_FAILURE;
    }

    const uint64_t now = TimeUtils::GetCurrentMonoMs();
    utils.Render(points);
    const uint64_t last = TimeUtils::GetCurrentMonoMs();
    std::cout << "Totally Usage: " << last - now << " ms" << std::endl;

    utils.GeneratePixelMap("../../../demos/3dgs/3dgs.png");

    return 0;
}
