if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
endif ()

# compile builtin shaders
set(BUILTIN_SHADER_DIR assets/builtin.shaders/*)
message("Builtin shaders dir: ${BUILTIN_SHADER_DIR}")
file(GLOB BUILTIN_SHADER_FILES ${BUILTIN_SHADER_DIR}/*.glsl)

foreach (SHADER_FILE ${BUILTIN_SHADER_FILES})
    message(${SHADER_FILE})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set(COMPILED_SHADER ${CMAKE_SOURCE_DIR}/shader_compiled/${SHADER_NAME}.spv)
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
add_custom_target(compile_operator_shaders ALL DEPENDS ${COMPILED_SHADERS})

add_library(compute_engine
        operators/IOperator.h
        operators/UnaryOperator.h
        operators/UnaryOperator.cpp
        operators/BinaryOperator.h
        operators/BinaryOperator.cpp
        operators/impl/gpu/ReLUOperator.h
        operators/impl/gpu/ReLUOperator.cpp
        operators/impl/gpu/MatMulOperator.cpp
        operators/impl/gpu/MatMulOperator.h
        operators/impl/gpu/SigmoidOperator.cpp
        operators/impl/gpu/SigmoidOperator.h
        operators/impl/gpu/TanhOperator.cpp
        operators/impl/gpu/TanhOperator.h
        operators/impl/gpu/SoftmaxOperator.cpp
        operators/impl/gpu/SoftmaxOperator.h
        operators/impl/gpu/GELUOperator.cpp
        operators/impl/gpu/GELUOperator.h
        operators/impl/gpu/SiLUOperator.cpp
        operators/impl/gpu/SiLUOperator.h
        operators/impl/gpu/RMSNormOperator.cpp
        operators/impl/gpu/RMSNormOperator.h
        operators/impl/gpu/LayerNormOperator.cpp
        operators/impl/gpu/LayerNormOperator.h
        operators/impl/cpu/CPUComputeNode.cpp
        operators/impl/cpu/CPUComputeNode.h
        operators/impl/cpu/AvgOperator.cpp
        operators/impl/cpu/AvgOperator.h
        operators/impl/cpu/VarianceOperator.cpp
        operators/impl/cpu/VarianceOperator.h
        Matrix.h
        Matrix.cpp
        ComputeEngine.h
        ComputeEngine.cpp
        operators/impl/cpu/SumOperator.cpp
        operators/impl/cpu/SumOperator.h
        operators/impl/cpu/RMSOperator.cpp
        operators/impl/cpu/RMSOperator.h
        operators/impl/gpu/SplitOperator.cpp
        operators/impl/gpu/SplitOperator.h
        operators/Operator.cpp
        operators/Operator.h
        operators/impl/gpu/ConcatOperator.cpp
        operators/impl/gpu/ConcatOperator.h
        operators/impl/gpu/MulOperator.cpp
        operators/impl/gpu/MulOperator.h
        operators/impl/gpu/ScaleOperator.cpp
        operators/impl/gpu/ScaleOperator.h
        operators/impl/gpu/AddOperator.cpp
        operators/impl/gpu/AddOperator.h
        operators/impl/gpu/RoPEOperator.cpp
        operators/impl/gpu/RoPEOperator.h
        operators/impl/gpu/RoPEMulOperator.cpp
        operators/impl/gpu/RoPEMulOperator.h
        operators/impl/gpu/TransposeOperator.cpp
        operators/impl/gpu/TransposeOperator.h
        operators/impl/cpu/ExpSumOperator.cpp
        operators/impl/cpu/ExpSumOperator.h
        operators/impl/cpu/MaxOperator.cpp
        operators/impl/cpu/MaxOperator.h
        operators/impl/gpu/LogSoftmaxOperator.cpp
        operators/impl/gpu/LogSoftmaxOperator.h
        operators/impl/gpu/SinOperator.cpp
        operators/impl/gpu/SinOperator.h
        operators/impl/gpu/CosOperator.cpp
        operators/impl/gpu/CosOperator.h
        operators/impl/gpu/ExpOperator.cpp
        operators/impl/gpu/ExpOperator.h
        operators/impl/gpu/LogOperator.cpp
        operators/impl/gpu/LogOperator.h
)
target_link_libraries(compute_engine runtime)