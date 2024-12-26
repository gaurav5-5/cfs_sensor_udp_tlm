#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_evs.h"
#include "cfe_msg.h"
#include "cfe_sb.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_tbl.h"

#include "udp_tlm_app.h"
#include "osapi-sockets.h"
#include "udp_tlm_perfids.h"
#include "udp_tlm_msgids.h"
#include "udp_tlm_table.h"
#include "udp_tlm_events.h"
#include "udp_tlm_msg.h"
#include "udp_tlm_version.h"
#include "sns_rd_app_msgids.h"
#include "sns_rd_app_msg.h"

#include "common_types.h"

#include <stdio.h>

/*
** Global Data
*/
UDP_TLM_Data_t UDP_TLM_Data;

/* Application Entry Point and Main Loop */
void UDP_TLM_APP_Main(void)
{
    CFE_Status_t     status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(UDP_TLM_PERF_ID);

    status = UDP_TLM_Init();
    if (status != CFE_SUCCESS)
    {
        UDP_TLM_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;

        CFE_EVS_SendEvent(UDP_TLM_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "UDP_TLM APP: Initialization Error, App Will Exit");
    }
    CFE_ES_PerfLogExit(UDP_TLM_PERF_ID);

    while (CFE_ES_RunLoop(&UDP_TLM_Data.RunStatus) == true)
    {

        CFE_ES_PerfLogEntry(UDP_TLM_PERF_ID);
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, UDP_TLM_Data.CommandPipe, CFE_SB_POLL);
        if (status == CFE_SUCCESS)
        {
            UDP_TLM_ProcessMessage(SBBufPtr);
        }
        else if (status != CFE_SB_NO_MESSAGE)
        {
            CFE_EVS_SendEvent(UDP_TLM_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "UDP Telemetry App: SB Command Pipe Read Error, App will Exit");

            UDP_TLM_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
        CFE_ES_PerfLogExit(UDP_TLM_PERF_ID);
    }

    CFE_ES_ExitApp(UDP_TLM_Data.RunStatus);
}

