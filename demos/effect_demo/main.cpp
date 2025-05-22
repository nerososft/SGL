//
// Created by neoyang on 2025/3/6.
//

#include <iostream>
#include <effect_engine/blenders/impl/LighterColorBlender.h>
#include <effect_engine/filters/impl/MedianFilter.h>
#include <effect_engine/filters/impl/OldGaussianBlurFilter.h>
#include <effect_engine/filters/impl/RadialBlurFilter.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "effect_engine/blenders/impl/LighterColorBlender.h"
#include "effect_engine/EffectEngine.h"
#include "effect_engine/blenders/impl/DissolveBlender.h"
#include "effect_engine/blenders/impl/MultiplyBlender.h"
#include "effect_engine/blenders/impl/OverlayBlender.h"
#include "effect_engine/filters/impl/ChinesePaintFilter.h"
#include "effect_engine/filters/impl/ColorPencilSketchFilter.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"
#include "effect_engine/filters/impl/FastGaussianBlurFilter.h"
#include "effect_engine/filters/impl/FocusBlurFilter.h"
#include "effect_engine/filters/impl/GaussianBlurFilter.h"
#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/NES8BitMosaicFilter.h"
#include "effect_engine/filters/impl/PaletteKnifeFilter.h"
#include "effect_engine/filters/impl/ScaleFilter.h"
#include "effect_engine/filters/impl/SurfaceBlurFilter.h"
#include "effect_engine/filters/impl/ThresholdSplitFilter.h"
#include "effect_engine/filters/impl/TransformFilter.h"
#include "effect_engine/filters/impl/VibranceFilter.h"
#include "effect_engine/filters/impl/VoronoiFilter.h"
#include "gpu_engine/log/Log.h"

