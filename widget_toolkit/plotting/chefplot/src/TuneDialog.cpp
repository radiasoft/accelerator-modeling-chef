#include "TuneDialog.h"

#include <qlineedit.h>

TuneDialog::TuneDialog(QWidget* parent, const char* name, WFlags f):
  TuneDialogBase(parent, name,f)
{



}

void TuneDialog::setTunes( double htune, double vtune)
{

  QString shvalue;
  QString svvalue;

  hTuneLineEdit->setText( shvalue.setNum(htune) );
  vTuneLineEdit->setText( svvalue.setNum(vtune) );

} 

