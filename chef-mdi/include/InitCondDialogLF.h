/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:                                                        ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:    Jean-Francois Ostiguy                             ******
******             Fermilab                                          ****** 
******             ostiguy@fnal.gov                                  ****** 
******                                                               ******  
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
**************************************************************************/

#ifndef INITCONDDIALOGLF_H
#define INITCONDDIALOGLF_H

#include <initconddialoglfbase.h>
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
   const LattFuncSage::lattFunc& getInitCond() const;
   void setInitCond( const LattFuncSage::lattFunc& lf );
  
 private:

   LattFuncSage::lattFunc _lf;
 
};

#endif
