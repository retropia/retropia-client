#include "../parse.h"
#include "../parse-internal.h"
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#define R_PARSE_NES_MIN_FSIZE (24 * 1024)

r_game_info_t r_parse_ines(const char *filename, const uint8_t *data, size_t len)
{
    r_game_info_t info;

    if (len < R_PARSE_NES_MIN_FSIZE || strncmp("NES\x1a", data, 4) != 0) return NULL;

    info = r_game_info_create();

    if (info)
    {
        r_parse_common(filename, data, len, info);

        info->platform = strdup("NES");

        if (len % 8192 != 16)
        {
            info->title = malloc(129);
            strncpy(info->title, data + len - 128, 128);
            info->title[128] = '\0';

            r_stringutil_rtrim(info->title);
            r_stringutil_normalize_case(info->title);
        }
        else
        {
            char *title = basename(strdup(filename));
            char *suffix = strrchr(title, '.');
            if (suffix)
            {
                *suffix = '\0';
            }
            info->title = title;
        }


    }

    return info;
}
