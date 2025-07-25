add_executable(image_demo
        main.cpp
)
target_link_libraries(image_demo image_engine)
target_link_libraries(image_demo png)