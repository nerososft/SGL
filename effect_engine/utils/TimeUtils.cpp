//
// Created by 1234 on 2025/3/6.
//

#include "TimeUtils.h"

#include <_time.h>
#include <sys/_types/_timespec.h>

#define MSEC_PER_SEC (1000)
#define NSEC_PER_MSEC (1000000)

uint64_t TimeUtils::GetCurrentMonoMs() {
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * MSEC_PER_SEC + ts.tv_nsec / NSEC_PER_MSEC;
}
