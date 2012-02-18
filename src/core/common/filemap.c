#include "filemap.h"
#include "logger.h"

#if defined(WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif

struct _r_filemap_s
{
    void *data;
    size_t length;

    #if defined(WIN32)
    HANDLE hFile;
    HANDLE hMapping;

    #else
    int fd;
    #endif
};

#if defined(WIN32)
int _r_filemap_mmap(const char *path, struct _r_filemap_s *fmap_s)
{
    void *p;
    HANDLE hFile, hMapping;
    DWORD length;

    hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "error opening file (%d)", GetLastError());
        return 0;
    }

    if ((length = GetFileSize(hFile, NULL)) == 0)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "cannot map a zero-byte file");
        CloseHandle(hFile);
        return 0;
    }

    hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!hMapping)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "unable to create file mapping (%d)", GetLastError());
        CloseHandle(hFile);
        return 0;
    }

    p = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!p)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "unable to get map view (%d)", GetLastError());
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 0;
    }

    fmap_s->data = p;
    fmap_s->length = (size_t) length;
    fmap_s->hFile = hFile;
    fmap_s->hMapping = hMapping;

    return 1;
}

void _r_filemap_munmap(struct _r_filemap_s *fmap_s)
{
    UnmapViewOfFile(fmap_s->data);
    CloseHandle(fmap_s->hMapping);
    CloseHandle(fmap_s->hFile);
}

#else
int _r_filemap_mmap(const char *path, struct _r_filemap_s *fmap_s)
{
    int fd;
    struct stat st;

    fd = open(path, O_RDONLY);

    if (fd < 0)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        return 0;
    }

    if (fstat(fd, &st) < 0)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        close(fd);
        return 0;
    }

    if (st.st_size == 0)
    {
        close(fd);
        return 0;
    }

    p = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (p == MAP_FAILED)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        close(fd);
        return 0;
    }

    fmap_s->data = p;
    fmap_s->length = st.st_size;
    fmap_s->fd = fd;

    return 1;
}

void _r_filemap_munmap(struct _r_filemap_s *fmap_s)
{
    munmap(fmap_s->data, fmap_s->length);
    close(fmap_s->fd);
}
#endif

r_filemap_t r_filemap_create(const char *path)
{
    int rc;
    struct _r_filemap_s *fmap_s;

    fmap_s = malloc(sizeof(struct _r_filemap_s));
    if (!fmap_s)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        return NULL;
    }

    memset(fmap_s, 0, sizeof(struct _r_filemap_s));

    rc = _r_filemap_mmap(path, fmap_s);

    if (!rc)
    {
        free(fmap_s);
        return NULL;
    }

    return (r_filemap_t)fmap_s;
}

void r_filemap_destroy(r_filemap_t fmap)
{
    struct _r_filemap_s *fmap_s;
    fmap_s = (struct _r_filemap_s *) fmap;

    _r_filemap_munmap(fmap_s);

    free(fmap_s);
}

size_t r_filemap_get_data(r_filemap_t fmap, void **ptr)
{
    struct _r_filemap_s *fmap_s;
    fmap_s = (struct _r_filemap_s *)fmap;

    *ptr = fmap_s->data;

    return fmap_s->length;
}
