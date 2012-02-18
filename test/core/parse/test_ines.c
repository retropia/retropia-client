#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "ines.c"
#include "parse.h"
#include "game_info.h"
#include "td_ines.h"
#include "stringutil.h"

void setUp(void)
{
    R_TS_SETUP_LOGGER
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER
}

void test_r_parse_ines_without_title(void)
{
    r_game_info_t info;

    info = r_parse_ines("rom_file_name.nes", r_td_thwaite_nes_without_title, r_td_thwaite_nes_without_title_len);

    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL_STRING("rom_file_name", info->title);
    TEST_ASSERT_EQUAL_STRING("NES", info->platform);

    r_game_info_destroy(info);
}

void test_r_parse_ines_with_title(void)
{
    r_game_info_t info;

    info = r_parse_ines("rom_file_name.nes", r_td_thwaite_nes_with_title, r_td_thwaite_nes_with_title_len);

    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL_STRING("Thwaite", info->title);
    TEST_ASSERT_EQUAL_STRING("NES", info->platform);

    r_game_info_destroy(info);
    r_game_info_destroy(info);
}
