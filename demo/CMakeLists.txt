add_executable(demo
        main.cpp
)
target_link_libraries(demo gpu_effect_engine)
target_link_libraries(demo png)