#include "IWDReader.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define MW3_PREVIEW_MAX (1024u * 1024u)
#define MW3_COMP_MAX (16u * 1024u * 1024u)
#define MW3_TAIL_MAX (65535u + 22u)

typedef struct {
    uint32_t offset;
    uint32_t compressed;
    uint32_t plain;
    uint32_t crc;
    uint16_t flags;
    uint16_t method;
    char name[256];
} IWDEntry;

typedef struct {
    FILE *f;
    long file_size;
    long cd_start;
    uint32_t cd_size;
    uint32_t total;
} IWDArchive;

static uint16_t le16(const unsigned char *p) {
    return (uint16_t)((unsigned)p[0] | ((unsigned)p[1] << 8));
}
static uint32_t le32(const unsigned char *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static int get_at(FILE *f, long pos, void *buf, size_t n) {
    return pos >= 0 && fseek(f, pos, SEEK_SET) == 0 &&
           fread(buf, 1, n, f) == n;
}
static int safe_name(const char *name, size_t n) {
    if (n == 0 || n > 255 || name[0] == '/' || name[0] == '\\') return 0;
    size_t begin = 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char ch = (unsigned char)name[i];
        if (ch < 32 || ch > 126 || ch == '\\' || ch == ':' || ch == 0)
            return 0;
        if (ch == '/') {
            if (i - begin == 2 && name[begin] == '.' && name[begin+1] == '.')
                return 0;
            begin = i + 1;
        }
    }
    if (n - begin == 2 && name[begin] == '.' && name[begin+1] == '.')
        return 0;
    return name[n-1] != '/';
}

static MW3IWDStatus archive_open(const char *filename, IWDArchive *a) {
    memset(a, 0, sizeof(*a));
    if (!filename || !(a->f = fopen(filename, "rb"))) return MW3_IWD_IO;
    if (fseek(a->f, 0, SEEK_END) != 0) return MW3_IWD_IO;
    a->file_size = ftell(a->f);
    if (a->file_size < 22) return MW3_IWD_FORMAT;
    size_t tail_size = (size_t)((a->file_size < MW3_TAIL_MAX) ?
                                  a->file_size : MW3_TAIL_MAX);
    unsigned char *tail = (unsigned char *)malloc(tail_size);
    if (!tail) return MW3_IWD_IO;
    if (!get_at(a->f, a->file_size - (long)tail_size, tail, tail_size)) {
        free(tail);
        return MW3_IWD_IO;
    }
    MW3IWDStatus result = MW3_IWD_FORMAT;
    for (size_t pos = tail_size - 22; ; --pos) {
        if (le32(tail+pos) == 0x06054b50u &&
            pos + 22u + le16(tail+pos+20) == tail_size) {
            if (le16(tail+pos+4) != 0 || le16(tail+pos+6) != 0 ||
                le16(tail+pos+8) != le16(tail+pos+10) ||
                le16(tail+pos+10) == 0xffffu ||
                le32(tail+pos+12) == 0xffffffffu ||
                le32(tail+pos+16) == 0xffffffffu) {
                result = MW3_IWD_UNSUPPORTED;
                break;
            }
            a->total = le16(tail+pos+10);
            a->cd_size = le32(tail+pos+12);
            a->cd_start = (long)le32(tail+pos+16);
            long eocd = a->file_size - (long)tail_size + (long)pos;
            if (a->cd_start < 0 ||
                (uint64_t)a->cd_start + a->cd_size > (uint64_t)eocd) {
                result = MW3_IWD_FORMAT;
                break;
            }
            result = MW3_IWD_OK;
            break;
        }
        if (pos == 0) break;
    }
    free(tail);
    return result;
}

static MW3IWDStatus next_entry(IWDArchive *a, long *cursor, IWDEntry *out) {
    unsigned char h[46];
    if (*cursor < a->cd_start ||
        (uint64_t)*cursor + sizeof(h) >
            (uint64_t)a->cd_start + a->cd_size ||
        !get_at(a->f, *cursor, h, sizeof(h)) || le32(h) != 0x02014b50u)
        return MW3_IWD_FORMAT;
    unsigned name_len = le16(h+28), extra = le16(h+30), comment = le16(h+32);
    uint64_t after = (uint64_t)*cursor + 46u + name_len + extra + comment;
    if (after > (uint64_t)a->cd_start + a->cd_size ||
        after > (uint64_t)a->file_size || name_len > 255 ||
        le32(h+20) == 0xffffffffu || le32(h+24) == 0xffffffffu ||
        le32(h+42) == 0xffffffffu || le16(h+34) != 0) return MW3_IWD_UNSUPPORTED;
    char name[256] = {0};
    if (!get_at(a->f, *cursor + 46, name, name_len)) return MW3_IWD_IO;
    if (!safe_name(name, name_len)) return MW3_IWD_UNSUPPORTED;
    memset(out, 0, sizeof(*out));
    memcpy(out->name, name, name_len);
    out->flags = le16(h+8);
    out->method = le16(h+10);
    out->crc = le32(h+16);
    out->compressed = le32(h+20);
    out->plain = le32(h+24);
    out->offset = le32(h+42);
    *cursor = (long)after;
    return MW3_IWD_OK;
}

