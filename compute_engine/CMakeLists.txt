if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "OHOS")
    message(STATUS "Running on OH")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
endif ()

add_library(compute_engine
        ComputeEngine.cpp
        ComputeEngine.h
        operators/IOperator.h
        operators/UnaryOperator.h
        operators/UnaryOperator.cpp
        operators/BinaryOperator.h
        operators/BinaryOperator.cpp
        operators/impl/ReLUOperator.h
        operators/impl/ReLUOperator.cpp
)
target_link_libraries(compute_engine gpu_engine)