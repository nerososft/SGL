add_executable(demo
        main.cpp
        utils/ImageUtils.cpp
        utils/ImageUtils.h
)
target_link_libraries(demo gpu_effect_engine)