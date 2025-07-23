add_library(libSGL
        sgl.h
        sgl.cpp
)
target_link_libraries(libSGL compute_engine)