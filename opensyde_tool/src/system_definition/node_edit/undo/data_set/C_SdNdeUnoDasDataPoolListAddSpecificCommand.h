//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set add specific undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTADDSPECIFICCOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTADDSPECIFICCOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SdNdeUnoDasDataPoolListAddCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListAddSpecificCommand :
   public C_SdNdeUnoDasDataPoolListAddCommand
{
public:
   C_SdNdeUnoDasDataPoolListAddSpecificCommand(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                               const std::vector<stw_types::uint32> & orc_Indices,
                                               const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
                                               const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
                                               QUndoCommand * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif