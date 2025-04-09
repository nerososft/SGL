#include "c_api.h"
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"

#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/OldGaussianBlurFilter.h"
#include "effect_engine/filters/impl/SurfaceBlurFilter.h"
#include "effect_engine/filters/impl/VibranceFilter.h"
#include "effect_engine/filters/impl/ThresholdSplitFilter.h"
#include "effect_engine/filters/impl/PaletteKnifeFilter.h"
#include "effect_engine/filters/impl/CustomKernelFilter.h"
#include "effect_engine/filters/impl/ColorBalanceFilter.h"
#include "effect_engine/filters/impl/BlackWhiteFilter.h"
#include "effect_engine/filters/impl/ScaleFilter.h"
#include "effect_engine/filters/impl/FastGaussianBlurFilter.h"
#include "effect_engine/filters/impl/BlurEdgeFilter.h"
#include "effect_engine/filters/impl/DistortGlassFilter.h"
#include "effect_engine/filters/impl/MedianFilter.h"

#include "effect_engine/filters/impl/MidValueFilter.h"
#include"effect_engine/filters/impl/PathBlurFilter.h"
#include"effect_engine/filters/impl/CrystallizeFilter.h"
#include"effect_engine/filters/impl/RotationBlurFilter.h"
#include "effect_engine/filters/impl/OldGaussianBlurFloatFilter.h"

#include "log/Log.h"

FunctionPtr g_dbg = nullptr;
EffectEngine g_effect_engine;
bool g_effect_engine_inited = false;

bool threshold_split_filter_gpu(void *in, void *out, const int bright) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<ThresholdSplitFilter>();
    filter->SetBright(bright);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool gaussian_blur_filter_gpu(void *in, void *out, const int r) {
    if (in == nullptr || out == nullptr) return false;
    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    if (r >= 3) {
        const auto filter = std::make_shared<FastGaussianBlurFilter>();
        filter->SetRadius(r);
        g_effect_engine.Process(*input, *output, filter);
    } else {
        const auto filter = std::make_shared<OldGaussianBlurFilter>();
        filter->SetRadius(r);
        g_effect_engine.Process(*input, *output, filter);
    }
    return true;
}


bool gaussian_blur_filter_float_gpu(void *in, void *out, const int r) {
    if (in == nullptr || out == nullptr) return false;
    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    const auto filter = std::make_shared<OldGaussianBlurFloatFilter>();
    filter->SetRadius(r);
    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool surface_blur_filter_gpu(void *in, void *out, const int r, const int th) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<SurfaceBlurFilter>();
    filter->SetBlurRadius(r);
    filter->SetThreshold(th);

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool distort_glass_filter_gpu(void *in, void *out, float scale, float intensity, float zoom) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<DistortGlassFilter>();

    scale = scale / 50;
    intensity = 45 - intensity * 3;

    filter->SetScale(scale);
    filter->SetIntensity(intensity);
    filter->SetZoom(1);


    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool adjust_saturation_gpu(void *in, void *out, const int v, const int s) {
    if (in == nullptr || out == nullptr) return false;
    if (1) {
        const auto filter = std::make_shared<VibranceFilter>();

        filter->SetVibrance(v);
        filter->SetSaturation(s);

        const auto *input = static_cast<ImageInfo *>(in);
        const auto *output = static_cast<ImageInfo *>(out);
        Logger() << "c_api adjust_saturation_gpu begin " << std::endl;
        g_effect_engine.Process(*input, *output, filter);
        Logger() << "c_api adjust_saturation_gpu end  " << std::endl;


        Logger() << "c_api filter destory begin  " << std::endl;

        filter->Destroy();
        Logger() << "c_api filter destory end  " << std::endl;
    }

    if (0) {
        const auto filter = std::make_shared<DistortGlassFilter>();


        float scale = v + 2;
        float intensity = s + 2;

        scale = scale / 50;
        intensity = 45 - intensity * 3;

        filter->SetScale(scale);
        filter->SetIntensity(intensity);
        filter->SetZoom(1);


        const ImageInfo *input = static_cast<ImageInfo *>(in);
        const ImageInfo *output = static_cast<ImageInfo *>(out);

        g_effect_engine.Process(*input, *output, filter);
    }

    if (0) {
        const auto filter = std::make_shared<MedianFilter>();

        const ImageInfo *input = static_cast<ImageInfo *>(in);
        const ImageInfo *output = static_cast<ImageInfo *>(out);
        filter->SetRadius(v);
        g_effect_engine.Process(*input, *output, filter);
    }


    return true;
}

bool palette_knife_gpu(void *in, void *out, const int r, const int s) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<PaletteKnifeFilter>();
    filter->SetRadius(r);
    filter->SetQuantScale(s);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool hue_equal_filter_gpu(void *in, void *out) {
    if (in == nullptr || out == nullptr) return false;
    //const auto filter = std::make_shared<HueEqualFilter>();
    //const auto* input = static_cast<ImageInfo*>(in);
    //const auto* output = static_cast<ImageInfo*>(out);

    //g_effect_engine.Process(*input, *output, filter);


    //const auto filter = std::make_shared<customKernelFilter>();
    //const auto* input = static_cast<ImageInfo*>(in);
    //const auto* output = static_cast<ImageInfo*>(out);


    //int k[25] = {0 , 0 ,0 ,0 ,0,
    //            0 , 0 ,-1 ,0 ,0,
    //            0 , -1 ,5 ,-1 ,0,
    //            0 , 0 ,-1 ,0 ,0,
    //           0 , 0 ,0 ,0 ,0 };


    //filter->SetK(k, 25);
    //filter->SetOffset(0);
    //filter->SetScale(1);
    //filter->SetRadius(2);

    //g_effect_engine.Process(*input, *output, filter);

    const auto filter = std::make_shared<ColorBalanceFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    int p[9] = {
        0, 100, 0,
        0, 100, 0,
        0, 100, 0
    };

    int adjustP[9] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };


    filter->SetP(p, 9);
    filter->SetP(adjustP, 9);
    filter->SetL(0);

    g_effect_engine.Process(*input, *output, filter);


    return true;
}

