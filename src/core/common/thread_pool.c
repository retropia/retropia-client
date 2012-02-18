#include "thread_pool.h"
#include "logger.h"
#include <gdsl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

struct _r_thread_pool_thread_s;

struct _r_thread_pool_s
{
    char *name;

    struct _r_thread_pool_thread_s **threads;
    gdsl_queue_t tasks;

    unsigned int n_threads;
    unsigned int n_live_threads;

    pthread_mutex_t mutex;
    pthread_cond_t task_queued;
    pthread_cond_t task_accepted;
};

struct _r_thread_pool_thread_s
{
    unsigned int id;
    pthread_t thread;
    struct _r_thread_pool_s *owner;
};

struct _r_thread_pool_task_s
{

    void (*func)(void *);
    void *arg;

    void (*cleanup_func)(void *);
    void *cleanup_arg;
};

void _r_thread_pool_task_s_destroy(void *ptr)
{

    struct _r_thread_pool_task_s *task_s;

    task_s = (struct _r_thread_pool_task_s *)ptr;

    R_SAFE_FREE(task_s);
}

void *_r_thread_pool_on_thread_exit(void *self)
{
    struct _r_thread_pool_thread_s *thread_s;

    thread_s = (struct _r_thread_pool_thread_s*) self;

    r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d terminated", thread_s->id);
}

void *_r_thread_pool_do_work(void *self)
{
    unsigned int thread_no;

    struct _r_thread_pool_thread_s *thread_s = NULL;
    struct _r_thread_pool_task_s *task_s = NULL;

    thread_s = (struct _r_thread_pool_thread_s *) self;

    pthread_cleanup_push(_r_thread_pool_on_thread_exit, (void*)thread_s);
    pthread_cleanup_push(pthread_mutex_unlock, (void*)&(thread_s->owner->mutex));

    r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d started", thread_s->id);

    for( ; ; )
    {
        if (pthread_mutex_lock(&(thread_s->owner->mutex)) == 0)
        {

            r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d is ready for work", thread_s->id);
            while(gdsl_queue_is_empty(thread_s->owner->tasks))
            {
                pthread_cond_wait(&(thread_s->owner->task_queued), &(thread_s->owner->mutex));
            }

            task_s = gdsl_queue_remove(thread_s->owner->tasks);
            pthread_cleanup_push(_r_thread_pool_task_s_destroy, (void*)task_s);
            pthread_cond_signal(&(thread_s->owner->task_accepted));

            r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d is processing a task", thread_no);

            if (pthread_mutex_unlock(&(thread_s->owner->mutex)) != 0)
            {
                r_logger_print(r_logger, R_LOG_FATAL, "Thread %d failed to unlock mutex, terminating", thread_s->id);
                abort();
            }

            if (task_s->cleanup_func)
            {
                pthread_cleanup_push(task_s->cleanup_func, task_s->cleanup_arg);
                task_s->func(task_s->arg);
                pthread_cleanup_pop(1);
            }
            else
            {
                task_s->func(task_s->arg);
            }

            r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d is done processing", thread_s->id);
            pthread_cleanup_pop(1); // _r_thread_pool_task_s_destroy(task_s)
            r_logger_print(r_logger, R_LOG_DEBUG, "Thread %d has destroyed the task it completed", thread_s->id);
        }
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    return NULL;
}

r_thread_pool_t r_thread_pool_create(unsigned int n_threads, const char *name)
{
    unsigned int i;
    struct _r_thread_pool_s *pool_s;

    pool_s = malloc(sizeof(struct _r_thread_pool_s));
    if (!pool_s)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        return NULL;
    }

    memset(pool_s, 0, sizeof(struct _r_thread_pool_s));

    pool_s->tasks = gdsl_queue_alloc("_r_thread_pool_s_tasks", NULL, _r_thread_pool_task_s_destroy);

    if (!pool_s->tasks)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        r_thread_pool_destroy((r_thread_pool_t *)pool_s);
        return NULL;
    }

    if (name)
    {
        pool_s->name = strdup(name);
    }

    pool_s->threads = malloc(sizeof(struct _r_thread_pool_thread_s*)*n_threads);
    if (!pool_s->threads)
    {
        r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
        r_thread_pool_destroy((r_thread_pool_t *)pool_s);
        return NULL;
    }

    pool_s->n_threads = n_threads;

    for (i = 0; i < pool_s->n_threads; i++)
    {
        pool_s->threads[i] = malloc(sizeof(struct _r_thread_pool_thread_s));
        if(!pool_s->threads[i])
        {
            break;
        }

        memset(pool_s->threads[i], 0, sizeof(struct _r_thread_pool_thread_s));

        pool_s->threads[i]->id = (i+1);
        pool_s->threads[i]->owner = pool_s;

        if (pthread_create(&(pool_s->threads[i]->thread), NULL, _r_thread_pool_do_work, (void*) pool_s->threads[i]) != 0)
        {
            r_logger_print(r_logger, R_LOG_ERROR, strerror(errno));
            break;
        }

        ++(pool_s->n_live_threads);
    }

    if (pool_s->n_live_threads < pool_s->n_threads)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "Thread pool only allocated %d out of %d threads", pool_s->n_live_threads, pool_s->n_threads);
        r_thread_pool_destroy((r_thread_pool_t *)pool_s);
        return NULL;
    }

    pthread_mutex_init(&(pool_s->mutex), NULL);
    pthread_cond_init(&(pool_s->task_queued), NULL);
    pthread_cond_init(&(pool_s->task_accepted), NULL);

    return (r_thread_pool_t *)pool_s;
}

