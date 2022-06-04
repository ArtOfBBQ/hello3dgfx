#define SHARED_APPLE_PLATFORM
#define PLATFORM_NS_FILEMANAGER

#include "../shared/platform_layer.h"

uint32_t platform_get_directory_separator_size() {
    return 1;
}

void platform_get_directory_separator(
    char * recipient)
{
    recipient[0] = '/';
    recipient[1] = '\0';
}

uint64_t platform_get_current_time_microsecs() {
    uint64_t result = mach_absolute_time() / 1000;
    
    return result;
}

/*
Get a file's size. Returns -1 if no such file

same as platform_get_filesize() except it assumes
the resources directory
*/
uint64_t platform_get_resource_size(
    const char * filename)
{
    char pathfile[500];
    resource_filename_to_pathfile(
        filename,
        /* recipient: */ pathfile,
        /* assert_capacity: */ 500);
    
    return platform_get_filesize(pathfile);
}

/*
Get a file's size. Returns 0 if no such file
*/
uint64_t platform_get_filesize(
    const char * filepath)
{
    NSString * nsfilepath = [NSString
        stringWithCString:filepath
        encoding:NSASCIIStringEncoding];
    
    NSError * error_value = nil;
    
    uint64_t file_size = (uint64_t)[
        [[NSFileManager defaultManager]
        attributesOfItemAtPath:nsfilepath
        error:&error_value] fileSize];
    
    if (error_value != nil) {
        NSLog(
            @" error => %@ ",
            [error_value userInfo]);
        return 0;
    }
    
    if (file_size < 1) {
        log_append("ERROR - failed to get file ");
        log_append(filepath);
        log_append(" size for unknown reasons\n");
        log_dump_and_crash();
    }
    
    // let's not use 20MB+ files in development
    assert(file_size < 20000000);
    
    return file_size;
}

void platform_read_resource_file(
    const char * filename,
    FileBuffer * out_preallocatedbuffer)
{
    char pathfile[500];
    resource_filename_to_pathfile(
        filename,
        /* recipient: */ pathfile,
        /* capacity: */ 500);
    
    platform_read_file(
        /* filepath :*/
            pathfile,
        /* out_preallocatedbuffer: */
            out_preallocatedbuffer);
}

void platform_read_file(
    const char * filepath,
    FileBuffer * out_preallocatedbuffer)
{
    NSString * nsfilepath = [NSString
        stringWithCString:filepath
        encoding:NSASCIIStringEncoding];
    
    NSURL * file_url = [NSURL fileURLWithPath: nsfilepath];
    
    if (file_url == nil) {
        log_append("couldn't find file: ");
        log_append(filepath);
        log_append("\n");
        out_preallocatedbuffer->size = 0;
        out_preallocatedbuffer->good = false;
        return;
    }
    
    NSInputStream * input_stream = [NSInputStream
        inputStreamWithURL:file_url];
    [input_stream open];
    
    if (input_stream == nil) {
        log_append("Error - failed to create NSInputStream from viable file NSURL\n");
        out_preallocatedbuffer->size = 0;
        out_preallocatedbuffer->good = false;
        return;
    }
    
    NSInteger result =
        [input_stream
            read:
                (uint8_t *)out_preallocatedbuffer->contents
            maxLength:
                out_preallocatedbuffer->size - 1];
    
    if (result < 1) {
        NSError * stream_error = input_stream.streamError;
        
        if (stream_error != NULL) {
            log_append("ERROR: ");
            log_append(
                [[[stream_error userInfo] description]
                    cStringUsingEncoding: NSASCIIStringEncoding]);
            log_dump_and_crash();
        }
        
        out_preallocatedbuffer->size = 0;
        out_preallocatedbuffer->good = false;
        [input_stream close];
        
        return;
    }
    
    out_preallocatedbuffer->size = (uint64_t)result + 1;
    out_preallocatedbuffer->
        contents[out_preallocatedbuffer->size - 1] = '\0';
    out_preallocatedbuffer->good = true;
    [input_stream close];
}

bool32_t platform_file_exists(
    const char * filepath)
{
    NSString * nsfilepath = [NSString
        stringWithCString:filepath
        encoding:NSASCIIStringEncoding];
    
    if ([[NSFileManager defaultManager]
            fileExistsAtPath:nsfilepath])
    {
        return true;
    }
    
    return false;
}

