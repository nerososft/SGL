#pragma once

#define GPU_API __declspec(dllexport)
typedef void (*FunctionPtr)(char*);
extern "C"  GPU_API  bool threshold_split_filter_gpu(void* in, void* out, int);
extern "C"  GPU_API  bool gaussian_blur_filter_gpu(void* in, void* out, int);
extern "C"  GPU_API  bool init_gpu_engine();
extern "C"  GPU_API  bool destroy_gpu_engine();
extern "C"  GPU_API  bool set_debug_cb(void*  dbg);
extern "C"  GPU_API  bool gray_filter_gpu(void* in, void* out, float r ,float g, float b);
extern "C"  GPU_API  bool color_separation_filter_gpu(void* in, void* out, int roff, int goff, int boff);
extern "C"  GPU_API	 bool surface_blur_filter_gpu(void* in, void* out, int r, int th);
extern "C"  GPU_API	 bool adjust_saturation_gpu(void* in, void* out, int v , int s);