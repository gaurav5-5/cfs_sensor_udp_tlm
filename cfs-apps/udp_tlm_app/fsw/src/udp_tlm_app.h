/**
 * @file
 *
 * Main header file for the UDP Telemetry application
 */

#ifndef UDP_TLM_APP_H
#define UDP_TLM_APP_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "udp_tlm_msg.h"

/***********************************************************************/
#define UDP_TLM_PIPE_DEPTH \
    32 /* Depth of the Command Pipe for Application - Maximum number of messages in pipe at any point */

#define UDP_TLM_NUMBER_OF_TABLES 1 /* Number of Table(s) - UDP_TLM_Table_t */

/* Define filenames of default data images for tables */
#define UDP_TLM_TABLE_FILE "/cf/udp_tlm_app_tbl.tbl"

#define UDP_TLM_TABLE_OUT_OF_RANGE_ERR_CODE -1

#define UDP_TLM_CFG_DEST_IPADDR "127.0.0.1"
#define UDP_TLM_CFG_DEST_PORT   8080

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
    uint8 RecvCounter;
    uint8 SentCounter;

    /*
    ** Network Parameters
    */

    /*
    ** Telemetry packets
    */
    UDP_TLM_HkTlm_t HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    enum CFE_ES_RunStatus RunStatus;

    /*
    ** Operational data
    */
    CFE_SB_PipeId_t CommandPipe;
    osal_id_t       SockId;

    /*
    ** Initialization data
    */
    char   CmdPipeName[CFE_MISSION_MAX_API_LEN];
    uint16 CmdPipeDepth;

    CFE_TBL_Handle_t TblHandles[UDP_TLM_NUMBER_OF_TABLES];
} UDP_TLM_Data_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (UDP_TLM_APP_Main), these
**       functions are not called from any other source module.
*/
void         UDP_TLM_APP_Main(void);
CFE_Status_t UDP_TLM_Init(void);
CFE_Status_t UDP_TLM_NetworkInit(void);
CFE_Status_t UDP_TLM_NetworkForwardTelemetry(CFE_SB_Buffer_t *SBBufPtr);
void         UDP_TLM_ProcessMessage(CFE_SB_Buffer_t *);
void         UDP_TLM_ProcessTelemetryPacket(CFE_SB_Buffer_t *SBBufPtr);
CFE_Status_t UDP_TLM_TblValidationFunc(void *TblData);
void         UDP_TLM_ReadEventTimCallback(osal_id_t);
void         UDP_TLM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
CFE_Status_t UDP_TLM_ReportHousekeeping(CFE_MSG_CommandHeader_t *);
CFE_Status_t UDP_TLM_Noop(const UDP_TLM_NoopCmd_t *Msg);
CFE_Status_t UDP_TLM_ResetCounters(const UDP_TLM_ResetCountersCmd_t *Msg);
bool         UDP_TLM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* UDP_TLM_APP_H */
