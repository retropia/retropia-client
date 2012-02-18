#include "unity.h"
#include "logger.h"
#include "ts_setup.h"
#include "stringutil.c"

void setUp(void)
{
    R_TS_SETUP_LOGGER
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER
}

void test_r_stringutil_rtrim_trims_trailing_whitespace(void)
{
    char *str = strdup("hello world    \t    \n");
    r_stringutil_rtrim(str);

    TEST_ASSERT_EQUAL_STRING("hello world", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_lowercase_character(void)
{
    char *str = strdup("h");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("H", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_uppercase_letter(void)
{
    char *str = strdup("H");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("H", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_lowercase_word(void)
{
    char *str = strdup("hello");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_uppercase_word(void)
{
    char *str = strdup("HELLO");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_mixedcase_word(void)
{
    char *str = strdup("heLlO");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_lowercase_sentence(void)
{
    char *str = strdup("hello world, how are you doing today?");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello World, How Are You Doing Today?", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_uppercase_sentence(void)
{
    char *str = strdup("HELLO WORLD, HOW ARE YOU DOING TODAY?");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello World, How Are You Doing Today?", str);

    free(str);
}

void test_r_stringutil_normalize_case_on_mixedcase_sentence_with_tabs_and_newlines(void)
{
    char *str = strdup("HELLo WoRlD, hOW\t\tare YOu \ndOING 2DAY?");

    r_stringutil_normalize_case(str);

    TEST_ASSERT_EQUAL_STRING("Hello World, How\t\tAre You \nDoing 2day?", str);

    free(str);
}
