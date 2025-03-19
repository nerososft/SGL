#include "c_api.h"
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"

#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/OldGaussianBlurFilter.h"
#include "effect_engine/filters/impl/SurfaceBlurFilter.h"
#include "effect_engine/filters/impl/VibranceFilter.h"
#include "effect_engine/filters/impl/ThresholdSplitFilter.h"

#include "log/Log.h"

FunctionPtr g_dbg = nullptr;
EffectEngine g_effect_engine;


bool threshold_split_filter_gpu(void *in, void *out, const int bright) {
    const auto filter = std::make_shared<ThresholdSplitFilter>();
    filter->SetBright(bright);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool gaussian_blur_filter_gpu(void *in, void *out, const int r) {
    const auto filter = std::make_shared<OldGaussianBlurFilter>();
    filter->SetRadius(r);

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool surface_blur_filter_gpu(void *in, void *out, const int r, const int th) {
    const auto filter = std::make_shared<SurfaceBlurFilter>();
    filter->SetBlurRadius(r);
    filter->SetThreshold(th);

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool adjust_saturation_gpu(void *in, void *out, const int v, const int s) {
    const auto filter = std::make_shared<VibranceFilter>();

    filter->SetVibrance(v);
    filter->SetSaturation(s);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}


bool gray_filter_gpu(void *in, void *out, const float r, const float g, const float b) {
    const auto filter = std::make_shared<GrayFilter>();
    filter->SetRedFactor(r);
    filter->SetGreenFactor(g);
    filter->SetBlueFactor(b);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}


bool color_separation_filter_gpu(void *in, void *out, const int roff, const int goff, const int boff) {
    const auto filter = std::make_shared<ColorSeparationFilter>();

    filter->SetRedOffsetX(roff);
    filter->SetGreenOffsetX(goff);
    filter->SetBlueOffsetX(boff);
    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}


bool init_gpu_engine() {
    return g_effect_engine.Init();
}

bool destroy_gpu_engine() {
    return true;
}

bool set_debug_cb(void *dbg) {
    return true;
}
