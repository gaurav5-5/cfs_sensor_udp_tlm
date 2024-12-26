/**
 * @file
 *
 * Define UDP Telemetry App Messages and info
 */

#ifndef UDP_TLM_MSG_H
#define UDP_TLM_MSG_H

#include "common_types.h"
#include "sns_rd_app_msg.h"

/*
** SNS_RD App command codes
*/
#define UDP_TLM_NOOP_CC           0
#define UDP_TLM_RESET_COUNTERS_CC 1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} UDP_TLM_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef UDP_TLM_NoArgsCmd_t UDP_TLM_NoopCmd_t;
typedef UDP_TLM_NoArgsCmd_t UDP_TLM_ResetCountersCmd_t;

/*************************************************************************/
/*
** Type definition (UDP Telemetry App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 SubscribeCounter;
    uint8 SendToCounter;
} UDP_TLM_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    UDP_TLM_HkTlm_Payload_t   Payload;         /**< \brief Telemetry payload */
} UDP_TLM_HkTlm_t;

#endif /* UDP_TLM_MSG_H */
