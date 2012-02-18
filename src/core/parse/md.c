#include "../parse.h"
#include "../parse-internal.h"
#include "../common/def.h"
#include "../common/stringutil.h"
#include <stdlib.h>
#include <string.h>

#define R_PARSE_MD_HEADER_SIZE 512
#define R_PARSE_MD_BLOCK_SIZE (16 * 1024)

int _r_parse_md_is_valid_header(const uint8_t *data, size_t len)
{
    return
        (len >= R_PARSE_MD_HEADER_SIZE) &&
        (
            strncmp("SEGA MEGA DRIVE ", data, 16) == 0 ||
            strncmp("SEGA GENESIS     ", data, 16) == 0
        );
}

void _r_parse_md_deinterleave(uint8_t *dst, const uint8_t *src, size_t len)
{
    size_t i, middle = len / 2;
    for (i = 0; i < middle; i++)
    {
        dst[i*2 + 1] = src[i];
    }

    for (; i < len; i++)
    {
        dst[(i-middle)*2] = src[i];
    }
}

r_game_info_t r_parse_md(const char *filename, const uint8_t *data, size_t len)
{
    char *ptr = data;

    if (!_r_parse_md_is_valid_header(ptr + 0x100, len - 0x100))
    {
        if (len % R_PARSE_MD_BLOCK_SIZE != 0)
        {
            ptr = malloc(R_PARSE_MD_BLOCK_SIZE);
            if (!ptr)
            {
                return NULL;
            }

            memcpy(ptr, data + R_PARSE_MD_HEADER_SIZE, R_PARSE_MD_BLOCK_SIZE);
            _r_parse_md_deinterleave(ptr, data, R_PARSE_MD_BLOCK_SIZE);
        }
        else
        {
            ptr = malloc(len);
            if (!ptr)
            {
                return NULL;
            }

            memcpy(ptr, data, len);
            _r_parse_md_deinterleave(ptr, data, len);
        }

        if (!_r_parse_md_is_valid_header(ptr + 0x100, len - 0x100))
        {
            free(ptr);
            return NULL;
        }
    }

    r_game_info_t info;

    info = r_game_info_create();

    if (info)
    {
        r_parse_common(filename, ptr, len, info);

        info->title = malloc(49);
        strncpy(info->title, data + 0x120, 48);
        info->title[48] = '\0';
        r_stringutil_rtrim(info->title);
        r_stringutil_normalize_case(info->title);

        info->platform = strdup("Genesis/Mega Drive");
    }

    if (ptr != data)
    {
        free(ptr);
    }

    return info;
}
