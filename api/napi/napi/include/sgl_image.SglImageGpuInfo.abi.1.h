#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBuffer.abi.1.h"
#include "sgl_image.SglImageGpuInfo.abi.0.h"
struct sgl_image_SglImageGpuInfo_t0 {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bytesPerLine;
    struct sgl_image_SglBuffer_t0 gpuBuf;
};
#pragma clang diagnostic pop
