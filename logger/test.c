#include "logger.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

int main (int argc, char **argv) {
    log_info("this is an info message");
    log_debug("this is a debug message");
    log_warning("this is a warning message");
    errno = 0;
    double not_a_number = log(-1.0);
    printf("nan? %f\n", not_a_number);
    double nan2 = not_a_number * 2;
    printf("nan2? %f\n", nan2);
    log_error("this is an error message");

    return 0;
}
