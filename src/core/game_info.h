#ifndef __RETROPIA__CORE__GAME_INFO_H__
#define __RETROPIA__CORE__GAME_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stdint.h>

    typedef struct _r_game_info_s
    {
        char *title;
        char *platform;
        uint32_t crc32;
        uint8_t md5[16];
    }
    *r_game_info_t;

    r_game_info_t r_game_info_create(void);
    void r_game_info_destroy(r_game_info_t info);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__GAME_INFO_H__ */
