#ifndef __RETROPIA__CORE__COMMON__LOGGER_H__
#define __RETROPIA__CORE__COMMON__LOGGER_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stdio.h>
    #include <wchar.h>

    typedef enum _r_log_level_e
    {
        R_LOG_DEBUG,
        R_LOG_INFO,
        R_LOG_WARN,
        R_LOG_ERROR,
        R_LOG_FATAL
    }
    r_log_level_t;

    typedef int (*_r_logger_vfprint_func_t)(FILE *, const void *, va_list);
    typedef void *r_logger_t;

    r_logger_t r_logger_create(void);
    void r_logger_destroy(r_logger_t logger);
    int r_logger_add_stream(r_logger_t logger, FILE *stream, r_log_level_t level);
    int r_logger_add_file(r_logger_t logger, const char *filename, r_log_level_t level);
    void _r_logger_print_with_line(r_logger_t logger, r_log_level_t level, const char *file, unsigned int line, _r_logger_vfprint_func_t vfprint_func, const void *format, ...);

    #define r_logger_print(logger, level, format, ...) _r_logger_print_with_line(logger, level, __FILE__, __LINE__, (_r_logger_vfprint_func_t)vfprintf, (void *)format, ##__VA_ARGS__)
    #define r_logger_wprint(logger, level, format, ...) _r_logger_print_with_line(logger, level, __FILE__, __LINE__, (_r_logger_vfprint_func_t)vfwprintf, (void *)format, ##__VA_ARGS__)

    extern r_logger_t r_logger;

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__CORE__COMMON__LOGGER_H__ */
