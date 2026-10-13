#ifndef MW3_HOST_CLOCK_H
#define MW3_HOST_CLOCK_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Portable HOST-side timing primitives for a future AOT engine adapter.
 * These are not direct replacements for Win32 import thunks or IW5 code.
 * All counters are based on host monotonic elapsed time.
 */
int mw3_host_clock_supported(void);
int64_t mw3_host_performance_counter(void);
int64_t mw3_host_performance_frequency(void);
uint32_t mw3_host_tick_count_ms(void);
void mw3_host_sleep_ms(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif
#endif
