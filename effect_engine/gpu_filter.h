#pragma once


#include <stdint.h>


#include <iostream>


#define GPU_API __declspec(dllexport)


typedef void (*FunctionPtr)(char*);

extern "C"  GPU_API  bool threshold_split_filter_gpu(void* in, void* out, int);
extern "C"  GPU_API  bool gaussian_blur_filter_gpu(void* in, void* out, int);
extern "C"  GPU_API  bool init_gpu_engine();
extern "C"  GPU_API  bool destory_gpu_engine();
extern "C"  GPU_API  bool set_qdebug_cb(void*  dbg);

