//
// Created by 1234 on 2025/3/6.
//

#ifndef VKGPUCONTEXT_H
#define VKGPUCONTEXT_H
#include <vector>


class VKGPUContext {
public:
    VKGPUContext() = default;

    ~VKGPUContext() = default;

    bool Init(std::vector<const char *> requiredExtensions, std::vector<const char *> enabledExtensions);
};


#endif //VKGPUCONTEXT_H