static MW3IWDStatus locate_entry(
    IWDArchive *a, const char *target, MW3IWDStats *stats, IWDEntry *found
) {
    long cursor = a->cd_start;
    if (stats) memset(stats, 0, sizeof(*stats));
    for (uint32_t i = 0; i < a->total; ++i) {
        IWDEntry item;
        MW3IWDStatus code = next_entry(a, &cursor, &item);
        if (code != MW3_IWD_OK) return code;
        if (stats) {
            ++stats->entry_count;
            if ((item.flags & 1u) || (item.method != 0 && item.method != 8)) {
                ++stats->unsupported_count;
            } else {
                if (item.method == 0) ++stats->stored_count;
                else ++stats->deflated_count;
                if (!stats->sample_name[0] && item.plain > 0 &&
                    item.plain <= MW3_PREVIEW_MAX &&
                    item.compressed <= MW3_COMP_MAX) {
                    memcpy(stats->sample_name, item.name, sizeof(item.name));
                    stats->sample_uncompressed_size = item.plain;
                }
            }
        }
        if (target && strcmp(item.name, target) == 0) {
            *found = item;
            return MW3_IWD_OK;
        }
    }
    return target ? MW3_IWD_NOT_FOUND : MW3_IWD_OK;
}

MW3IWDStatus mw3_iwd_probe(const char *path, MW3IWDStats *stats) {
    if (!stats || !path) return MW3_IWD_RANGE;
    memset(stats, 0, sizeof(*stats));
    IWDArchive a;
    MW3IWDStatus code = archive_open(path, &a);
    if (code == MW3_IWD_OK) code = locate_entry(&a, NULL, stats, NULL);
    if (a.f) fclose(a.f);
    return code;
}

MW3IWDStatus mw3_iwd_read_entry(
    const char *path, const char *name, unsigned char *out, size_t capacity,
    size_t *out_size
) {
    if (!path || !name || !out || !out_size) return MW3_IWD_RANGE;
    *out_size = 0;
    if (!safe_name(name, strlen(name)) || capacity > MW3_PREVIEW_MAX)
        return MW3_IWD_RANGE;
    IWDArchive a;
    MW3IWDStatus code = archive_open(path, &a);
    IWDEntry e;
    if (code == MW3_IWD_OK) code = locate_entry(&a, name, NULL, &e);
    if (code != MW3_IWD_OK) goto done;
    if ((e.flags & 1u) || (e.method != 0 && e.method != 8))
        { code = MW3_IWD_UNSUPPORTED; goto done; }
    if (e.plain > MW3_PREVIEW_MAX || e.compressed > MW3_COMP_MAX ||
        e.plain > capacity || e.plain == 0)
        { code = MW3_IWD_RANGE; goto done; }
    unsigned char hdr[30];
    if (!get_at(a.f, (long)e.offset, hdr, sizeof(hdr)) ||
        le32(hdr) != 0x04034b50u) {
        code = MW3_IWD_FORMAT; goto done;
    }
    uint64_t data_pos = (uint64_t)e.offset + 30 + le16(hdr+26) + le16(hdr+28);
    if (data_pos + e.compressed > (uint64_t)a.file_size) {
        code = MW3_IWD_FORMAT; goto done;
    }
    if ((le16(hdr+6) & 1u) || le16(hdr+8) != e.method) {
        code = MW3_IWD_UNSUPPORTED; goto done;
    }
    /* Reject a local/central directory name mismatch. */
    size_t local_name_length = le16(hdr+26);
    if (local_name_length != strlen(e.name)) {
        code = MW3_IWD_FORMAT; goto done;
    }
    char local_name[256] = {0};
    if (!get_at(a.f, (long)e.offset+30, local_name, local_name_length) ||
        memcmp(local_name, e.name, local_name_length) != 0) {
        code = MW3_IWD_FORMAT; goto done;
    }
    unsigned char *encoded = (unsigned char *)malloc(e.compressed ? e.compressed : 1);
    if (!encoded) { code = MW3_IWD_IO; goto done; }
    if (!get_at(a.f, (long)data_pos, encoded, e.compressed)) {
        code = MW3_IWD_IO;
    } else if (e.method == 0) {
        if (e.compressed != e.plain) code = MW3_IWD_FORMAT;
        else { memcpy(out, encoded, e.plain); *out_size = e.plain; }
    } else {
        z_stream z = {0};
        z.next_in = encoded;
        z.avail_in = e.compressed;
        z.next_out = out;
        z.avail_out = (uInt)capacity;
        if (inflateInit2(&z, -MAX_WBITS) != Z_OK) code = MW3_IWD_FORMAT;
        else {
            int ret = inflate(&z, Z_FINISH);
            if (ret != Z_STREAM_END || z.total_out != e.plain ||
                z.total_in != e.compressed) code = MW3_IWD_FORMAT;
            else *out_size = (size_t)z.total_out;
            inflateEnd(&z);
        }
    }
    free(encoded);
    if (code == MW3_IWD_OK && crc32(0L, out, (uInt)*out_size) != e.crc) {
        *out_size = 0;
        code = MW3_IWD_CHECKSUM;
    }
done:
    if (a.f) fclose(a.f);
    return code;
}
