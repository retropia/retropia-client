#include "parse-internal.h"
#include <zlib.h>
#include <openssl/md5.h>

void r_parse_common(const char *filename, const uint8_t *data, size_t len, r_game_info_t info)
{
    uint32_t crc;
    crc = crc32(0L, Z_NULL, 0);
    info->crc32 = crc32(crc, data, len);

    MD5_CTX md5;

    MD5_Init(&md5);
    MD5_Update(&md5, data, len);
    MD5_Final(info->md5, &md5);
}
