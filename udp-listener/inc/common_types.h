#ifndef INC_COMMON_TYPES_H
#define INC_COMMON_TYPES_H

#include <stdint.h>

#define DEFAULT_LOGFILE_PATH_MAX_LEN 100
#define LOGFILE_PATH_MAX_LEN         DEFAULT_LOGFILE_PATH_MAX_LEN

typedef struct {
    char     logfile[LOGFILE_PATH_MAX_LEN];
    int      sockfd;
    uint16_t port;
    uint32_t hostname;
} UDPLogger_t;

typedef struct {
    int id;
    int cnt;
    int x, y, z;
} tlm_msg_t;

#endif // INC_COMMON_TYPES_H