void platform_mkdir_if_not_exist(
    const char * dirname)
{
    log_append(
        "attempt to create directory: ");
    log_append(dirname);
    log_append("\n");
    
    NSString * directory_path = [NSString
        stringWithCString:dirname
        encoding:NSASCIIStringEncoding];
    NSURL * directory_url = [NSURL
        fileURLWithPath: directory_path
        isDirectory: true];
    assert(directory_url != NULL);
    
    if (![[NSFileManager defaultManager]
        fileExistsAtPath:directory_path])
    {
        log_append("no directory there, creating it...\n");
        NSError * error = NULL;
        
        //        bool success = [[NSFileManager defaultManager]
        //            createDirectoryAtURL:directory_url
        //            withIntermediateDirectories:true
        //            attributes:NULL 
        //            error:&error];
        bool success = [[NSFileManager defaultManager]
            createDirectoryAtPath:directory_path
            withIntermediateDirectories:true
            attributes:NULL 
            error:&error];
        
        if (!success) {
            log_append("ERROR - tried to create a directory and failed\n");
            if (error != NULL) {
                NSLog(@" error => %@ ", [error userInfo]);
            }
            assert(0);
        } else {
            assert([[NSFileManager defaultManager]
                fileExistsAtPath:directory_path]);
        }
    } else {
        log_append("that directory seems to already exist, ignoring request...\n");
    }
    
    return;
}

void platform_delete_file(
    const char * filepath)
{
    NSString * nsfilepath = [NSString
        stringWithCString:filepath
        encoding:NSASCIIStringEncoding];
    NSURL * file_url = [NSURL URLWithString: nsfilepath];
    
    NSError * error = NULL;
    if (file_url != nil) {
        [
            [NSFileManager defaultManager]
            removeItemAtURL:file_url
            error:&error];
    }
}

void platform_copy_file(
    const char * filepath_source,
    const char * filepath_destination)
{
    NSString * nsfilepath_source = [NSString
        stringWithCString:filepath_source
        encoding:NSASCIIStringEncoding];
    NSString * nsfilepath_destination = [NSString
        stringWithCString:filepath_destination
        encoding:NSASCIIStringEncoding];

    NSError * error = NULL;
    
    [[NSFileManager defaultManager]
        copyItemAtPath: nsfilepath_source
        toPath: nsfilepath_destination
        error: &error];

    if (error != NULL) {
        NSLog(@" error => %@ ", [error userInfo]);
        assert(0);
    }
}

void platform_write_file(
    const char * filepath,
    const char * output,
    const uint32_t output_size)
{
    NSString * nsfilepath = [NSString
        stringWithCString:filepath
        encoding:NSASCIIStringEncoding];

    NSData * nsdata = [NSData
        dataWithBytes:output
        length:output_size];
    
    [[NSFileManager defaultManager]
        createFileAtPath: 
            nsfilepath
        contents:
            nsdata
        attributes:
            nil];
}

void platform_get_filenames_in(
    const char * directory,
    char ** filenames,
    const uint32_t recipient_capacity,
    uint32_t * recipient_size)
{
    *recipient_size = 0;
    
    NSString * path = [NSString
        stringWithCString:directory
        encoding:NSASCIIStringEncoding];
    NSURL * url = [NSURL URLWithString: path];
    NSError * error = NULL;
    
    NSArray * results = [[NSFileManager defaultManager]
        contentsOfDirectoryAtURL:url
        includingPropertiesForKeys: nil
        options: NSDirectoryEnumerationSkipsHiddenFiles
        error: &error];
    
    if (error != NULL) {
        NSLog(@" error => %@ ", [error userInfo]);
        assert(0);
    }
    
    uint32_t storable_results =
        (uint32_t)[results count] > recipient_capacity ?
            recipient_capacity
            : (uint32_t)[results count];
    
    for (
        uint32_t i = 0;
        i < storable_results;
        i++)
    {
        NSString * current_result =
            [results[i] lastPathComponent];
        
        filenames[i] =
            (char *)[current_result
                cStringUsingEncoding:NSASCIIStringEncoding];
        *recipient_size += 1;
    }
}

char * platform_get_application_path() {
    return (char *)
        [[[NSBundle mainBundle] bundlePath] cStringUsingEncoding: NSASCIIStringEncoding];
}

char * platform_get_resources_path() {
    return (char *)
        [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding: NSASCIIStringEncoding];
}

void platform_start_thread(
    void (*function_to_run)(int32_t),
    int32_t argument)
{
    dispatch_async(
        dispatch_get_global_queue(
            DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0),
        ^{
            function_to_run(argument);
        });
}