void effect_engine_main() {
    EffectEngine effectEngine;
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
    // inputImageInfo.data = malloc(inputImageInfo.width * inputImageInfo.height * inputImageInfo.channels);
    //
    // ImageInfo outputImageInfo{};
    // outputImageInfo.width = 128;
    // outputImageInfo.height = 128;
    // outputImageInfo.channels = 4;
    // outputImageInfo.bytesPerLine = outputImageInfo.width * 4;
    // outputImageInfo.data = malloc(outputImageInfo.width * outputImageInfo.height * outputImageInfo.channels);
    // effectEngine.Process(inputImageInfo, outputImageInfo, filter);

    // const auto filter = std::make_shared<GrayFilter>();
    // filter->SetRedFactor(0.299);
    // filter->SetGreenFactor(0.587);
    // filter->SetBlueFactor(0.114);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_gray.png", filter);

    // const auto filter = std::make_shared<GaussianBlurFilter>();
    // filter->SetRadius(300);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_blur.png", filter);

    // const auto filter = std::make_shared<ChinesePaintFilter>();
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_chinese_paint.png", filter);

    // const auto filter = std::make_shared<ColorPencilSketchFilter>();
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_color_pencil_sketch.png", filter);

    // const auto filter = std::make_shared<ColorSeparationFilter>();
    // filter->SetRedOffsetX(60.0f);
    // filter->SetGreenOffsetX(120.0f);
    // filter->SetBlueOffsetX(180.0f);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_color_separation.png", filter);

    // const auto filter = std::make_shared<ScaleFilter>();
    // filter->SetTargetWidth(409);
    // filter->SetTargetHeight(613);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_scale_down.png", 409, 613, filter);

    // const auto filter = std::make_shared<OldGaussianBlurFilter>();
    // filter->SetRadius(12);
    // effectEngine.Process("../../../demos/effect_demo/images/girl_scale_down.png", "../../../demos/effect_demo/images/girl_scale_down_blur.png", filter);

    // const auto filter = std::make_shared<ScaleFilter>();
    // filter->SetTargetWidth(4093);
    // filter->SetTargetHeight(6139);
    // effectEngine.Process("../../../demos/effect_demo/images/girl_scale_down_blur.png",
    //                      "../../../demos/effect_demo/images/girl_scale_up_blur.png", 4093, 6139, filter);

    // const auto filter = std::make_shared<RadialBlurFilter>();
    // filter->SetCenter(2000, 3000);
    // filter->SetStrength(500);
    // filter->SetRotationStrength(600);
    // filter->SetSharpness(0.1);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_radial_blur.png", filter);

    // const auto filter = std::make_shared<FocusBlurFilter>();
    // filter->SetCenter(390, 510);
    // filter->SetBlurRadius(110);
    // filter->SetCenterRadius(100);
    // filter->SetSharpness(3);
    // effectEngine.Process("../../../demos/effect_demo/images/colorful.png", "../../../demos/effect_demo/images/colorful_focus_blur.png", filter);

    // const auto filter = std::make_shared<SurfaceBlurFilter>();
    // filter->SetBlurRadius(60);
    // filter->SetThreshold(128);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_surface_blur.png", filter);

    // const auto blender = std::make_shared<DissolveBlender>();
    // blender->SetDissolveFactor(0.5);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png",
    //                      "../../../demos/effect_demo/images/colorful.png",
    //                      500,
    //                      800,
    //                      "../../../demos/effect_demo/images/girl_colorful_dissolve.png", blender);

    // const auto blender = std::make_shared<MultiplyBlender>();
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png",
    //                      "../../../demos/effect_demo/images/colorful.png",
    //                      500,
    //                      800,
    //                      "../../../demos/effect_demo/images/girl_colorful_multiply.png", blender);

    // const auto blender = std::make_shared<OverlayBlender>();
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png",
    //                      "../../../demos/effect_demo/images/colorful.png",
    //                      500,
    //                      800,
    //                      "../../../demos/effect_demo/images/girl_colorful_lighter.png", blender);
    //                      "../../../demos/effect_demo/images/girl_colorful_overlay.png", blender);

    // const auto blender = std::make_shared<LighterColorBlender>();
    // blender->SetBrightnessThreshold(0.7f);
    // blender->SetBlendFactor(0.3f);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png",
    //                      "../../../demos/effect_demo/images/colorful.png",
    //                      500,
    //                      800,
    //                      "../../../demos/effect_demo/images/girl_colorful_lighter.png", blender);

    // const auto filter = std::make_shared<VibranceFilter>();
    // filter->SetVibrance(40);
    // filter->SetSaturation(50);
    // effectEngine.Process("../../../demos/effect_demo/images/colorful.png", "../../../demos/effect_demo/images/colorful_vibrance.png", filter);

    // const auto filter = std::make_shared<PaletteKnifeFilter>();
    // filter->SetRadius(10);
    // filter->SetQuantScale(5);
    // effectEngine.Process("../../../demos/effect_demo/images/colorful.png", "../../../demos/effect_demo/images/colorful_palette_knife.png", filter);

    // const auto filter = std::make_shared<NES8BitMosaicFilter>();
    // filter->SetBlockSize(64);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_8bit.png", filter);

    // const auto filter = std::make_shared<ThresholdSplitFilter>();
    // filter->SetBright(10);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_threshold.png", filter);

    // const auto filter = std::make_shared<VoronoiFilter>();
    // filter->SetGridSize(64);
    // filter->SetEnableEdge(true);
    // effectEngine.Process("../../../demos/effect_demo/images/colorful.png", "../../../demos/effect_demo/images/colorful_voronoi.png", filter);

    // const auto filter = std::make_shared<OldGaussianBlurFilter>();
    // filter->SetRadius(128);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_blur.png", filter);

    // const auto filter = std::make_shared<FastGaussianBlurFilter>();
    // filter->SetRadius(120);
    // effectEngine.Process("../../../demos/effect_demo/images/girl.png", "../../../demos/effect_demo/images/girl_fast_blur.png", filter);

    // const auto filter = std::make_shared<MedianFilter>();
    // filter->SetRadius(50);
    // effectEngine.Process("../../../demos/effect_demo/images/test.png", "../../../demos/effect_demo/images/test_median.png", filter);

    const auto filter = std::make_shared<TransformFilter>();

    glm::mat4 result(
        // 第一列
        1.01035, -0.0184876, 0.0f, 30.5184,
        // 第二列
        0.154128, 0.578685, 0.0f, 120.122,
        // 第三列
        0.0f, 0.0f, 1.0f, 0.0f,
        // 第四列 - 包含透视元素
        0.00054694, -0.000263822, 0.0f, 0.975477
    );

    filter->SetTransformMatrix(result);
    effectEngine.Process("../../../demos/effect_demo/images/girl.png",
                         "../../../demos/effect_demo/images/girl_transform.png", filter);
}

int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";
    effect_engine_main();
    // getchar();
    return 0;
}
