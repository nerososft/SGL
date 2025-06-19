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

add_library(ml_engine
        operators/IOperator.h
        operators/UnaryOperator.h
        operators/UnaryOperator.cpp
        operators/BinaryOperator.h
        operators/BinaryOperator.cpp
        operators/impl/ReLUOperator.h
        operators/impl/ReLUOperator.cpp
        operators/impl/MatMulOperator.cpp
        operators/impl/MatMulOperator.h
        operators/impl/SigmoidOperator.cpp
        operators/impl/SigmoidOperator.h
        operators/impl/TanhOperator.cpp
        operators/impl/TanhOperator.h
        operators/impl/SoftmaxOperator.cpp
        operators/impl/SoftmaxOperator.h
        operators/impl/GELUOperator.cpp
        operators/impl/GELUOperator.h
        operators/impl/RMSNormOperator.cpp
        operators/impl/RMSNormOperator.h
        Matrix.h
        Matrix.cpp
        MLEngine.h
        MLEngine.cpp
        operators/impl/LayerNormOperator.cpp
        operators/impl/LayerNormOperator.h
)
target_link_libraries(ml_engine core)