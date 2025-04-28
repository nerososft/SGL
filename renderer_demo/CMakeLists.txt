if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/Cellar/glfw/3.4/lib")
    include_directories("/opt/homebrew/Cellar/glfw/3.4/include")
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
add_custom_target(compile_renderer_shaders ALL DEPENDS ${COMPILED_SHADERS})

add_executable(renderer_demo
        renderer/Renderer.h
        renderer/Renderer.cpp
        main.cpp
        event/IEventHandler.h
        window/IWindow.h
        window/IWindow.cpp
        window/impl/GLFWWindowImpl.h
        window/impl/GLFWWindowImpl.cpp
        GraphicsApp.h
        GraphicsApp.cpp
        scene/ModelLoader.cpp
        scene/ModelLoader.h
        scene/Mesh.cpp
        scene/Mesh.h
        scene/Camera.cpp
        scene/Camera.h
)
target_link_libraries(renderer_demo gpu_engine)
target_link_libraries(renderer_demo glfw)
target_link_libraries(renderer_demo png)
target_link_libraries(renderer_demo assimp)