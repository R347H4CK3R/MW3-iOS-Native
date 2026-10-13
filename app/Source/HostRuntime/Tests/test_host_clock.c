#include "HostClock.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

int main(void) {
    assert(mw3_host_clock_supported());
    assert(mw3_host_performance_frequency() == INT64_C(1000000000));
    int64_t before = mw3_host_performance_counter();
    uint32_t tick_before = mw3_host_tick_count_ms();
    assert(before >= 0);
    mw3_host_sleep_ms(3);
    int64_t after = mw3_host_performance_counter();
    uint32_t tick_after = mw3_host_tick_count_ms();
    assert(after >= before);
    /* TickCount intentionally wraps at 2^32 milliseconds, like Win32. */
    assert((uint32_t)(tick_after - tick_before) < UINT32_C(10000));
    assert(after - before >= INT64_C(1000000));
    printf("host clock test passed; elapsed_ns=%" PRId64 "\n", after-before);
    return 0;
}
