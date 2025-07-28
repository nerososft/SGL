#include "include/sgl_image.h"

#if defined(OS_OPEN_HARMONY) || defined(ENABLE_WIN64)
#include "psutil/imageprocess/GpuFilterCAPI.h"
#endif

#include "core/image/ImageEngine.h"
#include "core/image/filters/impl/AccentedEdgeFilter.h"
#include "core/image/filters/impl/BlackWhiteFilter.h"
#include "core/image/filters/impl/BlurEdgeFilter.h"
#include "core/image/filters/impl/CloudsFilter.h"
#include "core/image/filters/impl/ColorBalanceFilter.h"
#include "core/image/filters/impl/ColorSeparationFilter.h"
#include "core/image/filters/impl/ColorhalftoneFilter.h"
#include "core/image/filters/impl/CrystallizeFilter.h"
#include "core/image/filters/impl/CustomKernelFilter.h"
#include "core/image/filters/impl/DistortGlassFilter.h"
#include "core/image/filters/impl/FacetFilter.h"
#include "core/image/filters/impl/FastGaussianBlurFilter.h"
#include "core/image/filters/impl/GrayFilter.h"
#include "core/image/filters/impl/IrisBlurFilter.h"
#include "core/image/filters/impl/MidValueFilter.h"
#include "core/image/filters/impl/MinMaxFilter.h"
#include "core/image/filters/impl/MotionblurFilter.h"
#include "core/image/filters/impl/OldGaussianBlurFilter.h"
#include "core/image/filters/impl/OldGaussianBlurFloatFilter.h"
#include "core/image/filters/impl/PaletteKnifeFilter.h"
#include "core/image/filters/impl/PathBlurFilter.h"
#include "core/image/filters/impl/PinchFilter.h"
#include "core/image/filters/impl/PolarCoordinatesFilter.h"
#include "core/image/filters/impl/RadialBlurNewFilter.h"
#include "core/image/filters/impl/RotationBlurFilter.h"
#include "core/image/filters/impl/RotationalBlurFilter.h"
#include "core/image/filters/impl/ScaleFilter.h"
#include "core/image/filters/impl/SharpenFilter.h"
#include "core/image/filters/impl/SpherizeFilter.h"
#include "core/image/filters/impl/SurfaceBlurFilter.h"
#include "core/image/filters/impl/ThresholdSplitFilter.h"
#include "core/image/filters/impl/TiltshiftBlurFilter.h"
#include "core/image/filters/impl/TwirlWarpFilter.h"
#include "core/image/filters/impl/VibranceFilter.h"
#include "core/image/filters/impl/WaveFilter.h"
#include "core/image/filters/impl/ZigzagFilter.h"
#include "runtime/log/Log.h"

sgl::image::ImageEngine gImageEngine;

bool threshold_split_filter_gpu(void *in, void *out, const int bright) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<ThresholdSplitFilter>();
  filter->SetBright(bright);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool gaussian_blur_filter_gpu(void *in, void *out, const int r) {
  if (in == nullptr || out == nullptr)
    return false;
  const sgl::image::ImageInfo *input = static_cast<sgl::image::ImageInfo *>(in);
  const sgl::image::ImageInfo *output =
      static_cast<sgl::image::ImageInfo *>(out);

  if (r >= 3) {
    const auto filter = std::make_shared<FastGaussianBlurFilter>();
    filter->SetRadius(r);
    gImageEngine.Process(*input, *output, filter);
  } else {
    const auto filter = std::make_shared<OldGaussianBlurFilter>();
    filter->SetRadius(r);
    gImageEngine.Process(*input, *output, filter);
  }
  return true;
}

