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

sgl_error_t sgl_image_threshold_split(const sgl_image_info_t &in,
                                      const sgl_image_info_t &out,
                                      const int bright) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<ThresholdSplitFilter>();
  filter->SetBright(bright);

  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_gaussian_blur(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out, const int r) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  if (r >= 3) {
    const auto filter = std::make_shared<FastGaussianBlurFilter>();
    filter->SetRadius(r);
    sgl::image::ImageEngine::Process(in, out, filter);
  } else {
    const auto filter = std::make_shared<OldGaussianBlurFilter>();
    filter->SetRadius(r);
    sgl::image::ImageEngine::Process(in, out, filter);
  }
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_gaussian_blur_float(const sgl_image_info_t &in,
                                          const sgl_image_info_t &out,
                                          const int r) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<OldGaussianBlurFloatFilter>();
  filter->SetRadius(r);
  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_surface_blur(const sgl_image_info_t &in,
                                   const sgl_image_info_t &out, const int r,
                                   const int th) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<SurfaceBlurFilter>();
  filter->SetBlurRadius(r);
  filter->SetThreshold(th);

  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_distort_glass(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out,
                                    const float scale, const float intensity,
                                    const float zoom) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<DistortGlassFilter>();

  filter->SetScale(scale);
  filter->SetIntensity(intensity);
  filter->SetZoom(zoom);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_adjust_saturation(const sgl_image_info_t &in,
                                        const sgl_image_info_t &out,
                                        const float v, const float s) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<VibranceFilter>();

  filter->SetVibrance(v);
  filter->SetSaturation(s);

  Logger() << "c_api adjust_saturation_gpu begin " << std::endl;
  sgl::image::ImageEngine::Process(in, out, filter);
  Logger() << "c_api adjust_saturation_gpu end  " << std::endl;

  Logger() << "c_api filter destory begin  " << std::endl;
  filter->Destroy();
  Logger() << "c_api filter destory end  " << std::endl;

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_palette_knife(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out, const int r,
                                    const int s) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<PaletteKnifeFilter>();
  filter->SetRadius(r);
  filter->SetQuantScale(s);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_hue_equal(const sgl_image_info_t &in,
                                const sgl_image_info_t &out) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<ColorBalanceFilter>();

  int p[9] = {0, 100, 0, 0, 100, 0, 0, 100, 0};

  int adjustP[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  filter->SetP(p, 9);
  filter->SetP(adjustP, 9);
  filter->SetL(0);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_blur_edge(const sgl_image_info_t &in,
                                const sgl_image_info_t &out, const int r,
                                const int s, const int kernel_type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<BlurEdgeFilter>();

  filter->SetRadius(r);
  filter->SetSigma(s);
  filter->SetKernel(kernel_type);

  sgl::image::ImageEngine::Process(in, out, filter);

  filter->Destroy();

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_custom_kernel(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out, int *k,
                                    const int radius, const int offset,
                                    const int scale) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  if (k == nullptr) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<CustomKernelFilter>();

  const int k_size = 2 * radius + 1;
  filter->SetK(k, k_size * k_size);
  filter->SetOffset(offset);
  filter->SetScale(scale);
  filter->SetRadius(radius);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_color_balance(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out, float *adjustP,
                                    int *p, const int l) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  if (adjustP == nullptr || p == nullptr) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<ColorBalanceFilter>();

  filter->SetP(p, 9);
  filter->SetAdjustP(adjustP, 9);
  filter->SetL(l);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_black_white(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out, float *weight,
                                  const int wSize) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  if (weight == nullptr) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<BlackWhiteFilter>();

  filter->SetWeight(weight, wSize);
  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_scale(const sgl_image_info_t &in,
                            const sgl_image_info_t &out, const int weight,
                            const int height, const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<ScaleFilter>();
  filter->SetTargetWidth(weight);
  filter->SetTargetHeight(height);
  filter->SetInterpType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_gray(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, const float r,
                           const float g, const float b) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<GrayFilter>();
  filter->SetRedFactor(r);
  filter->SetGreenFactor(g);
  filter->SetBlueFactor(b);

  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_color_separation(const sgl_image_info_t &in,
                                       const sgl_image_info_t &out,
                                       const float roff, const float goff,
                                       const float boff) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<ColorSeparationFilter>();

  filter->SetRedOffsetX(roff);
  filter->SetGreenOffsetX(goff);
  filter->SetBlueOffsetX(boff);

  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_mid_value(const sgl_image_info_t &in,
                                const sgl_image_info_t &out, const float radius,
                                const float threshold) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<MidValueFilter>();
  filter->SetRadius(radius);
  filter->SetThreshold(threshold);

  sgl::image::ImageEngine::Process(in, out, filter);
  return SGL_SUCCESS;
}

sgl_error_t sgl_image_path_blur(const sgl_image_info_t &in,
                                const sgl_image_info_t &out, float *vec,
                                const int amount, const int width,
                                const int height, float *startpos,
                                float *endpos, float *startvec, float *endvec,
                                const int num) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  if (vec == nullptr) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<pathBlurFilter>();

  const int k_size = width * height * 2;
  filter->SetK1(vec, k_size);
  filter->SetK(k_size);
  filter->SetAmount(amount);
  filter->SetStartPos(startpos, num);
  filter->SetEndPos(endpos, num);
  filter->SetStartVec(startvec, num);
  filter->SetEndVec(endvec, num);
  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_crystallize(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out, float *posx,
                                  float *posy, const int n) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  if (posx == nullptr) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<CrystallizeFilter>();

  const int k_size = n;
  filter->SetPos(posx, posy, k_size);
  filter->SetN(n);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_rotation_blur(const sgl_image_info_t &in,
                                    const sgl_image_info_t &in2,
                                    const sgl_image_info_t &out, const float x,
                                    const float y, const float a, const float b,
                                    const float ina, const float inb,
                                    const int strength, const float angle) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(in2) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(in);
  inputs.push_back(in2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(out);

  const auto filter = std::make_shared<RotationBlurFilter>();
  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetStrength(strength);
  filter->SetAngle(angle);

  sgl::image::ImageEngine::Process(inputs, outputs, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_facet(const sgl_image_info_t &in,
                            const sgl_image_info_t &out, const int radius,
                            const int intensitylevel) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<FacetFilter>();
  filter->SetRadius(radius);
  filter->SetLevel(intensitylevel);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_accented_edge(const sgl_image_info_t &in,
                                    const sgl_image_info_t &out, int *sobelx,
                                    int *sobely, const int size,
                                    const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<AccentedEdgeFilter>();
  filter->SetSobelx(sobelx, size);
  filter->SetSobely(sobely, size);
  filter->SetType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_irisblur(const sgl_image_info_t &in,
                               const sgl_image_info_t &in2,
                               const sgl_image_info_t &out, const float x,
                                const float y, const float a, const float b,
                               const float ina, const float inb,
                               const float angle) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(in2) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(in);
  inputs.push_back(in2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(out);

  const auto filter = std::make_shared<IrisBlurFilter>();
  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetAngle(angle);

  sgl::image::ImageEngine::Process(inputs, outputs, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_tilt_shift_blur(const sgl_image_info_t &in,
                                      const sgl_image_info_t &in2,
                                      const sgl_image_info_t &out, float *A,
                                      float *B, float *C, const float xoffset,
                                      const float yoffset, const int size) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(in2) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  std::vector<sgl::image::ImageInfo> inputs;
  inputs.push_back(in);
  inputs.push_back(in2);
  std::vector<sgl::image::ImageInfo> outputs;
  outputs.push_back(out);

  const auto filter = std::make_shared<TiltshiftBlurFilter>();
  filter->SetA(A, size);
  filter->SetB(B, size);
  filter->SetC(C, size);
  filter->SetOffset(xoffset, yoffset);

  sgl::image::ImageEngine::Process(inputs, outputs, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_radial_blur(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out,
                                  const float sharpness, const int strength,
                                  const float xCenter, const float yCenter) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<RadialBlurNewFilter>();

  filter->SetSharpness(sharpness);
  filter->SetStrength(strength);
  filter->SetCenter(xCenter, yCenter);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_rotational_blur(const sgl_image_info_t &in,
                                      const sgl_image_info_t &out,
                                      const float angle, const int strength,
                                      const float x, const float y) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }
  const auto filter = std::make_shared<RotationalBlurFilter>();

  filter->SetAngle(angle);
  filter->SetStrength(strength);
  filter->SetCenter(x, y);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_minmax(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, const int radius,
                             const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<MinMaxFilter>();
  filter->SetRadius(radius);
  filter->SetType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_color_halftone(const sgl_image_info_t &in,
                                     const sgl_image_info_t &out,
                                     const float cyanAngle,
                                     const float yellowAngle,
                                     const float magentaAngle,
                                     const float radius, float *lookup) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  constexpr int size = 256;
  const auto filter = std::make_shared<ColorhalftoneFilter>();
  filter->SetColor(cyanAngle, yellowAngle, magentaAngle, radius);
  filter->SetLookup(lookup, size);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_sharpen(const sgl_image_info_t &in,
                              const sgl_image_info_t &out, int *kernel,
                              const int size) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<SharpenFilter>();
  filter->SetKernel(kernel, size);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_polar_coordinates(const sgl_image_info_t &in,
                                        const sgl_image_info_t &out,
                                        const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<PolarCoordinatesFilter>();
  filter->SetType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_clouds(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, int *permuteLookup,
                             const int size, const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<CloudsFilter>();
  filter->SetLookup(permuteLookup, size);
  filter->SetType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_motion_blur(const sgl_image_info_t &in,
                                  const sgl_image_info_t &out,
                                  const int distance, const int angle,
                                  const float proportion) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<MotionBlurFilter>();
  filter->SetAngle(angle);
  filter->SetDistance(distance);
  filter->SetPro(proportion);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_twirl_warp(const sgl_image_info_t &in,
                                 const sgl_image_info_t &out, const int angle) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<TwirlWarpFilter>();
  filter->SetAngle(angle);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_zigzag(const sgl_image_info_t &in,
                             const sgl_image_info_t &out, const int wavelength,
                             const int amplitude, const int type_wave,
                             const float proportion) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<ZigzagFilter>();
  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetTypeWave(type_wave);
  filter->SetPro(proportion);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_spherize(const sgl_image_info_t &in,
                               const sgl_image_info_t &out, const int alpha,
                               const int type) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<SpherizeFilter>();
  filter->SetAlpha(alpha);
  filter->SetType(type);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_pinch(const sgl_image_info_t &in,
                            const sgl_image_info_t &out, const int amount) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<PinchFilter>();
  filter->SetAmount(amount);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_wave(const sgl_image_info_t &in,
                           const sgl_image_info_t &out, const int wavelength,
                           const int amplitude, const int x_pro,
                           const int y_pro, const int type, const int method) {
  if (sgl_image_check_args(in) != SGL_SUCCESS ||
      sgl_image_check_args(out) != SGL_SUCCESS) {
    return SGL_INVALID_ARGUMENT;
  }

  const auto filter = std::make_shared<WaveFilter>();
  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetPro(x_pro, y_pro);
  filter->SetType(type);
  filter->SetMethod(method);

  sgl::image::ImageEngine::Process(in, out, filter);

  return SGL_SUCCESS;
}

sgl_error_t sgl_image_check_args(const sgl_image_info_t &info) {
  if (info.type == SGL_IMAGE_TYPE_GPU) {
    if (info.info.gpu.gpuBuf.bufHandle == nullptr) {
      return SGL_INVALID_ARGUMENT;
    }
    if (info.info.gpu.gpuBuf.memHandle == nullptr) {
      return SGL_INVALID_ARGUMENT;
    }
  }

  if (info.type == SGL_IMAGE_TYPE_CPU) {
    if (info.info.cpu.data == nullptr) {
      return SGL_INVALID_ARGUMENT;
    }
  }

  return SGL_SUCCESS;
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

  image->gray = sgl_image_gray;
  image->threshold_split = sgl_image_threshold_split;
  image->gaussian_blur = sgl_image_gaussian_blur;
  image->gaussian_blur_float = sgl_image_gaussian_blur_float;
  image->surface_blur = sgl_image_surface_blur;
  image->distort_glass = sgl_image_distort_glass;
  image->adjust_saturation = sgl_image_adjust_saturation;
  image->palette_knife = sgl_image_palette_knife;
  image->hue_equal = sgl_image_hue_equal;
  image->blur_edge = sgl_image_blur_edge;
  image->custom_kernel = sgl_image_custom_kernel;
  image->color_balance = sgl_image_color_balance;
  image->black_white = sgl_image_black_white;
  image->scale = sgl_image_scale;
  image->color_separation = sgl_image_color_separation;
  image->mid_value = sgl_image_mid_value;
  image->path_blur = sgl_image_path_blur;
  image->crystallize = sgl_image_crystallize;
  image->rotation_blur = sgl_image_rotation_blur;
  image->facet = sgl_image_facet;
  image->accented_edge = sgl_image_accented_edge;
  image->iris_blur = sgl_image_irisblur;
  image->tilt_shift_blur = sgl_image_tilt_shift_blur;
  image->radial_blur = sgl_image_radial_blur;
  image->rotational_blur = sgl_image_rotational_blur;
  image->minmax = sgl_image_minmax;
  image->color_halftone = sgl_image_color_halftone;
  image->sharpen = sgl_image_sharpen;
  image->polar_coordinates = sgl_image_polar_coordinates;
  image->clouds = sgl_image_clouds;
  image->motion_blur = sgl_image_motion_blur;
  image->twirl_warp = sgl_image_twirl_warp;
  image->zigzag = sgl_image_zigzag;
  image->spherize = sgl_image_spherize;
  image->pinch = sgl_image_pinch;
  image->wave = sgl_image_wave;

  return image;
}
void sgl_image_destroy(sgl_image_t *image) {
  if (image == nullptr) {
    return;
  }
  free(image);
}