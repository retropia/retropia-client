#ifndef __RETROPIA__CORE__PARSE_H__
#define __RETROPIA__CORE__PARSE_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stdint.h>
    #include "game_info.h"

    r_game_info_t r_parse_ines(const char *filename, const uint8_t *data, size_t len);
    r_game_info_t r_parse_sfc(const char *filename, const uint8_t *data, size_t len);
    r_game_info_t r_parse_md(const char *filename, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__PARSE_H__ */
