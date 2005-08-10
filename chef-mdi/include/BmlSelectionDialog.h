/*************************************************************************
**************************************************************************
**************************************************************************
******
******  CHEF:      An application layered on the Beamline/mxyzptlk
******             class libraries.
******
******  File:      BmlSelectionDialog.h
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Author:    Jean-Francois Ostiguy
******             Fermilab
******             ostiguy@fnal.gov
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************/

#ifndef BMLSELECTIONDIALOG_H
#define BMLSELECTIONDIALOG_H

#include <bmlselectiondialogbase.h>

#include <list>
#include <string>

class bmlfactory;

class BmlSelectionDialog: public BmlSelectionDialogBase {

 enum beamparameter { MOMENTUM, ET, EK, GAMMA, BRHO };

 Q_OBJECT

 public:

  BmlSelectionDialog(QWidget* parent=0, const char* name=0, WFlags f=0);
  void setList( std::list<std::string>& bmllist, const char* use_name=0);
  void getSelected();
  void setBeamParameters(const bmlfactory& bf); 
  double getBRHO() {return _brho;}

 private:

  void computeBeamParameters( double value, int datatype);
  void refreshBeamParameters();

 private slots:

  void momentumCheck(bool set); 
  void ETCheck(bool set); 
  void EKCheck(bool set); 
  void GammaCheck(bool set); 
  void BrhoCheck(bool set); 

  void momentumChanged(); 
  void ETChanged(); 
  void EKChanged(); 
  void GammaChanged(); 
  void BrhoChanged(); 
  void updateBeamParameters();

  virtual void _cancel();

 private:

  std::list<std::string>* _bmllist; 

  double _momentum;
  double _et;
  double _ek;
  double _gamma;
  double _brho;

};



#endif
