#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif
    void __attribute__((no_instrument_function))
    __cyg_profile_func_enter(
        void *this_fn,
        void *call_site);

    void __attribute__((no_instrument_function))
    __cyg_profile_func_exit(
        void *this_fn,
        void *call_site);
#ifdef __cplusplus
}
#endif

// #define LOGGER_SILENCE
#ifndef LOGGER_SILENCE
#include "stdio.h"
#include <dlfcn.h>
#endif

#include <stdlib.h>

#include "assert.h"
#include "common.h"
#include "platform_layer.h"

#define LOG_SIZE 5000000

#define log_append(string) internal_log_append(string, __func__)
#define log_append_float(num) internal_log_append_float(num, __func__)
#define log_append_int(num) internal_log_append_int(num, __func__)
#define log_append_uint(num) internal_log_append_uint(num, __func__)

extern bool32_t application_running;

/*
Allocates memory. This is only necessary in C99
*/
void __attribute__((no_instrument_function))
setup_log();

/*
don't use the internal_ functions, use the macros that call them.
*/
void __attribute__((no_instrument_function))
internal_log_append(
    const char * to_append,
    const char * caller_function_name);

/*
don't use the internal_ functions, use the macros that call them.
*/
void __attribute__((no_instrument_function))
internal_log_append_int(
    const int32_t to_append,
    const char * caller_function_name);

/*
don't use the internal_ functions, use the macros that call them.
*/
void __attribute__((no_instrument_function))
internal_log_append_uint(
    const uint32_t to_append,
    const char * caller_function_name);


/*
don't use the internal_ functions, use the macros that call them.
*/
void __attribute__((no_instrument_function))
internal_log_append_uint(
    const uint32_t to_append,
    const char * caller_function_name);

/*
don't use the internal_ functions, use the macros that call them.
*/
void __attribute__((no_instrument_function))
internal_log_append_float(
    const float to_append,
    const char * caller_function_name);

/*
dump the entire debug log to debuglog.txt
*/
void __attribute__((no_instrument_function))
log_dump();

/*
dump the entire debug log to debuglog.txt,
then crash the application
*/
void __attribute__((no_instrument_function))
log_dump_and_crash();

void __attribute__((no_instrument_function))
register_function_name(
    uint64_t address,
    char * name);

#endif

