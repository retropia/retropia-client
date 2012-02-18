#ifndef __RETROPIA__CORE__COMMON__FILEMAP_H__
#define __RETROPIA__CORE__COMMON__FILEMAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stddef.h>

    typedef void *r_filemap_t;

    r_filemap_t r_filemap_create(const char *path);
    void r_filemap_destroy(r_filemap_t fmap);
    size_t r_filemap_get_data(r_filemap_t fmap, void **ptr);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__COMMON__FILEMAP_H__ */


