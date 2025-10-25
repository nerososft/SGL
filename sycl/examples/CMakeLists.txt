include_directories("../include/")

set(CASE_1_SOURCE
        basic_arithmetic/1.cpp
        ../include/hm_sycl/queue.hpp
        ../include/hm_sycl/range.hpp
        ../include/hm_sycl/access.hpp
        ../include/hm_sycl/handler.hpp
        ../include/hm_sycl/accessor.hpp
        ../include/hm_sycl/event.hpp
)
add_executable(case_1
        ${CASE_1_SOURCE}
)