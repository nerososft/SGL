//
// Created by 1234 on 2025/3/6.
//

#ifndef VKGPUCONTEXT_H
#define VKGPUCONTEXT_H
#include <vector>


#include <vulkan/vulkan.h>

struct DeviceQueue {
    VkQueue queue;
    uint32_t queueIndex;
    uint32_t queueFamilyIndex;
};

struct DeviceQueueFamily {
    std::vector<DeviceQueue> queues;
    VkQueueFamilyProperties queueFamilyProp;
    uint32_t queueFamilyIndex;
};

struct PhysicalDeviceQueueFamilyProps {
    uint32_t queueFamilyPropsNums;
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
};

class VkGPUContext {
    uint32_t physicalDeviceNums = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<VkPhysicalDeviceProperties> physicalDevicesProperties;
    std::vector<VkPhysicalDeviceFeatures> physicalDeviceFeatures;
    std::vector<VkPhysicalDeviceMemoryProperties> physicalDevicesMemoryProperties;
    std::vector<PhysicalDeviceQueueFamilyProps> queuesFamilyProps;
    std::vector<DeviceQueueFamily> queueFamilies;

    uint32_t instanceVersion = 0;

    std::vector<const char *> instanceEnableLayers;
    std::vector<const char *> instanceEnableExtensions;

    VkInstance instance = VK_NULL_HANDLE;

    uint32_t selectedGPUIndex = 0;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    std::vector<const char *> defaultDeviceEnableLayers;
    std::vector<const char *> defaultDeviceEnableExtensions;
    VkDevice device = VK_NULL_HANDLE;

    std::vector<VkCommandPool> commandPools;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

public:
    explicit VkGPUContext(const std::vector<const char *> &requiredInstanceExtensions);

    void AddInstanceEnableLayer(const char *layerName);

    void AddInstanceEnableExtension(const char *extensionName);

    void AddDeviceEnabledLayer(const char *layerName);

    void AddDeviceEnabledExtension(const char *extensionName);

    void SelectCPU(uint32_t gpuIndex);

    VkResult CreateDevice(const std::vector<const char *> &deviceEnableLayers,
                          std::vector<const char *> deviceEnableExtensions);

    DeviceQueue DispatchQueue(VkQueueFlags flag);

    VkResult Init();

    void Reset() const;

    ~VkGPUContext() = default;

    [[nodiscard]] VkDevice GetCurrentDevice() const { return this->device; }
    [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return this->physicalDevice; }

    [[nodiscard]] VkCommandPool GetCommandPool(const uint32_t queueFamilyIdx) const {
        return this->commandPools[queueFamilyIdx];
    }

    [[nodiscard]] VkDescriptorPool GetDescriptorPool() const { return this->descriptorPool; }
    [[nodiscard]] VkPipelineCache GetPipelineCache() const { return this->pipelineCache; }
    [[nodiscard]] VkInstance GetInstance() const { return this->instance; }
    [[nodiscard]] uint32_t GetGPUIndex() const { return this->selectedGPUIndex; }

    [[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const {
        return this->physicalDevicesMemoryProperties[selectedGPUIndex];
    }

    [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const {
        return this->physicalDevicesProperties[selectedGPUIndex];
    }
};


#endif //VKGPUCONTEXT_H
