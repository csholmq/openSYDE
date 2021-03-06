//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header file with constants

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef CAMCONSTANTS_H
#define CAMCONSTANTS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */
//User roles
//QBitArray
const stw_types::sintn msn_USER_ROLE_MARKER = static_cast<stw_types::sintn>(Qt::UserRole) + 110;
//QString
const stw_types::sintn msn_USER_ROLE_MARKER_TEXT = static_cast<stw_types::sintn>(Qt::UserRole) + 111;
//QByteArray
const stw_types::sintn msn_USER_ROLE_MARKER_TRANSPARENCY = static_cast<stw_types::sintn>(Qt::UserRole) + 112;
//QStringList
const stw_types::sintn msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_1 = static_cast<stw_types::sintn>(Qt::UserRole) +
                                                                       120;
//QStringList
const stw_types::sintn msn_USER_ROLE_TABLE_ALL_STRINGS_DYNAMIC_COL_2 = static_cast<stw_types::sintn>(Qt::UserRole) +
                                                                       121;
//QStringList
const stw_types::sintn msn_USER_ROLE_STRING_PARTS = static_cast<stw_types::sintn>(Qt::UserRole) + 130;
const stw_types::sintn msn_USER_ROLE_SORT = static_cast<stw_types::sintn>(Qt::UserRole) + 140;

//Transparency configuration for gray out visualization
const stw_types::sintn msn_TRACE_TRANSPARENCY_REFRESH_TIME = 250;
const stw_types::uint32 mu32_TRACE_TRANSPARENCY_TIME_START = 1000U; // First time when gray out engine starts
const stw_types::uint32 mu32_TRACE_TRANSPARENCY_TIME_END = 5000U;   // Last time for last transparency step
const stw_types::uint32 mu32_TRACE_TRANSPARENCY_TIME_DIFF = mu32_TRACE_TRANSPARENCY_TIME_END -
                                                            mu32_TRACE_TRANSPARENCY_TIME_START;
const stw_types::sintn msn_TRACE_TRANSPARENCY_START = 255;
const stw_types::sintn msn_TRACE_TRANSPARENCY_END = 80;
const stw_types::sintn msn_TRACE_TRANSPARENCY_STEPS = msn_TRACE_TRANSPARENCY_START - msn_TRACE_TRANSPARENCY_END;

// Paths (different from paths in constants.h!)
const QString mc_DLL_PATH_PEAK_CAM = "..\\STW_dlls\\stwpeak2\\stwpeak2.dll";
const QString mc_DLL_PATH_VECTOR_CAM = "..\\STW_dlls\\stwvec32\\stwvec32.dll";

// Path variables for can monitor only
const QString mc_PATH_VARIABLE_CAN_MONITOR_BIN = "%{CAN_MONITOR_BINARY}";
const QString mc_PATH_VARIABLE_CAN_MONITOR_PROJ = "%{CAN_MONITOR_PROJECT}";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
