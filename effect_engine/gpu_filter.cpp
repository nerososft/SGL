#include <stdint.h>


#include <iostream>
#include "effect_engine/EffectEngine.h"
#include "effect_engine/filters/impl/ChinesePaintFilter.h"
#include "effect_engine/filters/impl/ColorPencilSketchFilter.h"
#include "effect_engine/filters/impl/ColorSeparationFilter.h"
#include "effect_engine/filters/impl/GaussianBlurFilter.h"
#include "effect_engine/filters/impl/thresholdSplit.h"
#include "gpu_filter.h"



#include "himi.h"
//FunctionPtr g_dbg = nullptr;
FunctionPtr g_dbg ;
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
#if ENABLE_HIMI_HILOG
    OH_LOG_INFO(LOG_APP, "hwtest call guassian");
#endif // ENABLE_HIMI_HILOG


    const auto filter = std::make_shared<GaussianBlurFilter>();
    filter->SetRadius(r);

    ImageInfo* input = (ImageInfo*)in;
    ImageInfo* output = (ImageInfo*)out;

    effectEngine.Process(*input, *output, filter);
    return true;

}


bool init_gpu_engine() {

#if ENABLE_HIMI_HILOG
    OH_LOG_INFO(LOG_APP, "hwtest lib init");
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