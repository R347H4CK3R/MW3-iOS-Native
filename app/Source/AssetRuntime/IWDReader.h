#ifndef MW3_IWD_READER_H
#define MW3_IWD_READER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Read-only subset of ZIP/IWD. ZIP64, multipart and encrypted archives
 * are unsupported. Never writes to the user's game directory. */
typedef enum MW3IWDStatus {
    MW3_IWD_OK = 0,
    MW3_IWD_IO = 1,
    MW3_IWD_FORMAT = 2,
    MW3_IWD_UNSUPPORTED = 3,
    MW3_IWD_RANGE = 4,
    MW3_IWD_NOT_FOUND = 5,
    MW3_IWD_CHECKSUM = 6
} MW3IWDStatus;

typedef struct MW3IWDStats {
    uint32_t entry_count;
    uint32_t stored_count;
    uint32_t deflated_count;
    uint32_t unsupported_count;
    uint32_t sample_uncompressed_size;
    char sample_name[256];
} MW3IWDStats;

/* Reads only ZIP directory metadata, not asset payloads. */
MW3IWDStatus mw3_iwd_probe(const char *archive_path, MW3IWDStats *stats);

/* Finds one named entry and decodes it into CALLER-owned memory only.
 * Up to 1 MiB decompressed and 16 MiB compressed. Performs CRC32 check.
 * No files are extracted, saved, or uploaded. */
MW3IWDStatus mw3_iwd_read_entry(
    const char *archive_path, const char *entry_name,
    unsigned char *out, size_t capacity, size_t *out_size
);

#ifdef __cplusplus
}
#endif
#endif
