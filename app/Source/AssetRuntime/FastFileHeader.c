#include "FastFileHeader.h"
#include <stdio.h>
#include <string.h>
static uint32_t le32(const unsigned char *p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
    ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
MW3FFStatus mw3_ff_header_read(const char *path, MW3FFHeader *out) {
  if (!path || !out) return MW3_FF_IO;
  memset(out, 0, sizeof(*out));
  FILE *f = fopen(path, "rb");
  if (!f) return MW3_FF_IO;
  size_t n = fread(out->first24, 1, sizeof(out->first24), f);
  fclose(f);
  if (n != sizeof(out->first24)) return MW3_FF_TRUNCATED;
  if (memcmp(out->first24, "IWffu100", 8) == 0)
    out->variant = MW3_FF_UNSIGNED;
  else if (memcmp(out->first24, "IWff0100", 8) == 0)
    out->variant = MW3_FF_SIGNED;
  else return MW3_FF_UNKNOWN;
  out->prefix_u32 = le32(out->first24+8);
  return MW3_FF_OK;
}
