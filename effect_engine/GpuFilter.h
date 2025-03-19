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
extern "C"  GPU_API  bool gray_filter_gpu(void* in, void* out, float r ,float g, float b);
//��ɫ���� ��Ϊ�������
extern "C"  GPU_API  bool colorSeparation_filter_gpu(void* in, void* out, int roff, int goff, int boff);
//����ģ��
extern "C"  GPU_API	 bool surface_blur_filter_gpu(void* in, void* out, int r, int th);
//ƽ��ģ��
extern "C"  GPU_API	 bool average_blur_filter_gpu(void* in, void* out);
//���Ͷ�
extern "C"  GPU_API	 bool adjust_saturation_gpu(void* in, void* out, int v , int s);