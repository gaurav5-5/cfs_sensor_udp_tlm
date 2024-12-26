/**
 * @file
 *
 * Define SNS_RD App  Messages and info
 */

#ifndef SNS_RD_APP_MSG_H
#define SNS_RD_APP_MSG_H

#include "common_types.h"

/*
** SNS_RD App command codes
*/
#define SNS_RD_APP_NOOP_CC           0
#define SNS_RD_APP_RESET_COUNTERS_CC 1

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} SNS_RD_APP_NoArgsCmd_t;

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef SNS_RD_APP_NoArgsCmd_t SNS_RD_APP_NoopCmd_t;
typedef SNS_RD_APP_NoArgsCmd_t SNS_RD_APP_ResetCountersCmd_t;

/*************************************************************************/
/*
** Type definition (SNS_RD App housekeeping)
*/

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
    uint8 PacketCounter;
    uint8 spare[1];
} SNS_RD_APP_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    SNS_RD_APP_HkTlm_Payload_t Payload;         /**< \brief Telemetry payload */
} SNS_RD_APP_HkTlm_t;

typedef struct
{
    uint16 id;
    uint16 count;
    int32  x, y, z;
} SNS_RD_APP_SensorData_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    SNS_RD_APP_SensorData_t   Payload;
} SNS_RD_APP_SensorTlm_t;

#endif /* SNS_RD_APP_MSG_H */
