#include "c_api.h"
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"

#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/OldGaussianBlurFilter.h"
#include "effect_engine/filters/impl/SurfaceBlurFilter.h"
#include "effect_engine/filters/impl/VibranceFilter.h"
#include "effect_engine/filters/impl/ThresholdSplitFilter.h"
#include "effect_engine/filters/impl/PaletteKnifeFilter.h"
#include "filters/impl/FastGaussianBlurFilter.h"
#include "effect_engine/filters/impl/HueEqualFilter.h"
#include "effect_engine/filters/impl/customKernelFilter.h"
#include "effect_engine/filters/impl/colorBalanceFilter.h"
#include "effect_engine/filters/impl/BlackWhiteFilter.h" 
#include "effect_engine/filters/impl/ScaleFilter.h"
#include "effect_engine/filters/impl/FastGaussianBlurFilter.h"

#include "effect_engine/filters/impl/MidValueFilter.h" 
#include"effect_engine/filters/impl/PathBlurFilter.h" 

#include "log/Log.h"

FunctionPtr g_dbg = nullptr;
EffectEngine g_effect_engine;
bool g_effect_engine_inited = false;

bool threshold_split_filter_gpu(void *in, void *out, const int bright) {
    const auto filter = std::make_shared<ThresholdSplitFilter>();
    filter->SetBright(bright);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}

bool gaussian_blur_filter_gpu(void *in, void *out, const int r) {
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
    Logger() << "c_api adjust_saturation_gpu begin " << std::endl;
    g_effect_engine.Process(*input, *output, filter);
    Logger() << "c_api adjust_saturation_gpu end  " << std::endl;


    Logger() << "c_api filter destory begin  " << std::endl;

    filter->Destroy();
    Logger() << "c_api filter destory end  " << std::endl;

    return true;
}

bool palette_knife_gpu(void *in, void *out, const int r, const int s) {
    const auto filter = std::make_shared<PaletteKnifeFilter>();
    filter->SetRadius(r);
    filter->SetQuantScale(s);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}



bool hue_equal_filter_gpu(void* in, void* out) {

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


    const auto filter = std::make_shared<colorBalanceFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);





    int p[9] = {0 , 100 ,0 ,
                0 , 100 ,0 ,
                0 , 100 ,0  };

    int adjustP[9] = { 0 , 0 ,0 ,
            0 , 0 ,0 ,
            0 , 0 ,0 };



    filter->SetP(p, 9);
    filter->SetP(adjustP, 9);
    filter->SetL(0);

    g_effect_engine.Process(*input, *output, filter);


    return true;
}

bool custom_kernel_filter_gpu(void* in, void* out, int * k , int radius, int offset, int scale)
{


    const auto filter = std::make_shared<customKernelFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);


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

bool color_balance_filter_gpu(void* in, void* out,  float* adjustP, int *p, int l)
{

    const auto filter = std::make_shared<colorBalanceFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);





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




bool black_white_filter_gpu(void* in, void* out, float* weight, int wSize) {

    const auto filter = std::make_shared<BlackWhiteFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);

    filter->SetWeight(weight , wSize);
    g_effect_engine.Process(*input, *output, filter);
    return true;
}
bool scale_filter_gpu(void* in, void* out, int  weight, int height)
{
    const auto filter = std::make_shared<ScaleFilter>();
    filter->SetTargetWidth(weight);
    filter->SetTargetHeight(height);

    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);

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


/*bool midvalue_filter_gpu(void* in, void* out, const float radius, const float threshold) {
    const auto filter = std::make_shared<MidValueFilter>();
    filter->SetRadius(radius);
    filter->SetThreshold(threshold);

    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);

    g_effect_engine.Process(*input, *output, filter);
    return true;
}


bool pathblur_filter_gpu(void* in, void* out, float* vec, int amount,int width,int height)
{


    const auto filter = std::make_shared<pathBlurFilter>();
    const auto* input = static_cast<ImageInfo*>(in);
    const auto* output = static_cast<ImageInfo*>(out);

    int k_size = width*height*2;
    filter->SetK(vec, k_size);
    filter->SetAmount(amount);

    g_effect_engine.Process(*input, *output, filter);

    return true;
}*/