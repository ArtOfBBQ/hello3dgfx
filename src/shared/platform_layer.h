/*
These functions are not defined in the 'shared' folder
they must be provided by the platform layer in a
platform_layer.c file

For example, 'platform_read_file' is currently defined
in /shared_windows_macos/platform_layer.c for windows
and mac os X, but it's defined elsewhere for iOS
*/

#ifndef PLATFORM_LAYER_H
#define PLATFORM_LAYER_H

#ifdef PLATFORM_NS_FILEMANAGER
#import <Foundation/Foundation.h>
extern NSFileManager * file_manager;
#endif

#ifdef SHARED_APPLE_PLATFORM
#include <mach/mach_time.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "clientlogic.h"
#include "debigulator/src/decodedimage.h"

typedef struct FileBuffer {
    uint64_t size;
    char * contents;
} FileBuffer;

// get current working directory
char * platform_get_application_path();
char * platform_get_cwd();

// Read a file (without path, only filename)
// and return its contents as a buffer of bytes
FileBuffer * platform_read_file(
    const char * filename);

// Run a task in the background
// This will trigger clientlogic.c's client_logic_threadmain()
// passing the threadmain_id to it
// you have to implement client_logic_threadmain() to do
// what you want it to do when it gets that id
void platform_start_thread(int32_t threadmain_id);

// TODO: is platform_start_timer still needed?
// void platform_start_timer(void);
uint64_t platform_get_current_time_microsecs(void);

#endif

