#include "logger.h"

char application_name[100];
char * assert_failed_message;

static bool32_t logger_activated = false;
static char * log;
static uint32_t log_i = 0;

#define MAX_TIMED_FUNCTION_NAME 80
char last_log_func[MAX_TIMED_FUNCTION_NAME];

typedef struct TimedFunction {
    uint64_t function_address;
    char function_name[MAX_TIMED_FUNCTION_NAME];
    uint32_t times_ran;
    uint64_t time_tally;
    uint64_t current_first_start_at;
    uint32_t currently_running;
} TimedFunction;

#define TIMED_FUNCTION_LINK_SIZE 30
typedef struct TimedFunctionLink {
    TimedFunction linked_list[TIMED_FUNCTION_LINK_SIZE];
} TimedFunctionLink;

#define TIMED_FUNCTION_MAP_SIZE 4096
TimedFunctionLink * timed_function_map = NULL;

/*
By 'backtrace circle' I mean a conceptually circular array
where the element at position 0 follows the final element etc.

So we store each function name wheenver it runs, and when
we get to the end of the array, we start storing at the front
again. If you request the last 5 functions when backtrace_i
is at 3, it would be elements 50,0,1,2,3
*/
#define BACKTRACE_CIRCLE_SIZE 50
typedef struct BacktraceCircle {
    char function_names[MAX_TIMED_FUNCTION_NAME][BACKTRACE_CIRCLE_SIZE];
    
    uint32_t thread_id;
} BacktraceCircle;

BacktraceCircle * backtrace_circles = NULL;
uint32_t backtrace_i = 0;
#define BACKTRACE_FUNCTIONS_TO_DISPLAY 5

#ifdef __cplusplus
extern "C" {
#endif
    void __attribute__((no_instrument_function))
    __cyg_profile_func_enter(
        void *this_fn,
        void *call_site)
    {
        logger_activated = true;
        
        if (
            timed_function_map == NULL
            || !application_running)
        {
            return;
        }
        
        uint32_t entry_i =
            (uint64_t)this_fn & (TIMED_FUNCTION_MAP_SIZE - 1);
        
        uint32_t link_i = 0;
        while (
            timed_function_map[entry_i]
                .linked_list[link_i]
                .function_address
                    != (uint64_t)this_fn
            && timed_function_map[entry_i]
                .linked_list[link_i]
                .function_address != 0)
        {
            link_i += 1;
            log_assert(link_i < TIMED_FUNCTION_LINK_SIZE);
        }
        
        // record +1 run for this function address
        timed_function_map[entry_i]
            .linked_list[link_i]
            .function_address =
                (uint64_t)this_fn;
        timed_function_map[entry_i]
            .linked_list[link_i]
            .times_ran += 1;
        
        // record function name if necessary
        if (timed_function_map[entry_i]
            .linked_list[link_i]
            .function_name[0] == '\0')
        {
            Dl_info info;
            
            if (dladdr(this_fn, &info)) {
                // info.dli_fname;
                
                assert(timed_function_map != NULL);
                strcpy_capped(
                    /* recipient: */
                        timed_function_map[entry_i]
                            .linked_list[link_i]
                            .function_name,
                    /* recipient_size: */
                        MAX_TIMED_FUNCTION_NAME,
                    /* origin: */
                        info.dli_sname);
            }
        }
        
        // record when this function started running 
        timed_function_map[entry_i]
            .linked_list[link_i]
            .currently_running += 1;
        if (
            timed_function_map[entry_i]
                .linked_list[link_i]
                .currently_running == 1)
        {
            timed_function_map[entry_i]
                .linked_list[link_i]
                .current_first_start_at =
                    platform_get_current_time_microsecs();
        }
    }
    
    void __attribute__((no_instrument_function))
    __cyg_profile_func_exit(
        void *this_fn,
        void *call_site)
    {
        if (
            timed_function_map == NULL
            || !application_running)
        {
            return;
        }
        
        uint32_t entry_i =
            (uint64_t)this_fn & (TIMED_FUNCTION_MAP_SIZE - 1);
        
        bool32_t found_link = false; 
        uint32_t link_i = 0;
        for (
            ;
            link_i < TIMED_FUNCTION_LINK_SIZE;
            link_i++)
        {
            if (
                timed_function_map[entry_i]
                    .linked_list[link_i]
                    .function_address
                        == (uint64_t)this_fn)
            {
                found_link = true;
                break;
            }
        }
        
        if (!found_link) { return; }

        // find out when this func started running 
        if (
            timed_function_map[entry_i]
            .linked_list[link_i]
            .currently_running == 1)
        {
            timed_function_map[entry_i]
                .linked_list[link_i]
                .time_tally +=
                    (platform_get_current_time_microsecs() -
                        timed_function_map[entry_i]
                            .linked_list[link_i]
                            .current_first_start_at);
            
            timed_function_map[entry_i]
                .linked_list[link_i]
                .current_first_start_at = 0;
        }
        
        timed_function_map[entry_i]
            .linked_list[link_i]
            .currently_running -= 1;
    }
#ifdef __cplusplus
}
#endif

