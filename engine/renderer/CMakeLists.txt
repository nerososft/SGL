if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
endif ()

add_library(render_engine
        renderer/Renderer.h
        renderer/Renderer.cpp
        renderer/RendererMesh.cpp
        renderer/RendererMesh.h
        renderer/RendererLight.cpp
        renderer/RendererLight.h
        renderer/RendererCamera.cpp
        renderer/RendererCamera.h
        renderer/RendererScene.cpp
        renderer/RendererScene.h
)
target_link_libraries(render_engine core)