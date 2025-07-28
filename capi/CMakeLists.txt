add_library(libSGL
        sgl.h
        sgl.cpp
        sgl_error.h
        sgl_compute.h
        sgl_image.h
        sgl_image.cpp
        sgl_buffer.h
        sgl_buffer.cpp
        sgl_buffer.cpp
        sgl_gpu_ctx.h
        sgl_compute.cpp
)
target_link_libraries(libSGL compute_engine)
target_link_libraries(libSGL image_engine)