if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
    include_directories(".")
    #include_directories("D:/vcpkg_install/vcpkg/installed/x64-windows/include/libpng16")
    #link_directories("$D:/vcpkg_install/vcpkg/installed/x64-windows/lib")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")
    include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")
endif ()

set(ENGINE_GPU_SOURCE gpu/VkGPUHelper.cpp
        gpu/VkGPUHelper.h
        gpu/VkGPUContext.cpp
        gpu/VkGPUContext.h
        gpu/VkGPUComputePipeline.cpp
        gpu/VkGPUComputePipeline.h
        gpu/VkGPUDescriptorSet.cpp
        gpu/VkGPUDescriptorSet.h
        gpu/VkGPUBuffer.cpp
        gpu/VkGPUBuffer.h
        gpu/VkGPUGraphicsPipeline.cpp
        gpu/VkGPUGraphicsPipeline.h
        gpu/VkGPURenderPass.cpp
        gpu/VkGPURenderPass.h
        gpu/VkGPUFramebuffer.cpp
        gpu/VkGPUFramebuffer.h
        gpu/VkGPUSwapChain.cpp
        gpu/VkGPUSwapChain.h
        gpu/compute_graph/ComputeGraph.cpp
        gpu/compute_graph/ComputeGraph.h
        gpu/compute_graph/IComputeGraphNode.h
        gpu/compute_graph/ComputePipelineNode.cpp
        gpu/compute_graph/ComputePipelineNode.h
        gpu/compute_graph/BufferCopyNode.h
        gpu/compute_graph/BufferCopyNode.cpp
        gpu/compute_graph/GraphicsPipelineNode.cpp
        gpu/compute_graph/GraphicsPipelineNode.h
        gpu/compute_graph/GraphicsRenderPassNode.cpp
        gpu/compute_graph/GraphicsRenderPassNode.h
        gpu/compute_graph/SubComputeGraph.cpp
        gpu/compute_graph/SubComputeGraph.h
        gpu/compute_graph/ImageToBufferCopyNode.cpp
        gpu/compute_graph/ImageToBufferCopyNode.h)

set(ENGINE_SOURCE ${ENGINE_GPU_SOURCE} utils/IOUtils.cpp
        utils/IOUtils.h
        utils/TimeUtils.cpp
        utils/TimeUtils.h
        utils/ImageUtils.cpp
        utils/ImageUtils.h
        log/Log.h
        config.h
        operators/IOperator.h
        operators/UnaryOperator.cpp
        operators/UnaryOperator.h
        operators/BinaryOperator.cpp
        operators/BinaryOperator.h)

include_directories("./")

if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    add_library(gpu_engine
            ${ENGINE_SOURCE}
    )
else ()
    add_library(gpu_engine SHARED
            ${ENGINE_SOURCE}
    )
    target_link_libraries(gpu_engine png)
endif ()

target_link_libraries(gpu_engine vulkan)
if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    target_link_libraries(gpu_engine hilog_ndk.z)
endif ()