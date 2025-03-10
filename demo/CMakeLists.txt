link_directories("/opt/homebrew/Cellar/libpng/1.6.47/lib")
include_directories("/opt/homebrew/Cellar/libpng/1.6.47/include")

add_executable(demo
        main.cpp
        utils/ImageUtils.cpp
        utils/ImageUtils.h
)
target_link_libraries(demo gpu_effect_engine)
target_link_libraries(demo png)