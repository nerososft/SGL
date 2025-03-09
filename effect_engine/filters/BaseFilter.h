//
// Created by 1234 on 2025/3/6.
//

#ifndef BASEFILTER_H
#define BASEFILTER_H
#include <vulkan/vulkan_core.h>

#include "IFilter.h"
#include "effect_engine/gpu/VkGPUContext.h"

struct FilterParams {
    size_t paramsSize;
    void *paramsData;
    std::string shaderPath;
};

class BaseFilter : public IFilter {
public:
    BaseFilter() = default;

    ~BaseFilter() override = default;

    VkResult DoApply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                     VkDeviceSize bufferSize,
                     uint32_t width,
                     uint32_t height,
                     VkBuffer inputBuffer,
                     VkBuffer outputBuffer,
                     const FilterParams& filterParams);

    VkResult Apply(const std::shared_ptr<VkGPUContext> &gpuCtx,
                   VkDeviceSize bufferSize,
                   uint32_t width,
                   uint32_t height,
                   VkBuffer inputBuffer,
                   VkBuffer outputBuffer) override;
};


#endif //BASEFILTER_H