CFE_Status_t UDP_TLM_Init(void)
{
    CFE_Status_t status;

    UDP_TLM_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    UDP_TLM_Data.CmdCounter    = 0;
    UDP_TLM_Data.ErrCmdCounter = 0;
    UDP_TLM_Data.SentCounter   = 0;
    UDP_TLM_Data.RecvCounter   = 0;

    /*
    ** Initialize app configuration data
    */
    UDP_TLM_Data.CmdPipeDepth = UDP_TLM_PIPE_DEPTH;
    strncpy(UDP_TLM_Data.CmdPipeName, "UDP_TLM_CMD_PIPE", sizeof(UDP_TLM_Data.CmdPipeName));
    UDP_TLM_Data.CmdPipeName[sizeof(UDP_TLM_Data.CmdPipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
     ** Initialize housekeeping packet (clear user data area).
     */
    CFE_MSG_Init(CFE_MSG_PTR(UDP_TLM_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(UDP_TLM_HK_TLM_MID),
                 sizeof(UDP_TLM_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&UDP_TLM_Data.CommandPipe, UDP_TLM_Data.CmdPipeDepth, UDP_TLM_Data.CmdPipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error creating Command pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to MIDs (SEND_HK/CMD/SNS_RD_SENSOR_TLM)
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SNS_RD_APP_SENSOR_TLM_MID), UDP_TLM_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry APP: Error subscribing to MID: %d, RC = 0x%08lX\n",
                             SNS_RD_APP_SENSOR_TLM_MID, (unsigned long)status);
    }
    else
    {
        CFE_EVS_SendEvent(UDP_TLM_PIPE_SUBCRIBED_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "UDP_TLM_APP Subscribed to 0x%04x", SNS_RD_APP_SENSOR_TLM_MID);
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(UDP_TLM_SEND_HK_MID), UDP_TLM_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Subscribing to HK Request, RC = 0x%08lX\n",
                             (unsigned long)status);

        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(UDP_TLM_CMD_MID), UDP_TLM_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&UDP_TLM_Data.TblHandles[0], "TlmConfTable", sizeof(UDP_TLM_Table_t), CFE_TBL_OPT_DEFAULT,
                              UDP_TLM_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load(UDP_TLM_Data.TblHandles[0], CFE_TBL_SRC_FILE, UDP_TLM_TABLE_FILE);
    }

    status = UDP_TLM_NetworkInit();
    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Initializing Network, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    CFE_EVS_SendEvent(UDP_TLM_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "UDP_TLM App Initialized.%s",
                      UDP_TLM_VERSION_STRING);

    return CFE_SUCCESS;
}

CFE_Status_t UDP_TLM_NetworkInit(void)
{
    CFE_Status_t status;

    CFE_ES_PerfLogEntry(UDP_TLM_PERF_ID);

    status = OS_SocketOpen(&UDP_TLM_Data.SockId, OS_SocketDomain_INET, OS_SocketType_DATAGRAM);
    if (status < CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(UDP_TLM_SOCKET_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "UDP_TLM App: Socket Open Error, RC = 0x%08lX", (unsigned long)status);
    }

    CFE_ES_PerfLogExit(UDP_TLM_PERF_ID);

    return status;
}

CFE_Status_t UDP_TLM_NetworkForwardTelemetry(CFE_SB_Buffer_t *SBBufPtr)
{
    OS_SockAddr_t destAddr;
    CFE_Status_t  status;
    size_t        msgSize;

    OS_SocketAddrInit(&destAddr, OS_SocketDomain_INET);
    OS_SocketAddrSetPort(&destAddr, UDP_TLM_CFG_DEST_PORT);
    OS_SocketAddrFromString(&destAddr, UDP_TLM_CFG_DEST_IPADDR);

    CFE_MSG_GetSize(&SBBufPtr->Msg, &msgSize);

    CFE_ES_PerfLogEntry(UDP_TLM_PERF_ID);

    status = OS_SocketSendTo(UDP_TLM_Data.SockId, SBBufPtr, msgSize, &destAddr);
    if (status < CFE_SUCCESS)
    {

        CFE_EVS_SendEvent(UDP_TLM_SOCKET_SEND_ERR_EID, CFE_EVS_EventType_ERROR,
                          "UDP_TLM_APP: Error Sending Message over UDP");
        CFE_ES_WriteToSysLog("UDP Telemetry App: Error Sending buffer over UDP, Size = %lu, RC = 0x%lx", msgSize,
                             (unsigned long)status);
    }

    CFE_ES_PerfLogExit(UDP_TLM_PERF_ID);

    return status;
}

void UDP_TLM_ProcessMessage(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case UDP_TLM_CMD_MID:
            UDP_TLM_ProcessGroundCommand(SBBufPtr);
            break;

        case UDP_TLM_SEND_HK_MID:
            UDP_TLM_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        case SNS_RD_APP_SENSOR_TLM_MID:
            UDP_TLM_ProcessTelemetryPacket(SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(UDP_TLM_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SNS_RD: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

CFE_Status_t UDP_TLM_ReportHousekeeping(CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    UDP_TLM_Data.HkTlm.Payload.CommandErrorCounter = UDP_TLM_Data.ErrCmdCounter;
    UDP_TLM_Data.HkTlm.Payload.CommandCounter      = UDP_TLM_Data.CmdCounter;
    UDP_TLM_Data.HkTlm.Payload.SendToCounter       = UDP_TLM_Data.SentCounter;
    UDP_TLM_Data.HkTlm.Payload.SubscribeCounter    = UDP_TLM_Data.RecvCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(UDP_TLM_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(UDP_TLM_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < UDP_TLM_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(UDP_TLM_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

void UDP_TLM_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" SAMPLE app ground commands
    */
    switch (CommandCode)
    {
        case UDP_TLM_NOOP_CC:
            if (UDP_TLM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UDP_TLM_NoopCmd_t)))
            {
                UDP_TLM_Noop((UDP_TLM_NoopCmd_t *)SBBufPtr);
            }

            break;

        case UDP_TLM_RESET_COUNTERS_CC:
            if (UDP_TLM_VerifyCmdLength(&SBBufPtr->Msg, sizeof(UDP_TLM_ResetCountersCmd_t)))
            {
                UDP_TLM_ResetCounters((UDP_TLM_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(UDP_TLM_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code: CC = %d",
                              CommandCode);
            break;
    }
}

CFE_Status_t UDP_TLM_Noop(const UDP_TLM_NoopCmd_t *Msg)
{
    UDP_TLM_Data.CmdCounter++;

    CFE_EVS_SendEvent(UDP_TLM_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SNS_RD: NOOP command %s",
                      UDP_TLM_VERSION);

    return CFE_SUCCESS;
}

CFE_Status_t UDP_TLM_ResetCounters(const UDP_TLM_ResetCountersCmd_t *Msg)
{
    UDP_TLM_Data.CmdCounter    = 0;
    UDP_TLM_Data.ErrCmdCounter = 0;
    UDP_TLM_Data.SentCounter   = 0;
    UDP_TLM_Data.RecvCounter   = 0;

    CFE_EVS_SendEvent(UDP_TLM_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SNS_RD: RESET command");

    return CFE_SUCCESS;
}

void UDP_TLM_ProcessTelemetryPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    UDP_TLM_NetworkForwardTelemetry(SBBufPtr);
}

bool UDP_TLM_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(UDP_TLM_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        UDP_TLM_Data.ErrCmdCounter++;
    }

    return result;
}

CFE_Status_t UDP_TLM_TblValidationFunc(void *TblData)
{

    int32            ReturnCode = CFE_SUCCESS;
    UDP_TLM_Table_t *TblDataPtr = (UDP_TLM_Table_t *)TblData;

    /*
    ** SNS_RD Table Validation
    */
    CFE_ES_WriteToSysLog("Table Validated: %d\n", TblDataPtr->TlmPort);

    return ReturnCode;
}
