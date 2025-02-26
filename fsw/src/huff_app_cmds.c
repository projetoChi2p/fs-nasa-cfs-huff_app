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
 *   This file contains the source code for the Sample App Ground Command-handling functions
 */

/*
** Include Files:
*/
#include "huff_app.h"
#include "huff_app_cmds.h"
#include "huff_app_msgids.h"
#include "huff_app_eventids.h"
#include "huff_app_version.h"
#include "huff_app_tbl.h"
#include "huff_app_utils.h"
#include "huff_app_msg.h"

/* The bench_lib module provides the benchmark functions prototypes */
#include "bench_lib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t HUFF_APP_SendHkCmd(const HUFF_APP_SendHkCmd_t *Msg)
{
    //int i;

    /*
    ** Get command execution counters...
    */
    HUFF_APP_Data.HkTlm.Payload.CommandErrorCounter = HUFF_APP_Data.ErrCounter;
    HUFF_APP_Data.HkTlm.Payload.CommandCounter      = HUFF_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(HUFF_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(HUFF_APP_Data.HkTlm.TelemetryHeader), true);

    // /*
    // ** Manage any pending table loads, validations, etc.
    // */
    // for (i = 0; i < HUFF_APP_NUMBER_OF_TABLES; i++)
    // {
    //     CFE_TBL_Manage(HUFF_APP_Data.TblHandles[i]);
    // }

    return CFE_SUCCESS;
}

#define REPORT_MAX_LENGTH 128


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t HUFF_APP_RunCmd(const HUFF_APP_RunCmd_t *Msg)
{
    int32              status;
    uint8_t            table;
    uint16_t           check_e;
    uint16_t           check_d;
    OS_time_t          LocalTime;
    int64              RefrTimeMillis;
    int64              NowTimeMillis;

//OS_printf("%s %d\n", __func__, __LINE__);

    uint8_t print_buffer[16];
    char task_report_buffer[REPORT_MAX_LENGTH];
    char task_report_buffer_checksum[3];


    memset(&LocalTime, 0, sizeof(LocalTime));
    CFE_PSP_GetTime(&LocalTime);
    RefrTimeMillis = OS_TimeGetTotalMilliseconds(LocalTime);

    /* Invoke the benchmark function provided by Benchmark library */
    status = BENCH_LIB_HuffBenchTask(
        BENCH_LIB_u16Maj(HUFF_APP_Data.RandomizingSeed_1, HUFF_APP_Data.RandomizingSeed_2, HUFF_APP_Data.RandomizingSeed_3),
        &table, &check_e, &check_d);

    memset(&LocalTime, 0, sizeof(LocalTime));
    CFE_PSP_GetTime(&LocalTime);
    NowTimeMillis = OS_TimeGetTotalMilliseconds(LocalTime);

    if (status != CFE_SUCCESS) {
        CFE_ES_WriteToSysLog("HUFF App: Fail to run benchmark: 0x%08lx", (unsigned long)status);
    }

    strncpy(task_report_buffer, "$HUNU,", REPORT_MAX_LENGTH);

    // Current time
    BENCH_LIB_vPrintU32(print_buffer, RefrTimeMillis);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    // Compiler debug flags
    BENCH_LIB_vPrintHexU8(print_buffer, BENCH_LIB_u8BuildFlags());
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, "-", REPORT_MAX_LENGTH);
    // Machine caches settings
    BENCH_LIB_vPrintHexU8(print_buffer, BENCH_LIB_u8GetCacheSettings());
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    // Test case results
    BENCH_LIB_vPrintU32(print_buffer, NowTimeMillis-RefrTimeMillis);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    BENCH_LIB_vPrintHexU32(print_buffer, status);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    BENCH_LIB_vPrintHexU16(print_buffer, BENCH_LIB_u16Maj(HUFF_APP_Data.RandomizingSeed_1, HUFF_APP_Data.RandomizingSeed_2, HUFF_APP_Data.RandomizingSeed_3));
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    BENCH_LIB_vPrintHexU8(print_buffer, table);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    BENCH_LIB_vPrintHexU16(print_buffer, check_e);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, ",", REPORT_MAX_LENGTH);

    BENCH_LIB_vPrintHexU16(print_buffer, check_d);
    strncat(task_report_buffer, (char*)print_buffer, REPORT_MAX_LENGTH);
    strncat(task_report_buffer, "*", REPORT_MAX_LENGTH);

    BENCH_LIB_pcGenerateChecksum(task_report_buffer, task_report_buffer_checksum);
    task_report_buffer_checksum[2] = 0;
    strncat(task_report_buffer, task_report_buffer_checksum, REPORT_MAX_LENGTH);
    
    task_report_buffer[REPORT_MAX_LENGTH-1] = '\0';

    HUFF_APP_Data.RandomizingSeed_1 = check_d;
    HUFF_APP_Data.RandomizingSeed_2 = check_d;
    HUFF_APP_Data.RandomizingSeed_3 = check_d;

    /*
    ** Send result telemetry packet...
    */
    status = CFE_SB_MessageStringSet(
        HUFF_APP_Data.ResultTlm.Payload.ResultStr,
        task_report_buffer,
        sizeof(HUFF_APP_Data.ResultTlm.Payload.ResultStr),
        sizeof(task_report_buffer)
    );
    if (status == CFE_SB_BAD_ARGUMENT) {
        CFE_ES_WriteToSysLog("HUFF App: Fail to post result: 0x%08lx", (unsigned long)status);
    }
    HUFF_APP_Data.ResultTlm.Payload.ResultStr[sizeof(HUFF_APP_Data.ResultTlm.Payload.ResultStr)-1] = '\0';
//OS_printf("%s %u %s\n", __func__, __LINE__, HUFF_APP_Data.ResultTlm.Payload.ResultStr);
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(HUFF_APP_Data.ResultTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(HUFF_APP_Data.ResultTlm.TelemetryHeader), true /* IsOrigination: fix sequence, timestamp etc. */);

    return CFE_SUCCESS;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* HUFF NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
CFE_Status_t HUFF_APP_NoopCmd(const HUFF_APP_NoopCmd_t *Msg)
{
    HUFF_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(HUFF_APP_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "HUFF: NOOP command %s",
                      HUFF_APP_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
CFE_Status_t HUFF_APP_ResetCountersCmd(const HUFF_APP_ResetCountersCmd_t *Msg)
{
    HUFF_APP_Data.CmdCounter = 0;
    HUFF_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(HUFF_APP_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "HUFF: RESET command");

    return CFE_SUCCESS;
}
