/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
** File: coveragetest_sns_rd_app.c
**
** Purpose:
** Coverage Unit Test cases for the SNS_RD Application
**
** Notes:
** This implements various test cases to exercise all code
** paths through all functions defined in the SNS_RD application.
**
** It is primarily focused at providing examples of the various
** stub configurations, hook functions, and wrapper calls that
** are often needed when coercing certain code paths through
** complex functions.
*/

/*
 * Includes
 */

#include "sns_rd_app_coveragetest_common.h"
#include "ut_sns_rd_app.h"

/*
 * Unit test check event hook information
 */
typedef struct
{
    uint16      ExpectedEvent;
    uint32      MatchCount;
    const char *ExpectedFormat;
} UT_CheckEvent_t;

/*
 * An example hook function to check for a specific event.
 */
static int32 UT_CheckEvent_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context,
                                va_list va)
{
    UT_CheckEvent_t *State = UserObj;
    uint16           EventId;
    const char      *Spec;

    /*
     * The CFE_EVS_SendEvent stub passes the EventID as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        EventId = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        if (EventId == State->ExpectedEvent)
        {
            if (State->ExpectedFormat != NULL)
            {
                Spec = UT_Hook_GetArgValueByName(Context, "Spec", const char *);
                if (Spec != NULL)
                {
                    /*
                     * Example of how to validate the full argument set.
                     * ------------------------------------------------
                     *
                     * If really desired one can call something like:
                     *
                     * char TestText[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
                     * vsnprintf(TestText, sizeof(TestText), Spec, va);
                     *
                     * And then compare the output (TestText) to the expected fully-rendered string.
                     *
                     * NOTE: While this can be done, use with discretion - This isn't really
                     * verifying that the FSW code unit generated the correct event text,
                     * rather it is validating what the system snprintf() library function
                     * produces when passed the format string and args.
                     *
                     * This type of check has been demonstrated to make tests very fragile,
                     * because it is influenced by many factors outside the control of the
                     * test case.
                     *
                     * __This derived string is not an actual output of the unit under test__
                     */
                    if (strcmp(Spec, State->ExpectedFormat) == 0)
                    {
                        ++State->MatchCount;
                    }
                }
            }
            else
            {
                ++State->MatchCount;
            }
        }
    }

    return 0;
}

