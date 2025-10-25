#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

int main()
{
  constexpr int N = 1000000;
  const auto a = static_cast<float *>(malloc(N * sizeof(float)));
  const auto b = static_cast<float *>(malloc(N * sizeof(float)));
  const auto c = static_cast<float *>(malloc(N * sizeof(float)));

  for(float index = 0.0f; index < N; ++index) {
    a[static_cast<unsigned int>(index)] = index;
    b[static_cast<unsigned int>(index)] = index + 0.5f;
    c[static_cast<unsigned int>(index)] = index + 1.0f;
  }
  const auto d = static_cast<float *>(malloc(N * sizeof(float)));

  sycl::queue q;
  {
    sycl::buffer<float, 1> a_buf(a, sycl::range<1>(N));
    sycl::buffer<float, 1> b_buf(b, sycl::range<1>(N));
    sycl::buffer<float, 1> c_buf(c, sycl::range<1>(N));
    sycl::buffer<float, 1> d_buf(d, sycl::range<1>(N));

    q.submit([&](sycl::handler &h) {
        auto a_acc = a_buf.get_access<sycl::access::mode::read>(h);
        auto b_acc = b_buf.get_access<sycl::access::mode::read>(h);
        auto c_acc = c_buf.get_access<sycl::access::mode::read>(h);
        auto d_acc = d_buf.get_access<sycl::access::mode::write>(h);

        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> i) {
            d_acc[i] = a_acc[i] + b_acc[i] + c_acc[i];
        });
    });
  }

  std::cout << "15.Result: ";
  for(size_t index = 0; index < N; ++index) {
    if (d[index] != a[index] + b[index] + c[index]) {
      std::cout << "FAILED" << std::endl;
      return 0;
    }
  }
  std::cout << "PASS" << std::endl;
}