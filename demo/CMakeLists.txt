if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
    include_directories("D:/vcpkg_install/vcpkg/installed/x64-windows/include/libpng16")
	link_directories("$D:/vcpkg_install/vcpkg/installed/x64-windows/lib")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")
    include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")
endif ()

add_executable(demo
        main.cpp
        ../effect_engine/utils/ImageUtils.cpp
        ../effect_engine/utils/ImageUtils.h
)
target_link_libraries(demo gpu_effect_engine)
target_link_libraries(demo png)