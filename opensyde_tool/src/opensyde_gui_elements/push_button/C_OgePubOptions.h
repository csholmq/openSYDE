//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Button for options (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBOPTIONS_H
#define C_OGEPUBOPTIONS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubOptions :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubOptions(QWidget * const opc_Parent = NULL);
   void SetCustomIcon(QString oc_IconPathEnabled, QString oc_IconPathDisabled);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

   QIcon mc_IconEnabled;
   QIcon mc_IconDisabled;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif