include_directories("../include/")

set(SYCL_HEADER ../include/hm_sycl/queue.hpp
        ../include/hm_sycl/range.hpp
        ../include/hm_sycl/access.hpp
        ../include/hm_sycl/handler.hpp
        ../include/hm_sycl/accessor.hpp
        ../include/hm_sycl/event.hpp
        ../include/hm_sycl/sycl.hpp
        ../include/hm_sycl/buffer.hpp
        ../include/sycl/sycl.hpp)

set(CASE_1_SOURCE
        basic_arithmetic/1.cpp
        ${SYCL_HEADER}
)
add_executable(case_1
        ${CASE_1_SOURCE}
)

set(CASE_2_SOURCE
        basic_arithmetic/2.cpp
        ${SYCL_HEADER}
)
add_executable(case_2
        ${CASE_2_SOURCE}
)

set(CASE_3_SOURCE
        basic_arithmetic/3.cpp
        ${SYCL_HEADER}
)
add_executable(case_3
        ${CASE_3_SOURCE}
)

set(CASE_4_SOURCE
        basic_arithmetic/4.cpp
        ${SYCL_HEADER}
)
add_executable(case_4
        ${CASE_4_SOURCE}
)

set(CASE_5_SOURCE
        basic_arithmetic/5.cpp
        ${SYCL_HEADER}
)
add_executable(case_5
        ${CASE_5_SOURCE}
)

set(CASE_6_SOURCE
        types_and_logical/6.cpp
        ${SYCL_HEADER}
)
add_executable(case_6
        ${CASE_6_SOURCE}
)

set(CASE_7_SOURCE
        types_and_logical/7.cpp
        ${SYCL_HEADER}
)
add_executable(case_7
        ${CASE_7_SOURCE}
)

set(CASE_8_SOURCE
        types_and_logical/8.cpp
        ${SYCL_HEADER}
)
add_executable(case_8
        ${CASE_8_SOURCE}
)

set(CASE_9_SOURCE
        types_and_logical/9.cpp
        ${SYCL_HEADER}
)
add_executable(case_9
        ${CASE_9_SOURCE}
)

set(CASE_10_SOURCE
        types_and_logical/10.cpp
        ${SYCL_HEADER}
)
add_executable(case_10
        ${CASE_10_SOURCE}
)

set(CASE_11_SOURCE
        arrays_of_different_sizes/11.cpp
        ${SYCL_HEADER}
)
add_executable(case_11
        ${CASE_11_SOURCE}
)

set(CASE_12_SOURCE
        arrays_of_different_sizes/12.cpp
        ${SYCL_HEADER}
)
add_executable(case_12
        ${CASE_12_SOURCE}
)

set(CASE_13_SOURCE
        arrays_of_different_sizes/13.cpp
        ${SYCL_HEADER}
)
add_executable(case_13
        ${CASE_13_SOURCE}
)

set(CASE_14_SOURCE
        arrays_of_different_sizes/14.cpp
        ${SYCL_HEADER}
)
add_executable(case_14
        ${CASE_14_SOURCE}
)

set(CASE_15_SOURCE
        arrays_of_different_sizes/15.cpp
        ${SYCL_HEADER}
)
add_executable(case_15
        ${CASE_15_SOURCE}
)

set(CASE_16_SOURCE
        final/16.cpp
        ${SYCL_HEADER}
)
add_executable(case_16
        ${CASE_16_SOURCE}
)
