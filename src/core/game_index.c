#include "game_index.h"
#include "common/logger.h"
#include "common/file_processor.h"
#include "common/fs.h"
#include "parse.h"
#include <gdsl.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

struct _r_game_index_s
{
    sqlite3 *db;
    gdsl_list_t search_paths_to_add;
    gdsl_list_t search_paths_to_remove;

    time_t last_refresh;
    void (*on_refresh_callback)(const r_game_info_t, void *);
    void *on_refresh_ctx;

    r_file_processor_t file_processor;
};

int _r_game_index_initialize_db(struct _r_game_index_s *gi_s)
{
    if (sqlite3_exec(gi_s->db, "CREATE TABLE IF NOT EXISTS games (title TEXT, platform TEXT, crc32 INT, md5 BLOB, dir TEXT, fname TEXT);", NULL, NULL, NULL) != SQLITE_OK)
    {
        return 0;
    }

    if (sqlite3_exec(gi_s->db, "CREATE INDEX IF NOT EXISTS crc32_idx ON GAMES (crc32);", NULL, NULL, NULL) != SQLITE_OK)
    {
        return 0;
    }

    if (sqlite3_exec(gi_s->db, "CREATE INDEX IF NOT EXISTS md5_idx ON GAMES (md5);", NULL, NULL, NULL) != SQLITE_OK)
    {
        return 0;
    }

    if (sqlite3_exec(gi_s->db, "CREATE INDEX IF NOT EXISTS dir_idx ON GAMES (dir);", NULL, NULL, NULL) != SQLITE_OK)
    {
        return 0;
    }

    return 1;
}

void *_r_game_index_insert_into_db(const char *path, void *result, void *ctx)
{
    int rc;
    struct _r_game_index_s *gi_s;
    r_game_info_t info;
    sqlite3_stmt *stmt = NULL;
    char *dir, *fname;

    info = (r_game_info_t) result;
    gi_s = (struct _r_game_index_s *)ctx;

    dir = dirname(strdup(path));
    fname = basename(strdup(path));

    sqlite3_prepare_v2(gi_s->db, "INSERT OR REPLACE INTO games (title, platform, crc32, md5, dir, fname) VALUES (?, ?, ?, ?, ?, ?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, info->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, info->platform, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, info->crc32);
    sqlite3_bind_blob(stmt, 4, info->md5, 16, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, dir, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, fname, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_OK && rc != SQLITE_DONE)
    {
        r_logger_print(r_logger, R_LOG_ERROR, "%s failed (%s)", sqlite3_sql(stmt), sqlite3_errmsg(gi_s->db));
    }
    else
    {
        if (gi_s->on_refresh_callback)
        {
            gi_s->on_refresh_callback(info, gi_s->on_refresh_ctx);
        }
    }

    sqlite3_finalize(stmt);

    free(dir);
    free(fname);

    r_game_info_destroy(info);

    return NULL;
}

int _r_game_index_refresh_file(const char *path, void *ctx)
{
    struct _r_game_index_s *gi_s;

    gi_s = (struct _r_game_index_s *) ctx;
    r_file_processor_process_file(gi_s->file_processor, path, _r_game_index_insert_into_db, ctx);

    return 1;
}

int _r_game_index_refresh_dir(struct _r_game_index_s *gi_s, const char *dir)
{
    return r_fs_walk_tree(dir, _r_game_index_refresh_file, S_IFREG, gi_s->last_refresh, (void*)gi_s) > 0;
}

int _r_game_index_remove_stale_entries(struct _r_game_index_s *gi_s)
{
    gdsl_list_cursor_t cur;
    sqlite3_stmt *stmt = NULL;
    char *dir;
    int rc;

    rc = sqlite3_prepare_v2(gi_s->db, "DELETE FROM games WHERE dir = ?;", -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        cur = gdsl_list_cursor_alloc(gi_s->search_paths_to_remove);
        for (gdsl_list_cursor_move_to_head(cur); (dir = gdsl_list_cursor_remove(cur)); )
        {
            if ((rc = sqlite3_bind_text(stmt, 1, dir, -1, SQLITE_STATIC)) != SQLITE_OK)
            {
                free(dir);
                break;
            }

            rc = sqlite3_step(stmt);

            if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW)
            {
                free(dir);
                break;
            }

            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);

            free(dir);
        }
    }

    sqlite3_finalize(stmt);

    return (rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW);
}

int _r_game_index_update_existing_entries(struct _r_game_index_s *gi_s)
{
    int rc;
    sqlite3_stmt *stmt = NULL;
    struct stat st;
    char *dir = NULL;

    rc = sqlite3_prepare_v2(gi_s->db, "SELECT DISTINCT dir FROM games GROUP BY dir;", -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            memset(&st, 0, sizeof(struct stat));

            dir = sqlite3_column_text(stmt, 0);
            rc = stat(dir, &st);

            if (rc != -1 && S_ISDIR(st.st_mode))
            {
                _r_game_index_refresh_dir(gi_s, dir);
            }
            else
            {
                r_logger_print(r_logger, R_LOG_ERROR, "%s is no longer accessible", dir);
            }
        }
    }

    if (rc == SQLITE_DONE || rc == SQLITE_OK)
    {
        rc = 1;
    }
    else
    {
        r_logger_print(r_logger, R_LOG_ERROR, sqlite3_errmsg(gi_s->db));
        rc = 0;
    }

    sqlite3_finalize(stmt);

    return rc;
}