bool blur_edge_filter_gpu(void *in, void *out, const int r, const int s, const int kernel_type) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<BlurEdgeFilter>();

    filter->SetRadius(r);
    filter->SetSigma(s);
    filter->SetKernel(kernel_type);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);
    g_effect_engine.Process(*input, *output, filter);


    filter->Destroy();

    return true;
}

bool custom_kernel_filter_gpu(void *in, void *out, int *k, const int radius, const int offset, const int scale) {
    if (in == nullptr || out == nullptr | k == nullptr) return false;
    const auto filter = std::make_shared<CustomKernelFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);


    //int k[25] = { 0 , 0 ,0 ,0 ,0,
    //            0 , 0 ,-1 ,0 ,0,
    //            0 , -1 ,5 ,-1 ,0,
    //            0 , 0 ,-1 ,0 ,0,
    //           0 , 0 ,0 ,0 ,0 };


    int k_size = 2 * radius + 1;
    filter->SetK(k, k_size * k_size);
    filter->SetOffset(offset);
    filter->SetScale(scale);
    filter->SetRadius(radius);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool color_balance_filter_gpu(void *in, void *out, float *adjustP, int *p, int l) {
    if (in == nullptr || out == nullptr || adjustP == nullptr || p == nullptr) return false;
    const auto filter = std::make_shared<ColorBalanceFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);


    //int p[9] = { 0 , 50 ,0 ,
    //            0 , 50 ,0 ,
    //            0 , 50 ,0 };

    //int adjustP[9] = { 0 , 0 ,0 ,
    //                   0 , 0 ,0 ,
    //                   0 , 0 ,0 };


    filter->SetP(p, 9);
    filter->SetAdjustP(adjustP, 9);
    filter->SetL(l);

    g_effect_engine.Process(*input, *output, filter);


    return true;
}

bool black_white_filter_gpu(void *in, void *out, float *weight, const int wSize) {
    if (in == nullptr || out == nullptr || weight == nullptr) return false;
    const auto filter = std::make_shared<BlackWhiteFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    filter->SetWeight(weight, wSize);
    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool scale_filter_gpu(void *in, void *out, const int weight, const int height) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<ScaleFilter>();
    filter->SetTargetWidth(weight);
    filter->SetTargetHeight(height);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool gray_filter_gpu(void *in, void *out, const float r, const float g, const float b) {
    if (in == nullptr || out == nullptr) return false;
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
    if (in == nullptr || out == nullptr) return false;
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
    if (!g_effect_engine_inited) {
        g_effect_engine_inited = g_effect_engine.Init();
    }
    return g_effect_engine_inited;
}

bool destroy_gpu_engine() {
    return true;
}

bool set_debug_cb(void *dbg) {
    return true;
}

bool midvalue_filter_gpu(void *in, void *out, const float radius, const float threshold) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<MidValueFilter>();
    filter->SetRadius(radius);
    filter->SetThreshold(threshold);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool pathblur_filter_gpu(void *in, void *out, float *vec, const int amount, const int width, const int height) {
    if (in == nullptr || out == nullptr || vec == nullptr) return false;
    const auto filter = std::make_shared<pathBlurFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    int k_size = width * height * 2;
    filter->SetK(vec, k_size);
    filter->SetAmount(amount);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool crystallize_filter_gpu(void *in, void *out, float *posx, float *posy, const int n) {
    if (in == nullptr || out == nullptr || posx == nullptr) return false;
    const auto filter = std::make_shared<CrystallizeFilter>();
    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    int k_size = n;
    filter->SetPos(posx, posy, k_size);
    filter->SetN(n);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}

bool rotationblur_filter_gpu(void *in, void *out, const float x, const float y, const float a, const float b,
                             const float ina, const float inb, const int strength, const float angle) {
    if (in == nullptr || out == nullptr) return false;
    const auto filter = std::make_shared<RotationBlurFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);
    const auto* input2 = static_cast<ImageInfo*>(in2);
    
    std::vector<ImageInfo> inputs;
    inputs.push_back(*input);
    inputs.push_back(*input2);
    std::vector<ImageInfo> outputs;
    outputs.push_back(*output);

    filter->SetCenterX(x);
    filter->SetCenterY(y);
    filter->SetA(a);
    filter->SetB(b);
    filter->SetinA(ina);
    filter->SetinB(inb);
    filter->SetStrength(strength);
    filter->SetAngle(angle);

    g_effect_engine.Process(inputs, outputs, filter);

    return true;
}