void r_thread_pool_destroy(r_thread_pool_t pool)
{
    struct _r_thread_pool_s *pool_s;

    pool_s = (struct _r_thread_pool_s *)pool;

    r_logger_print(r_logger, R_LOG_DEBUG, "Destroying thread pool %s", pool_s->name);

    if (pool_s->threads)
    {
        unsigned int i;
        for (i = 0; i < pool_s->n_threads; i++)
        {
            if (pthread_cancel(pool_s->threads[i]->thread) != ESRCH)
            {
                r_logger_print(r_logger, R_LOG_DEBUG, "Cancelling thread %d", i+1);
            }
            else
            {
                r_logger_print(r_logger, R_LOG_DEBUG, "Can't cancel thread %d as it is not running", i+1);
            }
        }

        r_thread_pool_synchronize(pool); // block until all threads have terminated

        R_SAFE_FREE(pool_s->threads);
    }

    pthread_cond_destroy(&(pool_s->task_queued));
    pthread_cond_destroy(&(pool_s->task_accepted));
    pthread_mutex_destroy(&(pool_s->mutex));

    if (pool_s->tasks)
    {
        gdsl_queue_free(pool_s->tasks);
    }

    R_SAFE_FREE(pool_s->name);

    free(pool_s);
}

int r_thread_pool_dispatch(r_thread_pool_t pool, void (*func)(void *), void *arg, void (*cleanup_func)(void *), void *cleanup_arg)
{
    struct _r_thread_pool_s *pool_s = NULL;
    struct _r_thread_pool_task_s *task_s = NULL;

    pool_s = (struct _r_thread_pool_s *)pool;

    r_logger_print(r_logger, R_LOG_DEBUG, "Dispatching to %s", pool_s->name);

    pthread_cleanup_push(_r_thread_pool_task_s_destroy, (void*)task_s);
    task_s = malloc(sizeof(struct _r_thread_pool_task_s));
    if (!task_s)
    {
        return 0;
    }

    memset(task_s, 0, sizeof(struct _r_thread_pool_task_s));

    task_s->func = func;
    task_s->arg = arg;
    task_s->cleanup_func = cleanup_func;
    task_s->cleanup_arg = cleanup_arg;

    pthread_cleanup_push(pthread_mutex_unlock, (void*)&(pool_s->mutex));

    if (pthread_mutex_lock(&(pool_s->mutex)) != 0)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "Failed to lock mutex, can't dispatch");
        free(task_s);
        return 0;
    }

    if (!gdsl_queue_insert(pool_s->tasks, (void*)task_s))
    {
        r_logger_print(r_logger, R_LOG_ERROR, "Unable to queue thread pool task (%s)", pool_s->name, strerror(errno));
        free(task_s);
        return 0;
    }

    pthread_cond_signal(&(pool_s->task_queued));
    pthread_cond_wait(&(pool_s->task_accepted), &(pool_s->mutex));

    if (pthread_mutex_unlock(&(pool_s->mutex)) != 0)
    {
        r_logger_print(r_logger, R_LOG_FATAL, "Failed to unlock mutex, aborting");
        abort();
    }

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return 1;
}

void r_thread_pool_synchronize(r_thread_pool_t pool)
{
    unsigned int i;
    struct _r_thread_pool_s *pool_s;

    pool_s = (struct _r_thread_pool_s *)pool;

    for (i = 0; i < pool_s->n_threads; i++)
    {
        r_logger_print(r_logger, R_LOG_DEBUG, "Waiting for thread %d to terminate", pool_s->threads[i]->id);
        pthread_join(pool_s->threads[i]->thread, NULL);
        R_SAFE_FREE(pool_s->threads[i]);
        --(pool_s->n_live_threads);
    }

    r_logger_print(r_logger, R_LOG_DEBUG, "All threads terminated");
}
