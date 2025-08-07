add_library(SGL SHARED
        include/sgl.h
        include/sgl_error.h
        include/sgl_compute.h
        include/sgl_image.h
        include/sgl_buffer.h
        include/sgl_gpu_ctx.h
        sgl.cpp
        sgl_image.cpp
        sgl_buffer.cpp
        sgl_buffer.cpp
        sgl_compute.cpp
)
target_link_libraries(SGL compute_engine)
target_link_libraries(SGL image_engine)