#ifndef __RETROPIA__TEST__SUPPORT__TS_SETUP_H__
#define __RETROPIA__TEST__SUPPORT__TS_SETUP_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include "logger.h"

    #define R_TS_SETUP_LOGGER { \
      r_logger = r_logger_create(); \
      r_logger_add_file(r_logger, "test.log", R_LOG_DEBUG); \
    }

    #define R_TS_TEARDOWN_LOGGER { \
      if (r_logger) { \
        r_logger_destroy(r_logger); \
        r_logger = NULL; \
      } \
    }

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__TEST__SUPPORT__TS_SETUP_H__ */
