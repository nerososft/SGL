//
// Created by 1234 on 2025/3/6.
//

#include "TimeUtils.h"

#define MSEC_PER_SEC (1000)
#define NSEC_PER_MSEC (1000000)

#ifdef OS_OPEN_HARMONY
#include <ctime>
#endif

#ifdef ENABLE_WIN64
#include <windows.h>
#else
#include <time.h>
#endif

#ifdef ENABLE_WIN64
uint64_t TimeUtils::GetCurrentMonoMs() {
  LARGE_INTEGER frequency;
  LARGE_INTEGER counter;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&counter);

  return (counter.QuadPart * MSEC_PER_SEC) / frequency.QuadPart;
}
#else
uint64_t TimeUtils::GetCurrentMonoMs() {
  timespec ts{};
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * MSEC_PER_SEC + ts.tv_nsec / NSEC_PER_MSEC;
}
#endif
