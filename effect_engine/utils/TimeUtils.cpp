//
// Created by 1234 on 2025/3/6.
//

#include "TimeUtils.h"
#ifdef Q_OS_OPENHARMONY
#include <ctime>
#else

#ifdef ENNABLE_WIN64
#include <windows.h>
#define MSEC_PER_SEC (1000)
#define NSEC_PER_MSEC (1000000)
uint64_t TimeUtils::GetCurrentMonoMs() {
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;

    // ��ȡ��ʱ��Ƶ�ʣ�ÿ��ļ���������
    QueryPerformanceFrequency(&frequency);

    // ��ȡ��ǰ��ʱ������
    QueryPerformanceCounter(&counter);

    // �������ʱ��
    return (counter.QuadPart * MSEC_PER_SEC) / frequency.QuadPart;
}

#else

#include <_time.h>
#include <sys/_types/_timespec.h>
#endif
#define MSEC_PER_SEC (1000)
#define NSEC_PER_MSEC (1000000)

uint64_t TimeUtils::GetCurrentMonoMs() {
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * MSEC_PER_SEC + ts.tv_nsec / NSEC_PER_MSEC;
}
#endif


