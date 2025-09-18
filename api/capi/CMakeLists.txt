add_library(SGL SHARED
        include/sgl/sgl.h
        include/sgl/sgl_error.h
        include/sgl/sgl_compute.h
        include/sgl/sgl_image.h
        include/sgl/sgl_buffer.h
        include/sgl/sgl_gpu_ctx.h
        sgl.cpp
        sgl_image.cpp
        sgl_buffer.cpp
        sgl_buffer.cpp
        sgl_compute.cpp
)
target_link_libraries(SGL compute_engine)
target_link_libraries(SGL image_engine)

add_executable(SGL_test
        tests/test.c
)
include_directories("./include")
target_link_libraries(SGL_test SGL)