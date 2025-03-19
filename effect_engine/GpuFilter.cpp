#include <stdint.h>


#include <iostream>
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ChinesePaintFilter.h"
#include "effect_engine/filters/impl/ColorPencilSketchFilter.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"
#include "effect_engine/filters/impl/GaussianBlurFilter.h"
//#include "effect_engine/filters/impl/thresholdSplit.h"
#include "GpuFilter.h"
#include "effect_engine/filters/impl/GrayFilter.h"
#include "effect_engine/filters/impl/OldGaussianBlurFilter.h"
#include "effect_engine/filters/impl/AverageBlurFilter.h"
#include "effect_engine/filters/impl/SurfaceBlurFilter.h"
#include "effect_engine/filters/impl/adjustSaturation.h"

#include "himi.h"
//FunctionPtr g_dbg = nullptr;
FunctionPtr g_dbg ;
EffectEngine effectEngine;


bool threshold_split_filter_gpu(void* in, void* out, int bright) {

   // const auto filter = std::make_shared<thresholdSplitFilter>();
   // filter->SetBright(bright);

  //  ImageInfo* input = (ImageInfo*)in;
  //  ImageInfo* output = (ImageInfo*)out;

    //effectEngine.Process(*input, *output, filter);
    return true;

}


//高斯模糊
bool gaussian_blur_filter_gpu(void* in, void* out, int r) {
#if ENABLE_HIMI_HILOG
  //  OH_LOG_INFO(LOG_APP, "hwtest call guassian 3");
#endif // ENABLE_HIMI_HILOG


    const auto filter = std::make_shared<OldGaussianBlurFilter>();
    filter->SetRadius(r);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}

//表面模糊
bool surface_blur_filter_gpu(void* in, void* out, int r, int th) {
    const auto filter = std::make_shared<SurfaceBlurFilter>();
    filter->SetBlurRadius(r);
    filter->SetThreshold(th);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}

//平均模糊
bool average_blur_filter_gpu(void* in, void* out) {
    const auto filter = std::make_shared<AverageBlurFilter>();

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}

bool adjust_saturation_gpu(void* in, void* out , int v, int s)
{
    
    const auto filter = std::make_shared<adjustSaturationFilter>();

    filter->SetVibrance(v);
    filter->SetSaturation(s);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
 
    return true;
}


bool gray_filter_gpu(void* in, void* out, float r ,float g, float b) {
#if ENABLE_HIMI_HILOG
  //  OH_LOG_INFO(LOG_APP, "hwtest call gray_filter_gpu");
#endif // ENABLE_HIMI_HILOG


    const auto filter = std::make_shared<GrayFilter>();
    filter->SetRedFactor(r);
    filter->SetGreenFactor(g);
    filter->SetBlueFactor(b);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}


bool colorSeparation_filter_gpu(void* in, void* out, int roff, int goff, int boff) {
#if ENABLE_HIMI_HILOG
   // OH_LOG_INFO(LOG_APP, "hwtest call ColorSeparationFilter");
#endif // ENABLE_HIMI_HILOG

    const auto filter = std::make_shared<ColorSeparationFilter>();
 

     filter->SetRedOffsetX(60.0f);
    filter->SetGreenOffsetX(120.0f);
     filter->SetBlueOffsetX(180.0f);
    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}


bool init_gpu_engine() {

#if ENABLE_HIMI_HILOG
 //   OH_LOG_INFO(LOG_APP, "hwtest lib init");
#endif // ENABLE_HIMI_HILOG
    if(g_dbg){
      //  g_dbg("hwtest g_gbd");

    }
    return effectEngine.Init();

}

bool destory_gpu_engine() {

    return true;

}





bool set_qdebug_cb(void*  dbg){

    //g_dbg = (FunctionPtr)dbg;
    return true;

}