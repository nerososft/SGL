//
// Created by neoyang on 2025/3/6.
//

#include <engine/image/blenders/impl/LighterColorBlender.h>
#include <engine/image/filters/impl/MedianFilter.h>
#include <engine/image/filters/impl/OldGaussianBlurFilter.h>
#include <engine/image/filters/impl/RadialBlurFilter.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "engine/image/ImageEngine.h"
#include "engine/image/blenders/impl/DissolveBlender.h"
#include "engine/image/blenders/impl/LighterColorBlender.h"
#include "engine/image/blenders/impl/MultiplyBlender.h"
#include "engine/image/blenders/impl/OverlayBlender.h"
#include "engine/image/filters/impl/ChinesePaintFilter.h"
#include "engine/image/filters/impl/ColorPencilSketchFilter.h"
#include "engine/image/filters/impl/ColorSeparationFilter.h"
#include "engine/image/filters/impl/FastGaussianBlurFilter.h"
#include "engine/image/filters/impl/FocusBlurFilter.h"
#include "engine/image/filters/impl/GaussianBlurFilter.h"
#include "engine/image/filters/impl/GrayFilter.h"
#include "engine/image/filters/impl/NES8BitMosaicFilter.h"
#include "engine/image/filters/impl/PaletteKnifeFilter.h"
#include "engine/image/filters/impl/ScaleFilter.h"
#include "engine/image/filters/impl/SurfaceBlurFilter.h"
#include "engine/image/filters/impl/ThresholdSplitFilter.h"
#include "engine/image/filters/impl/Transform3DFilter.h"
#include "engine/image/filters/impl/TransformFilter.h"
#include "engine/image/filters/impl/VibranceFilter.h"
#include "engine/image/filters/impl/VoronoiFilter.h"
#include "engine/image/utils/TransformUtils.h"
#include "runtime/log/Log.h"

