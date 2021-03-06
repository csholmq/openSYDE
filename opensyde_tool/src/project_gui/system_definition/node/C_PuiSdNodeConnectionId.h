//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECONNECTIONID_H
#define C_PUISDNODECONNECTIONID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QString>
#include "stwtypes.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeConnectionId
{
public:
   C_PuiSdNodeConnectionId();
   bool operator ==(const C_PuiSdNodeConnectionId & orc_Ref) const;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_opensyde_core::C_OSCSystemBus::E_Type e_InterfaceType;
   stw_types::uint8 u8_InterfaceNumber;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
