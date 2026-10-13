#include "IWDReader.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: iwd_probe_win64.exe path_to_owned_iwd_archive\n");
        return 2;
    }
    MW3IWDStats stats;
    MW3IWDStatus code = mw3_iwd_probe(argv[1], &stats);
    if (code != MW3_IWD_OK) {
        printf("PROBE_FAILED code=%d\n", (int)code);
        return 1;
    }
    printf("IWD_INDEX_OK entries=%u stored=%u deflated=%u unsupported=%u\n",
           stats.entry_count, stats.stored_count, stats.deflated_count,
           stats.unsupported_count);
    if (stats.sample_name[0]) {
        unsigned char *data = (unsigned char *)malloc(stats.sample_uncompressed_size);
        if (!data) return 3;
        size_t len = 0;
        code = mw3_iwd_read_entry(argv[1], stats.sample_name, data,
                                 stats.sample_uncompressed_size, &len);
        free(data);
        if (code != MW3_IWD_OK || len != stats.sample_uncompressed_size) {
            printf("IWD_SAMPLE_FAILED code=%d\n", (int)code);
            return 1;
        }
        /* Intentionally don't print or persist proprietary entry bytes. */
        printf("IWD_SAMPLE_CRC_OK bytes=%u\n", stats.sample_uncompressed_size);
    } else {
        printf("IWD_NO_SMALL_READABLE_SAMPLE\n");
    }
    return 0;
}
