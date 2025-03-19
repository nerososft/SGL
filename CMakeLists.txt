cmake_minimum_required(VERSION 3.20)
project(gpu_effect_engine)

set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
    set(VULKAN_SDK_PATH "D:/vulkanSDK")
    set(SHADER_COMPILER "${VULKAN_SDK_PATH}/Bin/glslang.exe")
    add_definitions(-DENABLE_WIN64=1)
    include_directories("${VULKAN_SDK_PATH}/Include")
    include_directories("D:/vcpkg_install/vcpkg/installed/x64-windows/include/libpng16")
    link_directories("${VULKAN_SDK_PATH}/Lib/")
    link_directories("D:/vcpkg_install/vcpkg/installed/x64-windows/lib")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
    set(OH_SDK_PATH "C:/Huawei/DevEcoStudio/sdk")
    add_definitions(-DOS_OPEN_HARMONY)
    include_directories("${OH_SDK_PATH}/default/openharmony/native/sysroot/usr/include")
    add_compile_options(-fdeclspec)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    set(VULKAN_SDK_PATH "/Users/neo/VulkanSDK/1.3.290.0")
    set(SHADER_COMPILER "${VULKAN_SDK_PATH}/macOS/bin/glslang")
    include_directories("/usr/local/include")
    link_directories("${VULKAN_SDK_PATH}/macOS/lib/")
    link_directories("/opt/homebrew/lib/")
    add_compile_options(-fdeclspec)
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    add_compile_options(-O3 -march=armv8-a+simd)
endif ()

# compile builtin shaders
set(BUILTIN_SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/builtin.shaders/*)
message("Builtin shaders dir: ${BUILTIN_SHADER_DIR}")
file(GLOB BUILTIN_SHADER_FILES ${BUILTIN_SHADER_DIR}/*.glsl)

foreach (SHADER_FILE ${BUILTIN_SHADER_FILES})
    message(${SHADER_FILE})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set(COMPILED_SHADER ${CMAKE_CURRENT_SOURCE_DIR}/shader_compiled/${SHADER_NAME}.spv)
    message("${SHADER_COMPILER} -V ${SHADER_FILE} -o ${COMPILED_SHADER} -e main")
    add_custom_command(
            OUTPUT ${COMPILED_SHADER}
            COMMAND ${SHADER_COMPILER} -V ${SHADER_FILE} -o ${COMPILED_SHADER} -e main
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling ${SHADER_FILE} to ${COMPILED_SHADER}"
            VERBATIM
    )
    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach ()
message("Need: ${COMPILED_SHADERS}")
add_custom_target(compile_shaders ALL DEPENDS ${COMPILED_SHADERS})

add_subdirectory(effect_engine)
add_subdirectory(demo)
