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
 * @file
 *   Specification for the HUFF_APP command and telemetry
 *   message data types.
 *
 * @note
 *   Constants and enumerated types related to these message structures
 *   are defined in HUFF_APP_msgdefs.h.
 */
#ifndef HUFF_APP_MSGSTRUCT_H
#define HUFF_APP_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/

#include "huff_app_mission_cfg.h"
#include "huff_app_msgdefs.h"
#include "cfe_msg_hdr.h"

/*************************************************************************/

/*
** The following commands all share the "NoArgs" format
**
** They are each given their own type name matching the command name, which
** allows them to change independently in the future without changing the prototype
** of the handler function
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} HUFF_APP_NoopCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} HUFF_APP_ResetCountersCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} HUFF_APP_RunCmd_t;

// typedef struct
// {
//     CFE_MSG_CommandHeader_t           CommandHeader; /**< \brief Command header */
//     HUFF_APP_DisplayParam_Payload_t Payload;
// } HUFF_APP_DisplayParamCmd_t;



/*************************************************************************/
/*
** Type definition (HUFF App housekeeping)
*/

typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} HUFF_APP_SendHkCmd_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    HUFF_APP_HkTlm_Payload_t Payload;            /**< \brief Telemetry payload */
} HUFF_APP_HkTlm_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t  TelemetryHeader; /**< \brief Telemetry header */
    HUFF_APP_ResultTlm_Payload_t   Payload;         /**< \brief Processing result payload */
} HUFF_APP_ResultTlm_t;


#endif /* HUFF_APP_MSGSTRUCT_H */