void __attribute__((no_instrument_function))
setup_log() {

    log_assert(application_name != NULL);
    
    // create a log for debug text
    log = (char *)malloc_from_unmanaged(LOG_SIZE);
    
    // create a hashmap for the functions in our app 
    // this is used for backtrace and profiling
    timed_function_map =
        (TimedFunctionLink *)malloc_from_unmanaged(
            TIMED_FUNCTION_MAP_SIZE * sizeof(TimedFunctionLink)); 
    // initialize values
    for (
        uint32_t i = 0;
        i < TIMED_FUNCTION_MAP_SIZE;
        i++)
    {
        for (
            uint32_t j = 0;
            j < TIMED_FUNCTION_LINK_SIZE;
            j++)
        {
            timed_function_map[i]
                .linked_list[j]
                .function_address = 0;
            timed_function_map[i]
                .linked_list[j]
                .time_tally = 0;
            timed_function_map[i]
                .linked_list[j]
                .times_ran = 0;
            timed_function_map[i]
                .linked_list[j]
                .current_first_start_at = 0;
            timed_function_map[i]
                .linked_list[j]
                .currently_running = 0;
        }
    }
}

void __attribute__((no_instrument_function))
internal_log_append_uint(
    const uint32_t to_append,
    const char * caller_function_name)
{
    char converted[1000];
    uint_to_string(
        /* const uint32_t input: */
            to_append,
        /* char * recipient: */
            converted,
        /* const uint32_t recipient_size: */
            1000);
    
    internal_log_append(
        converted,
        caller_function_name);
}

void __attribute__((no_instrument_function))
internal_log_append_char(
    const char to_append,
    const char * caller_function_name)
{
    char to_append_array[2];
    to_append_array[0] = to_append;
    to_append_array[1] = '\0';
    
    internal_log_append(
        to_append_array,
        caller_function_name);
}

void __attribute__((no_instrument_function))
internal_log_append_int(
    const int32_t to_append,
    const char * caller_function_name)
{
    char converted[1000];
    int_to_string(
        /* const int32_t input: */
            to_append,
        /* char * recipient: */
            converted,
        /* const uint32_t recipient_size: */
            1000);
    
    internal_log_append(
        converted,
        caller_function_name);
}

void __attribute__((no_instrument_function))
internal_log_append_float(
    const float to_append,
    const char * caller_function_name)
{
    char float_str[1000];
    float_to_string(
        /* const int32_t input: */
            to_append,
        /* char * recipient: */
            float_str,
        /* const uint32_t recipient_size: */
            1000);
    
    internal_log_append(
        float_str,
        caller_function_name);
}

void __attribute__((no_instrument_function))
internal_log_append(
    const char * to_append,
    const char * caller_function_name)
{
    #ifndef LOGGER_SILENCE
    if (application_running) {
        printf(
            "%s\n",
            to_append);
    }
    #endif
    
    return;
    #ifndef LOGGER_SILENCE 
    uint32_t initial_log_i = log_i;
    #endif
    
    if (
        !are_equal_strings(
            caller_function_name,
            last_log_func))
    {
        strcpy_capped(
            /* recipient: */
                last_log_func,
            /* recipient_size: */
                MAX_TIMED_FUNCTION_NAME,
            /* origin: */
                caller_function_name);
        
        char * prefix = (char *)"[";
        uint32_t prefix_length = get_string_length(prefix);
        assert(log_i + prefix_length < LOG_SIZE);
        strcpy_capped(
            /* recipient: */
                log + log_i,
            /* recipient_size: */
                LOG_SIZE,
            /* origin: */
                prefix);
        log_i += prefix_length;
        assert(log_i < LOG_SIZE);
        
        uint32_t func_length = get_string_length(
        caller_function_name);
        assert(log_i + func_length < LOG_SIZE);
        
        strcpy_capped(
            /* recipient: */
                log + log_i,
            /* recipient_size: */
                LOG_SIZE,
            /* origin: */
                caller_function_name);
        log_i += func_length;
        assert(log_i < LOG_SIZE);
        
        char * glue = (char *)"]: ";
        uint32_t glue_length = get_string_length(glue);
        assert(log_i + glue_length < LOG_SIZE);
        strcpy_capped(
            /* recipient: */
                log + log_i,
            /* recipient_size: */
                LOG_SIZE,
            /* origin: */
                glue);
        log_i += glue_length;
        assert(log_i < LOG_SIZE);
    }
    
    uint32_t to_append_length = get_string_length(to_append);
    assert(log_i + to_append_length < LOG_SIZE);
    strcpy_capped(
        /* recipient: */
            log + log_i,
        /* recipient_size: */
            LOG_SIZE,
        /* origin: */
            to_append);
    log_i += to_append_length;
    assert(log_i < LOG_SIZE);
}

