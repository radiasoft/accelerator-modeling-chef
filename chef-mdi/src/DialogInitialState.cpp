/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DialogInitialState.cpp                            ****** 
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
***************************************************************************
***************************************************************************
**************************************************************************/

#include <DialogInitialState.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qpushbutton.h>

DialogInitialState::DialogInitialState(QWidget* parent, const char* name, WFlags f)
 : DialogInitialStateBase(parent,name,f)
{

  connect( pushButton_ok,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( pushButton_cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );


  QDoubleValidator* positiveValidator = new QDoubleValidator(0.0, 1.0e9, 6, this, "posValidator");  
  QDoubleValidator* dbleValidator     = new QDoubleValidator(this, "dbleValidator");  

  lineEdit_x->setValidator( dbleValidator );
  lineEdit_y->setValidator( dbleValidator );

  lineEdit_cdt->setValidator( dbleValidator );
  lineEdit_npx->setValidator( dbleValidator );

  lineEdit_npy->setValidator( dbleValidator );
  lineEdit_ndp->setValidator( positiveValidator );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector DialogInitialState::values() const
{


  Vector state(6);

  state[0]       = lineEdit_x->text().toDouble();
  state[1]       = lineEdit_y->text().toDouble();
  state[2]       = lineEdit_cdt->text().toDouble();

  state[3]       = lineEdit_npx->text().toDouble();
  state[4]       = lineEdit_npy->text().toDouble();
  state[5]       = lineEdit_ndp->text().toDouble();


  return state;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DialogInitialState:: setValues( Vector const& state) 
{

  QString s("%1"); 

    lineEdit_x->setText(  s.arg( state[0]) );
    lineEdit_y->setText(  s.arg( state[1]) );
  lineEdit_cdt->setText(  s.arg( state[2]) );

  lineEdit_npx->setText(  s.arg( state[3]) );
  lineEdit_npy->setText(  s.arg( state[4]) );
  lineEdit_ndp->setText(  s.arg( state[5]) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