bool gaussian_blur_filter_float_gpu(void *in, void *out, const int r) {
  if (in == nullptr || out == nullptr)
    return false;
  const sgl::image::ImageInfo *input = static_cast<sgl::image::ImageInfo *>(in);
  const sgl::image::ImageInfo *output =
      static_cast<sgl::image::ImageInfo *>(out);

  const auto filter = std::make_shared<OldGaussianBlurFloatFilter>();
  filter->SetRadius(r);
  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool surface_blur_filter_gpu(void *in, void *out, const int r, const int th) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<SurfaceBlurFilter>();
  filter->SetBlurRadius(r);
  filter->SetThreshold(th);

  const sgl::image::ImageInfo *input = static_cast<sgl::image::ImageInfo *>(in);
  const sgl::image::ImageInfo *output =
      static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool distort_glass_filter_gpu(void *in, void *out, const float scale,
                              const float intensity, const float zoom) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<DistortGlassFilter>();

  //   scale = scale / 50;
  //    intensity = 45 - intensity * 3;

  filter->SetScale(scale);
  filter->SetIntensity(intensity);
  filter->SetZoom(zoom);

  const sgl::image::ImageInfo *input = static_cast<sgl::image::ImageInfo *>(in);
  const sgl::image::ImageInfo *output =
      static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool adjust_saturation_gpu(void *in, void *out, const int v, const int s) {
  if (in == nullptr || out == nullptr)
    return false;

  const auto filter = std::make_shared<VibranceFilter>();

  filter->SetVibrance(v);
  filter->SetSaturation(s);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);
  Logger() << "c_api adjust_saturation_gpu begin " << std::endl;
  gImageEngine.Process(*input, *output, filter);
  Logger() << "c_api adjust_saturation_gpu end  " << std::endl;

  Logger() << "c_api filter destory begin  " << std::endl;
  filter->Destroy();
  Logger() << "c_api filter destory end  " << std::endl;

  return true;
}

bool palette_knife_gpu(void *in, void *out, const int r, const int s) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<PaletteKnifeFilter>();
  filter->SetRadius(r);
  filter->SetQuantScale(s);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool hue_equal_filter_gpu(void *in, void *out) {
  if (in == nullptr || out == nullptr)
    return false;

  const auto filter = std::make_shared<ColorBalanceFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  int p[9] = {0, 100, 0, 0, 100, 0, 0, 100, 0};

  int adjustP[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  filter->SetP(p, 9);
  filter->SetP(adjustP, 9);
  filter->SetL(0);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool blur_edge_filter_gpu(void *in, void *out, const int r, const int s,
                          const int kernel_type) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<BlurEdgeFilter>();

  filter->SetRadius(r);
  filter->SetSigma(s);
  filter->SetKernel(kernel_type);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);
  gImageEngine.Process(*input, *output, filter);

  filter->Destroy();

  return true;
}

bool custom_kernel_filter_gpu(void *in, void *out, int *k, const int radius,
                              const int offset, const int scale) {
  if (in == nullptr || out == nullptr | k == nullptr)
    return false;
  const auto filter = std::make_shared<CustomKernelFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  // int k[25] = { 0 , 0 ,0 ,0 ,0,
  //             0 , 0 ,-1 ,0 ,0,
  //             0 , -1 ,5 ,-1 ,0,
  //             0 , 0 ,-1 ,0 ,0,
  //            0 , 0 ,0 ,0 ,0 };

  int k_size = 2 * radius + 1;
  filter->SetK(k, k_size * k_size);
  filter->SetOffset(offset);
  filter->SetScale(scale);
  filter->SetRadius(radius);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool color_balance_filter_gpu(void *in, void *out, float *adjustP, int *p,
                              const int l) {
  if (in == nullptr || out == nullptr || adjustP == nullptr || p == nullptr)
    return false;
  const auto filter = std::make_shared<ColorBalanceFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  // int p[9] = { 0 , 50 ,0 ,
  //             0 , 50 ,0 ,
  //             0 , 50 ,0 };

  // int adjustP[9] = { 0 , 0 ,0 ,
  //                    0 , 0 ,0 ,
  //                    0 , 0 ,0 };

  filter->SetP(p, 9);
  filter->SetAdjustP(adjustP, 9);
  filter->SetL(l);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool black_white_filter_gpu(void *in, void *out, float *weight,
                            const int wSize) {
  if (in == nullptr || out == nullptr || weight == nullptr)
    return false;
  const auto filter = std::make_shared<BlackWhiteFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetWeight(weight, wSize);
  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool scale_filter_gpu(void *in, void *out, const int weight, const int height,
                      const int type) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<ScaleFilter>();
  filter->SetTargetWidth(weight);
  filter->SetTargetHeight(height);
  filter->SetInterpType(type);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);

  unsigned char *data0 = (unsigned char *)output->info.cpu.data;

  int p0 = data0[0];
  int p1 = data0[1];
  int p2 = data0[2];
  int p3 = data0[3];

  unsigned char *data1 =
      (unsigned char *)output->info.cpu.data + weight * height / 2 * 4;

  int p4 = data1[0];
  int p5 = data1[1];
  int p6 = data1[2];
  int p7 = data1[3];

  return true;
}

bool gray_filter_gpu(void *in, void *out, const float r, const float g,
                     const float b) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<GrayFilter>();
  filter->SetRedFactor(r);
  filter->SetGreenFactor(g);
  filter->SetBlueFactor(b);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool color_separation_filter_gpu(void *in, void *out, const int roff,
                                 const int goff, const int boff) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<ColorSeparationFilter>();

  filter->SetRedOffsetX(roff);
  filter->SetGreenOffsetX(goff);
  filter->SetBlueOffsetX(boff);
  const sgl::image::ImageInfo *input = static_cast<sgl::image::ImageInfo *>(in);
  const sgl::image::ImageInfo *output =
      static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool midvalue_filter_gpu(void *in, void *out, const float radius,
                         const float threshold) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<MidValueFilter>();
  filter->SetRadius(radius);
  filter->SetThreshold(threshold);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);
  return true;
}