/* Macro to get expected event name */
#define UT_CHECKEVENT_SETUP(Evt, ExpectedEvent, ExpectedFormat) \
    UT_CheckEvent_Setup_Impl(Evt, ExpectedEvent, #ExpectedEvent, ExpectedFormat)

/*
 * Helper function to set up for event checking
 * This attaches the hook function to CFE_EVS_SendEvent
 */
static void UT_CheckEvent_Setup_Impl(UT_CheckEvent_t *Evt, uint16 ExpectedEvent, const char *EventName,
                                     const char *ExpectedFormat)
{
    if (ExpectedFormat == NULL)
    {
        UtPrintf("CheckEvent will match: %s(%u)", EventName, ExpectedEvent);
    }
    else
    {
        UtPrintf("CheckEvent will match: %s(%u), \"%s\"", EventName, ExpectedEvent, ExpectedFormat);
    }
    memset(Evt, 0, sizeof(*Evt));
    Evt->ExpectedEvent  = ExpectedEvent;
    Evt->ExpectedFormat = ExpectedFormat;
    UT_SetVaHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_CheckEvent_Hook, Evt);
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_SNS_RD_APP_Main(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    /*
     * Test Case For:
     * void SNS_RD_APP_Main( void )
     */

    UT_CheckEvent_t EventTest;

    /*
     * SNS_RD_APP_Main does not return a value,
     * but it has several internal decision points
     * that need to be exercised here.
     *
     * First call it in "nominal" mode where all
     * dependent calls should be successful by default.
     */
    SNS_RD_APP_Main();

    /*
     * Confirm that CFE_ES_ExitApp() was called at the end of execution
     */
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);

    /*
     * Now set up individual cases for each of the error paths.
     * The first is for SNS_RD_APP_Init().  As this is in the same
     * code unit, it is not a stub where the return code can be
     * easily set.  In order to get this to fail, an underlying
     * call needs to fail, and the error gets propagated through.
     * The call to CFE_EVS_Register is the first opportunity.
     * Any identifiable (non-success) return code should work.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);

    /*
     * Just call the function again.  It does not return
     * the value, so there is nothing to test for here directly.
     * However, it should show up in the coverage report that
     * the SNS_RD_APP_Init() failure path was taken.
     */
    SNS_RD_APP_Main();

    /*
     * This can validate that the internal "RunStatus" was
     * set to CFE_ES_RunStatus_APP_ERROR, by querying the struct directly.
     */
    UtAssert_UINT32_EQ(SNS_RD_APP_Data.RunStatus, CFE_ES_RunStatus_APP_ERROR);

    /*
     * Note that CFE_ES_RunLoop returns a boolean value,
     * so in order to exercise the internal "while" loop,
     * it needs to return TRUE.  But this also needs to return
     * FALSE in order to get out of the loop, otherwise
     * it will stay there infinitely.
     *
     * The deferred retcode will accomplish this.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set up buffer for command processing */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /*
     * Invoke again
     */
    SNS_RD_APP_Main();

    /*
     * Confirm that CFE_SB_ReceiveBuffer() (inside the loop) was called
     */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);

    /*
     * Now also make the CFE_SB_ReceiveBuffer call fail,
     * to exercise that error path.  This sends an
     * event which can be checked with a hook function.
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_PIPE_ERR_EID, "SNS_RD APP: SB Pipe Read Error, App Will Exit");

    /*
     * Invoke again
     */
    SNS_RD_APP_Main();

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_Init(void)
{
    /*
     * Test Case For:
     * int32 SNS_RD_APP_Init( void )
     */

    /* nominal case should return CFE_SUCCESS */
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_SUCCESS);

    /* trigger a failure for each of the sub-calls,
     * and confirm a write to syslog for each.
     * Note that this count accumulates, because the status
     * is _not_ reset between these test cases. */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_INVALID_PARAMETER);
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_EVS_INVALID_PARAMETER);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 2);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 3);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_SB_BAD_ARGUMENT);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 4);

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_INT32_EQ(SNS_RD_APP_Init(), CFE_TBL_ERR_INVALID_OPTIONS);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 5);
}

