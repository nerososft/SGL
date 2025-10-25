#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

int main()
{
  constexpr int N = 100000000;
  const auto a = static_cast<int *>(malloc(N * sizeof(int)));
  const auto b = static_cast<int *>(malloc(N * sizeof(int)));

  for(size_t index = 0; index < N; ++index) {
    a[index] = index;
    b[index] = index + 1;
  }
  const auto c = static_cast<int *>(malloc(N * sizeof(int)));

  sycl::queue q;
  {
    sycl::buffer<int, 1> a_buf(a, sycl::range<1>(N));
    sycl::buffer<int, 1> b_buf(b, sycl::range<1>(N));
    sycl::buffer<int, 1> c_buf(c, sycl::range<1>(N));

    q.submit([&](sycl::handler &h) {
        auto a_acc = a_buf.get_access<sycl::access::mode::read>(h);
        auto b_acc = b_buf.get_access<sycl::access::mode::read>(h);
        auto c_acc = c_buf.get_access<sycl::access::mode::write>(h);

        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> i) {
            c_acc[i] = a_acc[i] + b_acc[i];
        });
    });
  }

  std::cout << "14.Result: ";
  for(size_t index = 0; index < N; ++index) {
    if (c[index] != a[index] + b[index]) {
      std::cout << "FAILED" << std::endl;
      return 0;
    }
  }
  std::cout << "PASS" << std::endl;
}