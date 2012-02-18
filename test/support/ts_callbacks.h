#ifndef __RETROPIA__TEST__SUPPORT__TS_CALLBACKS_H__
#define __RETROPIA__TEST__SUPPORT__TS_CALLBACKS_H__

#ifdef __cplusplus
extern "C"
{
#endif

    #include <stdint.h>

    void *r_ts_unary_callback1(void *arg);
    void *r_ts_unary_callback2(void *arg);
    void *r_ts_unary_callback3(void *arg);
    void *r_ts_unary_callback4(void *arg);
    void *r_ts_binary_callback1(void *arg1, void *arg2);
    void *r_ts_binary_callback2(void *arg1, void *arg2);
    void *r_ts_binary_callback3(void *arg1, void *arg3);
    void *r_ts_binary_callback4(void *arg1, void *arg4);
    void *r_ts_ternary_callback1(void *arg1, void *arg2, void *arg3);
    void *r_ts_ternary_callback2(void *arg1, void *arg2, void *arg3);
    void *r_ts_ternary_callback3(void *arg1, void *arg2, void *arg3);
    void *r_ts_ternary_callback4(void *arg1, void *arg2, void *arg3);
    void *r_ts_file_processor_func1(const char *filename, uint8_t *data, size_t len);
    void *r_ts_file_processor_func2(const char *filename, uint8_t *data, size_t len);
    void *r_ts_file_processor_func3(const char *filename, uint8_t *data, size_t len);
    void *r_ts_file_processor_func4(const char *filename, uint8_t *data, size_t len);
    void *r_ts_file_processor_callback1(const char *filename, void *result, void *ctx);
    void *r_ts_file_processor_callback2(const char *filename, void *result, void *ctx);
    void *r_ts_file_processor_callback3(const char *filename, void *result, void *ctx);
    void *r_ts_file_processor_callback4(const char *filename, void *result, void *ctx);
    int r_ts_fs_walk_callback1(const char *filename, void *ctx);
    int r_ts_fs_walk_callback2(const char *filename, void *ctx);
    int r_ts_fs_walk_callback3(const char *filename, void *ctx);
    int r_ts_fs_walk_callback4(const char *filename, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __RETROPIA__TEST__SUPPORT__TS_CALLBACKS_H__ */
