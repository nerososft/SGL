//
// Created by neo on 25-7-23.
//

#ifndef SGL_IMAGE_H
#define SGL_IMAGE_H
#include "sgl_buffer.h"
#include "sgl_error.h"
#include "sgl_gpu_ctx.h"

#include <cstdint>

typedef struct sgl_image_gpu_info {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t channels = 0;
  uint32_t bytesPerLine = 0;
  sgl_buffer_t gpuBuf{};
} sgl_image_gpu_info_t;

typedef struct sgl_image_cpu_info {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t bytesPerLine;
  void *data;
} sgl_image_cpu_info_t;

typedef enum sgl_image_type {
  SGL_IMAGE_TYPE_CPU = 0,
  SGL_IMAGE_TYPE_GPU = 1,
  SGL_IMAGE_TYPE_UNKNOWN = 2
} sgl_image_type_t;

typedef struct sgl_image_info {
  sgl_image_type_t type = SGL_IMAGE_TYPE_UNKNOWN;
  union {
    sgl_image_gpu_info_t gpu;
    sgl_image_cpu_info_t cpu;
  } info;
} sgl_image_info_t;

typedef struct sgl_image {
  bool initialized = false;

  sgl_error_t (*gray)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                      float r, float g, float b);
  sgl_error_t (*threshold_split)(const sgl_image_info_t &in,
                                 const sgl_image_info_t &out, int bright);

  sgl_error_t (*gaussian_blur)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int r);

  sgl_error_t (*gaussian_blur_float)(const sgl_image_info_t &in,
                                     const sgl_image_info_t &out, int r);

  sgl_error_t (*surface_blur)(const sgl_image_info_t &in,
                              const sgl_image_info_t &out, int r, int th);

  sgl_error_t (*distort_glass)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, float scale,
                               float intensity, float zoom);

  sgl_error_t (*adjust_saturation)(const sgl_image_info_t &in,
                                   const sgl_image_info_t &out, float v,
                                   float s);

  sgl_error_t (*palette_knife)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int r, int s);

  sgl_error_t (*hue_equal)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out);

  sgl_error_t (*blur_edge)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, int r, int s,
                           int kernel_type);

  sgl_error_t (*custom_kernel)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int *k, int radius,
                               int offset, int scale);

  sgl_error_t (*color_balance)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, float *adjustP,
                               int *p, int l);

  sgl_error_t (*black_white)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, float *weight,
                             int wSize);

  sgl_error_t (*scale)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       int weight, int height, int type);

  sgl_error_t (*color_separation)(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out, float roff,
                                  float goff, float boff);

  sgl_error_t (*mid_value)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, float radius,
                           float threshold);

  sgl_error_t (*path_blur)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, float *vec, int amount,
                           int width, int height, float *startpos,
                           float *endpos, float *startvec, float *endvec,
                           int num);

  sgl_error_t (*crystallize)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, float *posx,
                             float *posy, int n);

  sgl_error_t (*rotation_blur)(const sgl_image_info_t &in,
                               const sgl_image_info_t &in2,
                               const sgl_image_info_t &out, float x, float y,
                               float a, float b, float ina, float inb,
                               int strength, float angle);

  sgl_error_t (*facet)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       int radius, int intensitylevel);

  sgl_error_t (*accented_edge)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int *sobelx,
                               int *sobely, int size, int type);
  sgl_error_t (*iris_blur)(const sgl_image_info_t &in,
                           const sgl_image_info_t &in2,
                           const sgl_image_info_t &out, float x, float y,
                           float a, float b, float ina, float inb, float angle);
  sgl_error_t (*tilt_shift_blur)(const sgl_image_info_t &in,
                                 const sgl_image_info_t &in2,
                                 const sgl_image_info_t &out, float *A,
                                 float *B, float *C, float xoffset,
                                 float yoffset, int size);

  sgl_error_t (*radial_blur)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, float sharpness,
                             int strength, float xCenter, float yCenter);

  sgl_error_t (*rotational_blur)(const sgl_image_info_t &in,
                                 const sgl_image_info_t &out, float angle,
                                 int strength, float x, float y);

  sgl_error_t (*minmax)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        int radius, int type);

  sgl_error_t (*color_halftone)(const sgl_image_info_t &in,
                                const sgl_image_info_t &out, float cyanAngle,
                                float yellowAngle, float magentaAngle,
                                float radius, float *lookup);

  sgl_error_t (*sharpen)(const sgl_image_info_t &in,
                         const sgl_image_info_t &out, int *kernel, int size);

  sgl_error_t (*polar_coordinates)(const sgl_image_info_t &in,
                                   const sgl_image_info_t &out, int type);

  sgl_error_t (*clouds)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        int *permuteLookup, int size, int type);

  sgl_error_t (*motion_blur)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, int distance,
                             int angle, float proportion);

  sgl_error_t (*twirl_warp)(const sgl_image_info_t &in,
                            const sgl_image_info_t &out, int angle);

  sgl_error_t (*zigzag)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        int wavelength, int amplitude, int type_wave,
                        float proportion);

  sgl_error_t (*spherize)(const sgl_image_info_t &in,
                          const sgl_image_info_t &out, int alpha, int type);

  sgl_error_t (*pinch)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       int amount);

  sgl_error_t (*wave)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                      int wavelength, int amplitude, int x_pro, int y_pro,
                      int type, int method);
} sgl_image_t;

sgl_error_t sgl_image_check_args(const sgl_image_info_t &info);

sgl_image_t *sgl_image_create(const sgl_gpu_ctx_t *gpu_ctx);

void sgl_image_destroy(sgl_image_t *image);

#endif // SGL_IMAGE_H