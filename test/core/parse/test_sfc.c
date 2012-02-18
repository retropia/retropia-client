#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "sfc.c"
#include "parse.h"
#include "game_info.h"
#include "td_sfc.h"
#include "stringutil.h"

void setUp(void)
{
    R_TS_SETUP_LOGGER
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER
}

void test_r_parse_sfc_without_header(void)
{
    r_game_info_t info;

    info = r_parse_sfc("rom_file_name.sfc", r_td_nwarp_sfc_without_header, r_td_nwarp_sfc_without_header_len);

    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL_STRING("N-Warp Daisakusen", info->title);
    TEST_ASSERT_EQUAL_STRING("SNES", info->platform);

    r_game_info_destroy(info);
}


void test_r_parse_sfc_with_header(void)
{
    r_game_info_t info;

    info = r_parse_sfc("rom_file_name.sfc", r_td_nwarp_sfc_with_header, r_td_nwarp_sfc_with_header_len);

    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL_STRING("N-Warp Daisakusen", info->title);
    TEST_ASSERT_EQUAL_STRING("SNES", info->platform);

    r_game_info_destroy(info);
}
