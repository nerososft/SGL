#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

#include <time.h>
#define MSEC_PER_SEC (1000)
#define NSEC_PER_MSEC (1000000)

uint64_t GetCurrentMonoMs() {
  timespec ts{};
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * MSEC_PER_SEC + ts.tv_nsec / NSEC_PER_MSEC;
}

int main() {
  constexpr int M = 1000;
  constexpr int N = 1000;
  const auto matA = static_cast<float *>(malloc(N * M * sizeof(float)));
  const auto matB = static_cast<float *>(malloc(N * M * sizeof(float)));

  for (float index = 0.0f; index < N * M; ++index) {
    matA[static_cast<unsigned int>(index)] = index;
    matB[static_cast<unsigned int>(index)] = index + 0.5f;
  }
  const auto matC = static_cast<float *>(malloc(N * M * sizeof(float)));

  const uint64_t startMs = GetCurrentMonoMs();
  sycl::queue q;
  {
    sycl::buffer<float, 2> a_buf(matA, sycl::range<2>(M, N));
    sycl::buffer<float, 2> b_buf(matB, sycl::range<2>(M, N));
    sycl::buffer<float, 2> c_buf(matC, sycl::range<2>(M, N));
    q.submit([&](sycl::handler &h) {
      auto a_acc = a_buf.get_access<sycl::access::mode::read>(h);
      auto b_acc = b_buf.get_access<sycl::access::mode::read>(h);
      auto c_acc = c_buf.get_access<sycl::access::mode::write>(h);

      h.parallel_for(sycl::range<2>(M, N), [=](sycl::id<2> i) {
        int row = i[0];
        int col = i[1];
        float sum = 0.0f;
        for (int k = 0; k < M; ++k) {
          sum += a_acc[row][k] * b_acc[k][col];
        }
        c_acc[row][col] = sum;
      });
    });
  }
  uint64_t endMs = GetCurrentMonoMs();

  std::cout << "16.Result: ";
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      float expected = 0.0f;
      for (int k = 0; k < M; ++k) {
        expected += matA[i * M + k] * matB[k * N + j];
      }
      if (expected != matC[i * N + j]) {
        std::cout << "FAILED" << std::endl;
        return 0;
      }
    }
  }
  std::cout << "PASS, Time: " << endMs - startMs << "ms" << std::endl;
}