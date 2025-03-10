//
// Created by neoyang on 2025/3/6.
//

#include <iostream>

#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/GrayFilter.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello World!\n";


    EffectEngine effectEngine;
    if (!effectEngine.Init()) {
        std::cout << "Failed to initialize effect_engine" << std::endl;
        return 0;
    }

    const auto filter = std::make_shared<GrayFilter>();
    filter->SetRedFactor(0.2);
    filter->SetGreenFactor(0.2);
    filter->SetBlueFactor(0.2);

    ImageInfo inputImageInfo{};
    inputImageInfo.width = 128;
    inputImageInfo.height = 128;
    inputImageInfo.channels = 4;
    inputImageInfo.bytesPerLine = inputImageInfo.width * 4;
    inputImageInfo.data = malloc(inputImageInfo.width * inputImageInfo.height * inputImageInfo.channels);

    ImageInfo outputImageInfo{};
    outputImageInfo.width = 128;
    outputImageInfo.height = 128;
    outputImageInfo.channels = 4;
    outputImageInfo.bytesPerLine = outputImageInfo.width * 4;
    outputImageInfo.data = malloc(outputImageInfo.width * outputImageInfo.height * outputImageInfo.channels);
    effectEngine.Process(inputImageInfo, outputImageInfo, filter);

    return 0;
}
