#ifndef __RETROPIA__CORE__COMMON__DEF_H__
#define __RETROPIA__CORE__COMMON__DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #define R_SAFE_FREE(p) { if ((p)) { free((p)); (p) = NULL; } }
    #define R_DE16(buf) ((((buf)[0] & 0xff) << 8) + (((buf)[1] & 0xff)))


#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__COMMON__DEF_H__ */
