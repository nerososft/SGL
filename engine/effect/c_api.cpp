// #include "c_api.h"

#if defined(OS_OPEN_HARMONY) || defined(ENABLE_WIN64)
#include "psutil/imageprocess/GpuFilterCAPI.h"
#endif

#include "core/log/Log.h"
#include "engine/effect/EffectEngine.h"
#include "engine/effect/filters/impl/AccentedEdgeFilter.h"
#include "engine/effect/filters/impl/BlackWhiteFilter.h"
#include "engine/effect/filters/impl/BlurEdgeFilter.h"
#include "engine/effect/filters/impl/CloudsFilter.h"
#include "engine/effect/filters/impl/ColorBalanceFilter.h"
#include "engine/effect/filters/impl/ColorSeparationFilter.h"
#include "engine/effect/filters/impl/ColorhalftoneFilter.h"
#include "engine/effect/filters/impl/CrystallizeFilter.h"
#include "engine/effect/filters/impl/CustomKernelFilter.h"
#include "engine/effect/filters/impl/DistortGlassFilter.h"
#include "engine/effect/filters/impl/FacetFilter.h"
#include "engine/effect/filters/impl/FastGaussianBlurFilter.h"
#include "engine/effect/filters/impl/GrayFilter.h"
#include "engine/effect/filters/impl/IrisBlurFilter.h"
#include "engine/effect/filters/impl/MidValueFilter.h"
#include "engine/effect/filters/impl/MinMaxFilter.h"
#include "engine/effect/filters/impl/MotionblurFilter.h"
#include "engine/effect/filters/impl/OldGaussianBlurFilter.h"
#include "engine/effect/filters/impl/OldGaussianBlurFloatFilter.h"
#include "engine/effect/filters/impl/PaletteKnifeFilter.h"
#include "engine/effect/filters/impl/PathBlurFilter.h"
#include "engine/effect/filters/impl/PinchFilter.h"
#include "engine/effect/filters/impl/PolarCoordinatesFilter.h"
#include "engine/effect/filters/impl/RadialBlurNewFilter.h"
#include "engine/effect/filters/impl/RotationBlurFilter.h"
#include "engine/effect/filters/impl/RotationalBlurFilter.h"
#include "engine/effect/filters/impl/ScaleFilter.h"
#include "engine/effect/filters/impl/SharpenFilter.h"
#include "engine/effect/filters/impl/SpherizeFilter.h"
#include "engine/effect/filters/impl/SurfaceBlurFilter.h"
#include "engine/effect/filters/impl/ThresholdSplitFilter.h"
#include "engine/effect/filters/impl/TiltshiftBlurFilter.h"
#include "engine/effect/filters/impl/TwirlWarpFilter.h"
#include "engine/effect/filters/impl/VibranceFilter.h"
#include "engine/effect/filters/impl/WaveFilter.h"
#include "engine/effect/filters/impl/ZigzagFilter.h"

EffectEngine g_effect_engine;
bool g_effect_engine_inited = false;

bool init_core() {
  if (!g_effect_engine_inited) {
    g_effect_engine_inited = g_effect_engine.Init();
  }
  return g_effect_engine_inited;
}

const char *get_gpu_engine_name() {
  if (!g_effect_engine_inited) {
    return "Not initialized";
  }
  return g_effect_engine.GetGPUName().c_str();
}

bool destroy_gpu_engine() { return true; }

bool threshold_split_filter_gpu(void *in, void *out, const int bright) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<ThresholdSplitFilter>();
  filter->SetBright(bright);

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);
  return true;
}

bool gaussian_blur_filter_gpu(void *in, void *out, const int r) {
  if (in == nullptr || out == nullptr)
    return false;
  const ImageInfo *input = static_cast<ImageInfo *>(in);
  const ImageInfo *output = static_cast<ImageInfo *>(out);

  if (r >= 3) {
    const auto filter = std::make_shared<FastGaussianBlurFilter>();
    filter->SetRadius(r);
    g_effect_engine.Process(*input, *output, filter);
  } else {
    const auto filter = std::make_shared<OldGaussianBlurFilter>();
    filter->SetRadius(r);
    g_effect_engine.Process(*input, *output, filter);
  }
  return true;
}

