#ifndef __RETROPIA__CORE__GAME_INDEX_H__
#define __RETROPIA__CORE__GAME_INDEX_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <sqlite3.h>
    #include "game_info.h"

    typedef void *r_game_index_t;

    r_game_index_t r_game_index_create(const char *db_file, unsigned int n_threads);
    void r_game_index_on_refresh(r_game_index_t gi, void (*callback)(const r_game_info_t, void *), void *ctx);
    void r_game_index_add_search_path(r_game_index_t gi, const char *path);
    void r_game_index_remove_search_path(r_game_index_t gi, const char *path);
    int r_game_index_refresh(r_game_index_t gi);
    void r_game_index_destroy(r_game_index_t gi);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__GAME_INDEX_H__ */
