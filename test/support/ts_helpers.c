#include "ts_helpers.h"
#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

void r_ts_join_threads(pthread_t threads[], unsigned int n)
{
    unsigned int i;

    for (i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void r_ts_sleep(unsigned int seconds)
{
    #if defined(WIN32)
    Sleep(seconds * 1000);
    #else
    sleep(seconds);
    #endif
}
