#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>

#ifndef __cplusplus
#define true 1
#define false 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef FUINT64
#define FUINT64 "%llu"
#endif

#define bool32_t uint32_t

#define local_only static

void __attribute__((no_instrument_function))
concat_strings(
    const char * string_1,
    const char * string_2,
    char * output,
    const uint32_t output_size);

void __attribute__((no_instrument_function))
copy_strings(
    char * recipient,
    const uint32_t recipient_size,
    const char * origin);

void __attribute__((no_instrument_function))
copy_strings(
    char * recipient,
    const uint32_t recipient_size,
    const char * origin,
    const uint32_t origin_size);

uint32_t __attribute__((no_instrument_function))
get_string_length(   
    const char * null_terminated_string);

bool32_t are_equal_strings(
    const char * str1,
    const char * str2);

bool32_t are_equal_strings_of_length(
    const char * str1,
    const char * str2,
    const uint64_t length);

void __attribute__((no_instrument_function))
int_to_string(
    const int32_t input,
    char * recipient,
    const uint32_t recipient_size);

void __attribute__((no_instrument_function))
uint_to_string(
    const uint32_t input,
    char * recipient,
    const uint32_t recipient_size);

void __attribute__((no_instrument_function))
float_to_string(
    const float input,
    char * recipient,
    const uint32_t recipient_size);

#endif

