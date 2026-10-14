#include "FastFileHeader.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void fixture(const char *path, const char *signature, size_t size) {
  FILE *f=fopen(path,"wb");
  assert(f);
  unsigned char bytes[24]={0};
  memcpy(bytes,signature,8);
  bytes[8]=1;
  assert(fwrite(bytes,1,size,f)==size);
  fclose(f);
}
int main(int argc,char **argv) {
  assert(argc==4);
  MW3FFHeader h;
  fixture(argv[1],"IWffu100",24);
  assert(mw3_ff_header_read(argv[1],&h)==MW3_FF_OK);
  assert(h.variant==MW3_FF_UNSIGNED && h.prefix_u32==1);
  fixture(argv[2],"IWff0100",24);
  assert(mw3_ff_header_read(argv[2],&h)==MW3_FF_OK);
  assert(h.variant==MW3_FF_SIGNED);
  fixture(argv[3],"IWffu100",8);
  assert(mw3_ff_header_read(argv[3],&h)==MW3_FF_TRUNCATED);
  puts("FastFile header checks passed");
  return 0;
}
