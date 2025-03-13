//
// Created by 1234 on 2025/3/6.
//

#include "VkGPUContext.h"

#include "VkGPUContext.h"

#ifdef Q_OS_OPENHARMONY
#include <effect_engine/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include <iostream>
#include <ostream>

VkGPUContext::VkGPUContext(const std::vector<const char *> &requiredInstanceExtensions) {
    for (auto requiredInstanceExtension: requiredInstanceExtensions) {
        this->instanceEnableExtensions.push_back(requiredInstanceExtension);
    }
}

void VkGPUContext::AddInstanceEnableLayer(const char *layerName) {
    this->instanceEnableLayers.push_back(layerName);
}

void VkGPUContext::AddInstanceEnableExtension(const char *extensionName) {
    this->instanceEnableExtensions.push_back(extensionName);
}

void VkGPUContext::SelectCPU(const uint32_t gpuIndex) {
    if (gpuIndex >= this->physicalDeviceNums) {
        std::cerr << "GPU index " << gpuIndex << " is out of bounds" << std::endl;
        return;
    }
    this->selectedGPUIndex = gpuIndex;
    this->physicalDevice = this->physicalDevices[gpuIndex];
}

VkResult VkGPUContext::CreateDevice(const std::vector<const char *> &deviceEnableLayers,
                                    std::vector<const char *> deviceEnableExtensions) {
    if (this->physicalDevice == VK_NULL_HANDLE) {
        std::cerr << "Current physical device is not selected" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

#ifdef __APPLE__
    deviceEnableExtensions.push_back("VK_KHR_portability_subset");
#endif /* __APPLE__ */

    constexpr float queuePriorities[1] = {1.0f};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.flags = 0;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;
    queueCreateInfos.push_back(queueCreateInfo);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceEnableExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceEnableExtensions.data();
    deviceCreateInfo.enabledLayerCount = deviceEnableLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = deviceEnableLayers.data();
    deviceCreateInfo.pEnabledFeatures = &this->physicalDeviceFeatures[this->selectedGPUIndex];
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    return vkCreateDevice(this->physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, &this->device);
}

VkQueue VkGPUContext::GetQueue() {
    vkGetDeviceQueue(this->device, this->queueFamilyIndex, this->queueIndex, &this->queue);
    return this->queue;
}

VkResult VkGPUContext::Init() {
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceVersion(&this->instanceVersion);
    if (result != VK_SUCCESS) {
        std::cout << "vkEnumerateInstanceVersion failed, err=" << string_VkResult(result) << std::endl;
        return result;
    }
    std::cout << "instance api:"
            << VK_API_VERSION_VARIANT(this->instanceVersion) << "."
            << VK_API_VERSION_MAJOR(this->instanceVersion) << "."
            << VK_API_VERSION_MINOR(this->instanceVersion) << "."
            << VK_API_VERSION_PATCH(this->instanceVersion) << std::endl;


    VkInstanceCreateFlags instanceCreateFlags = 0;
#ifdef __APPLE__
    instanceCreateFlags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    this->instanceEnableExtensions.push_back("VK_KHR_portability_enumeration");
#endif /* __APPLE__ */

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.apiVersion = this->instanceVersion;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan";
    appInfo.pApplicationName = "Engine";
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = instanceCreateFlags;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = this->instanceEnableLayers.size();
    createInfo.ppEnabledLayerNames = this->instanceEnableLayers.data();
    createInfo.enabledExtensionCount = this->instanceEnableExtensions.size();
    createInfo.ppEnabledExtensionNames = this->instanceEnableExtensions.data();
    result = vkCreateInstance(&createInfo, nullptr, &this->instance);
    if (result != VK_SUCCESS) {
        std::cout << "vkCreateInstance failed, err=" << string_VkResult(result) << std::endl;
        return result;
    }

    vkEnumeratePhysicalDevices(instance, &this->physicalDeviceNums, nullptr);
    std::cout << "physical devices num: " << this->physicalDeviceNums << std::endl;
    this->physicalDevices.resize(this->physicalDeviceNums);
    this->physicalDeviceFeatures.resize(this->physicalDeviceNums);
    this->physicalDevicesProperties.resize(this->physicalDeviceNums);
    this->physicalDevicesMemoryProperties.resize(this->physicalDeviceNums);
    this->queuesFamilyProps.resize(this->physicalDeviceNums);
    vkEnumeratePhysicalDevices(instance, &this->physicalDeviceNums, this->physicalDevices.data());

    for (uint32_t physicalDeviceIndex = 0; physicalDeviceIndex < this->physicalDeviceNums; physicalDeviceIndex++) {
        vkGetPhysicalDeviceProperties(this->physicalDevices[physicalDeviceIndex],
                                      &this->physicalDevicesProperties[physicalDeviceIndex]);
        std::cout << "physical device[" << physicalDeviceIndex << "]: " << std::endl
                << "\tdeviceType:" << string_VkPhysicalDeviceType(
                    this->physicalDevicesProperties[physicalDeviceIndex].deviceType) << std::endl
                << "\tdeviceName:" << this->physicalDevicesProperties[physicalDeviceIndex].deviceName << std::endl
                << "\tapiVersion:" << VK_API_VERSION_VARIANT(
                    this->physicalDevicesProperties[physicalDeviceIndex].apiVersion)
                << "." << VK_API_VERSION_MAJOR(this->physicalDevicesProperties[physicalDeviceIndex].apiVersion)
                << "." << VK_API_VERSION_MINOR(this->physicalDevicesProperties[physicalDeviceIndex].apiVersion)
                << "." << VK_API_VERSION_PATCH(this->physicalDevicesProperties[physicalDeviceIndex].apiVersion) <<
                std::endl
                << "\tdeviceId:" << this->physicalDevicesProperties[physicalDeviceIndex].deviceID << std::endl
                << "\tvendorId:" << this->physicalDevicesProperties[physicalDeviceIndex].vendorID << std::endl
                << "\tdriverVersion:" << VK_API_VERSION_VARIANT(
                    this->physicalDevicesProperties[physicalDeviceIndex].driverVersion)
                << "." << VK_API_VERSION_MAJOR(
                    this->physicalDevicesProperties[physicalDeviceIndex].driverVersion)
                << "." << VK_API_VERSION_MINOR(
                    this->physicalDevicesProperties[physicalDeviceIndex].driverVersion)
                << "." << VK_API_VERSION_PATCH(
                    this->physicalDevicesProperties[physicalDeviceIndex].driverVersion) << std::endl
                << "\tmaxWorkGroupSize: x=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupSize[0]
                << ", y=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupSize[1]
                << ", z=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupSize[2]
                << std::endl
                << "\tmaxWorkGroupCount: x=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupCount[0]
                << ", y=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupCount[1]
                << ", z=" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeWorkGroupCount[2]
                << std::endl;

        vkGetPhysicalDeviceFeatures(this->physicalDevices[physicalDeviceIndex],
                                    &this->physicalDeviceFeatures[physicalDeviceIndex]);

        vkGetPhysicalDeviceMemoryProperties(this->physicalDevices[physicalDeviceIndex],
                                            &this->physicalDevicesMemoryProperties[physicalDeviceIndex]);
        const auto [memoryTypeCount, memoryTypes, memoryHeapCount, memoryHeaps] = this->physicalDevicesMemoryProperties[
            physicalDeviceIndex];
        for (uint32_t heapIndex = 0; heapIndex < memoryHeapCount; heapIndex++) {
            std::cout << "\tmemoryHeaps[" << heapIndex << "]: "
                    << "size=" << memoryHeaps[heapIndex].size
                    << ", flags=" << memoryHeaps[heapIndex].flags
                    << std::endl;
        }
        for (uint32_t typeIndex = 0; typeIndex < memoryTypeCount; typeIndex++) {
            std::cout << "\tmemoryTypes[" << typeIndex << "]: "
                    << "heapIndex=" << memoryTypes[typeIndex].heapIndex
                    << ", propertiesFlags=" << string_VkMemoryPropertyFlags(
                        memoryTypes[typeIndex].propertyFlags)
                    << std::endl;
        }
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevices[physicalDeviceIndex],
                                                 &this->queuesFamilyProps[physicalDeviceIndex].queueNums, nullptr);
        this->queuesFamilyProps[physicalDeviceIndex].queueFamilyProps.resize(
            this->queuesFamilyProps[physicalDeviceIndex].queueNums);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevices[physicalDeviceIndex],
                                                 &this->queuesFamilyProps[physicalDeviceIndex].queueNums,
                                                 this->queuesFamilyProps[physicalDeviceIndex].queueFamilyProps.data());

        for (int format = VK_FORMAT_UNDEFINED; format <= VK_FORMAT_UNDEFINED; ++format) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(this->physicalDevices[physicalDeviceIndex],
                                                static_cast<VkFormat>(format), &props);

            std::cout << "\tFormat: " << format << std::endl;
            std::cout << "\t\tLinear Tiling Features: " << props.linearTilingFeatures << std::endl;
            std::cout << "\t\tOptimal Tiling Features: " << props.optimalTilingFeatures << std::endl;
            std::cout << "\t\tBuffer Features: " << props.bufferFeatures << std::endl;
        }
    }

    this->SelectCPU(0);
    result = this->CreateDevice(this->defaultDeviceEnableLayers, this->defaultDeviceEnableExtensions);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create device, err=" << string_VkResult(result) << std::endl;
    }
    this->GetQueue();


    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    VkDescriptorPoolSize storageBufferDescriptorPoolSize;
    storageBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferDescriptorPoolSize.descriptorCount = 10;
    descriptorPoolSizes.push_back(storageBufferDescriptorPoolSize);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 10;
    descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    result = vkCreateDescriptorPool(this->device, &descriptorPoolCreateInfo, nullptr, &this->descriptorPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create descriptor pool, err=" << string_VkResult(result) << std::endl;
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    result = vkCreateCommandPool(this->device, &commandPoolCreateInfo, nullptr, &this->commandPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command pool, err=" << string_VkResult(result) << std::endl;
    }

    return result;
}
