#ifndef __RETROPIA__CORE__COMMON__FILE_PROCESSOR_H__
#define __RETROPIA__CORE__COMMON__FILE_PROCESSOR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef void *r_file_processor_t;

    r_file_processor_t r_file_processor_create(unsigned int n_threads);
    void r_file_processor_destroy(r_file_processor_t processor);

    int r_file_processor_process_file(
        const r_file_processor_t processor,
        const char *filename,
        void *(*callback)(const char *, void *, void *), void *ctx
    );

    int r_file_processor_register_backend(
        r_file_processor_t processor,
        const char *name,
        void *(*process_func)(const char *, const uint8_t *, size_t),
        void *(*cleanup_func)(const char *, const uint8_t *, size_t)
    );

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__COMMON__FILE_PROCESSOR_H__ */
