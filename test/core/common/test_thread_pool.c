#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "ts_helpers.h"
#include "thread_pool.c"
#include "mock_ts_callbacks.h"
#include <unistd.h>

void setUp(void)
{
    R_TS_SETUP_LOGGER
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER
}

void test_r_thread_pool_create_sanity_check(void)
{
    r_thread_pool_t pool;
    struct _r_thread_pool_s *pool_s;

    pool = r_thread_pool_create(4, "test_threadpool");
    pool_s = (struct _r_thread_pool_s *)pool;

    TEST_ASSERT_NOT_NULL(pool);

    TEST_ASSERT_EQUAL_INT(4, pool_s->n_threads);
    TEST_ASSERT_EQUAL_INT(4, pool_s->n_live_threads);
    TEST_ASSERT_EQUAL_STRING("test_threadpool", pool_s->name);

    TEST_ASSERT(gdsl_queue_is_empty(pool_s->tasks));

    r_thread_pool_destroy(pool);
}

void test_r_thread_pool_dispatch_queues_a_task(void)
{
    r_thread_pool_t pool;
    struct _r_thread_pool_s *pool_s;

    pool = r_thread_pool_create(4, "test_threadpool");
    pool_s = (struct _r_thread_pool_s *)pool;

    char *arg1 = strdup("foo");
    char *arg2 = strdup("bar");

    r_ts_unary_callback1_ExpectAndReturn((void*)arg1, NULL);
    r_ts_unary_callback2_ExpectAndReturn((void*)arg2, NULL);

    r_thread_pool_dispatch(pool, r_ts_unary_callback1, (void*)arg1, r_ts_unary_callback2, (void*)arg2);
    r_ts_sleep(1);

    r_thread_pool_destroy(pool);
}