bool gaussian_blur_filter_float_gpu(void *in, void *out, const int r) {
  if (in == nullptr || out == nullptr)
    return false;
  const ImageInfo *input = static_cast<ImageInfo *>(in);
  const ImageInfo *output = static_cast<ImageInfo *>(out);

  const auto filter = std::make_shared<OldGaussianBlurFloatFilter>();
  filter->SetRadius(r);
  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool surface_blur_filter_gpu(void *in, void *out, const int r, const int th) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<SurfaceBlurFilter>();
  filter->SetBlurRadius(r);
  filter->SetThreshold(th);

  const ImageInfo *input = static_cast<ImageInfo *>(in);
  const ImageInfo *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);
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

  const ImageInfo *input = static_cast<ImageInfo *>(in);
  const ImageInfo *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool adjust_saturation_gpu(void *in, void *out, const int v, const int s) {
  if (in == nullptr || out == nullptr)
    return false;
  if (1) {
    const auto filter = std::make_shared<VibranceFilter>();

    filter->SetVibrance(v);
    filter->SetSaturation(s);

    const auto *input = static_cast<ImageInfo *>(in);
    const auto *output = static_cast<ImageInfo *>(out);
    Logger() << "c_api adjust_saturation_gpu begin " << std::endl;
    g_effect_engine.Process(*input, *output, filter);
    Logger() << "c_api adjust_saturation_gpu end  " << std::endl;

    Logger() << "c_api filter destory begin  " << std::endl;

    filter->Destroy();
    Logger() << "c_api filter destory end  " << std::endl;
  }

  if (0) {
    const auto filter = std::make_shared<DistortGlassFilter>();

    float scale = v + 2;
    float intensity = s + 2;

    scale = scale / 50;
    intensity = 45 - intensity * 3;

    filter->SetScale(scale);
    filter->SetIntensity(intensity);
    filter->SetZoom(1);

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);

    g_effect_engine.Process(*input, *output, filter);
  }

  if (0) {
    const auto filter = std::make_shared<RadialBlurNewFilter>();

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);
    // filter->SetAngle( (v + 4)/ 10.0);
    // filter->SetStrength(s *2 +4);

    filter->SetSharpness((v + 4) / 10.0);
    filter->SetStrength(s * 2 + 4);
    filter->SetCenter(0.5, 0.5);

    g_effect_engine.Process(*input, *output, filter);
  }

  if (0) {
    const auto filter = std::make_shared<RotationalBlurFilter>();

    const ImageInfo *input = static_cast<ImageInfo *>(in);
    const ImageInfo *output = static_cast<ImageInfo *>(out);
    // filter->SetAngle( (v + 4)/ 10.0);
    // filter->SetStrength(s *2 +4);

    filter->SetAngle((v + 4));
    filter->SetStrength(s + 4);
    filter->SetCenter(0.3, 0.3);

    g_effect_engine.Process(*input, *output, filter);
  }

  return true;
}

bool palette_knife_gpu(void *in, void *out, const int r, const int s) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<PaletteKnifeFilter>();
  filter->SetRadius(r);
  filter->SetQuantScale(s);

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool hue_equal_filter_gpu(void *in, void *out) {
  if (in == nullptr || out == nullptr)
    return false;
  // const auto filter = std::make_shared<HueEqualFilter>();
  // const auto* input = static_cast<ImageInfo*>(in);
  // const auto* output = static_cast<ImageInfo*>(out);

  // g_effect_engine.Process(*input, *output, filter);

  // const auto filter = std::make_shared<customKernelFilter>();
  // const auto* input = static_cast<ImageInfo*>(in);
  // const auto* output = static_cast<ImageInfo*>(out);

  // int k[25] = {0 , 0 ,0 ,0 ,0,
  //             0 , 0 ,-1 ,0 ,0,
  //             0 , -1 ,5 ,-1 ,0,
  //             0 , 0 ,-1 ,0 ,0,
  //            0 , 0 ,0 ,0 ,0 };

  // filter->SetK(k, 25);
  // filter->SetOffset(0);
  // filter->SetScale(1);
  // filter->SetRadius(2);

  // g_effect_engine.Process(*input, *output, filter);

  const auto filter = std::make_shared<ColorBalanceFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  int p[9] = {0, 100, 0, 0, 100, 0, 0, 100, 0};

  int adjustP[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  filter->SetP(p, 9);
  filter->SetP(adjustP, 9);
  filter->SetL(0);

  g_effect_engine.Process(*input, *output, filter);

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

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);
  g_effect_engine.Process(*input, *output, filter);

  filter->Destroy();

  return true;
}

