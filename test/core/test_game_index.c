#include "unity.h"
#include "ts_setup.h"
#include "logger.h"
#include "game_index.c"
#include "game_info.h"
#include "mock_sqlite3.h"
#include "mock_file_processor.h"
#include "mock_parse.h"
#include "mock_fs.h"
#include <string.h>

static struct _r_game_index_s *gi_s = NULL;
static void *db = 0x1, *file_processor = 0x2;
sqlite3_stmt *stmt = 0x3;

int _test_r_game_index_mocks_sqlite3_open_v2(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);

void setUp(void)
{
    R_TS_SETUP_LOGGER

    sqlite3_open_v2_StubWithCallback(_test_r_game_index_mocks_sqlite3_open_v2);
    sqlite3_exec_ExpectAndReturn(db, "CREATE TABLE IF NOT EXISTS games (title TEXT, platform TEXT, crc32 INT, md5 BLOB, dir TEXT, fname TEXT);", NULL, NULL, NULL, SQLITE_OK);
    sqlite3_exec_ExpectAndReturn(db, "CREATE INDEX IF NOT EXISTS crc32_idx ON GAMES (crc32);", NULL, NULL, NULL, SQLITE_OK);
    sqlite3_exec_ExpectAndReturn(db, "CREATE INDEX IF NOT EXISTS md5_idx ON GAMES (md5);", NULL, NULL, NULL, SQLITE_OK);
    sqlite3_exec_ExpectAndReturn(db, "CREATE INDEX IF NOT EXISTS dir_idx ON GAMES (dir);", NULL, NULL, NULL, SQLITE_OK);
    r_file_processor_create_ExpectAndReturn(4, file_processor);
    r_file_processor_register_backend_ExpectAndReturn(file_processor, "ines", r_parse_ines, NULL, 1);
    r_file_processor_register_backend_ExpectAndReturn(file_processor, "sfc", r_parse_sfc, NULL, 1);
    r_file_processor_register_backend_ExpectAndReturn(file_processor, "md", r_parse_md, NULL, 1);

    gi_s = r_game_index_create("db_file.db", 4);
}

void tearDown(void)
{
    R_TS_TEARDOWN_LOGGER

    if (gi_s)
    {
        sqlite3_close_ExpectAndReturn(db, SQLITE_OK);
        r_file_processor_destroy_Expect(file_processor);
        r_game_index_destroy(gi_s);
    }

}

int _test_r_game_index_mocks_sqlite3_open_v2(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs)
{
    TEST_ASSERT_EQUAL("db_file.db", filename);
    TEST_ASSERT_EQUAL_INT(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, flags);

    *ppDb = db;

    return SQLITE_OK;
}

void test_r_game_index_create_sanity_check(void)
{
    TEST_ASSERT_NOT_NULL(gi_s);
    TEST_ASSERT(gdsl_list_is_empty(gi_s->search_paths_to_add));
    TEST_ASSERT(gdsl_list_is_empty(gi_s->search_paths_to_remove));
}

void test_r_game_index_add_search_path(void)
{
    r_game_index_add_search_path(gi_s, "/foo/bar/baz");

    TEST_ASSERT_NOT_NULL(gdsl_list_search(gi_s->search_paths_to_add, strcmp, "/foo/bar/baz"));
}

void test_r_game_index_remove_search_path(void)
{
    r_game_index_remove_search_path(gi_s, "/foo/bar/baz");

    TEST_ASSERT_NOT_NULL(gdsl_list_search(gi_s->search_paths_to_remove, strcmp, "/foo/bar/baz"));
}

void test__r_game_index_add_new_entries_walks_added_search_paths(void)
{
    r_game_index_add_search_path(gi_s, "/foo/bar");
    r_game_index_add_search_path(gi_s, "/foo/baz");

    r_fs_walk_tree_ExpectAndReturn("/foo/bar", _r_game_index_refresh_file, S_IFREG, 0, gi_s, 1);
    r_fs_walk_tree_ExpectAndReturn("/foo/baz", _r_game_index_refresh_file, S_IFREG, 0, gi_s, 1);

    _r_game_index_add_new_entries(gi_s);

    TEST_ASSERT(gdsl_list_is_empty(gi_s->search_paths_to_add));
}

