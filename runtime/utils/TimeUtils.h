//
// Created by 1234 on 2025/3/6.
//

#ifndef TIMEUTILS_H
#define TIMEUTILS_H
#include <cstdint>

class TimeUtils {
public:
  TimeUtils() = default;

  ~TimeUtils() = default;

  static uint64_t GetCurrentMonoMs();
};

#endif // TIMEUTILS_H
