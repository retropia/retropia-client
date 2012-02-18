#include "fs.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef WIN32
#include <windef.h>
#endif

struct _r_fs_watcher_s
{
    #ifdef WIN32
    HANDLE handles[R_FS_WATCHER_MAX_HANDLES];
    #else
    int handles[R_FS_WATCHER_MAX_HANDLES];
    #endif

    unsigned int n_handles;

    int (*callback)(const char *, void *);
    void *ctx;

};

int r_fs_walk_tree(const char *dirname, int (*callback)(const char *, void *), int typeflags, time_t mtime, void *ctx)
{
    int rc = 1;
    char path[MAX_PATH] = {0};
    DIR *dir;
    struct dirent *entry;
    struct stat status;

    if (!(dir = opendir(dirname)))
    {
        return 0;
    }

    if(!(entry = readdir(dir)))
    {
        closedir(dir);
        return 0;
    }

    do
    {
        snprintf(path, MAX_PATH, "%s/%s", dirname, entry->d_name);

        memset(&status, 0, sizeof(struct stat));
        if (stat(path, &status) == -1)
        {
            rc = 0;
        }

        if (rc != 1)
        {
            break;
        }

        if (S_ISDIR(status.st_mode))
        {
            if (strcmp(".", entry->d_name) != 0 && strcmp("..", entry->d_name) != 0)
            {
                rc = r_fs_walk_tree(path, callback, typeflags, mtime, ctx);
            }
        }
        else if ((typeflags == 0 || (status.st_mode & typeflags)) &&
                 status.st_mtime > mtime)
        {
            if (!callback(path, ctx))
            {
                rc = 2;
            }
        }
    }
    while ((entry = readdir(dir)));

    closedir(dir);

    return rc;
}

r_fs_watcher_t r_fs_watcher_create(void)
{
    return NULL;
}

int r_fs_watcher_add_dir(r_fs_watcher_t watcher, const char *path)
{
    return 1;
}

int r_fs_watcher_notify(r_fs_watcher_t watcher, char *path)
{
    return 1;
}

int r_fs_watcher_notify_async(r_fs_watcher_t watcher, void (*callback)(const char *))
{
    return 1;
}

void r_fs_watcher_destroy(r_fs_watcher_t watcher)
{

}
