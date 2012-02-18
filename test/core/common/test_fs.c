#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "fs.c"
#include "mock_ts_callbacks.h"

void *ctx = 0x1;

void setUp(void)
{

}

void tearDown(void)
{

}

void test_r_fs_walk_tree_calls_function_once_for_each_file(void)
{
    int rc;

    r_ts_fs_walk_callback1_ExpectAndReturn("./test/support/testdata/dir/bar.txt", ctx, 1);
    r_ts_fs_walk_callback1_ExpectAndReturn("./test/support/testdata/dir/baz/foo.txt", ctx, 1);
    r_ts_fs_walk_callback1_ExpectAndReturn("./test/support/testdata/dir/foo.txt", ctx, 1);

    rc = r_fs_walk_tree("./test/support/testdata/dir", r_ts_fs_walk_callback1, S_IFREG, 0, ctx);

    TEST_ASSERT_EQUAL_INT(1, rc);
}

void test_r_fs_walk_tree_aborts_when_callback_returns_zero(void)
{
    int rc;

    r_ts_fs_walk_callback1_ExpectAndReturn("./test/support/testdata/dir/bar.txt", ctx, 1);
    r_ts_fs_walk_callback1_ExpectAndReturn("./test/support/testdata/dir/baz/foo.txt", ctx, 0);

    rc = r_fs_walk_tree("./test/support/testdata/dir", r_ts_fs_walk_callback1, S_IFREG, 0, ctx);

    TEST_ASSERT_EQUAL_INT(2, rc);
}

void test_r_fs_walk_tree_ignores_files_older_than_mtime(void)
{
    int rc;

    rc = r_fs_walk_tree("./test/support/testdata/dir", r_ts_fs_walk_callback1, S_IFREG, time(NULL), ctx);

    TEST_ASSERT_EQUAL_INT(1, rc);
}