bool pathblur_filter_gpu(void *in, void *out, float *vec, const int amount,
                         const int width, const int height, float *startpos,
                         float *endpos, float *startvec, float *endvec,
                         const int num) {
  if (in == nullptr || out == nullptr || vec == nullptr)
    return false;
  const auto filter = std::make_shared<pathBlurFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  int k_size = width * height * 2;
  filter->SetK1(vec, k_size);
  filter->SetK(k_size);
  filter->SetAmount(amount);
  filter->SetStartPos(startpos, num);
  filter->SetEndPos(endpos, num);
  filter->SetStartVec(startvec, num);
  filter->SetEndVec(endvec, num);
  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool crystallize_filter_gpu(void *in, void *out, float *posx, float *posy,
                            const int n) {
  if (in == nullptr || out == nullptr || posx == nullptr)
    return false;
  const auto filter = std::make_shared<CrystallizeFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  int k_size = n;
  filter->SetPos(posx, posy, k_size);
  filter->SetN(n);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool rotationblur_filter_gpu(void *in, void *in2, void *out, const float x,
                             const float y, const float a, const float b,
                             const float ina, const float inb,
                             const int strength, const float angle) {
  // return true;
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<RotationBlurFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);
  const auto *input2 = static_cast<sgl::image::ImageInfo *>(in2);

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetStrength(strength);
  filter->SetAngle(angle);

  gImageEngine.Process(inputs, outputs, filter);

  return true;
}

bool facet_filter_gpu(void *in, void *out, const int radius,
                      const int intensitylevel) {
  const auto filter = std::make_shared<FacetFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetRadius(radius);
  filter->SetLevel(intensitylevel);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool accented_edge_filter_gpu(void *in, void *out, int *sobelx, int *sobely,
                              const int size, const int type) {
  const auto filter = std::make_shared<AccentedEdgeFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetSobelx(sobelx, size);
  filter->SetSobely(sobely, size);
  filter->SetType(type);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool irisblur_filter_gpu(void *in, void *in2, void *out, const float x,
                         const float y, const float a, const float b,
                         const float ina, const float inb, const float angle) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<IrisBlurFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);
  const auto *input2 = static_cast<sgl::image::ImageInfo *>(in2);

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetAngle(angle);

  gImageEngine.Process(inputs, outputs, filter);

  return true;
}

bool tiltshiftblur_filter_gpu(void *in, void *in2, void *out, float *A,
                              float *B, float *C, const float xoffset,
                              const float yoffset, const int size) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<TiltshiftBlurFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);
  const auto *input2 = static_cast<sgl::image::ImageInfo *>(in2);

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetA(A, size);
  filter->SetB(B, size);
  filter->SetC(C, size);
  filter->SetOffset(xoffset, yoffset);

  gImageEngine.Process(inputs, outputs, filter);

  return true;
}

