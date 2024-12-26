#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Logger_printf(UDPLogger_t *udpl, const char *format, ...) {
    FILE *fd;

    // Get FD
    if (NULL != udpl->logfile) {
        fd = fopen(udpl->logfile, "a");
    } else {
        fd = fopen(DEFAULT_LOGFILE_PATH, "a");
    }

    if (NULL == fd) {
        perror("Failure in Opening Logfile\n");
        exit(EXIT_FAILURE);
    }

    // Timestamp
    time_t sysTime      = time(0);
    char   time_buf[20] = {0};
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S",
             localtime(&sysTime));

    va_list args;
    va_start(args, format);
    vfprintf(fd, format, args);
    va_end(args);

    fprintf(fd, "\n");

    fclose(fd);
}
