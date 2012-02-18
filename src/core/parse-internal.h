#ifndef __RETROPIA__CORE__PARSE_INTERNAL_H__
#define __RETROPIA__CORE__PARSE_INTERNAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stdint.h>
    #include "game_info.h"

    void r_parse_common(const char *filename, const uint8_t *data, size_t len, r_game_info_t info);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__PARSE_INTERNAL_H__ */
