#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.abi.0.h"
#include "sgl_image.SglImageGpuInfo.abi.1.h"
#include "sgl_image.SglImageCpuInfo.abi.1.h"
union sgl_image_Info_union0 {
    struct sgl_image_SglImageGpuInfo_t0 gpu;
    struct sgl_image_SglImageCpuInfo_t0 cpu;
};
struct sgl_image_Info_t0 {
    int m_tag;
    union sgl_image_Info_union0 m_data;
};
#pragma clang diagnostic pop
