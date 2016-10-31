#include <stdio.h>
#include <stdarg.h>


#ifdef _WIN32
  #define LOG_STRING_TRACE   "[ TRACE ] "
  #define LOG_STRING_DEBUG   "[ DEBUG ] "
  #define LOG_STRING_INFO    "[ INFO  ] "
  #define LOG_STRING_MESSAGE ""
  #define LOG_STRING_WARNING "[WARNING] "
  #define LOG_STRING_ERROR   "[ ERROR ] "
#else // color on unix
  // RED     "\e[1;31m"
  // BLUE    "\e[1;34m"
  // MAGENTA "\e[1;35m"
  // CYAN    "\e[1;36m"
  // GREEN   "\e[1;32m"
  // YELLOW  "\e[1;33m"
  // NC      "\e[0m"
  // use [0 instead of [1 to have non-bold text
  #define LOG_STRING_TRACE   "\e[1;36m[ TRACE ] "
  #define LOG_STRING_DEBUG   "\e[1;35m[ DEBUG ] "
  #define LOG_STRING_INFO    "\e[1;32m[ INFO  ] "
  #define LOG_STRING_MESSAGE ""
  #define LOG_STRING_WARNING "\e[1;33m[WARNING] "
  #define LOG_STRING_ERROR   "\e[1;31m[ ERROR ] "
#endif


void __internal_print_message__(const char *level, const char *msg)
{
  #ifdef _WIN32
    printf("%s%s",level, msg);
  #else
    printf("%s%s\e[0m",level, msg);
  #endif
}


// log calls
void logTrace(const char* logline, ...)
{
  #ifdef ENABLE_LOG_TRACE
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_TRACE, message);
  #endif
}

void logDebug(const char* logline, ...)
{
  #ifdef _DEBUG
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_DEBUG, message);
  #endif
}

void logInfo(const char* logline, ...)
{
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_INFO, message);
}

void logMessage(const char* logline, ...)
{
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_MESSAGE, message);
}

void logWarning(const char* logline, ...)
{
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_WARNING, message);
}

void logError(const char* logline, ...)
{
    // collenct messages in a string
    char message[1024];
    va_list args;
    va_start( args, logline );
    vsnprintf( message, 1024, logline, args );
    va_end( args );
    __internal_print_message__(LOG_STRING_ERROR, message);
}
    

