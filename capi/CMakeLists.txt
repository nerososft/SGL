add_library(libSGL
        include/sgl.h
        sgl.cpp
        include/sgl_error.h
        include/sgl_compute.h
        include/sgl_image.h
        sgl_image.cpp
        include/sgl_buffer.h
        sgl_buffer.cpp
        sgl_buffer.cpp
        include/sgl_gpu_ctx.h
        sgl_compute.cpp
)
target_link_libraries(libSGL compute_engine)
target_link_libraries(libSGL image_engine)