void Test_SNS_RD_APP_ProcessCommandPacket(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_ProcessCommandPacket
     */
    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Buffer_t      SBBuf;
        SNS_RD_APP_NoopCmd_t Noop;
    } TestMsg;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    UT_CheckEvent_t   EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_INVALID_MSGID_ERR_EID, "SNS_RD: invalid command packet,MID = 0x%x");

    /*
     * The CFE_MSG_GetMsgId() stub uses a data buffer to hold the
     * message ID values to return.
     */
    TestMsgId = CFE_SB_ValueToMsgId(SNS_RD_APP_CMD_MID);
    FcnCode   = SNS_RD_APP_NOOP_CC;
    MsgSize   = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    SNS_RD_APP_ProcessCommandPacket(&TestMsg.SBBuf);

    TestMsgId = CFE_SB_ValueToMsgId(SNS_RD_APP_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    SNS_RD_APP_ProcessCommandPacket(&TestMsg.SBBuf);

    /* invalid message id */
    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    SNS_RD_APP_ProcessCommandPacket(&TestMsg.SBBuf);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_ProcessGroundCommand(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_ProcessGroundCommand
     */
    CFE_MSG_FcnCode_t FcnCode;
    size_t            Size;

    /* a buffer large enough for any command message */
    union
    {
        CFE_SB_Buffer_t               SBBuf;
        SNS_RD_APP_NoopCmd_t          Noop;
        SNS_RD_APP_ResetCountersCmd_t Reset;
        SNS_RD_APP_ProcessCmd_t       Process;
    } TestMsg;
    UT_CheckEvent_t EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /*
     * call with each of the supported command codes
     * The CFE_MSG_GetFcnCode stub allows the code to be
     * set to whatever is needed.  There is no return
     * value here and the actual implementation of these
     * commands have separate test cases, so this just
     * needs to exercise the "switch" statement.
     */

    /* test dispatch of NOOP */
    FcnCode = SNS_RD_APP_NOOP_CC;
    Size    = sizeof(TestMsg.Noop);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_COMMANDNOP_INF_EID, NULL);

    SNS_RD_APP_ProcessGroundCommand(&TestMsg.SBBuf);

    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);

    /* test dispatch of RESET */
    FcnCode = SNS_RD_APP_RESET_COUNTERS_CC;
    Size    = sizeof(TestMsg.Reset);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_COMMANDRST_INF_EID, NULL);

    SNS_RD_APP_ProcessGroundCommand(&TestMsg.SBBuf);

    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);

    /* test dispatch of PROCESS */
    /* note this will end up calling SNS_RD_APP_Process(), and as such it needs to
     * avoid dereferencing a table which does not exist. */
    FcnCode = SNS_RD_APP_PROCESS_CC;
    Size    = sizeof(TestMsg.Process);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    SNS_RD_APP_ProcessGroundCommand(&TestMsg.SBBuf);

    /* test an invalid CC */
    FcnCode = 1000;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_COMMAND_ERR_EID, "Invalid ground command code: CC = %d");
    SNS_RD_APP_ProcessGroundCommand(&TestMsg.SBBuf);

    /*
     * Confirm that the event was generated only _once_
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_ReportHousekeeping(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_ReportHousekeeping( const CFE_SB_CmdHdr_t *Msg )
     */
    CFE_MSG_Message_t *MsgSend;
    CFE_MSG_Message_t *MsgTimestamp;
    CFE_SB_MsgId_t     MsgId = CFE_SB_ValueToMsgId(SNS_RD_APP_SEND_HK_MID);

    /* Set message id to return so SNS_RD_APP_Housekeeping will be called */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /* Set up to capture send message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TransmitMsg), &MsgSend, sizeof(MsgSend), false);

    /* Set up to capture timestamp message address */
    UT_SetDataBuffer(UT_KEY(CFE_SB_TimeStampMsg), &MsgTimestamp, sizeof(MsgTimestamp), false);

    /* Call unit under test, NULL pointer confirms command access is through APIs */
    SNS_RD_APP_ProcessCommandPacket((CFE_SB_Buffer_t *)NULL);

    /* Confirm message sent*/
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_ADDRESS_EQ(MsgSend, &SNS_RD_APP_Data.HkTlm);

    /* Confirm timestamp msg address */
    UtAssert_STUB_COUNT(CFE_SB_TimeStampMsg, 1);
    UtAssert_ADDRESS_EQ(MsgTimestamp, &SNS_RD_APP_Data.HkTlm);

    /*
     * Confirm that the CFE_TBL_Manage() call was done
     */
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
}

void Test_SNS_RD_APP_NoopCmd(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_NoopCmd( const SNS_RD_APP_Noop_t *Msg )
     */
    SNS_RD_APP_NoopCmd_t TestMsg;
    UT_CheckEvent_t      EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    /* test dispatch of NOOP */
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_COMMANDNOP_INF_EID, NULL);

    UtAssert_INT32_EQ(SNS_RD_APP_Noop(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_ResetCounters(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_ResetCounters( const SNS_RD_APP_ResetCounters_t *Msg )
     */
    SNS_RD_APP_ResetCountersCmd_t TestMsg;
    UT_CheckEvent_t               EventTest;

    memset(&TestMsg, 0, sizeof(TestMsg));

    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_COMMANDRST_INF_EID, "SNS_RD: RESET command");

    UtAssert_INT32_EQ(SNS_RD_APP_ResetCounters(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the event was generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_ProcessCC(void)
{
    /*
     * Test Case For:
     * void  SNS_RD_APP_ProcessCC( const SNS_RD_APP_Process_t *Msg )
     */
    SNS_RD_APP_ProcessCmd_t TestMsg;
    SNS_RD_APP_Table_t      TestTblData;
    void                   *TblPtr = &TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));
    memset(&TestMsg, 0, sizeof(TestMsg));

    /* Provide some table data for the SNS_RD_APP_Process() function to use */
    TestTblData.SensorReadInterval = 40;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &TblPtr, sizeof(TblPtr), false);
    UtAssert_INT32_EQ(SNS_RD_APP_Process(&TestMsg), CFE_SUCCESS);

    /*
     * Confirm that the CFE_TBL_GetAddress() call was done
     */
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);

    /*
    /*
     * Configure the CFE_TBL_GetAddress function to return an error
     * Exercise the error return path
     */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_UNREGISTERED);
    UtAssert_INT32_EQ(SNS_RD_APP_Process(&TestMsg), CFE_TBL_ERR_UNREGISTERED);
}

