#ifndef MW3_FASTFILE_HEADER_H
#define MW3_FASTFILE_HEADER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum MW3FFStatus {
  MW3_FF_OK=0, MW3_FF_IO=1, MW3_FF_TRUNCATED=2,
  MW3_FF_UNKNOWN=3
} MW3FFStatus;
typedef enum MW3FFVariant {
  MW3_FF_UNSIGNED=1, MW3_FF_SIGNED=2
} MW3FFVariant;
typedef struct MW3FFHeader {
  MW3FFVariant variant;
  uint32_t prefix_u32;
  unsigned char first24[24];
} MW3FFHeader;
/* Reads only 24 bytes, writes nothing, and makes no decompression claims. */
MW3FFStatus mw3_ff_header_read(const char *path, MW3FFHeader *out);
#ifdef __cplusplus
}
#endif
#endif