bool custom_kernel_filter_gpu(void *in, void *out, int *k, const int radius,
                              const int offset, const int scale) {
  if (in == nullptr || out == nullptr | k == nullptr)
    return false;
  const auto filter = std::make_shared<CustomKernelFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

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

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool color_balance_filter_gpu(void *in, void *out, float *adjustP, int *p,
                              const int l) {
  if (in == nullptr || out == nullptr || adjustP == nullptr || p == nullptr)
    return false;
  const auto filter = std::make_shared<ColorBalanceFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  // int p[9] = { 0 , 50 ,0 ,
  //             0 , 50 ,0 ,
  //             0 , 50 ,0 };

  // int adjustP[9] = { 0 , 0 ,0 ,
  //                    0 , 0 ,0 ,
  //                    0 , 0 ,0 };

  filter->SetP(p, 9);
  filter->SetAdjustP(adjustP, 9);
  filter->SetL(l);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool black_white_filter_gpu(void *in, void *out, float *weight,
                            const int wSize) {
  if (in == nullptr || out == nullptr || weight == nullptr)
    return false;
  const auto filter = std::make_shared<BlackWhiteFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetWeight(weight, wSize);
  g_effect_engine.Process(*input, *output, filter);
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

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);

  unsigned char *data0 = (unsigned char *)output->data;

  int p0 = data0[0];
  int p1 = data0[1];
  int p2 = data0[2];
  int p3 = data0[3];

  unsigned char *data1 =
      (unsigned char *)output->data + weight * height / 2 * 4;

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

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);
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
  const ImageInfo *input = static_cast<ImageInfo *>(in);
  const ImageInfo *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);
  return true;
}

bool midvalue_filter_gpu(void *in, void *out, const float radius,
                         const float threshold) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<MidValueFilter>();
  filter->SetRadius(radius);
  filter->SetThreshold(threshold);

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);
  return true;
}

bool pathblur_filter_gpu(void *in, void *out, float *vec, const int amount,
                         const int width, const int height, float *startpos,
                         float *endpos, float *startvec, float *endvec,
                         const int num) {
  if (in == nullptr || out == nullptr || vec == nullptr)
    return false;
  const auto filter = std::make_shared<pathBlurFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  int k_size = width * height * 2;
  filter->SetK1(vec, k_size);
  filter->SetK(k_size);
  filter->SetAmount(amount);
  filter->SetStartPos(startpos, num);
  filter->SetEndPos(endpos, num);
  filter->SetStartVec(startvec, num);
  filter->SetEndVec(endvec, num);
  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool crystallize_filter_gpu(void *in, void *out, float *posx, float *posy,
                            const int n) {
  if (in == nullptr || out == nullptr || posx == nullptr)
    return false;
  const auto filter = std::make_shared<CrystallizeFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  int k_size = n;
  filter->SetPos(posx, posy, k_size);
  filter->SetN(n);

  g_effect_engine.Process(*input, *output, filter);

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
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);
  const auto *input2 = static_cast<ImageInfo *>(in2);

  std::vector<ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetStrength(strength);
  filter->SetAngle(angle);

  g_effect_engine.Process(inputs, outputs, filter);

  return true;
}

