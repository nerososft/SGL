add_library(libsgl
       sgl.h
        sgl.c
)
target_link_libraries(libsgl compute_engine)