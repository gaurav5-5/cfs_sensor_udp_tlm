/**
 * @file
 *
 * Main header file for the SENSOR_READ application
 */

#ifndef SNS_RD_APP_H
#define SNS_RD_APP_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "sns_rd_app_perfids.h"
#include "sns_rd_app_msgids.h"
#include "sns_rd_app_msg.h"

/***********************************************************************/
#define SNS_RD_APP_PIPE_DEPTH \
    32 /* Depth of the Command Pipe for Application - Maximum number of messages in pipe at any point */

#define SNS_RD_APP_NUMBER_OF_TABLES 1 /* Number of Table(s) - SNS_RD_APP_Table_t */

/* Define filenames of default data images for tables */
#define SNS_RD_APP_TABLE_FILE "/cf/sns_rd_app_tbl.tbl"

#define SNS_RD_APP_TABLE_OUT_OF_RANGE_ERR_CODE -1

#define SNS_RD_APP_TABLE_READ_INTERVAL_MAX 0xffff

#define SNS_RD_APP_SENSOR_ID 0x04

#define SNS_RD_APP_TIMER_CLK_ACCURACY 100U /* Tolerance of +-100us */

/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCmdCounter;
    uint8 PktCounter;

    /*
    ** Flags
    */
    bool ReadSensorReady;

    /*
    ** Telemetry packets
    */
    SNS_RD_APP_HkTlm_t     HkTlm;
    SNS_RD_APP_SensorTlm_t SensorTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    enum CFE_ES_RunStatus RunStatus;

    /*
    ** Operational data
    */
    CFE_SB_PipeId_t CommandPipe;
    osal_id_t       ReadEventTimId;
    uint32          ReadIntervalMs;
    uint32          TimClkAccuracy;

    /*
    ** Initialization data
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[SNS_RD_APP_NUMBER_OF_TABLES];
} SNS_RD_APP_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SNS_RD_APP_Main), these
**       functions are not called from any other source module.
*/
void         SNS_RD_APP_Main(void);
CFE_Status_t SNS_RD_APP_Init(void);
void         SNS_RD_APP_ProcessCommandPacket(CFE_SB_Buffer_t *);
CFE_Status_t SNS_RD_APP_TblValidationFunc(void *TblData);

void         SNS_RD_APP_ReadEventTimCallback(osal_id_t);
void         SNS_RD_APP_ProcessSensorRead(void);
CFE_Status_t SNS_RD_APP_GetSensorData(int, SNS_RD_APP_SensorData_t *);
void         SNS_RD_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
CFE_Status_t SNS_RD_ReportHousekeeping(CFE_MSG_CommandHeader_t *);
CFE_Status_t SNS_RD_APP_Noop(const SNS_RD_APP_NoopCmd_t *Msg);
CFE_Status_t SNS_RD_APP_ResetCounters(const SNS_RD_APP_ResetCountersCmd_t *Msg);
bool         SNS_RD_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* SNS_RD_APP_H */
