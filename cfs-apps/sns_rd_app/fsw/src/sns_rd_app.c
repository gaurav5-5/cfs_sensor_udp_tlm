/**
 * \file
 *   This file contains the source code for the SNS_RD App.
 */

/*
** Include Files:
*/
#include "cfe_error.h"
#include "cfe_sb.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_tbl.h"
#include "osapi-timer.h"
#include "sns_rd_app_events.h"
#include "sns_rd_app_version.h"
#include "sns_rd_app.h"
#include "sns_rd_app_table.h"
#include "sns_rd_app_sensorlib.h"

#include <string.h>

/*
** global data
*/
SNS_RD_APP_Data_t SNS_RD_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void SNS_RD_APP_Main(void)
{
    CFE_Status_t     status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(SNS_RD_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = SNS_RD_APP_Init();
    if (status != CFE_SUCCESS)
    {
        SNS_RD_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;

        CFE_EVS_SendEvent(SNS_RD_APP_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "SNS_RD APP: Initialization Error, App Will Exit");
    }

    /*
    ** SNS_RD App Runloop
    */
    while (CFE_ES_RunLoop(&SNS_RD_APP_Data.RunStatus) == true)
    {

        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SNS_RD_APP_PERF_ID);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SNS_RD_APP_PERF_ID);
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, SNS_RD_APP_Data.CommandPipe, CFE_SB_POLL);
        if (status == CFE_SUCCESS)
        {
            SNS_RD_APP_ProcessCommandPacket(SBBufPtr);
        }
        else if (status != CFE_SB_NO_MESSAGE)
        {
            CFE_EVS_SendEvent(SNS_RD_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Sensor Read App: SB Pipe Read Error, App will Exit");

            SNS_RD_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(SNS_RD_APP_PERF_ID);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(SNS_RD_APP_PERF_ID);
        if (SNS_RD_APP_Data.ReadSensorReady)
        {
            SNS_RD_APP_ProcessSensorRead();
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(SNS_RD_APP_PERF_ID);

    CFE_ES_ExitApp(SNS_RD_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t SNS_RD_APP_Init(void)
{
    CFE_Status_t status;

    SNS_RD_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    SNS_RD_APP_Data.CmdCounter    = 0;
    SNS_RD_APP_Data.ErrCmdCounter = 0;
    SNS_RD_APP_Data.PktCounter    = 0;

    /*
    ** Initialize app configuration data
    */
    SNS_RD_APP_Data.ReadSensorReady = false;
    SNS_RD_APP_Data.PipeDepth       = SNS_RD_APP_PIPE_DEPTH;

    strncpy(SNS_RD_APP_Data.PipeName, "SNS_RD_APP_CMD_PIPE", sizeof(SNS_RD_APP_Data.PipeName));
    SNS_RD_APP_Data.PipeName[sizeof(SNS_RD_APP_Data.PipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
     ** Initialize housekeeping packet (clear user data area).
     */
    CFE_MSG_Init(CFE_MSG_PTR(SNS_RD_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(SNS_RD_APP_HK_TLM_MID),
                 sizeof(SNS_RD_APP_Data.HkTlm));

    /*
     ** Initialize sensordata packet (clear user data area).
     */
    CFE_MSG_Init(CFE_MSG_PTR(SNS_RD_APP_Data.SensorTlm.TelemetryHeader), CFE_SB_ValueToMsgId(SNS_RD_APP_SENSOR_TLM_MID),
                 sizeof(SNS_RD_APP_Data.SensorTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&SNS_RD_APP_Data.CommandPipe, SNS_RD_APP_Data.PipeDepth, SNS_RD_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to MIDs
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SNS_RD_APP_SEND_HK_MID), SNS_RD_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Subscribing to HK Request, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(SNS_RD_APP_CMD_MID), SNS_RD_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&SNS_RD_APP_Data.TblHandles[0], "SnsDataTable", sizeof(SNS_RD_APP_Table_t),
                              CFE_TBL_OPT_DEFAULT, SNS_RD_APP_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load(SNS_RD_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, SNS_RD_APP_TABLE_FILE);
    }

    status = Sensorlib_Init(SNS_RD_APP_SENSOR_ID);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Initializing Sensor, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Create Timer Object
    */
    status = OS_TimerCreate(&SNS_RD_APP_Data.ReadEventTimId, "SensorReadTimer", &SNS_RD_APP_Data.TimClkAccuracy,
                            SNS_RD_APP_ReadEventTimCallback);
    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Creating Timer, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Start Timer
    */
    status = OS_TimerSet(SNS_RD_APP_Data.ReadEventTimId, SNS_RD_APP_Data.ReadIntervalMs * 1000,
                         SNS_RD_APP_Data.ReadIntervalMs * 1000);
    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Setting Timer, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    CFE_EVS_SendEvent(SNS_RD_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "SNS_RD App Initialized.%s",
                      SNS_RD_APP_VERSION_STRING);

    return CFE_SUCCESS;
}

CFE_Status_t SNS_RD_APP_GetSensorData(int sensor_id, SNS_RD_APP_SensorData_t *out)
{
    CFE_Status_t status;

    status = Sensorlib_Read(sensor_id, &out->x);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Reading Sensor, RC = 0x%08lx\n", (unsigned long)status);
    }

    status = Sensorlib_Read(sensor_id, &out->y);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Reading Sensor, RC = 0x%08lx\n", (unsigned long)status);
    }

    status = Sensorlib_Read(sensor_id, &out->z);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sensor Read App: Error Reading Sensor, RC = 0x%08lx\n", (unsigned long)status);
    }

    return status;
}

void SNS_RD_APP_ReadEventTimCallback(osal_id_t TimId)
{
    if (TimId == SNS_RD_APP_Data.ReadEventTimId)
    {
        SNS_RD_APP_Data.ReadSensorReady = true;
    }
}

void SNS_RD_APP_ProcessSensorRead(void)
{
    CFE_Status_t            status;
    SNS_RD_APP_SensorData_t payload;
    payload.id = SNS_RD_APP_SENSOR_ID;

    status = SNS_RD_APP_GetSensorData(SNS_RD_APP_SENSOR_ID, &payload);
    if (status == CFE_SUCCESS)
    {
        payload.count = SNS_RD_APP_Data.PktCounter;

        SNS_RD_APP_Data.SensorTlm.Payload = payload;
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)&SNS_RD_APP_Data.SensorTlm);
        status = CFE_SB_TransmitMsg((CFE_MSG_Message_t *)&SNS_RD_APP_Data.SensorTlm, true);
        if (status < CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("SNS_RD_APP: SB Transmit Message Error: RC = 0x%08lx", (unsigned long)status);

            SNS_RD_APP_Data.ErrCmdCounter++;
        }
        else
        {
            SNS_RD_APP_Data.PktCounter++;
        }
    }
    else
    {
        CFE_EVS_SendEvent(SNS_RD_APP_SENSOR_READ_ERR_EID, CFE_EVS_EventType_ERROR,
                          "SNS_RD APP: Sensor Read Error: RC = 0x%08lx", (unsigned long)status);

        SNS_RD_APP_Data.ErrCmdCounter++;
    }

    SNS_RD_APP_Data.ReadSensorReady = false;
}

CFE_Status_t SNS_RD_APP_TblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    SNS_RD_APP_Table_t *TblDataPtr = (SNS_RD_APP_Table_t *)TblData;

    /*
    ** SNS_RD Table Validation
    */
    if (TblDataPtr->ReadIntervalMs > SNS_RD_APP_TABLE_READ_INTERVAL_MAX)
    {
        ReturnCode = SNS_RD_APP_TABLE_OUT_OF_RANGE_ERR_CODE;
    }
    else
    {
        SNS_RD_APP_Data.ReadIntervalMs = TblDataPtr->ReadIntervalMs;
    }
    SNS_RD_APP_Data.TimClkAccuracy = TblDataPtr->TimClkAccuracy;

    return ReturnCode;
}

void SNS_RD_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{

    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case SNS_RD_APP_CMD_MID:
            SNS_RD_APP_ProcessGroundCommand(SBBufPtr);
            break;

        case SNS_RD_APP_SEND_HK_MID:
            SNS_RD_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(SNS_RD_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SNS_RD: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

CFE_Status_t SNS_RD_ReportHousekeeping(CFE_MSG_CommandHeader_t *Msg)
{
    CFE_Status_t status;
    int          i;

    /*
    ** Get command execution counters...
    */
    SNS_RD_APP_Data.HkTlm.Payload.CommandErrorCounter = SNS_RD_APP_Data.ErrCmdCounter;
    SNS_RD_APP_Data.HkTlm.Payload.CommandCounter      = SNS_RD_APP_Data.CmdCounter;
    SNS_RD_APP_Data.HkTlm.Payload.PacketCounter       = SNS_RD_APP_Data.PktCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(SNS_RD_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(SNS_RD_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < SNS_RD_APP_NUMBER_OF_TABLES; i++)
    {
        status = CFE_TBL_Manage(SNS_RD_APP_Data.TblHandles[i]);
        if (status == CFE_TBL_INFO_UPDATED)
        {
            SNS_RD_APP_Table_t *TblPtr;

            status = CFE_TBL_GetAddress((void **)&TblPtr, SNS_RD_APP_Data.TblHandles[i]);
            if (status < CFE_SUCCESS)
            {

                CFE_ES_WriteToSysLog("Sensor Read App: Failed to get table address: 0x%08lx", (unsigned long)status);

                continue;
            }

            CFE_ES_WriteToSysLog("Sensor Read App: Table Updated: ReadIntervalMs: %u, TimClkAccuracy: %u",
                                 TblPtr->ReadIntervalMs, TblPtr->TimClkAccuracy);

            SNS_RD_APP_Data.ReadIntervalMs = TblPtr->ReadIntervalMs;
            SNS_RD_APP_Data.TimClkAccuracy = TblPtr->TimClkAccuracy;

            CFE_TBL_ReleaseAddress(SNS_RD_APP_Data.TblHandles[i]);
            if (status < CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("Sensor Read App: Failed to release table address: 0x%08lx",
                                     (unsigned long)status);
            }
        }
    }

    return CFE_SUCCESS;
}

void SNS_RD_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" SAMPLE app ground commands
    */
    switch (CommandCode)
    {
        case SNS_RD_APP_NOOP_CC:
            if (SNS_RD_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SNS_RD_APP_NoopCmd_t)))
            {
                SNS_RD_APP_Noop((SNS_RD_APP_NoopCmd_t *)SBBufPtr);
            }

            break;

        case SNS_RD_APP_RESET_COUNTERS_CC:
            if (SNS_RD_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(SNS_RD_APP_ResetCountersCmd_t)))
            {
                SNS_RD_APP_ResetCounters((SNS_RD_APP_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(SNS_RD_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }
}

CFE_Status_t SNS_RD_APP_Noop(const SNS_RD_APP_NoopCmd_t *Msg)
{
    SNS_RD_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(SNS_RD_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "SNS_RD: NOOP command %s",
                      SNS_RD_APP_VERSION);

    return CFE_SUCCESS;
}

CFE_Status_t SNS_RD_APP_ResetCounters(const SNS_RD_APP_ResetCountersCmd_t *Msg)
{
    SNS_RD_APP_Data.CmdCounter    = 0;
    SNS_RD_APP_Data.ErrCmdCounter = 0;
    SNS_RD_APP_Data.PktCounter    = 0;

    CFE_EVS_SendEvent(SNS_RD_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "SNS_RD: RESET command");

    return CFE_SUCCESS;
}

bool SNS_RD_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
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

        CFE_EVS_SendEvent(SNS_RD_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        SNS_RD_APP_Data.ErrCmdCounter++;
    }

    return result;
}