void __attribute__((no_instrument_function))
add_profiling_stats_to_log()
{
    TimedFunction top30_timedfuncs[30];
    for (uint32_t i = 0; i < 30; i++) {
        char emptyslotstr[] = "empty slot";
        strcpy_capped(
            /* recipient: */
                top30_timedfuncs[i].function_name,
            /* recipient_size: */
                MAX_TIMED_FUNCTION_NAME,
            /* origin: */
                emptyslotstr);
        
        top30_timedfuncs[i].times_ran = 0;
        top30_timedfuncs[i].time_tally = 0;
    }
    
    for (
        uint32_t i = 0;
        i < TIMED_FUNCTION_MAP_SIZE;
        i++)
    {
        for (
            uint32_t l = 0;
            l < TIMED_FUNCTION_LINK_SIZE;
            l++)
        {
            for (
                uint32_t j = 0;
                j < 30;
                j++)
            {
                if (
                    timed_function_map[i]
                        .linked_list[l]
                        .time_tally >
                            top30_timedfuncs[j].time_tally)
                {
                    top30_timedfuncs[j] =
                        timed_function_map[i].linked_list[l];
                    break;
                }
            }
        }
    }
    
    log_append("\n***TOP 30 FUNCTIONS***\n");
    for (uint32_t j = 0; j < 30; j++) {
        log_append("[");
        if (j < 10) {
            log_append(" ");
        }
        log_append_uint(j);
        log_append("] - ");
        log_append(top30_timedfuncs[j].function_name);
        log_append(" @");
        log_append_uint(top30_timedfuncs[j].function_address);
        log_append(" * times ran: ");
        log_append_uint(top30_timedfuncs[j].times_ran);
        log_append(", time spent: ");
        log_append_uint((uint32_t)top30_timedfuncs[j].time_tally);
        log_append("\n");
    }
}

void __attribute__((no_instrument_function)) log_dump(bool32_t * good) {
     
    log[log_i + 1] = '\0';
    
    platform_write_file_to_writables(
        /* filepath_destination : */
            (char *)"log.txt",
        /* const char * output  : */
            log,
        /* output_size          : */
            log_i + 1,
        /* good                 : */
            good);
}

void __attribute__((no_instrument_function))
log_dump_and_crash() {
    bool32_t log_dump_succesful = false;
    log_dump(&log_dump_succesful);
    application_running = false;
}

void __attribute__((no_instrument_function))
internal_log_assert(
    bool32_t condition,
    const char * str_condition,
    const char * file_name,
    const int line_number,
    const char * func_name)
{
    if (condition || !application_running) { return; }
    
    #ifndef LOGGER_SILENCE
    printf(
        "\n*****\nfailed condition (%s::%s::%i: %s\n*****\n",
        file_name != NULL ? file_name : "NULL",
        func_name != NULL ? func_name : "NULL",
        line_number,
        str_condition != NULL ? str_condition : "NULL");
    #endif
    assert(str_condition != NULL);
    assert(str_condition[0] != '\0');
    
    assert(0);
    log_dump_and_crash();
    
    uint32_t str_condition_len = get_string_length(str_condition);
    uint32_t file_name_len = get_string_length(file_name);
    uint32_t func_name_len = get_string_length(func_name);
    
    uint32_t screen_dump_size =
        func_name_len +
        str_condition_len +
        (BACKTRACE_FUNCTIONS_TO_DISPLAY
            * MAX_TIMED_FUNCTION_NAME) +
        MAX_TIMED_FUNCTION_NAME +
        25;
    assert_failed_message = (char *)malloc_from_unmanaged(screen_dump_size);
    
    for (
        uint32_t i = 0;
        i < screen_dump_size;
        i++)
    {
        assert_failed_message[i] = ' ';
    }
    
    uint32_t recipient_at = 0;
    
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            file_name);
    recipient_at += file_name_len;
    
    char * connector = (char *)" - ";
    uint32_t connector_length = get_string_length(connector);
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            connector);
    recipient_at += connector_length;
     
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            func_name);
    recipient_at += func_name_len;
    
    char * connector2 = (char *)" (line ";
    uint32_t connector2_length = get_string_length(connector2);
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            connector2);
    recipient_at += connector2_length;
    
    char str_line[100];
    int_to_string(
        line_number,
        str_line,
        100);
    uint32_t str_line_len = get_string_length(str_line);
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            str_line);
    recipient_at += str_line_len;
    
    char * connector3 = (char *)"):\nAssertion failed: ";
    uint32_t connector3_length = get_string_length(connector3);
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            connector3);
    recipient_at += connector3_length;
    
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            str_condition);
    recipient_at += str_condition_len;
    
    char * connector4 = (char *)"\nBacktrace:\n";
    uint32_t connector4_length = get_string_length(connector4);
    strcpy_capped(
        /* recipient: */
            assert_failed_message + recipient_at,
        /* recipient_size: */
            screen_dump_size - recipient_at,
        /* origin: */
            connector4);
    recipient_at += connector4_length;
}