void effect_engine_main() {
  sgl::image::ImageEngine effectEngine;
  if (!effectEngine.Init()) {
    Logger() << "Failed to initialize effect_engine" << std::endl;
    return;
  }

  // const auto filter = std::make_shared<GrayFilter>();
  // filter->SetRedFactor(0.299);
  // filter->SetGreenFactor(0.587);
  // filter->SetBlueFactor(0.114);

  // ImageInfo inputImageInfo{};
  // inputImageInfo.width = 128;
  // inputImageInfo.height = 128;
  // inputImageInfo.channels = 4;
  // inputImageInfo.bytesPerLine = inputImageInfo.width * 4;
  // inputImageInfo.data = malloc(inputImageInfo.width * inputImageInfo.height *
  // inputImageInfo.channels);
  //
  // ImageInfo outputImageInfo{};
  // outputImageInfo.width = 128;
  // outputImageInfo.height = 128;
  // outputImageInfo.channels = 4;
  // outputImageInfo.bytesPerLine = outputImageInfo.width * 4;
  // outputImageInfo.data = malloc(outputImageInfo.width *
  // outputImageInfo.height * outputImageInfo.channels);
  // effectEngine.Process(inputImageInfo, outputImageInfo, filter);

  // const auto filter = std::make_shared<GrayFilter>();
  // filter->SetRedFactor(0.299);
  // filter->SetGreenFactor(0.587);
  // filter->SetBlueFactor(0.114);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_gray.png", filter);

  // const auto filter = std::make_shared<GaussianBlurFilter>();
  // filter->SetRadius(300);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_blur.png", filter);

  // const auto filter = std::make_shared<ChinesePaintFilter>();
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_chinese_paint.png", filter);

  // const auto filter = std::make_shared<ColorPencilSketchFilter>();
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_color_pencil_sketch.png",
  // filter);

  // const auto filter = std::make_shared<ColorSeparationFilter>();
  // filter->SetRedOffsetX(60.0f);
  // filter->SetGreenOffsetX(120.0f);
  // filter->SetBlueOffsetX(180.0f);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_color_separation.png", filter);

  // const auto filter = std::make_shared<ScaleFilter>();
  // filter->SetTargetWidth(409);
  // filter->SetTargetHeight(613);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_scale_down.png", 409, 613,
  // filter);

  // const auto filter = std::make_shared<OldGaussianBlurFilter>();
  // filter->SetRadius(12);
  // effectEngine.Process("../../../examples/image_demo/images/girl_scale_down.png",
  // "../../../examples/image_demo/images/girl_scale_down_blur.png", filter);

  // const auto filter = std::make_shared<ScaleFilter>();
  // filter->SetTargetWidth(4093);
  // filter->SetTargetHeight(6139);
  // effectEngine.Process("../../../examples/image_demo/images/girl_scale_down_blur.png",
  //                      "../../../examples/image_demo/images/girl_scale_up_blur.png",
  //                      4093, 6139, filter);

  // const auto filter = std::make_shared<RadialBlurFilter>();
  // filter->SetCenter(2000, 3000);
  // filter->SetStrength(500);
  // filter->SetRotationStrength(600);
  // filter->SetSharpness(0.1);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_radial_blur.png", filter);

  // const auto filter = std::make_shared<FocusBlurFilter>();
  // filter->SetCenter(390, 510);
  // filter->SetBlurRadius(110);
  // filter->SetCenterRadius(100);
  // filter->SetSharpness(3);
  // effectEngine.Process("../../../examples/image_demo/images/colorful.png",
  // "../../../examples/image_demo/images/colorful_focus_blur.png", filter);

  // const auto filter = std::make_shared<SurfaceBlurFilter>();
  // filter->SetBlurRadius(60);
  // filter->SetThreshold(128);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_surface_blur.png", filter);

  // const auto blender = std::make_shared<DissolveBlender>();
  // blender->SetDissolveFactor(0.5);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  //                      "../../../examples/image_demo/images/colorful.png",
  //                      500,
  //                      800,
  //                      "../../../examples/image_demo/images/girl_colorful_dissolve.png",
  //                      blender);

  // const auto blender = std::make_shared<MultiplyBlender>();
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  //                      "../../../examples/image_demo/images/colorful.png",
  //                      500,
  //                      800,
  //                      "../../../examples/image_demo/images/girl_colorful_multiply.png",
  //                      blender);

  // const auto blender = std::make_shared<OverlayBlender>();
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  //                      "../../../examples/image_demo/images/colorful.png",
  //                      500,
  //                      800,
  //                      "../../../examples/image_demo/images/girl_colorful_lighter.png",
  //                      blender);
  //                      "../../../examples/image_demo/images/girl_colorful_overlay.png",
  //                      blender);

  // const auto blender = std::make_shared<LighterColorBlender>();
  // blender->SetBrightnessThreshold(0.7f);
  // blender->SetBlendFactor(0.3f);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  //                      "../../../examples/image_demo/images/colorful.png",
  //                      500,
  //                      800,
  //                      "../../../examples/image_demo/images/girl_colorful_lighter.png",
  //                      blender);

  // const auto filter = std::make_shared<VibranceFilter>();
  // filter->SetVibrance(40);
  // filter->SetSaturation(50);
  // effectEngine.Process("../../../examples/image_demo/images/colorful.png",
  // "../../../examples/image_demo/images/colorful_vibrance.png", filter);

  // const auto filter = std::make_shared<PaletteKnifeFilter>();
  // filter->SetRadius(10);
  // filter->SetQuantScale(5);
  // effectEngine.Process("../../../examples/image_demo/images/colorful.png",
  // "../../../examples/image_demo/images/colorful_palette_knife.png", filter);

  // const auto filter = std::make_shared<NES8BitMosaicFilter>();
  // filter->SetBlockSize(64);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_8bit.png", filter);

  // const auto filter = std::make_shared<ThresholdSplitFilter>();
  // filter->SetBright(10);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_threshold.png", filter);

  // const auto filter = std::make_shared<VoronoiFilter>();
  // filter->SetGridSize(64);
  // filter->SetEnableEdge(true);
  // effectEngine.Process("../../../examples/image_demo/images/colorful.png",
  // "../../../examples/image_demo/images/colorful_voronoi.png", filter);

  // const auto filter = std::make_shared<OldGaussianBlurFilter>();
  // filter->SetRadius(128);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_blur.png", filter);

  // const auto filter = std::make_shared<FastGaussianBlurFilter>();
  // filter->SetRadius(120);
  // effectEngine.Process("../../../examples/image_demo/images/girl.png",
  // "../../../examples/image_demo/images/girl_fast_blur.png", filter);

  // const auto filter = std::make_shared<MedianFilter>();
  // filter->SetRadius(50);
  // effectEngine.Process("../../../examples/image_demo/images/test.png",
  // "../../../examples/image_demo/images/test_median.png", filter);

  const auto filter = std::make_shared<TransformFilter>();
  // const auto filter = std::make_shared<Transform3DFilter>();
  std::vector<glm::vec3> from;
  from.emplace_back(100.0f, 100.0f, 0.0f);
  from.emplace_back(300.0f, 100.0f, 0.0f);
  from.emplace_back(100.0f, 300.0f, 0.0f);
  from.emplace_back(300.0f, 300.0f, 0.0f);
  std::vector<glm::vec3> to;
  to.emplace_back(100.0f, 100.0f, 0.0f);
  to.emplace_back(300.0f, 100.0f, 0.0f);
  to.emplace_back(100.0f, 300.0f, 0.0f);
  to.emplace_back(500.0f, 500.0f, 0.0f);
  filter->SetTransformMatrix(TransformUtils::Transform(from, to));
  effectEngine.Process(
      "../../../examples/image_demo/images/girl.png",
      "../../../examples/image_demo/images/girl_transform.png", filter);
}

int main(int argc, char *argv[]) {
  Logger() << "Hello World!\n";
  effect_engine_main();
  // getchar();
  return 0;
}
