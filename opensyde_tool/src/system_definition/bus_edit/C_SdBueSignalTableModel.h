//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Signal table model (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUESIGNALTABLEMODEL_H
#define C_SDBUESIGNALTABLEMODEL_H

/* -- Includes ------------------------------------------------------------- */
#include <QAbstractTableModel>
#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueSignalTableModel :
   public QAbstractTableModel
{
public:
   enum E_Columns
   {
      eINDEX = 0,
      eICON,
      eMESSAGE,
      eNAME,
      eCOMMENT,
      eSTART_BIT,
      eLENGTH,
      eBYTE_ORDER,
      eVALUE_TYPE,
      eINITIAL_VALUE,
      eFACTOR,
      eOFFSET,
      eAUTO_MIN_MAX,
      eMINIMUM_VALUE,
      eMAXIMUM_VALUE,
      eUNIT
   };

   explicit C_SdBueSignalTableModel(QObject * const opc_Parent = NULL);
   void SetMessageSyncManager(C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void UpdateData(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   // Header:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation, const stw_types::sintn osn_Role =
                                  static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   //lint -e{1735} Suppression, because default parameters are identical
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;
   //lint -restore

   static E_Columns h_ColumnToEnum(const stw_types::sint32 & ors32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns & ore_Value);
   stw_types::sint32 ConvertRowToSignal(const stw_types::sint32 & ors32_Row,
                                        stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        stw_types::uint32 & oru32_Signal) const;

private:
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_MessageIds;
   C_PuiSdNodeCanMessageSyncManager * mpc_SyncManager;
   stw_types::uint32 mu32_SignalCount;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif