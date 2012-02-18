#ifndef __RETROPIA__TEST__SUPPORT__TS_HELPERS_H__
#define __RETROPIA__TEST__SUPPORT__TS_HELPERS_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <pthread.h>

    void r_ts_join_threads(pthread_t threads[], unsigned int n);
    void r_ts_sleep(unsigned int seconds);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__TEST__SUPPORT__TS_HELPERS_H__ */
