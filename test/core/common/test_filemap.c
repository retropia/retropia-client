#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "filemap.c"

static r_filemap_t fmap = NULL;

void setUp(void)
{
    R_TS_SETUP_LOGGER

    fmap = r_filemap_create("./test/support/testdata/file.txt");
}

void tearDown(void)
{
    if (fmap)
    {
        r_filemap_destroy(fmap);
    }

    R_TS_TEARDOWN_LOGGER
}

void test_r_filemap_create_sanity_check(void)
{
    struct _r_filemap_s *fmap_s;

    fmap_s = (struct _r_filemap_s *) fmap;

    TEST_ASSERT_NOT_NULL(fmap);
    TEST_ASSERT_EQUAL_INT(9, fmap_s->length);
    TEST_ASSERT_EQUAL_MEMORY("a goose!\n", fmap_s->data, 9);
}
