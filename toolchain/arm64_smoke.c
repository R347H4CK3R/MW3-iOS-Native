#include <stdint.h>
#include <stddef.h>
uint32_t mw3_arm64_smoke(uint32_t x) {
    return (x * UINT32_C(3)) + UINT32_C(7);
}
