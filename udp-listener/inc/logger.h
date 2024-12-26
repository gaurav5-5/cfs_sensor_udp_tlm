#ifndef _LOGGER_H
#define _LOGGER_H

#include "common_types.h"

#define DEFAULT_LOGFILE_PATH "sensor_data.log"

void Logger_printf(UDPLogger_t *, const char *, ...);

#endif // _LOGGER_H