bool facet_filter_gpu(void *in, void *out, const int radius,
                      const int intensitylevel) {
  const auto filter = std::make_shared<FacetFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetRadius(radius);
  filter->SetLevel(intensitylevel);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool accented_edge_filter_gpu(void *in, void *out, int *sobelx, int *sobely,
                              const int size, const int type) {
  const auto filter = std::make_shared<AccentedEdgeFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetSobelx(sobelx, size);
  filter->SetSobely(sobely, size);
  filter->SetType(type);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool irisblur_filter_gpu(void *in, void *in2, void *out, const float x,
                         const float y, const float a, const float b,
                         const float ina, const float inb, const float angle) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<IrisBlurFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);
  const auto *input2 = static_cast<ImageInfo *>(in2);

  std::vector<ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetCenterX(x);
  filter->SetCenterY(y);
  filter->SetA(a);
  filter->SetB(b);
  filter->SetinA(ina);
  filter->SetinB(inb);
  filter->SetAngle(angle);

  g_effect_engine.Process(inputs, outputs, filter);

  return true;
}

bool tiltshiftblur_filter_gpu(void *in, void *in2, void *out, float *A,
                              float *B, float *C, const float xoffset,
                              const float yoffset, const int size) {
  if (in == nullptr || out == nullptr)
    return false;
  const auto filter = std::make_shared<TiltshiftBlurFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);
  const auto *input2 = static_cast<ImageInfo *>(in2);

  std::vector<ImageInfo> inputs;
  inputs.push_back(*input);
  inputs.push_back(*input2);
  std::vector<ImageInfo> outputs;
  outputs.push_back(*output);

  filter->SetA(A, size);
  filter->SetB(B, size);
  filter->SetC(C, size);
  filter->SetOffset(xoffset, yoffset);

  g_effect_engine.Process(inputs, outputs, filter);

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

  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);

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
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool minmax_filter_gpu(void *in, void *out, const int radius, const int type) {
  const auto filter = std::make_shared<MinMaxFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetRadius(radius);
  filter->SetType(type);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool colorhalftone_filter_gpu(void *in, void *out, const float cyanAngle,
                              const float yellowAngle, const float magentaAngle,
                              const float radius, float *lookup) {
  const auto filter = std::make_shared<ColorhalftoneFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  const int size = 256;
  filter->SetColor(cyanAngle, yellowAngle, magentaAngle, radius);
  filter->SetLookup(lookup, size);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool sharpen_filter_gpu(void *in, void *out, int *kernel, const int size) {
  const auto filter = std::make_shared<SharpenFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetKernel(kernel, size);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool polarcoordinates_filter_gpu(void *in, void *out, const int type) {
  const auto filter = std::make_shared<PolarCoordinatesFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetType(type);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool clouds_filter_gpu(void *in, void *out, int *permuteLookup, const int size,
                       const int type) {
  const auto filter = std::make_shared<CloudsFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetLookup(permuteLookup, size);
  filter->SetType(type);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool motionblur_filter_gpu(void *in, void *out, const int distance,
                           const int angle, const float proportion) {
  const auto filter = std::make_shared<MotionBlurFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetAngle(angle);
  filter->SetDistance(distance);
  filter->SetPro(proportion);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool twirlwarp_filter_gpu(void *in, void *out, int angle) {
  const auto filter = std::make_shared<TwirlWarpFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetAngle(angle);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool zigzag_filter_gpu(void *in, void *out, const int wavelength,
                       const int amplitude, const int type_wave,
                       const float proportion) {
  const auto filter = std::make_shared<ZigzagFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetTypeWave(type_wave);
  filter->SetPro(proportion);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool spherize_filter_gpu(void *in, void *out, const int alpha, const int type) {
  const auto filter = std::make_shared<SpherizeFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetAlpha(alpha);
  filter->SetType(type);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool pinch_filter_gpu(void *in, void *out, int amount) {
  const auto filter = std::make_shared<PinchFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetAmount(amount);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}

bool wave_filter_gpu(void *in, void *out, const int wavelength,
                     const int amplitude, const int x_pro, const int y_pro,
                     const int type, const int method) {
  const auto filter = std::make_shared<WaveFilter>();
  const auto *input = static_cast<ImageInfo *>(in);
  const auto *output = static_cast<ImageInfo *>(out);

  filter->SetWavelength(wavelength);
  filter->SetAmplitude(amplitude);
  filter->SetPro(x_pro, y_pro);
  filter->SetType(type);
  filter->SetMethod(method);

  g_effect_engine.Process(*input, *output, filter);

  return true;
}
