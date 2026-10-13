#include "HostClock.h"
#include <time.h>
#include <errno.h>

static int64_t monotonic_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return -1;
    return (int64_t)ts.tv_sec * INT64_C(1000000000) + (int64_t)ts.tv_nsec;
}

int mw3_host_clock_supported(void) {
    return monotonic_ns() >= 0;
}

int64_t mw3_host_performance_counter(void) {
    return monotonic_ns();
}

int64_t mw3_host_performance_frequency(void) {
    /* Both counter and frequency are expressed using nanoseconds. */
    return INT64_C(1000000000);
}

uint32_t mw3_host_tick_count_ms(void) {
    int64_t ns = monotonic_ns();
    if (ns < 0) return 0;
    return (uint32_t)((uint64_t)ns / UINT64_C(1000000));
}

void mw3_host_sleep_ms(uint32_t milliseconds) {
    struct timespec requested;
    requested.tv_sec = (time_t)(milliseconds / UINT32_C(1000));
    requested.tv_nsec = (long)(milliseconds % UINT32_C(1000)) * 1000000L;
    while (nanosleep(&requested, &requested) == -1 && errno == EINTR) {
        /* Remaining duration is supplied by nanosleep. */
    }
}
