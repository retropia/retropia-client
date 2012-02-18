#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "md.c"
#include "parse.h"
#include "game_info.h"
#include "td_md.h"
#include "stringutil.h"

void setUp(void)
{
    R_TS_SETUP_LOGGER
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER
}

void test_r_parse_md(void)
{
    r_game_info_t info;

    info = r_parse_md("rom_file_name", r_td_spacy_pixy_md, r_td_spacy_pixy_md_len);

    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL_STRING("Spacy Pixy", info->title);
    TEST_ASSERT_EQUAL_STRING("Genesis/Mega Drive", info->platform);

    r_game_info_destroy(info);
}
