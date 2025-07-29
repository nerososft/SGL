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
                                 const sgl_image_info_t &out, const int bright);

  sgl_error_t (*gaussian_blur)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, const int r);

  sgl_error_t (*gaussian_blur_float)(const sgl_image_info_t &in,
                                     const sgl_image_info_t &out, const int r);

  sgl_error_t (*surface_blur)(const sgl_image_info_t &in,
                              const sgl_image_info_t &out, const int r,
                              const int th);

  sgl_error_t (*distort_glass)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, const float scale,
                               const float intensity, const float zoom);

  sgl_error_t (*adjust_saturation)(const sgl_image_info_t &in,
                                   const sgl_image_info_t &out, const int v,
                                   const int s);

  sgl_error_t (*palette_knife)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, const int r,
                               const int s);

  sgl_error_t (*hue_equal)(void *in, void *out);

  sgl_error_t (*blur_edge)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, const int r,
                           const int s, const int kernel_type);

  sgl_error_t (*custom_kernel)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int *k,
                               const int radius, const int offset,
                               const int scale);

  sgl_error_t (*color_balance)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, float *adjustP,
                               int *p, const int l);

  sgl_error_t (*black_white)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, float *weight,
                             const int wSize);

  sgl_error_t (*scale)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       const int weight, const int height, const int type);

  sgl_error_t (*color_separation)(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out, const int roff,
                                  const int goff, const int boff);

  sgl_error_t (*midvalue)(const sgl_image_info_t &in,
                          const sgl_image_info_t &out, const float radius,
                          const float threshold);

  sgl_error_t (*pathblur)(const sgl_image_info_t &in,
                          const sgl_image_info_t &out, float *vec,
                          const int amount, const int width, const int height,
                          float *startpos, float *endpos, float *startvec,
                          float *endvec, const int num);

  sgl_error_t (*crystallize)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, float *posx,
                             float *posy, const int n);

  sgl_error_t (*rotationblur)(void *in, void *in2, void *out, const float x,
                              const float y, const float a, const float b,
                              const float ina, const float inb,
                              const int strength, const float angle);

  sgl_error_t (*facet)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       const int radius, const int intensitylevel);

  sgl_error_t (*accented_edge)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, int *sobelx,
                               int *sobely, const int size, const int type);
  sgl_error_t (*irisblur)(void *in, void *in2, void *out, const float x,
                          const float y, const float a, const float b,
                          const float ina, const float inb, const float angle);
  sgl_error_t (*tiltshiftblur)(void *in, void *in2, void *out, float *A,
                               float *B, float *C, const float xoffset,
                               const float yoffset, const int size);

  sgl_error_t (*radial_blur)(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, const int sharpness,
                             const int strength, const float xCenter,
                             const float yCenter);

  sgl_error_t (*rotational_blur)(const sgl_image_info_t &in,
                                 const sgl_image_info_t &out, const float angle,
                                 const int strength, const float x,
                                 const float y);

  sgl_error_t (*minmax)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        const int radius, const int type);

  sgl_error_t (*colorhalftone)(const sgl_image_info_t &in,
                               const sgl_image_info_t &out,
                               const float cyanAngle, const float yellowAngle,
                               const float magentaAngle, const float radius,
                               float *lookup);

  sgl_error_t (*sharpen)(const sgl_image_info_t &in,
                         const sgl_image_info_t &out, int *kernel,
                         const int size);

  sgl_error_t (*polarcoordinates)(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out, const int type);

  sgl_error_t (*clouds)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        int *permuteLookup, const int size, const int type);

  sgl_error_t (*motionblur)(const sgl_image_info_t &in,
                            const sgl_image_info_t &out, const int distance,
                            const int angle, const float proportion);

  sgl_error_t (*twirlwarp)(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, int angle);

  sgl_error_t (*zigzag)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                        const int wavelength, const int amplitude,
                        const int type_wave, const float proportion);

  sgl_error_t (*spherize)(const sgl_image_info_t &in,
                          const sgl_image_info_t &out, const int alpha,
                          const int type);

  sgl_error_t (*pinch)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                       int amount);

  sgl_error_t (*wave)(const sgl_image_info_t &in, const sgl_image_info_t &out,
                      const int wavelength, const int amplitude,
                      const int x_pro, const int y_pro, const int type,
                      const int method);
} sgl_image_t;

sgl_error_t sgl_image_check_args(const sgl_image_info_t &info);

sgl_image_t *sgl_image_create(const sgl_gpu_ctx_t *gpu_ctx);

void sgl_image_destroy(sgl_image_t *image);

#endif // SGL_IMAGE_H