bool radial_blur_filter_gpu(void *in, void *out, const int sharpness,
                            const int strength, const float xCenter,
                            const float yCenter) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<RadialBlurNewFilter>();

  filter->SetSharpness(sharpness);
  filter->SetStrength(strength);
  filter->SetCenter(xCenter, yCenter);

  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool rotational_blur_filter_gpu(void *in, void *out, const float angle,
                                const int strength, const float x,
                                const float y) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<RotationalBlurFilter>();

  filter->SetAngle(angle);
  filter->SetStrength(strength);
  filter->SetCenter(x, y);
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool minmax_filter_gpu(void *in, void *out, const int radius, const int type) {
  const auto filter = std::make_shared<MinMaxFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetRadius(radius);
  filter->SetType(type);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool colorhalftone_filter_gpu(void *in, void *out, const float cyanAngle,
                              const float yellowAngle, const float magentaAngle,
                              const float radius, float *lookup) {
  const auto filter = std::make_shared<ColorhalftoneFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  const int size = 256;
  filter->SetColor(cyanAngle, yellowAngle, magentaAngle, radius);
  filter->SetLookup(lookup, size);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool sharpen_filter_gpu(void *in, void *out, int *kernel, const int size) {
  const auto filter = std::make_shared<SharpenFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetKernel(kernel, size);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool polarcoordinates_filter_gpu(void *in, void *out, const int type) {
  const auto filter = std::make_shared<PolarCoordinatesFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetType(type);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool clouds_filter_gpu(void *in, void *out, int *permuteLookup, const int size,
                       const int type) {
  const auto filter = std::make_shared<CloudsFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetLookup(permuteLookup, size);
  filter->SetType(type);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool motionblur_filter_gpu(void *in, void *out, const int distance,
                           const int angle, const float proportion) {
  const auto filter = std::make_shared<MotionBlurFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetAngle(angle);
  filter->SetDistance(distance);
  filter->SetPro(proportion);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool twirlwarp_filter_gpu(void *in, void *out, int angle) {
  const auto filter = std::make_shared<TwirlWarpFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetAngle(angle);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool zigzag_filter_gpu(void *in, void *out, const int wavelength,
                       const int amplitude, const int type_wave,
                       const float proportion) {
  const auto filter = std::make_shared<ZigzagFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetTypeWave(type_wave);
  filter->SetPro(proportion);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool spherize_filter_gpu(void *in, void *out, const int alpha, const int type) {
  const auto filter = std::make_shared<SpherizeFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetAlpha(alpha);
  filter->SetType(type);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool pinch_filter_gpu(void *in, void *out, int amount) {
  const auto filter = std::make_shared<PinchFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetAmount(amount);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

bool wave_filter_gpu(void *in, void *out, const int wavelength,
                     const int amplitude, const int x_pro, const int y_pro,
                     const int type, const int method) {
  const auto filter = std::make_shared<WaveFilter>();
  const auto *input = static_cast<sgl::image::ImageInfo *>(in);
  const auto *output = static_cast<sgl::image::ImageInfo *>(out);

  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetPro(x_pro, y_pro);
  filter->SetType(type);
  filter->SetMethod(method);

  gImageEngine.Process(*input, *output, filter);

  return true;
}

sgl_error_t sgl_image_gray(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, const float r,
                           const float g, const float b) {
  const auto filter = std::make_shared<GrayFilter>();
  filter->SetRedFactor(r);
  filter->SetGreenFactor(g);
  filter->SetBlueFactor(b);
  gImageEngine.Process(in, out, filter);
}

sgl_image_t *sgl_image_create(const sgl_gpu_ctx_t *gpu_ctx) {
  if (gpu_ctx == nullptr) {
    Logger() << "gpu_ctx is null." << std::endl;
    return nullptr;
  }
  const auto image = static_cast<sgl_image_t *>(malloc(sizeof(sgl_image_t)));
  if (image == nullptr) {
    return nullptr;
  }

  if (!image->initialized) {
    image->initialized = gImageEngine.Init();
  }
  if (!image->initialized) {
    Logger() << "Failed to initialize image engine." << std::endl;
    free(image);
    return nullptr;
  }

  image->gray = sgl_image_gray;

  // TODO:
  return nullptr;
}
void sgl_image_destroy(sgl_image_t *image) {
  if (image == nullptr) {
    return;
  }
  free(image);
}