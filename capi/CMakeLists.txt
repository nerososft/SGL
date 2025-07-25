add_library(libSGL
        sgl.h
        sgl.cpp
        sdl_error.h
        sgl_compute.h
)
target_link_libraries(libSGL compute_engine)