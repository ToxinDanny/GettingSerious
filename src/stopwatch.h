#include <Windows.h>
#include <stdint.h>
#pragma intrinsic(__rdtsc)

#define measure(clock_cycles, time, func, ...)                                 \
  {                                                                            \
    LARGE_INTEGER freq, start_time, end_time;                                  \
                                                                               \
    QueryPerformanceFrequency(&freq);                                          \
                                                                               \
    QueryPerformanceCounter(&start_time);                                      \
    func(__VA_ARGS__);                                                         \
    QueryPerformanceCounter(&end_time);                                        \
                                                                               \
    uint64_t start_rdtsc = __rdtsc();                                          \
    func(__VA_ARGS__);                                                         \
    uint64_t end_rdtsc = __rdtsc();                                            \
                                                                               \
    *((uint64_t *)clock_cycles) = end_rdtsc - start_rdtsc;                     \
    *((uint64_t *)time) =                                                      \
        ((end_time.QuadPart - start_time.QuadPart) * 1000) / freq.QuadPart;    \
  }\
