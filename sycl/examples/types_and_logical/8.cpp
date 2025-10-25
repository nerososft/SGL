#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

int main()
{
  const int N = 4;
  bool a[N] = {true, false, true, false};
  bool b[N] = {true, true, false, false};
  int c[N];

  sycl::queue q;
  {
    sycl::buffer<bool, 1> a_buf(a, sycl::range<1>(N));
    sycl::buffer<bool, 1> b_buf(b, sycl::range<1>(N));
    sycl::buffer<int, 1> c_buf(c, sycl::range<1>(N));

    q.submit([&](sycl::handler &h) {
        auto a_acc = a_buf.get_access<sycl::access::mode::read>(h);
        auto b_acc = b_buf.get_access<sycl::access::mode::read>(h);
        auto c_acc = c_buf.get_access<sycl::access::mode::write>(h);

        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> i) {
          if (a_acc[i] & b_acc[i]) {
            c_acc[i] = 5;
          } else {
            c_acc[i] = 3;
          }
        });
    });
  }

  std::cout << "8.Result: ";
  for(size_t index = 0; index < N; ++index) {
    if (a[index] & b[index]) {
      if (c[index] != 5) {
        std::cout << "FAILED" << std::endl;
        return 0;
      }
    } else {
      if (c[index] != 3) {
        std::cout << "FAILED" << std::endl;
        return 0;
      }
    }
  }
  std::cout << "PASS" << std::endl;
}