#include "logger.h"

#include "def.h"
#include <gdsl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <stdarg.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

struct _r_logger_output_s
{
    r_log_level_t level;
    int managed;
    FILE *stream;
    pthread_mutex_t mutex;
};

struct _r_logger_s
{
    const char *date_format;
    gdsl_list_t outputs;
};

const char *_r_log_level_str(r_log_level_t level)
{
    switch (level)
    {
    case R_LOG_DEBUG:
        return "DEBUG";
    case R_LOG_INFO:
        return "INFO";
    case R_LOG_WARN:
        return "WARN";
    case R_LOG_ERROR:
        return "ERROR";
    case R_LOG_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

void _r_logger_output_s_destroy(void *ptr)
{
    struct _r_logger_output_s *output_s= (struct _r_logger_output_s *) ptr;

    if (output_s->managed)
    {
        fclose(output_s->stream);
    }

    free(output_s);
}

r_logger_t r_logger_create(void)
{
    struct _r_logger_s *logger_s;

    logger_s = malloc(sizeof(struct _r_logger_s));
    if (!logger_s)
    {
        return NULL;
    }

    memset(logger_s, 0, sizeof(struct _r_logger_s));

    logger_s->outputs = gdsl_list_alloc("_r_logger_s_outputs", NULL, _r_logger_output_s_destroy);

    if (!logger_s->outputs)
    {
        free(logger_s);
        return NULL;
    }

    return (r_logger_t)logger_s;
}

void r_logger_destroy(r_logger_t logger)
{
    struct _r_logger_s *logger_s;

    logger_s = (struct _r_logger_s *) logger;
    gdsl_list_free(logger_s->outputs);
    free(logger_s);
}

int _r_logger_add_output(struct _r_logger_s *logger_s, struct _r_logger_output_s *output_s)
{
    if (!gdsl_list_insert_tail(logger_s->outputs, (void*)output_s))
    {
        return 0;
    }

    if (pthread_mutex_init(&(output_s->mutex), NULL) != 0)
    {
        return 0;
    }

    return 1;
}

int r_logger_add_stream(r_logger_t logger, FILE *stream, r_log_level_t level)
{
    struct _r_logger_s *logger_s;
    struct _r_logger_output_s *output_s;

    logger_s = (struct _r_logger_s *)logger;

    output_s = malloc(sizeof(struct _r_logger_output_s));
    if (!output_s)
    {
        return 0;
    }

    output_s->managed = 0;
    output_s->stream = stream;
    output_s->level = level;

    if (!_r_logger_add_output(logger_s, output_s))
    {
        free(output_s);
        return 0;
    }

    return 1;
}

int r_logger_add_file(r_logger_t logger, const char *filename, r_log_level_t level)
{
    struct _r_logger_s *logger_s;
    struct _r_logger_output_s *output_s;

    logger_s = (struct _r_logger_s *)logger;

    output_s = malloc(sizeof(struct _r_logger_output_s));

    if (!output_s)
    {
        return 0;
    }

    FILE *stream = fopen(filename, "w");

    if (!stream)
    {
        free(output_s);
        return 0;
    }

    output_s->managed = 1;
    output_s->stream = stream;
    output_s->level = level;

    if (!_r_logger_add_output(logger_s, output_s))
    {
        fclose(stream);
        free(output_s);
        return 0;
    }

    return 1;
}

void _r_logger_print_with_line(r_logger_t logger, r_log_level_t level, const char *file, unsigned int line, _r_logger_vfprint_func_t vfprint_func, const void *format, ...)
{
    struct _r_logger_s *logger_s;

    logger_s = (struct _r_logger_s *) logger;
    if (logger_s == NULL || gdsl_list_is_empty(logger_s->outputs))
    {
        return;
    }

    struct _r_logger_output_s *output_s;
    gdsl_list_cursor_t cur;

    cur = gdsl_list_cursor_alloc(logger_s->outputs);

    if (!cur)
    {
        return;
    }

    char *date_format = logger_s->date_format ? logger_s->date_format : "%Y-%m-%d %H:%M:%S";
    time_t now = time(NULL);
    char timestamp[64] = {0};
    strftime(timestamp, 64, date_format, localtime(&now));
    timestamp[63] = '\0';

    va_list argp;
    va_start(argp, format);

    for (gdsl_list_cursor_move_to_head(cur); (output_s = gdsl_list_cursor_get_content(cur)); gdsl_list_cursor_step_forward(cur))
    {
        output_s = (struct _r_logger_output_s *)gdsl_list_cursor_get_content(cur);
        if (level >= output_s->level)
        {
            if (pthread_mutex_lock(&(output_s->mutex)) != 0)
            {
                continue;
            }

            fprintf(output_s->stream, timestamp);
            fprintf(output_s->stream, " [");
            fprintf(output_s->stream, _r_log_level_str(level));
            fprintf(output_s->stream, "] ");
            fprintf(output_s->stream, "(%s:%d) ", file, line);
            vfprint_func(output_s->stream, format, argp);
            fprintf(output_s->stream, "\n");
            fflush(output_s->stream);

            if (pthread_mutex_unlock(&(output_s->mutex)) != 0)
            {
                abort();
            }
        }
    }

    va_end(argp);

    gdsl_list_cursor_free(cur);
}
