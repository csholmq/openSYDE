//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for reconnect bus connector (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYRECONNECTBASECOMMAND_H
#define C_SDMANUNOTOPOLOGYRECONNECTBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SebUnoBaseCommand.h"
#include "C_GiLiBusConnector.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyReconnectBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SdManUnoTopologyReconnectBaseCommand(QGraphicsScene * const opc_Scene,
                                          const std::vector<stw_types::uint64> & orc_IDs,
                                          const stw_types::uint64 & oru64_StartingItemID,
                                          const stw_types::uint64 & oru64_LastItemID, const QPointF & orc_ConnectionPos,
                                          const stw_types::sint32 & ors32_Interface,
                                          const stw_types::uint8 & oru8_NodeId, const bool oq_ActivateDatapoolL2,
                                          const bool oq_ActivateDatapoolECeS, const bool oq_ActivateDatapoolECoS,
                                          const QString & orc_Description, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyReconnectBaseCommand(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

protected:
   const stw_types::uint64 mu64_StartingItemID;
   const stw_types::uint64 mu64_LastItemID;
   const QPointF mc_ConnectionPos;
   //TODO: Save initial position (If this command ever needs to be undone) -> also add to m_Reconnect interface
   const stw_types::sint32 ms32_Interface;
   stw_types::uint8 mu8_InitialInterface;
   const stw_types::uint8 mu8_NodeId;
   stw_types::uint8 mu8_InitialNodeId;
   bool mq_ActivateDatapoolL2;
   bool mq_ActivateDatapoolECeS;
   bool mq_ActivateDatapoolECoS;

   stw_opensyde_gui::C_GiLiBusConnector * m_GetBusConnector(void) const;
   virtual void m_Reconnect(const stw_types::uint64 & oru64_StartingID, const stw_types::uint64 & oru64_LastID,
                            const stw_types::sint32 & ors32_Interface, const stw_types::uint8 & oru8_NodeId,
                            const bool oq_ActivateDatapoolL2, const bool oq_ActivateDatapoolECeS,
                            const bool oq_ActivateDatapoolECoS) = 0;

private:
   bool mq_Merged;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
