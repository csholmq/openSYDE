//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Node data pool list table drawing delegate (implementation)

   Node data pool list table drawing delegate

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_SdNdeDataPoolListTableDelegate.h"
#include "C_OgeTedTable.h"
#include "C_OgeLeTable.h"
#include "C_OgeSpxTable.h"
#include "C_OgeSpxTableDouble.h"
#include "C_OgeCbxTable.h"
#include "C_OgeSpxFactorTable.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;
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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     08.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListTableDelegate::C_SdNdeDataPoolListTableDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_Model(NULL),
   mpc_UndoStack(NULL),
   mc_DisabledPixmapDark(":/images/CheckBoxDisabledNotChecked.svg"),
   mc_DisabledPixmapLight(":/images/CheckBoxDisabledNotCheckedAlternative.svg"),
   mc_CheckMark("://images/CheckBoxActiveWithoutBackground.svg"),
   ms32_HoveredRow(-1),
   ms32_EditCount(0),
   ms32_UndoStartIndex(-1),
   mq_ChangeInProgress(false),
   mq_Inital(false),
   mq_ChangeDetected(false)
{
   connect(this, &C_SdNdeDataPoolListTableDelegate::SigDestroyEditor, this,
           &C_SdNdeDataPoolListTableDelegate::m_OnDestroyEditor);
   connect(this, &C_SdNdeDataPoolListTableDelegate::SigCreateEditor, this,
           &C_SdNdeDataPoolListTableDelegate::m_OnCreateEditor);
   connect(this, &C_SdNdeDataPoolListTableDelegate::SigChangeAutoMinMax, this,
           &C_SdNdeDataPoolListTableDelegate::m_OnAutoMinMaxChange);
   connect(this, &C_SdNdeDataPoolListTableDelegate::SigSetModelData, this,
           &C_SdNdeDataPoolListTableDelegate::m_OnSetModelData);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten create editor event slot

   Here: Create appropriate editor widget

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Option Style options
   \param[in]     orc_Index  Correlating index

   \return
   Editor widget

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QWidget * C_SdNdeDataPoolListTableDelegate::createEditor(QWidget * const opc_Parent,
                                                         const QStyleOptionViewItem & orc_Option,
                                                         const QModelIndex & orc_Index) const
{
   QWidget * pc_Retval = NULL;

   Q_UNUSED(orc_Option)
   if ((orc_Index.isValid() == true) && (this->mpc_Model != NULL))
   {
      C_OgeSpxFactorTable * pc_SpinBoxFactor;
      C_OgeLeTable * pc_LineEdit;
      C_OgeSpxTableDouble * pc_DoubleSpinBox;
      C_OgeSpxTable * pc_SpinBox;
      C_OgeCbxTable * pc_ComboBox;
      const C_SdNdeDataPoolListTableModel::E_Columns e_Col = this->mpc_Model->ColumnToEnum(orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeDataPoolListTableModel::eINVALID:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eINDEX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eNAME:
         pc_LineEdit = new C_OgeLeTable(opc_Parent);
         //Ui restriction
         pc_LineEdit->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);
         pc_Retval = pc_LineEdit;
         break;
      case C_SdNdeDataPoolListTableModel::eCOMMENT:
         pc_Retval = new C_OgeTedTable(opc_Parent);
         break;
      case C_SdNdeDataPoolListTableModel::eVALUE_TYPE:
         pc_ComboBox = new C_OgeCbxTable(opc_Parent);
         //Init
         pc_ComboBox->addItem(C_GtGetText::h_GetText("uint8 - data type 8bit unsigned"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("sint8 - data type 8bit signed"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("uint16 - data type 16bit unsigned"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("sint16 - data type 16bit signed"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("uint32 - data type 32bit unsigned"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("sint32 - data type 32bit signed"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("uint64 - data type 64bit unsigned"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("sint64 - data type 64bit signed"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("float32 - data type IEEE 32bit float"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("float64 - data type IEEE 64bit float"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("string - character array"));
         pc_ComboBox->ResizeViewToContents();
         //lint -e{64,826,918,929,1025,1703} Qt interface
         connect(pc_ComboBox, static_cast<void (C_OgeCbxTable::*)(sintn)>(&C_OgeCbxTable::currentIndexChanged), this,
                 &C_SdNdeDataPoolListTableDelegate::m_OnValueTypeChange);
         pc_Retval = pc_ComboBox;
         break;
      case C_SdNdeDataPoolListTableModel::eARRAY_SIZE:
         pc_SpinBox = new C_OgeSpxTable(opc_Parent);
         //Special string handling
         if (this->mpc_Model->IsString(orc_Index) == true)
         {
            pc_SpinBox->SetMinimumCustom(2);
         }
         else
         {
            //Default
            pc_SpinBox->SetMinimumCustom(1);
         }

         pc_SpinBox->SetMaximumCustom(500);
         //lint -e{64,826,918,929,1025,1703} Qt interface
         connect(pc_SpinBox, static_cast<void (QSpinBox::*)(
                                            sintn)>(&QSpinBox::valueChanged), this,
                 &C_SdNdeDataPoolListTableDelegate::m_OnArraySizeChange);
         pc_Retval = pc_SpinBox;
         break;
      case C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eMIN:
         pc_Retval = m_CreateEditor(opc_Parent, orc_Index, e_Col);
         break;
      case C_SdNdeDataPoolListTableModel::eMAX:
         pc_Retval = m_CreateEditor(opc_Parent, orc_Index, e_Col);
         break;
      case C_SdNdeDataPoolListTableModel::eFACTOR:
         pc_SpinBoxFactor = new C_OgeSpxFactorTable(opc_Parent);
         //Factor needs to be above 0
         pc_SpinBoxFactor->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);
         pc_Retval = pc_SpinBoxFactor;
         break;
      case C_SdNdeDataPoolListTableModel::eOFFSET:
         pc_DoubleSpinBox = new C_OgeSpxTableDouble(opc_Parent);
         //lint -e{530,10,1015,1013}  c++11 feature
         pc_DoubleSpinBox->SetMinimumCustom(static_cast<float64>(std::numeric_limits<float64>::lowest()));
         pc_DoubleSpinBox->SetMaximumCustom(static_cast<float64>(std::numeric_limits<float64>::max()));
         pc_Retval = pc_DoubleSpinBox;
         break;
      case C_SdNdeDataPoolListTableModel::eUNIT:
         pc_Retval = new C_OgeLeTable(opc_Parent);
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SET:
         pc_Retval = m_CreateEditor(opc_Parent, orc_Index, e_Col);
         break;
      case C_SdNdeDataPoolListTableModel::eACCESS:
         pc_ComboBox = new C_OgeCbxTable(opc_Parent);
         pc_ComboBox->addItem(C_GtGetText::h_GetText("RW - read / write access"));
         pc_ComboBox->addItem(C_GtGetText::h_GetText("RO - read only access"));
         pc_ComboBox->ResizeViewToContents();
         pc_Retval = pc_ComboBox;
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SIZE:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eADDRESS:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eEVENT_CALL:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eICON:
      case C_SdNdeDataPoolListTableModel::eUNKNOWN:
      default:
         break;
      }
   }
   if (pc_Retval != NULL)
   {
      Q_EMIT this->SigCreateEditor(orc_Index);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten destroy editor event slot

   Here: Register destruction

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::destroyEditor(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   QStyledItemDelegate::destroyEditor(opc_Editor, orc_Index);
   Q_EMIT this->SigDestroyEditor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten set editor data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in]     orc_Index  Correlating index

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const
{
   if (((opc_Editor != NULL) && (orc_Index.isValid() == true)) && (this->mpc_Model != NULL))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QLineEdit * const pc_LineEdit = dynamic_cast<QLineEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QTextEdit * const pc_TextEdit = dynamic_cast<QTextEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QSpinBox * const pc_SpinBoxInt = dynamic_cast<QSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QDoubleSpinBox * const pc_SpinBoxDouble = dynamic_cast<QDoubleSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QComboBox * const pc_ComboBox = dynamic_cast<QComboBox * const>(opc_Editor);
      const C_SdNdeDataPoolListTableModel::E_Columns e_Col = this->mpc_Model->ColumnToEnum(orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeDataPoolListTableModel::eINVALID:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eINDEX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eNAME:
         if (pc_LineEdit != NULL)
         {
            pc_LineEdit->setText(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toString());
            //lint -e{64,918,1025,1703} Qt interface
            connect(pc_LineEdit, &QLineEdit::textChanged, this, &C_SdNdeDataPoolListTableDelegate::m_OnNameChange);
         }
         break;
      case C_SdNdeDataPoolListTableModel::eCOMMENT:
         if (pc_TextEdit != NULL)
         {
            pc_TextEdit->setText(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toString());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eVALUE_TYPE:
         if (pc_ComboBox != NULL)
         {
            pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eARRAY_SIZE:
         if (pc_SpinBoxInt != NULL)
         {
            pc_SpinBoxInt->setValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eMIN:
         C_SdNdeDataPoolUtil::h_SetGenericEditorDataVariable(opc_Editor, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eMAX:
         C_SdNdeDataPoolUtil::h_SetGenericEditorDataVariable(opc_Editor, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eFACTOR:
         if (pc_SpinBoxDouble != NULL)
         {
            pc_SpinBoxDouble->setValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toDouble());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eOFFSET:
         if (pc_SpinBoxDouble != NULL)
         {
            pc_SpinBoxDouble->setValue(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toDouble());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eUNIT:
         if (pc_LineEdit != NULL)
         {
            QString c_Unit = orc_Index.data(static_cast<sintn>(Qt::EditRole)).toString();
            if (c_Unit == "-")
            {
               pc_LineEdit->setText("");
            }
            else
            {
               pc_LineEdit->setText(c_Unit);
            }
         }
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SET:
         C_SdNdeDataPoolUtil::h_SetGenericEditorDataVariable(opc_Editor, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eACCESS:
         if (pc_ComboBox != NULL)
         {
            pc_ComboBox->setCurrentIndex(orc_Index.data(static_cast<sintn>(Qt::EditRole)).toInt());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SIZE:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eADDRESS:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eEVENT_CALL:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eICON:
      case C_SdNdeDataPoolListTableModel::eUNKNOWN:
      default:
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten set model data event slot

   Here: Pass relevant data

   \param[in,out] opc_Editor Editor widget
   \param[in,out] opc_Model  Model object
   \param[in]     orc_Index  Correlating index

   \created     14.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                                    const QModelIndex & orc_Index) const
{
   if ((((opc_Editor != NULL) && (opc_Model != NULL)) && (orc_Index.isValid() == true)) && (this->mpc_Model != NULL))
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QLineEdit * const pc_LineEdit = dynamic_cast<const QLineEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QTextEdit * const pc_TextEdit = dynamic_cast<const QTextEdit * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QSpinBox * const pc_SpinBoxInt = dynamic_cast<QSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QDoubleSpinBox * const pc_SpinBoxDouble = dynamic_cast<QDoubleSpinBox * const>(opc_Editor);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const QComboBox * const pc_ComboBox = dynamic_cast<const QComboBox * const>(opc_Editor);
      const C_SdNdeDataPoolListTableModel::E_Columns e_Col = this->mpc_Model->ColumnToEnum(orc_Index.column());
      switch (e_Col)
      {
      case C_SdNdeDataPoolListTableModel::eINVALID:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eINDEX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eNAME:
         if (pc_LineEdit != NULL)
         {
            opc_Model->setData(orc_Index, pc_LineEdit->text());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eCOMMENT:
         if (pc_TextEdit != NULL)
         {
            opc_Model->setData(orc_Index, pc_TextEdit->toPlainText());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eVALUE_TYPE:
         if (pc_ComboBox != NULL)
         {
            const sintn sn_NewValue = pc_ComboBox->currentIndex();
            //Clean up undo stack
            if ((this->mq_ChangeInProgress == false) && (this->mpc_UndoStack != NULL))
            {
               this->mpc_UndoStack->setIndex(this->ms32_UndoStartIndex);
            }
            opc_Model->setData(orc_Index, sn_NewValue);
         }
         break;
      case C_SdNdeDataPoolListTableModel::eARRAY_SIZE:
         if (pc_SpinBoxInt != NULL)
         {
            const sintn sn_NewValue = pc_SpinBoxInt->value();
            //Clean up undo stack
            if ((this->mq_ChangeInProgress == false) && (this->mpc_UndoStack != NULL))
            {
               this->mpc_UndoStack->setIndex(this->ms32_UndoStartIndex);
            }
            opc_Model->setData(orc_Index, sn_NewValue);
         }
         break;
      case C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eMIN:
         C_SdNdeDataPoolUtil::h_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eMAX:
         C_SdNdeDataPoolUtil::h_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eFACTOR:
         if (pc_SpinBoxDouble != NULL)
         {
            opc_Model->setData(orc_Index, pc_SpinBoxDouble->value());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eOFFSET:
         if (pc_SpinBoxDouble != NULL)
         {
            opc_Model->setData(orc_Index, pc_SpinBoxDouble->value());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eUNIT:
         if (pc_LineEdit != NULL)
         {
            opc_Model->setData(orc_Index, pc_LineEdit->text());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SET:
         C_SdNdeDataPoolUtil::h_SetModelGenericDataVariable(opc_Editor, opc_Model, orc_Index);
         break;
      case C_SdNdeDataPoolListTableModel::eACCESS:
         if (pc_ComboBox != NULL)
         {
            opc_Model->setData(orc_Index, pc_ComboBox->currentIndex());
         }
         break;
      case C_SdNdeDataPoolListTableModel::eDATA_SIZE:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eADDRESS:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eEVENT_CALL:
         //No edit
         break;
      case C_SdNdeDataPoolListTableModel::eICON:
      case C_SdNdeDataPoolListTableModel::eUNKNOWN:
      default:
         break;
      }
   }
   Q_EMIT this->SigSetModelData();
   //lint -e{1763} Qt interface
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint item

   Here: special handling for boolean & deactivated cells

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \created     17.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                             const QModelIndex & orc_Index) const
{
   bool q_CallOriginal = true;

   if (orc_Index.isValid() == true)
   {
      //Special handling for boolean to draw checked symbol instead
      if (orc_Index.data(static_cast<sintn>(Qt::EditRole)).type() == QVariant::Type::Bool)
      {
         bool q_Edit;
         if (this->ms32_EditCount > 0)
         {
            if (this->mc_Edit == orc_Index)
            {
               q_Edit = true;
            }
            else
            {
               q_Edit = false;
            }
         }
         else
         {
            q_Edit = false;
         }
         //Background
         QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
         //Content
         C_SdNdeDataPoolUtil::h_DrawTableBoolean(opc_Painter, orc_Option, orc_Index, this->mc_CheckMark, q_Edit);

         q_CallOriginal = false;
      }

      //Special icon handling
      //Check deactivated cells
      if (this->mpc_Model != NULL)
      {
         const sint32 s32_Column = orc_Index.column();
         const C_SdNdeDataPoolListTableModel::E_Columns e_Column = this->mpc_Model->ColumnToEnum(s32_Column);
         bool q_DrawX = false;
         switch (e_Column)
         {
         case C_SdNdeDataPoolListTableModel::eAUTO_MIN_MAX:
         case C_SdNdeDataPoolListTableModel::eFACTOR:
         case C_SdNdeDataPoolListTableModel::eOFFSET:
         case C_SdNdeDataPoolListTableModel::eUNIT:
            //Usually editable row
            if ((this->mpc_Model->flags(orc_Index) & static_cast<sintn>(Qt::ItemIsEditable)) !=
                static_cast<sintn>(Qt::ItemIsEditable))
            {
               q_DrawX = true;
            }
            break;
         case C_SdNdeDataPoolListTableModel::eMIN:
         case C_SdNdeDataPoolListTableModel::eMAX:
            //Usually editable row
            if ((this->mpc_Model->flags(orc_Index) & static_cast<sintn>(Qt::ItemIsEditable)) !=
                static_cast<sintn>(Qt::ItemIsEditable))
            {
               //Check if special case
               const C_PuiSdNodeDataPoolListElement * const pc_UIElement = this->mpc_Model->GetUIElement(orc_Index);
               if (pc_UIElement != NULL)
               {
                  if (pc_UIElement->q_InterpretAsString == true)
                  {
                     q_DrawX = true;
                  }
               }
            }
            break;
         default:
            //no handling
            break;
         }
         if (q_DrawX == true)
         {
            QPixmap c_ScaledDisabledPixmap;
            //Draw manually
            //Draw background
            C_SdNdeDataPoolUtil::h_DrawTableBackground(opc_Painter, orc_Option, true);
            //Draw disabled symbol
            if (orc_Option.state.testFlag(QStyle::State_Selected) == true)
            {
               c_ScaledDisabledPixmap = mc_DisabledPixmapLight.scaled(
                  orc_Option.rect.size(),
                  Qt::KeepAspectRatio,
                  Qt::SmoothTransformation);
            }
            else
            {
               c_ScaledDisabledPixmap = mc_DisabledPixmapDark.scaled(
                  orc_Option.rect.size(),
                  Qt::KeepAspectRatio,
                  Qt::SmoothTransformation);
            }
            //Checked symbol
            //Offset 3: align with check mark
            opc_Painter->drawPixmap(orc_Option.rect.topLeft().x() + 3,
                                    orc_Option.rect.topLeft().y(),
                                    c_ScaledDisabledPixmap.width(),
                                    c_ScaledDisabledPixmap.height(), c_ScaledDisabledPixmap);

            q_CallOriginal = false;
         }
      }
   }

   if (q_CallOriginal == true)
   {
      //Original
      QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set hovered row index

   \param[in] orc_Value New hovered row index

   \return
   true  Change
   false No change

   \created     22.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDataPoolListTableDelegate::SetHoveredRow(const stw_types::sint32 & ors32_Value)
{
   bool q_Retval;

   if (this->ms32_HoveredRow == ors32_Value)
   {
      q_Retval = false;
   }
   else
   {
      q_Retval = true;
   }
   this->ms32_HoveredRow = ors32_Value;
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set model for column look up

   \param[in] opc_Model  Model for column look up

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::SetModel(C_SdNdeDataPoolListTableModel * const opc_Value)
{
   this->mpc_Model = opc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set current undo stack

   \param[in,out] opc_Value Current undo stack

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::SetUndoStack(QUndoStack * const opc_Value)
{
   this->mpc_UndoStack = opc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create widget to use for editing this value

   \param[in,out] opc_Parent Parent widget
   \param[in]     orc_Index  Correlating index
   \param[in]     oe_Col     Optional indicator which column this editor is for

   \return
   Editor widget

   \created     15.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QWidget * C_SdNdeDataPoolListTableDelegate::m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                                           const C_SdNdeDataPoolListTableModel::E_Columns oe_Col) const
{
   QWidget * pc_Retval = NULL;

   if (orc_Index.isValid() == true)
   {
      if (this->mpc_Model != NULL)
      {
         //Core data
         const C_OSCNodeDataPoolListElement * const pc_Element = this->mpc_Model->GetOSCElement(orc_Index);
         if (pc_Element != NULL)
         {
            if (oe_Col == C_SdNdeDataPoolListTableModel::eMIN)
            {
               //Adapt min
               C_OSCNodeDataPoolContent c_NewMin = pc_Element->c_MinValue;
               C_SdNdeDataPoolContentUtil::h_InitMin(c_NewMin);
               pc_Retval = C_SdNdeDataPoolUtil::h_CreateGenericEditor(opc_Parent, orc_Index, c_NewMin,
                                                                      pc_Element->c_MaxValue, pc_Element->f64_Factor,
                                                                      pc_Element->f64_Offset, 0, false);
            }
            else if (oe_Col == C_SdNdeDataPoolListTableModel::eMAX)
            {
               //Adapt max
               C_OSCNodeDataPoolContent c_NewMax = pc_Element->c_MaxValue;
               C_SdNdeDataPoolContentUtil::h_InitMax(c_NewMax);
               pc_Retval = C_SdNdeDataPoolUtil::h_CreateGenericEditor(opc_Parent, orc_Index, pc_Element->c_MinValue,
                                                                      c_NewMax, pc_Element->f64_Factor,
                                                                      pc_Element->f64_Offset, 0, false);
            }
            else
            {
               pc_Retval = C_SdNdeDataPoolUtil::h_CreateGenericEditor(opc_Parent, orc_Index, pc_Element->c_MinValue,
                                                                      pc_Element->c_MaxValue, pc_Element->f64_Factor,
                                                                      pc_Element->f64_Offset, 0, false);
            }
            if (pc_Retval == NULL)
            {
               //Send link click
               Q_EMIT this->SigLinkClicked(orc_Index);
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change data / state based on call of destroyEditor

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnDestroyEditor(void)
{
   --this->ms32_EditCount;
   if (this->ms32_EditCount == 0)
   {
      m_CleanUpLastOne();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change data / state based on call of createEditor

   \param[in] orc_Index Edited index

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnCreateEditor(const QModelIndex & orc_Index)
{
   if ((this->mq_ChangeInProgress == false) || (this->mc_Edit != orc_Index))
   {
      if (this->ms32_EditCount != 0)
      {
         m_CleanUpLastOne();
      }
      m_PrepareForNewOne();
   }
   this->mc_Edit = orc_Index;
   ++this->ms32_EditCount;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register name change

   \param[in] orc_Text New text

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnNameChange(const QString & orc_Text) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   QWidget * const pc_Widget = dynamic_cast<QWidget * const>(this->sender());

   if ((pc_Widget != NULL) && (this->mpc_Model != NULL))
   {
      //Check name
      const bool q_Valid = this->mpc_Model->CheckName(this->mc_Edit.row(), orc_Text);
      C_OgeWiUtil::h_ApplyStylesheetProperty(pc_Widget, "Valid", q_Valid);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle auto min / max change event

   \param[in] orq_Checked Flag if auto min max checked

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnAutoMinMaxChange(const bool & orq_Checked)
{
   if ((this->mpc_Model != NULL) && (this->mq_ChangeInProgress == false))
   {
      this->mq_ChangeInProgress = true;
      this->mpc_Model->setData(this->mc_Edit, QVariant(orq_Checked));
      this->mq_ChangeInProgress = false;
   }
   this->mq_Inital = false;
   //For auto min max there is always a change
   this->mq_ChangeDetected = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change data / state based on call of setModelData

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnSetModelData(void)
{
   if (this->mq_ChangeInProgress == false)
   {
      this->mq_ChangeDetected = true;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle value type change event

   \param[in] orsn_Index New value type index

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnValueTypeChange(const sintn & orsn_Index)
{
   if ((this->mpc_Model != NULL) && (this->mq_ChangeInProgress == false))
   {
      this->mq_ChangeInProgress = true;
      this->mpc_Model->setData(this->mc_Edit, QVariant(orsn_Index));
      this->mq_ChangeInProgress = false;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle array size change event

   \param[in] orsn_NewSize New size

   \created     07.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_OnArraySizeChange(const sintn & orsn_NewSize)
{
   if ((this->mpc_Model != NULL) && (this->mq_ChangeInProgress == false))
   {
      const uint32 u32_Size = this->mpc_Model->GetArraySize(this->mc_Edit.row());
      if (((u32_Size == 1) && (orsn_NewSize != 1)) || ((u32_Size != 1) && (orsn_NewSize == 1)))
      {
         //Only visible changes
         this->mq_ChangeInProgress = true;
         this->mpc_Model->setData(this->mc_Edit, QVariant(orsn_NewSize));
         this->mq_ChangeInProgress = false;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Prepare state for new editor

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_PrepareForNewOne(void)
{
   this->mq_Inital = true;
   this->mq_ChangeDetected = false;
   if (this->mpc_UndoStack != NULL)
   {
      this->ms32_UndoStartIndex = this->mpc_UndoStack->index();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up left over actions for last editor

   \created     08.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListTableDelegate::m_CleanUpLastOne(void)
{
   if (((this->mq_ChangeDetected == false) && (this->mpc_UndoStack != NULL)) && (this->ms32_UndoStartIndex > -1))
   {
      this->mpc_UndoStack->setIndex(this->ms32_UndoStartIndex);
   }
}