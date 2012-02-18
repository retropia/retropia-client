#include "ts_callbacks.h"
#include "logger.h"

void *r_ts_unary_callback1(void *arg)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_unary_callback1(%p)", arg);
    return NULL;
}

void *r_ts_unary_callback2(void *arg)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_unary_callback2(%p)", arg);
    return NULL;
}

void *r_ts_unary_callback3(void *arg)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_unary_callback3(%p)", arg);
    return NULL;
}

void *r_ts_unary_callback4(void *arg)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_unary_callback4(%p)", arg);
    return NULL;
}

void *r_ts_binary_callback1(void *arg1, void *arg2)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_binary_callback1(%p, %p)", arg1, arg2);
    return NULL;
}

void *r_ts_binary_callback2(void *arg1, void *arg2)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_binary_callback2(%p, %p)", arg1, arg2);
    return NULL;
}

void *r_ts_binary_callback3(void *arg1, void *arg2)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_binary_callback3(%p, %p)", arg1, arg2);
    return NULL;
}

void *r_ts_binary_callback4(void *arg1, void *arg2)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_binary_callback4(%p, %p)", arg1, arg2);
    return NULL;
}

void *r_ts_ternary_callback1(void *arg1, void *arg2, void *arg3)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_ternary_callback1(%p, %p, %p)", arg1, arg2, arg3);
    return NULL;
}

void *r_ts_ternary_callback2(void *arg1, void *arg2, void *arg3)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_ternary_callback2(%p, %p, %p)", arg1, arg2, arg3);
    return NULL;
}

void *r_ts_ternary_callback3(void *arg1, void *arg2, void *arg3)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_ternary_callback3(%p, %p, %p)", arg1, arg2, arg3);
    return NULL;
}

void *r_ts_ternary_callback4(void *arg1, void *arg2, void *arg3)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_ternary_callback4(%p, %p, %p)", arg1, arg2, arg3);
    return NULL;
}

void *r_ts_file_processor_func1(const char *filename, uint8_t *data, size_t len)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_file_processor_func1(%s, %p, %lu)", filename, data, len);
    return NULL;
}

void *r_ts_file_processor_func2(const char *filename, uint8_t *data, size_t len)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_file_processor_func2(%s, %p, %lu)", filename, data, len);
    return NULL;
}

void *r_ts_file_processor_func3(const char *filename, uint8_t *data, size_t len)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_file_processor_func3(%s, %p, %lu)", filename, data, len);
    return NULL;
}

void *r_ts_file_processor_func4(const char *filename, uint8_t *data, size_t len)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_file_processor_func4(%s, %p, %lu)", filename, data, len);
    return NULL;
}

void *r_ts_file_processor_callback1(const char *filename, void *result, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_file_processor_callback1(%s, %p, %p)", filename, result, ctx);
    return NULL;
}

void *r_ts_file_processor_callback2(const char *filename, void *result, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_file_processor_callback2(%s, %p, %p)", filename, result, ctx);
    return NULL;
}

void *r_ts_file_processor_callback3(const char *filename, void *result, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_file_processor_callback3(%s, %p, %p)", filename, result, ctx);
    return NULL;
}

void *r_ts_file_processor_callback4(const char *filename, void *result, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_file_processor_callback4(%s, %p, %p)", filename, result, ctx);
    return NULL;
}

int r_ts_fs_walk_callback1(const char *filename, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_fs_walk_callback1(%s, %p)", filename, ctx);
    return 1;
}

int r_ts_fs_walk_callback2(const char *filename, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_fs_walk_callback2(%s, %p)", filename, ctx);
    return 1;
}

int r_ts_fs_walk_callback3(const char *filename, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_fs_walk_callback3(%s, %p)", filename, ctx);
    return 1;
}

int r_ts_fs_walk_callback4(const char *filename, void *ctx)
{
    r_logger_print(r_logger, R_LOG_DEBUG, "INVOCATION: r_ts_fs_walk_callback4(%s, %p)", filename, ctx);
    return 1;
}
