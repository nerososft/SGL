//
// Created by neoyang on 2025/3/6.
//

#include <iostream>

#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ChinesePaintFilter.h"
#include "effect_engine/filters/impl/ColorPencilSketchFilter.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"
#include "effect_engine/filters/impl/GaussianBlurFilter.h"
#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/ScaleFilter.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello World!\n";

    EffectEngine effectEngine;
    if (!effectEngine.Init()) {
        std::cout << "Failed to initialize effect_engine" << std::endl;
        return 0;
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
    // effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_gray.png", filter);

    // const auto filter = std::make_shared<GaussianBlurFilter>();
    // filter->SetRadius(300);
    // effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_blur.png", filter);

    // const auto filter = std::make_shared<ChinesePaintFilter>();
    // effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_chinese_paint.png", filter);

    // const auto filter = std::make_shared<ColorPencilSketchFilter>();
    // effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_color_pencil_sketch.png", filter);

    // const auto filter = std::make_shared<ColorSeparationFilter>();
    // filter->SetRedOffsetX(60.0f);
    // filter->SetGreenOffsetX(120.0f);
    // filter->SetBlueOffsetX(180.0f);
    // effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_color_separation.png", filter);

    const auto filter = std::make_shared<ScaleFilter>();
    filter->SetTargetWidth(400);
    filter->SetTargetHeight(600);
    effectEngine.Process("../../demo/images/girl.png", "../../demo/images/girl_scale.png", 400, 600, filter);


    return 0;
}
