#include "game_info.h"

r_game_info_t r_game_info_create(void)
{
    struct _r_game_info_s *info_s;

    info_s = malloc(sizeof(struct _r_game_info_s));

    if (info_s)
    {
        memset(info_s, 0, sizeof(struct _r_game_info_s));
    }

    return info_s;
}

void r_game_info_destroy(r_game_info_t info)
{
    if (info->title)
    {
        free(info->title);
    }

    free(info);
}
