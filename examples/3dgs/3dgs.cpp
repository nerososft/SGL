//
// Created by neo on 25-5-20.
//

#include <core/gpu/compute_graph/ComputeGraph.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "GaussianSplatting3DRenderer.h"
#include "SpzModel.h"
#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"

int main(int argc, char *argv[]) {
  std::cout << "3dgs_demo" << std::endl;
  SpzModel model;
  const bool loaded =
      model.loadModel("../../../examples/3dgs/assets/builtin.models/butterfly");
  if (!loaded) {
    std::cout << "Failed to load model" << std::endl;
    return 1;
  }

  std::vector<GaussianPoint> points = model.getPoints();

  points.push_back({.position = {0, 0, 0, 1},
                    .scale = {100, 1, 1, 1},
                    .color = {1, 0, 0, 1},
                    .rotate = {0, 1, 0, 0},
                    .opacity = {0, 0, 0, 0}});

  GaussianSplatting3DParams gaussianSplatting3dParams{};
  gaussianSplatting3dParams.numPoints = points.size();
  gaussianSplatting3dParams.width = 1024;
  gaussianSplatting3dParams.height = 1024;

  gaussianSplatting3dParams.view =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 6000.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  gaussianSplatting3dParams.proj =
      glm::perspective(glm::radians(60.0f),
                       static_cast<float>(gaussianSplatting3dParams.width) /
                           static_cast<float>(gaussianSplatting3dParams.height),
                       0.1f, 10000.0f);

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

  utils.GeneratePixelMap("../../../examples/3dgs/3dgs.png");

  return 0;
}
