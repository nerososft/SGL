add_executable(huge_image_demo
        tile.cpp
)
target_link_libraries(huge_image_demo image_engine)
target_link_libraries(huge_image_demo png)
target_link_libraries(huge_image_demo jpeg)