void Test_SNS_RD_APP_VerifyCmdLength(void)
{
    /*
     * Test Case For:
     * bool SNS_RD_APP_VerifyCmdLength
     */
    UT_CheckEvent_t   EventTest;
    size_t            size    = 1;
    CFE_MSG_FcnCode_t fcncode = 2;
    CFE_SB_MsgId_t    msgid   = CFE_SB_ValueToMsgId(3);

    /*
     * test a match case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_CHECKEVENT_SETUP(&EventTest, SNS_RD_APP_LEN_ERR_EID,
                        "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u");

    SNS_RD_APP_VerifyCmdLength(NULL, size);

    /*
     * Confirm that the event was NOT generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 0);

    /*
     * test a mismatch case
     */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &msgid, sizeof(msgid), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &fcncode, sizeof(fcncode), false);
    SNS_RD_APP_VerifyCmdLength(NULL, size + 1);

    /*
     * Confirm that the event WAS generated
     */
    UtAssert_UINT32_EQ(EventTest.MatchCount, 1);
}

void Test_SNS_RD_APP_TblValidationFunc(void)
{
    /*
     * Test Case For:
     * int32 SNS_RD_APP_TblValidationFunc( void *TblData )
     */
    SNS_RD_APP_Table_t TestTblData;

    memset(&TestTblData, 0, sizeof(TestTblData));

    /* nominal case (0) should succeed */
    UtAssert_INT32_EQ(SNS_RD_APP_TblValidationFunc(&TestTblData), CFE_SUCCESS);

    /* error case should return SNS_RD_APP_TABLE_OUT_OF_RANGE_ERR_CODE */
    TestTblData.SensorReadInterval = 1 + SNS_RD_APP_TBL_READ_INTERVAL_MAX;
    UtAssert_INT32_EQ(SNS_RD_APP_TblValidationFunc(&TestTblData), SNS_RD_APP_TABLE_OUT_OF_RANGE_ERR_CODE);
}

void Test_SNS_RD_APP_GetCrc(void)
{
    /*
     * Test Case For:
     * void SNS_RD_APP_GetCrc( const char *TableName )
     */

    /*
     * The only branch point here is CFE_TBL_GetInfo()
     *
     * Either way this function just does a write to syslog,
     * and it is the same in both cases, just with
     * a different message.  This could actually verify
     * the message using a hook function, if desired.
     */

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_ERR_INVALID_NAME);
    SNS_RD_APP_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UT_ClearDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo));
    SNS_RD_APP_GetCrc("UT");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 2);
}

/*
 * Setup function prior to every test
 */
void SensorRead_UT_Setup(void)
{
    UT_ResetState(0);
}

/*
 * Teardown function after every test
 */
void SensorRead_UT_TearDown(void) {}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(SNS_RD_APP_Main);
    ADD_TEST(SNS_RD_APP_Init);
    ADD_TEST(SNS_RD_APP_ProcessCommandPacket);
    ADD_TEST(SNS_RD_APP_ProcessGroundCommand);
    ADD_TEST(SNS_RD_APP_ReportHousekeeping);
    ADD_TEST(SNS_RD_APP_NoopCmd);
    ADD_TEST(SNS_RD_APP_ResetCounters);
    ADD_TEST(SNS_RD_APP_ProcessCC);
    ADD_TEST(SNS_RD_APP_VerifyCmdLength);
    ADD_TEST(SNS_RD_APP_TblValidationFunc);
    ADD_TEST(SNS_RD_APP_GetCrc);
}
