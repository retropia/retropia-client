#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "ts_helpers.h"
#include "file_processor.c"
#include "mock_ts_callbacks.h"
#include "mock_filemap.h"
#include "mock_thread_pool.h"

static struct _r_file_processor_s *processor_s = NULL;

static void *thread_pool = 0x1, *fmap = 0x2, *ctx = 0x3, *result = 0x4;
static const char *data = "hello";

void setUp(void)
{
    R_TS_SETUP_LOGGER

    r_thread_pool_create_ExpectAndReturn(4, "_r_file_processor_s_thread_pool", thread_pool);

    processor_s = (struct _r_file_processor_s *)r_file_processor_create(4);
}

void tearDown(void)
{
    if (processor_s)
    {
        r_file_processor_destroy(processor_s);
    }

    R_TS_TEARDOWN_LOGGER
}

void test_r_file_processor_create_sanity_check(void)
{

    TEST_ASSERT_NOT_NULL(processor_s);

    TEST_ASSERT_EQUAL_PTR(thread_pool, processor_s->thread_pool);
    TEST_ASSERT(gdsl_list_is_empty(processor_s->backends));
}

void test_r_file_processor_register_backend_adds_a_backend(void)
{
    struct _r_file_processor_backend_s *backend_s;

    r_file_processor_register_backend(processor_s, "backend", r_ts_file_processor_func1, r_ts_file_processor_func2);

    TEST_ASSERT_EQUAL_INT(1, gdsl_list_get_size(processor_s->backends));

    backend_s = (struct _r_file_processor_backend_s *) gdsl_list_get_head(processor_s->backends);

    TEST_ASSERT_EQUAL_STRING("backend", backend_s->name);
    TEST_ASSERT_EQUAL_PTR(r_ts_file_processor_func1, backend_s->process_func);
    TEST_ASSERT_EQUAL_PTR(r_ts_file_processor_func2, backend_s->cleanup_func);
}

size_t _test_r_file_processor_filemap_get_data_stub(r_filemap_t fmap, void **ptr)
{
    *ptr = data;

    return 5;
}

int _test_r_file_processor_thread_pool_dispatch_stub(r_thread_pool_t pool, void (*func)(void *), void *arg, void (*cleanup_func)(void *), void *cleanup_arg)
{
    func(arg);
    cleanup_func(cleanup_arg);

    return 1;
}

void test_r_file_processor_tries_all_process_and_cleanup_functions_if_none_succeed(void)
{
    r_thread_pool_dispatch_StubWithCallback(_test_r_file_processor_thread_pool_dispatch_stub);
    r_filemap_create_ExpectAndReturn("foo.txt", fmap);
    r_filemap_get_data_StubWithCallback(_test_r_file_processor_filemap_get_data_stub);

    r_ts_file_processor_func1_ExpectAndReturn("foo.txt", data, 5, NULL);
    r_ts_file_processor_func2_ExpectAndReturn("foo.txt", data, 5, NULL);
    r_ts_file_processor_func3_ExpectAndReturn("foo.txt", data, 5, NULL);
    r_ts_file_processor_func4_ExpectAndReturn("foo.txt", data, 5, NULL);

    r_filemap_destroy_Expect(fmap);

    r_file_processor_register_backend(processor_s, "backend1", r_ts_file_processor_func1, r_ts_file_processor_func2);
    r_file_processor_register_backend(processor_s, "backend2", r_ts_file_processor_func3, r_ts_file_processor_func4);

    r_file_processor_process_file(processor_s, "foo.txt", r_ts_file_processor_callback1, NULL);
}

void test_r_file_processor_stops_after_first_nonnull_process_result_and_invokes_callback(void)
{
    r_thread_pool_dispatch_StubWithCallback(_test_r_file_processor_thread_pool_dispatch_stub);
    r_filemap_create_ExpectAndReturn("foo.txt", fmap);
    r_filemap_get_data_StubWithCallback(_test_r_file_processor_filemap_get_data_stub);

    r_ts_file_processor_func1_ExpectAndReturn("foo.txt", data, 5, result);
    r_ts_file_processor_callback1_ExpectAndReturn("foo.txt", result, ctx, NULL);
    r_ts_file_processor_func2_ExpectAndReturn("foo.txt", data, 5, NULL);

    r_filemap_destroy_Expect(fmap);

    r_file_processor_register_backend(processor_s, "backend1", r_ts_file_processor_func1, r_ts_file_processor_func2);
    r_file_processor_register_backend(processor_s, "backend2", r_ts_file_processor_func3, r_ts_file_processor_func4);
    r_file_processor_process_file(processor_s, "foo.txt", r_ts_file_processor_callback1, ctx);
}

