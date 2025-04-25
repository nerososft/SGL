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

add_executable(renderer_demo
        Renderer.h
        Renderer.cpp
        main.cpp
        event/IEventHandler.h
        window/IWindow.h
        window/IWindow.cpp
        window/impl/GLFWWindowImpl.h
        window/impl/GLFWWindowImpl.cpp
        GraphicsApp.h
        GraphicsApp.cpp
)
target_link_libraries(renderer_demo gpu_engine)
target_link_libraries(renderer_demo glfw)
target_link_libraries(renderer_demo png)