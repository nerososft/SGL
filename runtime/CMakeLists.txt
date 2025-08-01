if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
endif ()

set(RUNTIME_GPU_SOURCE gpu/VkGPUHelper.cpp
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
        gpu/VkGPUTexture.cpp
        gpu/VkGPUTexture.h
        gpu/compute_graph/ComputeGraph.cpp
        gpu/compute_graph/ComputeGraph.h
        gpu/compute_graph/IComputeGraphNode.h
        gpu/ComputePipelineCache.cpp
        gpu/ComputePipelineCache.h
        gpu/compute_graph/ComputePipelineNode.cpp
        gpu/compute_graph/ComputePipelineNode.h
        gpu/compute_graph/Sequence.cpp
        gpu/compute_graph/Sequence.h
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

set(RUNTIME_SOURCE ${RUNTIME_GPU_SOURCE}
        utils/TimeUtils.cpp
        utils/TimeUtils.h
        utils/IOUtils.cpp
        utils/IOUtils.h
        log/Log.h
        config.h)

include_directories(".")

if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    add_library(runtime
            ${ENGINE_SOURCE}
    )
else ()
    add_library(runtime SHARED
            ${RUNTIME_SOURCE}
    )
    target_link_libraries(runtime png)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(runtime vulkan-1)
else ()
    target_link_libraries(runtime vulkan)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    target_link_libraries(runtime hilog_ndk.z)
endif ()