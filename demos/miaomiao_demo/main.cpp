//
// Created by neo on 25-5-9.
//

#include <iostream>
#include <ostream>

#include "gpu_engine/gpu/VkGPUBuffer.h"
#include "gpu_engine/gpu/VkGPUContext.h"

void cpu_gpu_mem_share_demo() {
    std::vector<const char *> extensions = {};
    auto gpuCtx = std::make_shared<VkGPUContext>(extensions);

    if (gpuCtx->Init() != VK_SUCCESS) {
        std::cerr << "Failed to initialize GPU context!" << std::endl;
        return;
    }

    const VkDeviceSize bufferSize = 1024 * 1024 * 4;

    const auto sharedMem = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (sharedMem->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, bufferSize) != VK_SUCCESS) {
        std::cerr << "Failed to allocate shared memory!" << std::endl;
        return;
    };
    if (sharedMem->MapBuffer(bufferSize) != VK_SUCCESS) {
        std::cerr << "Failed to map shared memory buffer!" << std::endl;
        return;
    }

    void *sharedMemAddr = sharedMem->GetMappedAddr();
    static_cast<uint32_t *>(sharedMemAddr)[0] = 0xFFFFFFFF; // ACCESS from CPU

    sharedMem->UnMapBuffer();

    VkBuffer buffer = sharedMem->GetBuffer(); // this can be accessed in GPU
}

int main(int argc, char *argv[]) {
    std::cout << "MiaoYing demo" << std::endl;

    cpu_gpu_mem_share_demo();
    return 0;
}
