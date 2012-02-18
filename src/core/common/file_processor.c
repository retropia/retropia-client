#include "file_processor.h"
#include "thread_pool.h"
#include "logger.h"
#include "filemap.h"
#include <pthread.h>
#include <gdsl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct _r_file_processor_s
{
    gdsl_list_t backends;
    r_thread_pool_t thread_pool;
};

struct _r_file_processor_backend_s
{
    char *name;
    void *(*process_func)(const char *, const uint8_t *, size_t);
    void *(*cleanup_func)(const char *, const uint8_t *, size_t);
};

struct _r_file_processor_task_s
{
    struct _r_file_processor_s *processor;
    char *filename;
    void *(*callback)(const char *, void *, void *);
    void *ctx;
};

void *_r_file_processor_cleanup_task(void *ptr)
{
    struct _r_file_processor_task_s *task_s;
    task_s = (struct _r_file_processor_task_s *)ptr;

    R_SAFE_FREE(task_s->filename);

    return NULL;
}

void *_r_file_processor_process_task(void *ptr)
{
    r_filemap_t fmap;
    struct _r_file_processor_backend_s *backend_s;
    struct _r_file_processor_task_s *task_s;
    gdsl_list_cursor_t cur = NULL;

    pthread_cleanup_push(_r_file_processor_cleanup_task, ptr);
    task_s = (struct _r_file_processor_task_s *)ptr;

    cur = gdsl_list_cursor_alloc(task_s->processor->backends);
    if (!cur)
    {
        return NULL;
    }
    pthread_cleanup_push(gdsl_list_cursor_free, (void*)cur);

    fmap = r_filemap_create(task_s->filename);
    if (!fmap)
    {
        return NULL;
    }

    pthread_cleanup_push(r_filemap_destroy, (void*)fmap);

    void *data = NULL;
    size_t length;
    void *result = NULL;

    length = r_filemap_get_data(fmap, &data);

    for (gdsl_list_cursor_move_to_head(cur);
            (backend_s = (struct _r_file_processor_backend_s *) gdsl_list_cursor_get_content(cur)) && !result;
            gdsl_list_cursor_step_forward(cur))
    {
        r_logger_print(r_logger, R_LOG_DEBUG, "Processing %s using backend %s", task_s->filename, backend_s->name);

        result = backend_s->process_func(task_s->filename, data, length);
        if (result && task_s->callback)
        {
            task_s->callback(task_s->filename, result, task_s->ctx);
        }

        if (backend_s->cleanup_func)
        {
            backend_s->cleanup_func(task_s->filename, data, length);
        }
    }

    pthread_cleanup_pop(1); // r_filemap_destroy(fmap)
    pthread_cleanup_pop(1); // gdsl_list_cursor_free(cur)
    pthread_cleanup_pop(1); // _r_file_processor_cleanup_task(task_s)
    return NULL;
}

void _r_file_processor_backend_destroy(void *ptr)
{
    struct _r_file_processor_backend_s *backend_s = (struct _r_file_processor_backend_s *) ptr;
    if (backend_s->name)
    {
        free(backend_s->name);
    }

    free(backend_s);
}

r_file_processor_t r_file_processor_create(unsigned int n_threads)
{
    struct _r_file_processor_s *processor_s;

    processor_s = malloc(sizeof(struct _r_file_processor_s));
    if (!processor_s)
    {
        r_logger_print(r_logger, R_LOG_FATAL, strerror(errno));
        return NULL;
    }

    memset(processor_s, 0, sizeof(struct _r_file_processor_s));

    processor_s->backends = gdsl_list_alloc("_r_file_processor_s_backends", NULL, _r_file_processor_backend_destroy);
    if (!processor_s->backends)
    {
        r_logger_print(r_logger, R_LOG_FATAL, strerror(errno));
        free(processor_s);
        return NULL;
    }

    processor_s->thread_pool = r_thread_pool_create(n_threads, "_r_file_processor_s_thread_pool");
    if (!processor_s->thread_pool)
    {
        gdsl_list_free(processor_s->backends);
        free(processor_s);
        return NULL;
    }

    return (r_file_processor_t)processor_s;
}

void r_file_processor_destroy(r_file_processor_t processor)
{
    struct _r_file_processor_s *processor_s = (struct _r_file_processor_s *) processor;

    if (processor_s->backends)
    {
        gdsl_list_free(processor_s->backends);
    }

    free(processor_s);
}

int r_file_processor_process_file(
    const r_file_processor_t processor,
    const char *filename,
    void *(*callback)(const char *, void *, void *), void *ctx)
{
    struct _r_file_processor_s *processor_s;
    struct _r_file_processor_task_s *task_s;

    processor_s = (struct _r_file_processor_s *)processor;

    task_s = malloc(sizeof(struct _r_file_processor_task_s));

    if (!task_s)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        return 0;
    }

    memset(task_s, 0, sizeof(struct _r_file_processor_task_s));
    task_s->filename = strdup(filename);
    task_s->processor = processor_s;
    task_s->callback = callback;
    task_s->ctx = ctx;

    if (!r_thread_pool_dispatch(processor_s->thread_pool, _r_file_processor_process_task, task_s, _r_file_processor_cleanup_task, task_s))
    {
        free(task_s->filename);
        free(task_s);
        return 0;
    }

    return 1;
}

int r_file_processor_register_backend(
        r_file_processor_t processor,
        const char *name,
        void *(*process_func)(const char *, const uint8_t *, size_t),
        void *(*cleanup_func)(const char *, const uint8_t *, size_t))
{
    struct _r_file_processor_s *processor_s;
    struct _r_file_processor_backend_s *backend_s;

    processor_s = (struct _r_file_processor_s *)processor;

    backend_s = malloc(sizeof(struct _r_file_processor_backend_s));
    if (!backend_s)
    {
        r_logger_print(r_logger, R_LOG_FATAL, strerror(errno));
        return 0;
    }

    memset(backend_s, 0, sizeof(struct _r_file_processor_backend_s));
    backend_s->name = strdup(name);
    backend_s->process_func = process_func;
    backend_s->cleanup_func = cleanup_func;

    if (!gdsl_list_insert_tail(processor_s->backends, (void*)backend_s))
    {
        _r_file_processor_backend_destroy((void*)backend_s);
        return 0;
    }

    return 1;
}
