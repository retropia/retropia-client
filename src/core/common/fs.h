#ifndef __RETROPIA__CORE__COMMON__FS_H__
#define __RETROPIA__CORE__COMMON__FS_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #ifdef WIN32
    #include <windows.h>
    #define R_FS_WATCHER_MAX_HANDLES MAXIMUM_WAIT_OBJECTS
    #else
    #define R_FS_WATCHER_MAX_HANDLES 256
    #endif

    #include "def.h"
    #include <time.h>

    typedef void *r_fs_watcher_t;

    int r_fs_walk_tree(const char *dirname, int (*callback)(const char *, void *), int typeflags, time_t mtime, void *ctx);

    r_fs_watcher_t r_fs_watcher_create(void);
    int r_fs_watcher_add_dir(r_fs_watcher_t watcher, const char *path);
    int r_fs_watcher_notify(r_fs_watcher_t watcher, char *path);
    int r_fs_watcher_notify_async(r_fs_watcher_t watcher, void (*callback)(const char *));
    void r_fs_watcher_destroy(r_fs_watcher_t watcher);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__COMMON__FS_H__ */

