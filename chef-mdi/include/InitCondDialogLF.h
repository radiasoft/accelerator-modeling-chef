#ifndef INITCONDDIALOGLF_H
#define INITCONDDIALOGLF_H

#include "initconddialoglfbase.h"
#include <LattFuncSage.h>

class InitCondDialogLF : public InitCondDialogLFBase 
{

Q_OBJECT

 public:
  
   InitCondDialogLF(QWidget* parent, const char* name, WFlags f=0);
   void dispersionMode();
   void ULFMode();
   void momentsMode();

 public slots:

   void readInputValues();
   LattFuncSage::lattFunc& getInitCond();
   void setInitCond( const LattFuncSage::lattFunc& lf );
  
 private:

   LattFuncSage::lattFunc _lf;
 
};

#endif
