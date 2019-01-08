//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Offers system view setup specific visualization and functionality of a node. (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>

#include "gitypes.h"
#include "stwerrors.h"
#include "constants.h"

#include "C_GtGetText.h"
#include "C_ImpUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvNodeSyvSetup.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
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

   \param[in]       ou32_ViewIndex       Index of system view
   \param[in]       ors32_NodeIndex      Index of data element in system view
   \param[in]       oru64_ID             Unique ID
   \param[in]       orf64_Width          Width of node
   \param[in]       orf64_Height         Height of node
   \param[in,out]   opc_Parent           Optional pointer to parent

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiSvNodeSyvSetup::C_GiSvNodeSyvSetup(const uint32 ou32_ViewIndex, const sint32 & ors32_NodeIndex,
                                       const uint64 & oru64_ID, const float64 & orf64_Width,
                                       const float64 & orf64_Height, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvNodeSyvBase(ou32_ViewIndex, ors32_NodeIndex, oru64_ID, orf64_Width, orf64_Height, opc_Parent),
   mpc_HoveredInteractiveIcon(NULL),
   mq_EditMode(false)
{
   this->m_InitCheckBox();
   this->m_InitInteractiveIcons(0);

   this->mc_DefaultCursor = QCursor(Qt::ArrowCursor);
   this->RestoreDefaultCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiSvNodeSyvSetup::~C_GiSvNodeSyvSetup()
{
   //lint -e{1540}  no memory leak because of the parent of mpc_CheckBox and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the node connected state

   \param[in]     oq_Connected   Flag if connected or not

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::SetViewConnected(const bool oq_Connected)
{
   uint32 u32_Counter;

   C_GiSvNodeSyvBase::SetViewConnected(oq_Connected);

   if (this->mpc_CheckBox != NULL)
   {
      this->mpc_CheckBox->SetChecked(oq_Connected);
   }

   for (u32_Counter = 0U; u32_Counter < this->mc_InteractiveIcons.size(); ++u32_Counter)
   {
      if (this->mc_InteractiveIcons[u32_Counter] != NULL)
      {
         this->mc_InteractiveIcons[u32_Counter]->setVisible(oq_Connected);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the state of the connection to the view

   \return
   true     is connected
   false    is not connected

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_GiSvNodeSyvSetup::IsViewConnected(void) const
{
   return this->mq_ViewConnected;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the mode

   \param[in]     oq_Active      Flag for the edit mode

   \created     23.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::SetEditMode(const bool oq_Active)
{
   this->mq_EditMode = oq_Active;

   if (this->mpc_CheckBox != NULL)
   {
      this->mpc_CheckBox->setVisible(oq_Active);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   generate custom tool tip

   \created     28.06.2017 STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::GenerateHint(void)
{
   if (this->mpc_HoveredInteractiveIcon == NULL)
   {
      // generate original tool tip
      C_GiNode::GenerateHint();
   }
   else
   {
      if (this->mpc_HoveredInteractiveIcon->isVisible() == true)
      {
         for (uint32 u32_Counter = 0U; u32_Counter < this->mc_InteractiveIcons.size(); ++u32_Counter)
         {
            if (this->mc_InteractiveIcons[u32_Counter] == this->mpc_HoveredInteractiveIcon)
            {
               switch (u32_Counter)
               {
               case 0:
                  this->SetDefaultToolTipHeading("");
                  this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Generate source code"));
                  break;
               case 1:
                  this->SetDefaultToolTipHeading("");
                  this->SetDefaultToolTipContent(C_GtGetText::h_GetText("Start IDE"));
                  break;
               default:
                  this->SetDefaultToolTipContent("Information about the Interactive Icon");
                  break;
               }
            }
         }
      }
      else
      {
         // generate original tool tip
         C_GiNode::GenerateHint();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for updating internal data

   \created     09.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::UpdateData(void)
{
   C_PuiSvHandler::h_GetInstance()->SetViewNodeCheckedState(this->mu32_ViewIndex, static_cast<uint32>(this->ms32_Index),
                                                            this->IsViewConnected());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information

   \created     26.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (opc_Event->button() == Qt::LeftButton)
   {
      const QPointF c_Pos = opc_Event->scenePos();
      bool q_ClickRecognized = false;

      if ((this->mpc_CheckBox != NULL) &&
          (this->mq_EditMode == true))
      {
         const QRectF c_RectCheckBox = this->mpc_CheckBox->sceneBoundingRect();

         if (c_RectCheckBox.contains(c_Pos) == true)
         {
            this->SetViewConnected(!this->mpc_CheckBox->IsChecked());
            Q_EMIT this->SigViewConnectionChanged();
            q_ClickRecognized = true;
         }
      }

      if (q_ClickRecognized == false)
      {
         // check the interaction icons
         if (this->mc_InteractiveIcons.size() < 4)
         {
            uint32 u32_Counter;

            for (u32_Counter = 0U; u32_Counter < this->mc_InteractiveIcons.size(); ++u32_Counter)
            {
               const C_GiRectPixmap * const pc_InteracticeIcon = this->mc_InteractiveIcons[u32_Counter];
               if ((pc_InteracticeIcon != NULL) && (pc_InteracticeIcon->isVisible() == true))
               {
                  const QRectF c_RectInteractiveIcon = pc_InteracticeIcon->sceneBoundingRect();
                  if (c_RectInteractiveIcon.contains(c_Pos) == true)
                  {
                     QGraphicsView * const pc_View = this->scene()->views().at(0);
                     C_OgeWiCustomMessage c_MessageBox(pc_View);
                     switch (u32_Counter)
                     {
                     case 0:
                        //m_GenerateCode();
                        break;
                     case 1:
                        //m_OpenLogiCAD();
                        break;
                     default:
                        // TODO make useful action
                        c_MessageBox.SetHeading(C_GtGetText::h_GetText("Congratulations!"));
                        c_MessageBox.SetDescription(C_GtGetText::h_GetText("Interactive Icon was clicked. That was it."
                                                                           "Have a nice day. :)"));
                        c_MessageBox.Execute();
                        break;
                     }
                     q_ClickRecognized = true;
                  }
               }
            }
         }
      }

      if (q_ClickRecognized == true)
      {
         this->m_BlockMoveAndResize();
      }
      else
      {
         if ((this->mpc_CheckBox != NULL) && (this->mq_EditMode == true))
         {
            //New default
            this->SetViewConnected(!this->mpc_CheckBox->IsChecked());
            Q_EMIT this->SigViewConnectionChanged();
         }
         else
         {
            C_GiNode::mousePressEvent(opc_Event);
         }
      }
   }
   else
   {
      C_GiNode::mousePressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   \param[in,out] opc_Event Event identification and information

   \created     19.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::hoverMoveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   // adapt the hint
   const QPointF c_Pos = opc_Event->scenePos();
   bool q_StateChanged = false;
   bool q_ItemHovered = false;

   if (this->mc_InteractiveIcons.size() < 4)
   {
      uint32 u32_Counter;

      // search for a hovered sub item
      for (u32_Counter = 0U; u32_Counter < this->mc_InteractiveIcons.size(); ++u32_Counter)
      {
         if (this->mc_InteractiveIcons[u32_Counter] != NULL)
         {
            const QRectF c_RectInteractiveIcon = this->mc_InteractiveIcons[u32_Counter]->sceneBoundingRect();
            if (c_RectInteractiveIcon.contains(c_Pos) == true)
            {
               q_ItemHovered = true;
               if (this->mpc_HoveredInteractiveIcon != this->mc_InteractiveIcons[u32_Counter])
               {
                  q_StateChanged = true;
                  this->mpc_HoveredInteractiveIcon = this->mc_InteractiveIcons[u32_Counter];
               }
               break;
            }
         }
      }
   }

   if ((q_ItemHovered == false) &&
       (this->mpc_HoveredInteractiveIcon != NULL))
   {
      // previously one item was hovered
      this->mpc_HoveredInteractiveIcon = NULL;
      q_StateChanged = true;
   }

   if (q_StateChanged == true)
   {
      // for showing the adapted variant
      Q_EMIT this->SigHideToolTip();
   }

   C_GiNode::hoverMoveEvent(opc_Event);
}

//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::m_InitCheckBox(void)
{
   const sint32 s32_IconSize = this->m_GetIconSize();

   this->mpc_CheckBox = new C_GiCheckBox(QRect(9, 9, s32_IconSize, s32_IconSize));

   if (this->mpc_CheckBox != NULL)
   {
      this->mpc_CheckBox->setZValue(mf64_ZORDER_INIT_NODE + 1.0);

      this->mpc_CheckBox->SetChecked(false);
      this->SetDrawWhiteFilter(false);

      this->addToGroup(this->mpc_CheckBox);
      this->mpc_CheckBox->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::m_UpdateCheckBox(void)
{
   if (this->mpc_CheckBox != NULL)
   {
      const sint32 s32_IconSize = this->m_GetIconSize();
      this->mpc_CheckBox->SetNewSize(QSizeF(static_cast<float64>(s32_IconSize),
                                            static_cast<float64>(s32_IconSize)));
   }
}

//-----------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::m_InitInteractiveIcons(const uint32 ou32_Count)
{
   //bool q_Continue = true;

   //Temporary icon restriction for agritechnica
   //if (this->ms32_Index >= 0)
   //{
   //   const C_OSCNode * const pc_Node =
   //      C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(static_cast<uint32>(this->ms32_Index));
   //   if (pc_Node != NULL)
   //   {
   //      if (pc_Node->c_DeviceType != "ESX-3CM")
   //      {
   //         q_Continue = false;
   //      }
   //   }
   //}
   if ((ou32_Count > 0) && (ou32_Count < 4))
   {
      uint32 u32_Counter;
      const sint32 s32_IconSize = this->m_GetIconSize();
      const sint32 s32_SpaceBetween = s32_IconSize / 2;
      const sint32 s32_SpaceLeft = ((static_cast<sint32>(this->m_GetBoundary()->boundingRect().width()) -
                                     (s32_IconSize * static_cast<sint32>(ou32_Count))) -
                                    (s32_SpaceBetween * (static_cast<sint32>(ou32_Count) - 1))) / 2;
      const float64 f64_Height = this->m_GetBoundary()->boundingRect().height() / 1.6;

      this->mc_InteractiveIcons.reserve(ou32_Count);
      for (u32_Counter = 0; u32_Counter < ou32_Count; ++u32_Counter)
      {
         const sint32 s32_PosX = s32_SpaceLeft + (static_cast<sint32>(u32_Counter) * (s32_IconSize + s32_SpaceBetween));
         this->mc_InteractiveIcons.push_back(new C_GiRectPixmap(QRect(s32_PosX, static_cast<sint32>(f64_Height),
                                                                      s32_IconSize, s32_IconSize)));

         if (this->mc_InteractiveIcons[u32_Counter] != NULL)
         {
            QPixmap c_Pixmap;
            this->mc_InteractiveIcons[u32_Counter]->setZValue(mf64_ZORDER_INIT_NODE + 1.0);
            this->mc_InteractiveIcons[u32_Counter]->SetBackgroundColor(Qt::transparent);

            switch (u32_Counter)
            {
            case 0:
               c_Pixmap.load("://images/system_definition/IconGenerateCode.svg");
               break;
            case 1:
               c_Pixmap.load("://images/system_views/actions/logicad_icon_orange-grau.png");
               break;
            default:
               c_Pixmap.load(":images/tools_icons/EnterpriseArchitect.png");
               break;
            }

            this->mc_InteractiveIcons[u32_Counter]->SetPixmap(c_Pixmap);

            this->addToGroup(this->mc_InteractiveIcons[u32_Counter]);
         }
      }
   }
}