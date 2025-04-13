if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
    include_directories("../../")
    #include_directories("D:/vcpkg_install/vcpkg/installed/x64-windows/include/libpng16")
    #link_directories("$D:/vcpkg_install/vcpkg/installed/x64-windows/lib")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")
    include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")
endif ()

add_library(gpu_effect_engine SHARED
        EffectEngine.cpp
        EffectEngine.h
        c_api.cpp
        c_api.h
        filters/BasicFilter.cpp
        filters/BasicFilter.h
        filters/impl/GrayFilter.cpp
        filters/impl/GrayFilter.h
        filters/impl/PaletteKnifeFilter.h
        filters/impl/PaletteKnifeFilter.cpp
        filters/impl/BlurEdgeFilter.h
        filters/impl/BlurEdgeFilter.cpp

        filters/impl/DistortGlassFilter.h
        filters/impl/DistortGlassFilter.cpp
        gpu/VkGPUHelper.cpp
        gpu/VkGPUHelper.h
        utils/IOUtils.cpp
        utils/IOUtils.h
        utils/TimeUtils.cpp
        utils/TimeUtils.h
        utils/ImageUtils.cpp
        utils/ImageUtils.h
        gpu/VKGPUContext.cpp
        gpu/VKGPUContext.h
        filters/IFilter.h
        gpu/VkGPUComputePipeline.cpp
        gpu/VkGPUComputePipeline.h
        gpu/VkGPUDescriptorSet.cpp
        gpu/VkGPUDescriptorSet.h
        gpu/compute_graph/ComputeGraph.cpp
        gpu/compute_graph/ComputeGraph.h
        gpu/compute_graph/IComputeGraphNode.h
        gpu/compute_graph/ComputePipelineNode.cpp
        gpu/compute_graph/ComputePipelineNode.h
        filters/impl/GaussianBlurFilter.cpp
        filters/impl/GaussianBlurFilter.h
        filters/impl/OldGaussianBlurFilter.cpp
        filters/impl/OldGaussianBlurFilter.h
        filters/impl/OldGaussianBlurFloatFilter.cpp
        filters/impl/OldGaussianBlurFloatFilter.h
        gpu/compute_graph/BufferCopyNode.h
        gpu/compute_graph/BufferCopyNode.cpp
        filters/impl/ChinesePaintFilter.h
        filters/impl/ChinesePaintFilter.cpp
        filters/impl/ColorPencilSketchFilter.cpp
        filters/impl/ColorPencilSketchFilter.h
        filters/impl/ColorSeparationFilter.cpp
        filters/impl/ColorSeparationFilter.h
        filters/impl/ScaleFilter.cpp
        filters/impl/ScaleFilter.h
        filters/impl/RadialBlurFilter.cpp
        filters/impl/RadialBlurFilter.h
        filters/impl/FocusBlurFilter.cpp
        filters/impl/FocusBlurFilter.h
        filters/impl/SurfaceBlurFilter.cpp
        filters/impl/SurfaceBlurFilter.h
        filters/impl/ThresholdSplitFilter.h
        filters/impl/ThresholdSplitFilter.cpp
        filters/impl/customKernelFilter.h
        filters/impl/customKernelFilter.cpp
        filters/impl/colorBalanceFilter.h
        filters/impl/colorBalanceFilter.cpp
        filters/impl/BlackWhiteFilter.h
        filters/impl/BlackWhiteFilter.cpp
        blenders/IBlender.h
        blenders/BaseBlender.cpp
        blenders/BaseBlender.h
        blenders/impl/DissolveBlender.cpp
        blenders/impl/DissolveBlender.h
        blenders/impl/MultiplyBlender.cpp
        blenders/impl/MultiplyBlender.h
        blenders/impl/OverlayBlender.cpp
        blenders/impl/OverlayBlender.h
        blenders/impl/LighterColorBlender.cpp
        blenders/impl/LighterColorBlender.h
        log/Log.h
        config.h
        filters/impl/VibranceFilter.cpp
        filters/impl/VibranceFilter.h
        filters/impl/NES8BitMosaicFilter.cpp
        filters/impl/NES8BitMosaicFilter.h
        filters/impl/VoronoiFilter.cpp
        filters/impl/VoronoiFilter.h
        gpu/VkGPUBuffer.cpp
        gpu/VkGPUBuffer.h
        gpu/VkGPUGraphicsPipeline.cpp
        gpu/VkGPUGraphicsPipeline.h
        gpu/compute_graph/GraphicsPipelineNode.cpp
        gpu/compute_graph/GraphicsPipelineNode.h
        gpu/VkGPURenderPass.cpp
        gpu/VkGPURenderPass.h
        gpu/compute_graph/GraphicsRenderPassNode.cpp
        gpu/compute_graph/GraphicsRenderPassNode.h
        filters/impl/FastGaussianBlurFilter.cpp
        filters/impl/FastGaussianBlurFilter.h
        operators/IOperator.h
        operators/UnaryOperator.cpp
        operators/UnaryOperator.h
        operators/BinaryOperator.cpp
        operators/BinaryOperator.h
        filters/impl/HPSBlurFilter.cpp
        filters/impl/HPSBlurFilter.h
        filters/impl/MedianFilter.cpp
        filters/impl/MedianFilter.h
        gpu/compute_graph/SubComputeGraph.cpp
        gpu/compute_graph/SubComputeGraph.h
        filters/impl/MidValueFilter.h
        filters/impl/MidValueFilter.cpp
        filters/impl/PathBlurFilter.h
        filters/impl/PathBlurFilter.cpp
        filters/impl/CrystallizeFilter.h
        filters/impl/CrystallizeFilter.cpp
        filters/impl/RotationBlurFilter.h
        filters/impl/RotationBlurFilter.cpp
        Renderer.cpp
        Renderer.h
        gpu/VkGPUFramebuffer.cpp
        gpu/VkGPUFramebuffer.h
)
target_link_libraries(gpu_effect_engine vulkan)
if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
else ()
    target_link_libraries(gpu_effect_engine png)
endif ()
if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    target_link_libraries(gpu_effect_engine hilog_ndk.z)
endif ()