int _r_game_index_add_new_entries(struct _r_game_index_s *gi_s)
{
    int rc = 1;
    char *dir;
    gdsl_list_cursor_t cur;
    cur = gdsl_list_cursor_alloc(gi_s->search_paths_to_add);

    for (gdsl_list_cursor_move_to_head(cur); (dir = (char*) gdsl_list_cursor_remove(cur)); )
    {
        if (!_r_game_index_refresh_dir(gi_s, dir))
        {
            r_logger_print(r_logger, R_LOG_ERROR, "Failed to process %s", dir);
            free(dir);
            rc = 0;
            break;
        }

        free(dir);
    }

    gdsl_list_cursor_free(cur);

    return rc;
}

r_game_index_t r_game_index_create(const char *db_file, unsigned int n_threads)
{
    struct _r_game_index_s *gi_s;
    gi_s = malloc(sizeof(struct _r_game_index_s));

    if (!gi_s)
    {
        r_logger_print(r_logger, R_LOG_FATAL, strerror(errno));
        return NULL;
    }

    memset(gi_s, 0, sizeof(r_game_index_t));

    gi_s->search_paths_to_add = gdsl_list_alloc("r_game_index_t_search_paths_to_add", NULL, NULL);
    if (!gi_s->search_paths_to_add)
    {
        r_game_index_destroy(gi_s);
        return NULL;
    }

    gi_s->search_paths_to_remove = gdsl_list_alloc("r_game_index_t_search_paths_to_remove", NULL, NULL);
    if (!gi_s->search_paths_to_remove)
    {
        r_game_index_destroy(gi_s);
        return NULL;
    }

    if (sqlite3_open_v2(db_file, &(gi_s->db), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL) != SQLITE_OK)
    {
        r_game_index_destroy(gi_s);
        r_logger_print(r_logger, R_LOG_FATAL, strerror(errno));
        return NULL;
    }

    if (!_r_game_index_initialize_db(gi_s))
    {
        r_game_index_destroy(gi_s);
        return NULL;
    }

    gi_s->file_processor = r_file_processor_create(n_threads);
    if (!gi_s->file_processor)
    {
        r_game_index_destroy(gi_s);
        return NULL;
    }

    gi_s->last_refresh = 0;

    r_file_processor_register_backend(gi_s->file_processor, "ines", r_parse_ines, NULL);
    r_file_processor_register_backend(gi_s->file_processor, "sfc", r_parse_sfc, NULL);
    r_file_processor_register_backend(gi_s->file_processor, "md", r_parse_md, NULL);

    return (r_game_index_t)gi_s;
}

void r_game_index_on_refresh(r_game_index_t gi, void (*callback)(const r_game_info_t, void *), void *ctx)
{
    struct _r_game_index_s *gi_s;

    gi_s = (struct _r_game_index_s *) gi;

    gi_s->on_refresh_callback = callback;
    gi_s->on_refresh_ctx = ctx;
}

void r_game_index_add_search_path(r_game_index_t gi, const char *path)
{
    struct _r_game_index_s *gi_s;
    gi_s = (struct _r_game_index_s *)gi;

    gdsl_list_insert_tail(gi_s->search_paths_to_add, (void*)strdup(path));
}

void r_game_index_remove_search_path(r_game_index_t gi, const char *path)
{
    struct _r_game_index_s *gi_s;
    gi_s = (struct _r_game_index_s *)gi;

    gdsl_list_insert_tail(gi_s->search_paths_to_remove, (void*)strdup(path));
}

int r_game_index_refresh(r_game_index_t gi)
{
    int rc;
    struct _r_game_index_s *gi_s;
    gi_s = (struct _r_game_index_s *)gi;

    if (sqlite3_exec(gi_s->db, "BEGIN TRANSACTION;", NULL, NULL, NULL) != SQLITE_OK)
    {
        r_logger_print(r_logger, R_LOG_ERROR, sqlite3_errmsg(gi_s->db));
        return 0;
    }

    if (!_r_game_index_remove_stale_entries(gi_s))
    {
        r_logger_print(r_logger, R_LOG_ERROR, sqlite3_errmsg(gi_s->db));
        sqlite3_exec(gi_s->db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
        return 0;
    }

    if (!_r_game_index_update_existing_entries(gi_s))
    {
        r_logger_print(r_logger, R_LOG_ERROR, sqlite3_errmsg(gi_s->db));
        sqlite3_exec(gi_s->db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
        return 0;
    }

    if (!_r_game_index_add_new_entries(gi_s))
    {
        r_logger_print(r_logger, R_LOG_ERROR, sqlite3_errmsg(gi_s->db));
        sqlite3_exec(gi_s->db, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);
        return 0;
    }

    gi_s->last_refresh = time(NULL);

    rc = (sqlite3_exec(gi_s->db, "COMMIT TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK);

    if (rc)
    {
        gi_s->last_refresh = time(NULL);
    }

    return rc;
}

time_t r_game_index_last_refresh(const r_game_index_t gi)
{
    struct _r_game_index_s *gi_s;

    gi_s = (struct _r_game_index_s *) gi;

    return gi_s->last_refresh;
}

void r_game_index_destroy(r_game_index_t gi)
{
    struct _r_game_index_s *gi_s;
    gi_s = (struct _r_game_index_s *)gi;

    if (gi_s->search_paths_to_add)
    {
        gdsl_list_free(gi_s->search_paths_to_add);
    }

    if (gi_s->search_paths_to_remove)
    {
        gdsl_list_free(gi_s->search_paths_to_remove);
    }

    if (gi_s->db)
    {
        sqlite3_close(gi_s->db);
    }

    if (gi_s->file_processor)
    {
        r_file_processor_destroy(gi_s->file_processor);
    }
}

