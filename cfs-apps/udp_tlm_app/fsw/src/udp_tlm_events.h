/**
 * @file
 *
 * Define UDP Telemetry App Events IDs
 */

#ifndef UDP_TLM_EVENTS_H
#define UDP_TLM_EVENTS_H

#define UDP_TLM_RESERVED_EID           0
#define UDP_TLM_STARTUP_INF_EID        1
#define UDP_TLM_PIPE_SUBCRIBED_INF_EID 2
#define UDP_TLM_INIT_ERR_EID           3
#define UDP_TLM_INVALID_MSGID_ERR_EID  4
#define UDP_TLM_COMMAND_ERR_EID        5
#define UDP_TLM_COMMANDNOP_INF_EID     6
#define UDP_TLM_COMMANDRST_INF_EID     7
#define UDP_TLM_LEN_ERR_EID            8
#define UDP_TLM_PIPE_ERR_EID           9
#define UDP_TLM_SOCKET_OPEN_ERR_EID    10
#define UDP_TLM_SOCKET_BIND_ERR_EID    11
#define UDP_TLM_SOCKET_SEND_ERR_EID    12

#endif /* UDP_TLM_EVENTS_H */
