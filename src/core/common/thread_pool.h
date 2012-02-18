#ifndef __RETROPIA__CORE__COMMON__THREAD_POOL_H__
#define __RETROPIA__CORE__COMMON__THREAD_POOL_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include "def.h"

    typedef void *r_thread_pool_t;

    r_thread_pool_t r_thread_pool_create(unsigned int n_threads, const char *name);
    void r_thread_pool_destroy(r_thread_pool_t pool);
    int r_thread_pool_dispatch(r_thread_pool_t pool, void (*func)(void *), void *arg, void (*cleanup_func)(void *), void *cleanup_arg);
    void r_thread_pool_synchronize(r_thread_pool_t pool);


#ifdef __cplusplus
}
#endif

#endif /* _RETROPIA__CORE__COMMON__THREAD_POOL_H__ */
