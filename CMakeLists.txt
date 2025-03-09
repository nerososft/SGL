cmake_minimum_required(VERSION 3.30)
project(gpu_effect_engine)

set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories("/usr/local/include")
include_directories("/opt/homebrew/Cellar/sdl2/2.28.5/include")
include_directories("/opt/homebrew/Cellar/sdl2_ttf/2.20.2/include")
include_directories("/opt/homebrew/Cellar/libusb/1.0.27/include")
include_directories("/opt/homebrew/Cellar/glfw/3.4/include")
include_directories("/opt/homebrew/Cellar/cglm/0.9.4/include")
link_directories("/Users/neo/VulkanSDK/1.3.290.0/macOS/lib/")
link_directories("/opt/homebrew/lib/")

add_executable(gpu_effect_engine demo/main.cpp
        demo/main.cpp
        effect_engine/EffectEngine.cpp
        effect_engine/EffectEngine.h
        effect_engine/filters/BaseFilter.cpp
        effect_engine/filters/BaseFilter.h
        effect_engine/filters/impl/GrayFilter.cpp
        effect_engine/filters/impl/GrayFilter.h
        effect_engine/gpu/VkGPUHelper.cpp
        effect_engine/gpu/VkGPUHelper.h
        effect_engine/utils/IOUtils.cpp
        effect_engine/utils/IOUtils.h
        effect_engine/utils/TimeUtils.cpp
        effect_engine/utils/TimeUtils.h
        effect_engine/gpu/VKGPUContext.cpp
        effect_engine/gpu/VKGPUContext.h
        effect_engine/filters/IFilter.h
        effect_engine/gpu/VkGPUComputePipeline.cpp
        effect_engine/gpu/VkGPUComputePipeline.h)

target_link_libraries(gpu_effect_engine vulkan)