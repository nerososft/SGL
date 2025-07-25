if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/Cellar/libpng/1.6.50/lib")
    include_directories("/opt/homebrew/Cellar/libpng/1.6.50/include")
endif ()

add_library(render_engine
        ../utils/ImageUtils.h
        ../utils/ImageUtils.cpp
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
target_link_libraries(render_engine runtime)