#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

int main()
{
  const int N = 4;
  float a[N] = {1.0f, 2.0f, 3.0f, 4.0f};
  float b[N] = {2.0f, 3.0f, 4.0f, 5.0f};
  float c[N];

  sycl::queue q;
  {
    sycl::buffer<float, 1> a_buf(a, sycl::range<1>(N));
    sycl::buffer<float, 1> b_buf(b, sycl::range<1>(N));
    sycl::buffer<float, 1> c_buf(c, sycl::range<1>(N));

    q.submit([&](sycl::handler &h) {
        auto a_acc = a_buf.get_access<sycl::access::mode::read>(h);
        auto b_acc = b_buf.get_access<sycl::access::mode::read>(h);
        auto c_acc = c_buf.get_access<sycl::access::mode::write>(h);

        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> i) {
            c_acc[i] = a_acc[i] / b_acc[i];
        });
    });
  }

  std::cout << "6.Result: ";
  for(size_t index = 0; index < N; ++index) {
    if (c[index] != a[index] / b[index]) {
      std::cout << "FAILED" << std::endl;
      return 0;
    }
  }
  std::cout << "PASS" << std::endl;
}