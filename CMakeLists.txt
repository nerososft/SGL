cmake_minimum_required(VERSION 3.30)
project(gpu_effect_engine)

set(CMAKE_CXX_STANDARD 20)

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
        effect_engine/utils/TimeUtils.h)