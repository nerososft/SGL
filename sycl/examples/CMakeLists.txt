include_directories("../include/")

set(SYCL_HEADER ../include/hm_sycl/queue.hpp
        ../include/hm_sycl/range.hpp
        ../include/hm_sycl/access.hpp
        ../include/hm_sycl/handler.hpp
        ../include/hm_sycl/accessor.hpp
        ../include/hm_sycl/event.hpp)

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


set(CASE_16_SOURCE
        final/16.cpp
        ${SYCL_HEADER}
)
add_executable(case_16
        ${CASE_16_SOURCE}
)
