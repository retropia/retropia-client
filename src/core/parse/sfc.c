#include "../parse.h"
#include "../parse-internal.h"
#include "../common/def.h"
#include "../common/stringutil.h"
#include <stdlib.h>
#include <string.h>

#define R_PARSE_SFC_MIN_FSIZE (256 * 1024)

int _r_parse_sfc_is_valid_header(const uint8_t *data, size_t len)
{
    return (len >= 64) && ( R_DE16(data + 0x1c) ^ R_DE16(data + 0x1e) ) == 0xffff;
}

int _r_parse_sfc_contains_lorom_makeup(const uint8_t *data, size_t len)
{
    return ( (data[0x15] & 0xf) % 2 == 0 );
}

int _r_parse_sfc_is_lorom_header(const uint8_t *data, size_t len)
{
    return _r_parse_sfc_is_valid_header(data, len) && _r_parse_sfc_contains_lorom_makeup(data, len);
}

int _r_parse_sfc_is_hirom_header(const uint8_t *data, size_t len)
{
    return _r_parse_sfc_is_valid_header(data, len) && !_r_parse_sfc_contains_lorom_makeup(data, len);
}

const uint8_t *_r_parse_sfc_locate_header(const uint8_t *data, size_t len)
{
    uint8_t *header;
    size_t offset;

    offset = len % 32768 != 0 ? 512 : 0; // account for copier header

    header = data + 0x7fc0 + offset;
    if (_r_parse_sfc_is_valid_header(header, len - (header - data)))
    {
        return header; // LoROM or interleaved HiROM
    }

    header = data + 0xffc0 + offset;
    if (_r_parse_sfc_is_hirom_header(header, len - (header - data)))
    {
        return header; // HiROM
    }

    header = data + (len/ 65536) + offset;
    if (_r_parse_sfc_is_lorom_header(header, len - (header - data)))
    {
        return header; // interleaved LoROM
    }

    return NULL;
}

r_game_info_t r_parse_sfc(const char *filename, const uint8_t *data, size_t len)
{
    if (len < R_PARSE_SFC_MIN_FSIZE)
    {
        return NULL;
    }

    const uint8_t *header = _r_parse_sfc_locate_header(data, len);

    if (!header)
    {
        return NULL;
    }

    r_game_info_t info;

    info = r_game_info_create();

    if (info)
    {
        r_parse_common(filename, data, len, info);

        info->title = malloc(22);
        strncpy(info->title, header, 21);
        info->title[21] = '\0';
        r_stringutil_rtrim(info->title);
        r_stringutil_normalize_case(info->title);

        info->platform = strdup("SNES");
    }

    return info;
}
