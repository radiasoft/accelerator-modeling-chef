#ifndef TUNEDIALOG_H
#define TUNEDIALOG_H

#include "tunedialogbase.h"

class TuneDialog: public TuneDialogBase {

 public:

  TuneDialog(QWidget* parent, const char* name, WFlags f);

  void setTunes(double htune, double vtune);
 
};


#endif
