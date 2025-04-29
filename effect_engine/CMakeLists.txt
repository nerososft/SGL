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
	
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
	message(STATUS "Running on OHOS")
	include_directories("../../")
endif ()

set(ENGINE_GPU_SOURCE gpu/VkGPUHelper.cpp
        gpu/VkGPUHelper.h
        gpu/VKGPUContext.cpp
        gpu/VKGPUContext.h
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

set(ENGINE_SOURCE ${ENGINE_GPU_SOURCE} EffectEngine.cpp
        EffectEngine.h
        c_api.cpp
        c_api.h
        utils/IOUtils.cpp
        utils/IOUtils.h
        utils/TimeUtils.cpp
        utils/TimeUtils.h
        utils/ImageUtils.cpp
        utils/ImageUtils.h
        filters/IFilter.h
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
        filters/impl/GaussianBlurFilter.cpp
        filters/impl/GaussianBlurFilter.h
        filters/impl/OldGaussianBlurFilter.cpp
        filters/impl/OldGaussianBlurFilter.h
        filters/impl/OldGaussianBlurFloatFilter.cpp
        filters/impl/OldGaussianBlurFloatFilter.h
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
        filters/impl/VibranceFilter.cpp
        filters/impl/VibranceFilter.h
        filters/impl/NES8BitMosaicFilter.cpp
        filters/impl/NES8BitMosaicFilter.h
        filters/impl/VoronoiFilter.cpp
        filters/impl/VoronoiFilter.h
        filters/impl/FastGaussianBlurFilter.cpp
        filters/impl/FastGaussianBlurFilter.h
        filters/impl/MedianFilter.cpp
        filters/impl/MedianFilter.h
        filters/impl/MidValueFilter.h
        filters/impl/MidValueFilter.cpp
        filters/impl/PathBlurFilter.h
        filters/impl/PathBlurFilter.cpp
        filters/impl/CrystallizeFilter.h
        filters/impl/CrystallizeFilter.cpp
        filters/impl/RotationBlurFilter.h
        filters/impl/RotationBlurFilter.cpp
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
        operators/IOperator.h
        operators/UnaryOperator.cpp
        operators/UnaryOperator.h
        operators/BinaryOperator.cpp
        operators/BinaryOperator.h

        filters/impl/CrystallizeFilter.h
        filters/impl/CrystallizeFilter.cpp

        filters/impl/RotationBlurFilter.h
        filters/impl/RotationBlurFilter.cpp
        filters/impl/FacetFilter.h
        filters/impl/FacetFilter.cpp
        filters/impl/AccentedEdgeFilter.h
        filters/impl/AccentedEdgeFilter.cpp
        filters/impl/IrisBlurFilter.h
        filters/impl/IrisBlurFilter.cpp
        filters/impl/TiltshiftBlurFilter.h
        filters/impl/TiltshiftBlurFilter.cpp
        filters/impl/RotationalBlurFilter.h
        filters/impl/RotationalBlurFilter.cpp
        filters/impl/RadialBlurNewFilter.h
		filters/impl/RadialBlurNewFilter.cpp
        filters/impl/MinMaxFilter.h
        filters/impl/MinMaxFilter.cpp
        filters/impl/ColorhalftoneFilter.h
        filters/impl/ColorhalftoneFilter.cpp
        filters/impl/SharpenFilter.h
        filters/impl/SharpenFilter.cpp
        filters/impl/PolarCoordinatesFilter.h
        filters/impl/PolarCoordinatesFilter.cpp
        filters/impl/CloudsFilter.h
        filters/impl/CloudsFilter.cpp
        filters/impl/MotionblurFilter.h
        filters/impl/MotionblurFilter.cpp
        filters/impl/TwirlWarpFilter.h
        filters/impl/TwirlWarpFilter.cpp
        filters/impl/ZigzagFilter.h
        filters/impl/ZigzagFilter.cpp
        filters/impl/SpherizeFilter.h
        filters/impl/SpherizeFilter.cpp
        filters/impl/PinchFilter.h
        filters/impl/PinchFilter.cpp
        filters/impl/WaveFilter.h
        filters/impl/WaveFilter.cpp

        )

if (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    set(ENGINE_SOURCE ${ENGINE_SOURCE} filters/impl/HPSBlurFilter.cpp filters/impl/HPSBlurFilter.h)
endif ()

add_library(gpu_effect_engine SHARED
        ${ENGINE_SOURCE}
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