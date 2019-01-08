//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for selecting nodes (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <vector>

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SdBueNodeSelectorWidget.h"
#include "ui_C_SdBueNodeSelectorWidget.h"

#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNodeDataPool.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_SdUtil.h"
#include "C_CieUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     24.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueNodeSelectorWidget::C_SdBueNodeSelectorWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueNodeSelectorWidget),
   mu32_BusIndex(0U)
{
   mpc_Ui->setupUi(this);

   this->InitStaticNames();
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     29.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueNodeSelectorWidget::~C_SdBueNodeSelectorWidget()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_BueNodeSelectorTitleLabel->setText(C_GtGetText::h_GetText("Active Protocol Nodes"));
   this->mpc_Ui->pc_LabelNoNodes->setText(C_GtGetText::h_GetText("No nodes connected to this bus"));

   //tooltips
   this->mpc_Ui->pc_BueNodeSelectorTitleLabel->SetToolTipInformation(C_GtGetText::h_GetText("Active Protocol Nodes"),
                                                                     C_GtGetText::h_GetText(
                                                                        "This list shows all physical connection to "
                                                                        "this bus."
                                                                        "\nChecked connections are active participants "
                                                                        "of current selected CAN protocol."
                                                                        "\nA Datapool is created automatically on check"
                                                                        " if there is no COMM Datapool already declared"
                                                                        " on this node."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the bus id to load all connected nodes

   \param[in]     ou32_BusIndex     Bus id

   \created     31.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::SetBusId(const stw_types::uint32 ou32_BusIndex)
{
   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   std::vector<QString> c_NodeNames;

   // save the bus index
   this->mu32_BusIndex = ou32_BusIndex;

   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex, c_NodeIndexes,
                                                                                      c_InterfaceIndexes);
   if (C_SdUtil::h_GetNames(c_NodeIndexes, c_InterfaceIndexes, c_NodeNames) == C_NO_ERR)
   {
      this->mpc_Ui->pc_NodeSelectorListWidget->AddNodes(c_NodeNames, c_NodeIndexes,
                                                        c_InterfaceIndexes);
   }

   if (c_NodeNames.size() > 0)
   {
      this->mpc_Ui->pc_LabelNoNodes->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_VerticalSpacerTop->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_NodeSelectorListWidget->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_NodeSelectorListWidget->setVisible(false);
      this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
      this->mpc_Ui->pc_VerticalSpacerTop->changeSize(0, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);
      this->mpc_Ui->pc_LabelNoNodes->setVisible(true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the actual protocol to mark the protocol using nodes

   \param[in]     oe_Protocol     Protocol id

   \created     03.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::SetProtocol(const C_OSCCanProtocol::E_Type oe_Protocol)
{
   uint32 u32_NodeCounter;

   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   std::vector<uint32> c_NodeIndexesWithInterfaceDuplicates;
   std::vector<uint32> c_ReducedInterfaceIndexes;

   disconnect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled,
              this, &C_SdBueNodeSelectorWidget::m_NodeToggled);
   disconnect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigComImport,
              this, &C_SdBueNodeSelectorWidget::m_NodeComImport);
   // save the protocol
   this->me_Protocol = oe_Protocol;

   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_BusIndex,
                                                                                      c_NodeIndexes,
                                                                                      c_InterfaceIndexes);

   if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
   {
      // get all the nodes using the protocol
      for (u32_NodeCounter = 0U; u32_NodeCounter < c_NodeIndexes.size(); ++u32_NodeCounter)
      {
         const C_OSCNode * const pc_Node =
            C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(c_NodeIndexes[u32_NodeCounter]);

         if (pc_Node != NULL)
         {
            // get the datapool
            const C_OSCNodeDataPool * const pc_DataPool = pc_Node->GetComDataPoolConst(oe_Protocol);
            const C_OSCCanProtocol * const pc_Protcol = pc_Node->GetCANProtocolConst(oe_Protocol);

            if ((pc_DataPool != NULL) && (pc_Protcol != NULL))
            {
               if (c_InterfaceIndexes[u32_NodeCounter] < pc_Protcol->c_ComMessages.size())
               {
                  const C_OSCCanMessageContainer & rc_MessageContainer =
                     pc_Protcol->c_ComMessages[c_InterfaceIndexes[u32_NodeCounter]];
                  // check which interfaces are using the protocol
                  if (rc_MessageContainer.q_IsComProtocolUsedByInterface == true)
                  {
                     // the indexes of the nodes must map to the indexes of the interfaces
                     // use a second vector to create the mapping
                     c_NodeIndexesWithInterfaceDuplicates.push_back(c_NodeIndexes[u32_NodeCounter]);
                     c_ReducedInterfaceIndexes.push_back(c_InterfaceIndexes[u32_NodeCounter]);
                  }
               }
            }
         }
      }
   }

   this->mpc_Ui->pc_NodeSelectorListWidget->SetProtocol(oe_Protocol);
   this->mpc_Ui->pc_NodeSelectorListWidget->CheckNodes(c_NodeIndexesWithInterfaceDuplicates, c_ReducedInterfaceIndexes);

   connect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigNodeToggled,
           this, &C_SdBueNodeSelectorWidget::m_NodeToggled);
   connect(this->mpc_Ui->pc_NodeSelectorListWidget, &C_SdBueNodeSelectorCheckBoxListWidget::SigComImport,
           this, &C_SdBueNodeSelectorWidget::m_NodeComImport);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates in case of changed the items

   \created     09.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::Refresh(void)
{
   this->mpc_Ui->pc_NodeSelectorListWidget->Refresh();
}

//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::m_NodeToggled(const uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                              const bool oq_Checked) const
{
   if (oq_Checked == true)
   {
      // protocol will be used by this node on this bus
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

      if (pc_Node != NULL)
      {
         // check if a datapool exist
         const C_OSCNodeDataPool * const pc_DataPool = pc_Node->GetComDataPoolConst(this->me_Protocol);

         if (pc_DataPool != NULL)
         {
            // No new datapool necessary
            Q_EMIT this->SigConnectNodeToProt(ou32_NodeIndex, ou32_InterfaceIndex);
         }
         else
         {
            // No COM datapool yet. New one must be created if permissible
            if (C_SdUtil::h_CheckDatapoolNumber(ou32_NodeIndex, C_OSCNodeDataPool::eCOM, false,
                                                this->parentWidget()) == true)
            {
               Q_EMIT this->SigConnectNodeToProtAndAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex);
            }
            else // No more datapools can be added
            {
               //Uncheck checkbox
               this->mpc_Ui->pc_NodeSelectorListWidget->CheckSpecificNode(ou32_NodeIndex, ou32_InterfaceIndex, false);
            }
         }
      }
   }
   else
   {
      // Disconnect the COM datapool of the node
      Q_EMIT this->SigDisconnectNodeFromProt(ou32_NodeIndex, ou32_InterfaceIndex);
   }
}

//-----------------------------------------------------------------------------
void C_SdBueNodeSelectorWidget::m_NodeComImport(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
{
   sint32 s32_Return = C_NO_ERR;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(ou32_NodeIndex);
   uint32 u32_ProtocolCounter;
   uint32 u32_DataPoolIndex = 0U;

   if (pc_Node != NULL)
   {
      // check if a datapool exist
      const C_OSCNodeDataPool * const pc_DataPool = pc_Node->GetComDataPoolConst(this->me_Protocol);

      if (pc_DataPool == NULL)
      {
         // No COM datapool yet. New one must be created
         Q_EMIT this->SigAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex);
      }

      // Get the datapool index
      for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < pc_Node->c_ComProtocols.size(); ++u32_ProtocolCounter)
      {
         if (pc_Node->c_ComProtocols[u32_ProtocolCounter].e_Type == this->me_Protocol)
         {
            u32_DataPoolIndex = pc_Node->c_ComProtocols[u32_ProtocolCounter].u32_DataPoolIndex;
            break;
         }
      }

      s32_Return = C_CieUtil::h_ImportFile(ou32_NodeIndex, u32_DataPoolIndex, ou32_InterfaceIndex, this);

      if (s32_Return == C_NO_ERR)
      {
         // Refresh update
         Q_EMIT this->SigReload();
      }
   }
}