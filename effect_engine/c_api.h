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
extern "C"  GPU_API	 bool palette_knife_gpu(void* in, void* out, int r, int s);
extern "C"  GPU_API	 bool hue_equal_filter_gpu(void* in, void* out);
extern "C"  GPU_API	 bool custom_kernel_filter_gpu(void* in, void* out, int* k, int radius, int offset, int scale);
extern "C"  GPU_API	 bool color_balance_filter_gpu(void* in, void* out, float* adjustP, int* p, int l);
extern "C"  GPU_API	 bool black_white_filter_gpu(void* in, void* out, float* weight, int wSize);
extern "C"  GPU_API	 bool scale_filter_gpu(void* in, void* out, int weight, int height);
extern "C"  GPU_API	 bool blur_edge_filter_gpu(void* in, void* out, int r, int s,int kernel_type);
extern "C"  GPU_API	 bool distort_glass_filter_gpu(void* in, void* out, float scale, float intensity, float zoom);
extern "C"  GPU_API	 bool scale_filter_gpu(void* in, void* out, int weight, int height);
extern "C"  GPU_API	 bool midvalue_filter_gpu(void* in, void* out, float radius, float threshold);
extern "C"  GPU_API  bool pathblur_filter_gpu(void* in, void* out, float* vec, int amount,int width,int height,float *startpos,float* endpos,float* startvec,float *endvec,int num);
extern "C"  GPU_API  bool crystallize_filter_gpu(void* in, void* out, float* posx, float* posy, int n);
extern "C"  GPU_API  bool rotationblur_filter_gpu(void* in, void* in2, void* out, float x, float y, float a, float b, float ina, float inb, int strength, float angle);
extern "C"  GPU_API	 bool gaussian_blur_filter_float_gpu(void* in, void* out, const int r);
extern "C"  GPU_API  bool facet_filter_gpu(void* in, void* out, int radius, int intensitylevel);
extern "C"  GPU_API  bool accented_edge_filter_gpu(void* in, void* out, int* sobelx, int* sobely, int size,int type);
extern "C"  GPU_API  bool irisblur_filter_gpu(void* in, void* in2, void* out, float x, float y, float a, float b, float ina, float inb, float angle);
extern "C"  GPU_API  bool tiltshiftblur_filter_gpu(void* in, void* in2, void* out, float* A, float* B, float* C, float xoffset, float yoffset,int size);
extern "C" GPU_API const char *get_gpu_engine_name();