int _test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_prepare_v2(
    sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail)
{
    TEST_ASSERT_EQUAL_PTR(gi_s->db, db);
    TEST_ASSERT_EQUAL_STRING("SELECT DISTINCT dir FROM games GROUP BY dir;", zSql);
    TEST_ASSERT_EQUAL_INT(-1, nByte);

    *ppStmt = stmt;

    return SQLITE_OK;
}

int _test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_step(sqlite3_stmt *stmt, int numCalls)
{
    if (numCalls < 2)
    {
        return SQLITE_ROW;
    }
    else
    {
        return SQLITE_DONE;
    }
}

const unsigned char *_test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_column_text(sqlite3_stmt *stmt, int nCol, int numCalls)
{
    TEST_ASSERT_EQUAL_INT(0, nCol);

    switch (numCalls)
    {
    case 0:
        return "./test/support/testdata/dir";
    case 1:
        return "./test/support/testdata/dir2";
    default:
        TEST_FAIL_MESSAGE("sqlite3_column_text called too many times!");
    }
}

void test__r_game_index_update_existing_entries_walks_existing_search_paths(void)
{
    sqlite3_prepare_v2_StubWithCallback(_test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_prepare_v2);
    sqlite3_step_StubWithCallback(_test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_step);
    sqlite3_column_text_StubWithCallback(_test__r_game_index_update_existing_entries_walks_existing_search_paths_mock_sqlite3_column_text);

    r_fs_walk_tree_ExpectAndReturn("./test/support/testdata/dir", _r_game_index_refresh_file, S_IFREG, 0, gi_s, 1);
    r_fs_walk_tree_ExpectAndReturn("./test/support/testdata/dir2", _r_game_index_refresh_file, S_IFREG, 0, gi_s, 1);

    sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

    _r_game_index_update_existing_entries(gi_s);
}

int _test__r_game_index_remove_stale_entries_purges_database_mock_sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail)
{
    TEST_ASSERT_EQUAL_PTR(gi_s->db, db);
    TEST_ASSERT_EQUAL_STRING("DELETE FROM games WHERE dir = ?;", zSql);
    TEST_ASSERT_EQUAL_INT(-1, nByte);

    *ppStmt = stmt;

    return SQLITE_OK;
}

void test__r_game_index_remove_stale_entries_purges_database(void)
{
    sqlite3_prepare_v2_StubWithCallback(_test__r_game_index_remove_stale_entries_purges_database_mock_sqlite3_prepare_v2);
    sqlite3_bind_text_ExpectAndReturn(stmt, 1, "/foo/bar", -1, SQLITE_STATIC, SQLITE_OK);
    sqlite3_step_ExpectAndReturn(stmt, SQLITE_DONE);
    sqlite3_reset_ExpectAndReturn(stmt, SQLITE_OK);
    sqlite3_clear_bindings_ExpectAndReturn(stmt, SQLITE_OK);
    sqlite3_bind_text_ExpectAndReturn(stmt, 1, "/foo/baz", -1, SQLITE_STATIC, SQLITE_OK);
    sqlite3_step_ExpectAndReturn(stmt, SQLITE_DONE);
    sqlite3_reset_ExpectAndReturn(stmt, SQLITE_OK);
    sqlite3_clear_bindings_ExpectAndReturn(stmt, SQLITE_OK);
    sqlite3_finalize_ExpectAndReturn(stmt, SQLITE_OK);

    r_game_index_remove_search_path(gi_s, "/foo/bar");
    r_game_index_remove_search_path(gi_s, "/foo/baz");
    _r_game_index_remove_stale_entries(gi_s);

    TEST_ASSERT(gdsl_list_is_empty(gi_s->search_paths_to_remove));
}
