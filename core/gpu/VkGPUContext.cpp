//
// Created by 1234 on 2025/3/6.
//

#include "VkGPUContext.h"

#ifdef OS_OPEN_HARMONY
#include <core/gpu/utils/vk_enum_string_helper.h>
#else
#include <vulkan/vk_enum_string_helper.h>
#endif
#include <cstring>
#include <iostream>
#include <ostream>

#include "log/Log.h"

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

void VkGPUContext::AddDeviceEnabledLayer(const char *layerName) {
    this->defaultDeviceEnableLayers.push_back(layerName);
}

void VkGPUContext::AddDeviceEnabledExtension(const char *extensionName) {
    this->defaultDeviceEnableExtensions.push_back(extensionName);
}

void VkGPUContext::SelectGPU(const uint32_t gpuIndex) {
    if (gpuIndex >= this->physicalDeviceNums) {
        Logger() << "GPU index " << gpuIndex << " is out of bounds" << std::endl;
        return;
    }
    this->selectedGPUIndex = gpuIndex;
    this->physicalDevice = this->physicalDevices[gpuIndex];
}

VkResult VkGPUContext::CreateDevice(const std::vector<const char *> &deviceEnableLayers,
                                    std::vector<const char *> deviceEnableExtensions) {
    if (this->physicalDevice == VK_NULL_HANDLE) {
        Logger() << "Current physical device is not selected" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

#ifdef __APPLE__
    deviceEnableExtensions.push_back("VK_KHR_portability_subset");
#endif /* __APPLE__ */


    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (auto &queueFamily: queueFamilies) {
        std::vector<float> queuePriorities;
        queuePriorities.reserve(queueFamilies.size());
        for (size_t queueIndex = 0; queueIndex < queueFamilies.size(); queueIndex++) {
            queuePriorities.push_back(1.0f);
        }
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.flags = 0;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
        queueCreateInfo.queueCount = queueFamily.queues.size();
        queueCreateInfo.pQueuePriorities = queuePriorities.data();
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.pNext = nullptr;
    VkPhysicalDeviceSynchronization2Features sync2Feature{};
    for (const auto extName: deviceEnableExtensions) {
        if (std::strcmp(extName, "VK_KHR_synchronization2") == 0) {
            sync2Feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
            sync2Feature.synchronization2 = VK_TRUE;
            deviceCreateInfo.pNext = &sync2Feature;
        }
    }
    deviceCreateInfo.enabledExtensionCount = deviceEnableExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceEnableExtensions.data();
    deviceCreateInfo.enabledLayerCount = deviceEnableLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = deviceEnableLayers.data();
    deviceCreateInfo.pEnabledFeatures = &this->physicalDeviceFeatures[this->selectedGPUIndex];
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    return vkCreateDevice(this->physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, &this->device);
}

std::vector<DeviceQueue> VkGPUContext::FindQueuesByQueueFlag(const VkQueueFlags flag) const {
    std::vector<DeviceQueue> suitableQueues;
    for (size_t queueFamilyIndex = 0; queueFamilyIndex < this->queueFamilies.size(); queueFamilyIndex++) {
        if ((this->queueFamilies[queueFamilyIndex].queueFamilyProp.queueFlags & flag) == flag) {
            for (size_t queueIndex = 0; queueIndex < this->queueFamilies[queueFamilyIndex].queues.size(); queueIndex
                 ++) {
                suitableQueues.push_back(queueFamilies[queueFamilyIndex].queues[queueIndex]);
            }
        }
    }
    return suitableQueues;
}

DeviceQueue VkGPUContext::DispatchQueue(const VkQueueFlags flag) {
    const std::vector<DeviceQueue> suitableQueue = FindQueuesByQueueFlag(flag);
    if (suitableQueue.empty()) {
        Logger() << "no queue found with flag:" << string_VkQueueFlags(flag) << std::endl;
        return {};
    }
    const uint32_t selectedQueueIndex = this->dispatchQueueIndex % suitableQueue.size();
    const uint32_t queueFamilyIndex = suitableQueue[selectedQueueIndex].queueFamilyIndex;
    const uint32_t queueIndex = suitableQueue[selectedQueueIndex].queueIndex;

    Logger() << "Queue selected: " << queueFamilyIndex << "-" << queueIndex << std::endl;
    if (suitableQueue[selectedQueueIndex].queue == VK_NULL_HANDLE) {
        vkGetDeviceQueue(this->device,
                         queueFamilyIndex,
                         queueIndex,
                         &queueFamilies[queueFamilyIndex].queues[queueIndex].queue);
    }
    // this->dispatchQueueIndex++; // TODO: fix me, may crash on some gpu
    return queueFamilies[queueFamilyIndex].queues[queueIndex];
}

VkResult VkGPUContext::Init() {
    VkResult result = VK_SUCCESS;

    result = vkEnumerateInstanceVersion(&this->instanceVersion);
    if (result != VK_SUCCESS) {
        Logger() << "vkEnumerateInstanceVersion failed, err=" << string_VkResult(result) << std::endl;
        return result;
    }
    Logger() << "instance api:"
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
        Logger() << "vkCreateInstance failed, err=" << string_VkResult(result) << std::endl;
        return result;
    }

    vkEnumeratePhysicalDevices(instance, &this->physicalDeviceNums, nullptr);
    Logger() << "physical devices num: " << this->physicalDeviceNums << std::endl;
    this->physicalDevices.resize(this->physicalDeviceNums);
    this->physicalDeviceFeatures.resize(this->physicalDeviceNums);
    this->physicalDevicesProperties.resize(this->physicalDeviceNums);
    this->physicalDevicesMemoryProperties.resize(this->physicalDeviceNums);
    this->queuesFamilyProps.resize(this->physicalDeviceNums);
    vkEnumeratePhysicalDevices(instance, &this->physicalDeviceNums, this->physicalDevices.data());

    for (uint32_t physicalDeviceIndex = 0; physicalDeviceIndex < this->physicalDeviceNums; physicalDeviceIndex++) {
        vkGetPhysicalDeviceProperties(this->physicalDevices[physicalDeviceIndex],
                                      &this->physicalDevicesProperties[physicalDeviceIndex]);
        Logger() << "physical device[" << physicalDeviceIndex << "]: " << std::endl
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
                << std::endl
                << "\tmaxComputeSharedMemorySize:" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxComputeSharedMemorySize << std::endl
                << "\tmaxStorageBufferRange:" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxStorageBufferRange
                << std::endl
                << "\tmaxMemoryAllocationCount:" << this->physicalDevicesProperties[physicalDeviceIndex].limits.
                maxMemoryAllocationCount
                << std::endl;

        vkGetPhysicalDeviceFeatures(this->physicalDevices[physicalDeviceIndex],
                                    &this->physicalDeviceFeatures[physicalDeviceIndex]);

        vkGetPhysicalDeviceMemoryProperties(this->physicalDevices[physicalDeviceIndex],
                                            &this->physicalDevicesMemoryProperties[physicalDeviceIndex]);
        const auto [memoryTypeCount, memoryTypes, memoryHeapCount, memoryHeaps] = this->physicalDevicesMemoryProperties[
            physicalDeviceIndex];
        for (uint32_t heapIndex = 0; heapIndex < memoryHeapCount; heapIndex++) {
            Logger() << "\tmemoryHeaps[" << heapIndex << "]: "
                    << "size=" << memoryHeaps[heapIndex].size
                    << ", flags=" << memoryHeaps[heapIndex].flags
                    << std::endl;
        }
        for (uint32_t typeIndex = 0; typeIndex < memoryTypeCount; typeIndex++) {
            Logger() << "\tmemoryTypes[" << typeIndex << "]: "
                    << "heapIndex=" << memoryTypes[typeIndex].heapIndex
                    << ", propertiesFlags=" << string_VkMemoryPropertyFlags(
                        memoryTypes[typeIndex].propertyFlags)
                    << std::endl;
        }
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevices[physicalDeviceIndex],
                                                 &this->queuesFamilyProps[physicalDeviceIndex].queueFamilyPropsNums,
                                                 nullptr);
        this->queuesFamilyProps[physicalDeviceIndex].queueFamilyProps.resize(
            this->queuesFamilyProps[physicalDeviceIndex].queueFamilyPropsNums);
        vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevices[physicalDeviceIndex],
                                                 &this->queuesFamilyProps[physicalDeviceIndex].queueFamilyPropsNums,
                                                 this->queuesFamilyProps[physicalDeviceIndex].queueFamilyProps.data());

        for (int format = VK_FORMAT_UNDEFINED; format <= VK_FORMAT_UNDEFINED; ++format) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(this->physicalDevices[physicalDeviceIndex],
                                                static_cast<VkFormat>(format), &props);

            Logger() << "\tFormat: " << format << std::endl;
            Logger() << "\t\tLinear Tiling Features: " << props.linearTilingFeatures << std::endl;
            Logger() << "\t\tOptimal Tiling Features: " << props.optimalTilingFeatures << std::endl;
            Logger() << "\t\tBuffer Features: " << props.bufferFeatures << std::endl;
        }
    }

    this->SelectGPU(0); // TODO: select strongest GPU

    size_t currentDeviceQueueNums = 0;
    const PhysicalDeviceQueueFamilyProps currentQueueFamilyProps = this->queuesFamilyProps[this->selectedGPUIndex];
    this->queueFamilies.resize(currentQueueFamilyProps.queueFamilyProps.size());
    for (uint32_t queueFamilyIdx = 0; queueFamilyIdx < currentQueueFamilyProps.queueFamilyProps.size(); queueFamilyIdx
         ++) {
        const VkQueueFlags queueFlags = currentQueueFamilyProps.queueFamilyProps[queueFamilyIdx].queueFlags;
        const uint32_t queueNums = currentQueueFamilyProps.queueFamilyProps[queueFamilyIdx].queueCount;

        currentDeviceQueueNums += queueNums;

        queueFamilies[queueFamilyIdx].queueFamilyIndex = queueFamilyIdx;
        queueFamilies[queueFamilyIdx].queueFamilyProp = currentQueueFamilyProps.queueFamilyProps[queueFamilyIdx];
        queueFamilies[queueFamilyIdx].queues.resize(queueNums);

        for (uint32_t queueIdx = 0; queueIdx < queueNums; queueIdx++) {
            this->queueFamilies[queueFamilyIdx].queues[queueIdx].queueIndex = queueIdx;
            this->queueFamilies[queueFamilyIdx].queues[queueIdx].queueFamilyIndex = queueFamilyIdx;
            this->queueFamilies[queueFamilyIdx].queues[queueIdx].queue = VK_NULL_HANDLE;
        }

        Logger() << "\tQueue Family Idx: " << queueFamilyIdx << " "
                << (queueFlags & VK_QUEUE_COMPUTE_BIT ? "[COMPUTE]" : " ")
                << (queueFlags & VK_QUEUE_TRANSFER_BIT ? "[TRANSFER]" : " ")
                << (queueFlags & VK_QUEUE_GRAPHICS_BIT ? "[PRESENT]" : " ")
                << std::endl;
        for (uint32_t queueIdx = 0; queueIdx < queueNums; queueIdx++) {
            Logger() << "\t\tQueue Idx: " << queueIdx << std::endl;
        }
    }
    Logger() << "\tQueue Nums: " << currentDeviceQueueNums << std::endl;

    result = this->CreateDevice(this->defaultDeviceEnableLayers, this->defaultDeviceEnableExtensions);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create device, err=" << string_VkResult(result) << std::endl;
    }

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    VkDescriptorPoolSize storageBufferDescriptorPoolSize;
    storageBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferDescriptorPoolSize.descriptorCount = 8192;
    descriptorPoolSizes.push_back(storageBufferDescriptorPoolSize);

    VkDescriptorPoolSize uniformBufferDescriptorPoolSize;
    uniformBufferDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferDescriptorPoolSize.descriptorCount = 1024;
    descriptorPoolSizes.push_back(uniformBufferDescriptorPoolSize);

    VkDescriptorPoolSize samplerDescriptorPoolSize;
    samplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerDescriptorPoolSize.descriptorCount = 1024;
    descriptorPoolSizes.push_back(samplerDescriptorPoolSize);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 4096;
    descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    result = vkCreateDescriptorPool(this->device, &descriptorPoolCreateInfo, nullptr, &this->descriptorPool);
    if (result != VK_SUCCESS) {
        Logger() << "Failed to create descriptor pool, err=" << string_VkResult(result) << std::endl;
    }

    this->commandPools.resize(queueFamilies.size());

    for (uint32_t queueFamilyIdx = 0; queueFamilyIdx < queueFamilies.size(); queueFamilyIdx++) {
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIdx;
        result = vkCreateCommandPool(this->device, &commandPoolCreateInfo,
                                     nullptr,
                                     &this->commandPools[queueFamilyIdx]);
        if (result != VK_SUCCESS) {
            Logger() << "Failed to create command pool, err=" << string_VkResult(result) << std::endl;
        }
    }

    return result;
}

void VkGPUContext::Reset() const {
    vkResetDescriptorPool(device, this->descriptorPool, 0);
}
