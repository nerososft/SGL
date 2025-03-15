#include <stdint.h>


#include <iostream>
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ChinesePaintFilter.h"
#include "effect_engine/filters/impl/ColorPencilSketchFilter.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"
#include "effect_engine/filters/impl/GaussianBlurFilter.h"
#include "effect_engine/filters/impl/thresholdSplit.h"
#include "gpu_filter.h"





EffectEngine effectEngine;


bool threshold_split_filter_gpu(void* in, void* out, int bright) {

    const auto filter = std::make_shared<thresholdSplitFilter>();
    filter->SetBright(bright);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}


bool gaussian_blur_filter_gpu(void* in, void* out, int r) {
    const auto filter = std::make_shared<GaussianBlurFilter>();
    filter->SetRadius(r);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}


bool init_gpu_engine() {

    return effectEngine.Init();

}

bool destory_gpu_engine() {

    return true;

}