cmake_minimum_required(VERSION 3.30)
project(gpu_effect_engine)

set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    set(VULKAN_SDK "/Users/neo/VulkanSDK/1.3.290.0")
    set(SHADER_COMPILER "${VULKAN_SDK}/macOS/bin/glslang")
    include_directories("/usr/local/include")
    link_directories("${VULKAN_SDK}/macOS/lib/")
    link_directories("/opt/homebrew/lib/")
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
    list(APPEND ${COMPILED_SHADERS} ${COMPILED_SHADER})
    add_custom_command(
            OUTPUT ${COMPILED_SHADER}
            COMMAND ${SHADER_COMPILER} -V ${SHADER_FILE} -o ${COMPILED_SHADER} -e main
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling ${SHADER_FILE} to ${COMPILED_SHADER}"
            VERBATIM
    )
endforeach ()
add_custom_target(compile_shaders ALL DEPENDS ${COMPILED_SHADERS})

add_subdirectory(effect_engine)
add_subdirectory(demo)
