#include "IWDReader.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    assert(argc == 4);
    MW3IWDStats stats;
    assert(mw3_iwd_probe(argv[1], &stats) == MW3_IWD_OK);
    assert(stats.entry_count == 2u);
    assert(stats.stored_count == 1u);
    assert(stats.deflated_count == 1u);
    assert(stats.unsupported_count == 0u);
    assert(strcmp(stats.sample_name, "alpha.txt") == 0);
    assert(stats.sample_uncompressed_size == sizeof("TEST-IWD-ALPHA\n") - 1u);

    unsigned char out[128] = {0};
    size_t size = 0;
    assert(mw3_iwd_read_entry(argv[1], "alpha.txt", out, sizeof(out), &size)
           == MW3_IWD_OK);
    assert(size == sizeof("TEST-IWD-ALPHA\n")-1u);
    assert(memcmp(out, "TEST-IWD-ALPHA\n", size) == 0);
    assert(mw3_iwd_read_entry(argv[1], "tiny.bin", out, sizeof(out), &size)
           == MW3_IWD_OK);
    assert(size == 4u && out[0] == 0 && out[1] == 1 &&
           out[2] == 2 && out[3] == 3);
    assert(mw3_iwd_read_entry(argv[1], "missing.bin", out, sizeof(out), &size)
           == MW3_IWD_NOT_FOUND);
    assert(mw3_iwd_read_entry(argv[1], "alpha.txt", out, 1, &size)
           == MW3_IWD_RANGE);
    assert(mw3_iwd_read_entry(argv[1], "../outside.txt", out, sizeof(out), &size)
           == MW3_IWD_RANGE);
    assert(mw3_iwd_probe(argv[2], &stats) == MW3_IWD_FORMAT);
    assert(mw3_iwd_probe(argv[3], &stats) == MW3_IWD_UNSUPPORTED);
    puts("Synthetic ZIP/IWD archive reader tests passed");
    return 0;
}
