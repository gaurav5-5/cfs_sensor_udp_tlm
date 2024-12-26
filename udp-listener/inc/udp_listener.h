#ifndef _UDP_LISTENER_H
#define _UDP_LISTENER_H

#include "common_types.h"
#include <netinet/in.h>

#include <stdint.h>

int UDPListener_Init(UDPLogger_t *);
int UDPListener_Recv(UDPLogger_t *, char *buf, size_t len);
int UDPListener_Close(UDPLogger_t *);

#endif // _UDP_LISTENER_H
