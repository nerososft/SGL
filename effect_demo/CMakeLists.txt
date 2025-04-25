add_executable(effect_demo
        main.cpp
)
target_link_libraries(effect_demo effect_engine)
target_link_libraries(effect_demo png)