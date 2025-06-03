add_executable(transform_demo transform.cpp)
add_executable(blender5_demo blender5.cpp)

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
add_custom_target(compile_others_shaders ALL DEPENDS ${COMPILED_SHADERS})

target_link_libraries(transform_demo gpu_engine)
target_link_libraries(blender5_demo gpu_engine)