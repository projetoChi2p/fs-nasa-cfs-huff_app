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

/**
 * \file
 *   This file contains the source code for the Sample App.
 */

/*
** Include Files:
*/
#include "huff_app.h"
#include "huff_app_cmds.h"
#include "huff_app_utils.h"
#include "huff_app_eventids.h"
#include "huff_app_dispatch.h"
#include "huff_app_tbl.h"
#include "huff_app_version.h"

/*
** global data
*/
HUFF_APP_Data_t HUFF_APP_Data;

extern HUFF_APP_ExampleTable_t ExampleTable;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void HUFF_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(HUFF_APP_PERF_ID);

    /*
    ** Perform application-specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = HUFF_APP_Init();
    if (status != CFE_SUCCESS)
    {
        HUFF_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Sample App Runloop
    */
    while (CFE_ES_RunLoop(&HUFF_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(HUFF_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, HUFF_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(HUFF_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            HUFF_APP_TaskPipe(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(HUFF_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SAMPLE APP: SB Pipe Read Error, App Will Exit");

            HUFF_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(HUFF_APP_PERF_ID);

    CFE_ES_ExitApp(HUFF_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 HUFF_APP_Init(void)
{
    int32 status;
    char VersionString[HUFF_APP_CFG_MAX_VERSION_STR_LEN];
    osal_id_t     TimeBaseId = OS_OBJECT_ID_UNDEFINED;
    int32         OsStatus;

    /* Zero out the global data structure */
    memset(&HUFF_APP_Data, 0, sizeof(HUFF_APP_Data));

    HUFF_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app configuration data
    */
    HUFF_APP_Data.PipeDepth = HUFF_APP_PIPE_DEPTH;

    strncpy(HUFF_APP_Data.PipeName, "HUFF_APP_CMD_PIPE", sizeof(HUFF_APP_Data.PipeName));
    HUFF_APP_Data.PipeName[sizeof(HUFF_APP_Data.PipeName) - 1] = 0;

    /* The underlying timebase object should have been created by the PSP */
    OsStatus = OS_TimeBaseGetIdByName(&TimeBaseId, "cFS-Master");
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: OS_TimeBaseGetIdByName failed:RC=%ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }
    HUFF_APP_Data.TimeBaseId = TimeBaseId;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Sample App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        /*
         ** Initialize housekeeping packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(HUFF_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(HUFF_APP_HK_TLM_MID),
                     sizeof(HUFF_APP_Data.HkTlm));

        /*
         ** Initialize result packet (clear user data area).
         */
        CFE_MSG_Init(CFE_MSG_PTR(HUFF_APP_Data.ResultTlm.TelemetryHeader), CFE_SB_ValueToMsgId(HUFF_APP_RES_TLM_MID),
                     sizeof(HUFF_APP_Data.ResultTlm));
// {
// size_t                     ActualLength;
// size_t                     ExpectedLength1;
// size_t                     ExpectedLength2;
// ExpectedLength1 = sizeof(HUFF_APP_Data.ResultTlm);
// ExpectedLength2 = sizeof(HUFF_APP_ResultTlm_t);
// ActualLength = 0;
// CFE_MSG_GetSize(CFE_MSG_PTR(HUFF_APP_Data.ResultTlm.TelemetryHeader), &ActualLength);
// OS_printf("%s %d sz=%d\n", __func__, __LINE__, ActualLength);
// OS_printf("%s %d sz=%d\n", __func__, __LINE__, ExpectedLength1);
// OS_printf("%s %d sz=%d\n", __func__, __LINE__, ExpectedLength2);
// }

        /*
         ** Create Software Bus message pipe.
         */
        status = CFE_SB_CreatePipe(&HUFF_APP_Data.CommandPipe, HUFF_APP_Data.PipeDepth, HUFF_APP_Data.PipeName);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to Housekeeping request commands
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(HUFF_APP_SEND_HK_MID), HUFF_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to ground command packets
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(HUFF_APP_CMD_MID), HUFF_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        /*
        ** Subscribe to scheduled work command packets
        */
        status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(HUFF_APP_CMD_WORK_MID), HUFF_APP_Data.CommandPipe);
        if (status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Sample App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        // /*
        // ** Register Example Table(s)
        // */
        // status = CFE_TBL_Register(&HUFF_APP_Data.TblHandles[0], "ExampleTable", sizeof(HUFF_APP_ExampleTable_t),
        //                           CFE_TBL_OPT_DEFAULT, HUFF_APP_TblValidationFunc);
        // if (status != CFE_SUCCESS)
        // {
        //     CFE_ES_WriteToSysLog("Sample App: Error Registering Example Table, RC = 0x%08lX\n", (unsigned long)status);
        // }
        // else
        // {
        //     //status = CFE_TBL_Load(HUFF_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, HUFF_APP_TABLE_FILE);
        //     status = CFE_TBL_Load(HUFF_APP_Data.TblHandles[0], CFE_TBL_SRC_ADDRESS, &ExampleTable);
        // }

        CFE_Config_GetVersionString(VersionString, HUFF_APP_CFG_MAX_VERSION_STR_LEN, "HUFF App",
                          HUFF_APP_VERSION, HUFF_APP_BUILD_CODENAME, HUFF_APP_LAST_OFFICIAL);

        CFE_EVS_SendEvent(HUFF_APP_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "HUFF App Initialized. %s",
                          VersionString);
    }

    return status;
}
