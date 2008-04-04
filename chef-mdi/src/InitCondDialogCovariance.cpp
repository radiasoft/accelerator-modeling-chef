/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      InitCondDialogCovariance.cpp                      ****** 
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

#include <basic_toolkit/Matrix.h>
#include <InitCondDialogLF.h>
#include <qvalidator.h>
#include <qlineedit.h>

InitCondDialogCovariance::InitCondDialogCovariance(QWidget* parent, const char* name, WFlags f)
 : InitCondDialogCovarianceBase(parent,name,f)
{
  QDoubleValidator* positiveValidator = new QDoubleValidator(0.0, 1.0e9, 6, this, "betaValidator");  
  QDoubleValidator* dbleValidator     = new QDoubleValidator(this, "dbleValidator");  

  x_xLineEdit->setValidator( positiveValidator );
  y_yLineEdit->setValidator( positiveValidator );
 
  npx_npxLineEdit->setValidator( positiveValidator );
  npy_npyLineEdit->setValidator( positiveValidator );

    x_npxLineEdit->setValidator( dbleValidator );
    x_npyLineEdit->setValidator( dbleValidator );
    y_npxLineEdit->setValidator( dbleValidator );
  npx_npyLineEdit->setValidator( dbleValidator );
      x_yLineEdit->setValidator( dbleValidator );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFuncs InitCondDialogCovariance::getInitCond() const
{

  LattFuncs info;

  // FIXME !
#if 0
  Matrix& covariance = info.covariance;

  covariance(0,0)                         =      x_xLineEdit->text().toDouble();
  covariance(0,1)    =   covariance(1,0)  =    x_npxLineEdit->text().toDouble();
  covariance(1,1)                         =  npx_npxLineEdit->text().toDouble();

  covariance(2,2)                         =     y_yLineEdit->text().toDouble();
  covariance(2,3)    =   covariance(3,2)  =   y_npyLineEdit->text().toDouble();
  covariance(3,3)                         = npy_npyLineEdit->text().toDouble();

  covariance(0,2)    =   covariance(2,0)  =    x_yLineEdit->text().toDouble();
  covariance(0,3)    =   covariance(0,3)  =  x_npyLineEdit->text().toDouble();
  covariance(2,1)    =   covariance(1,2)  =  y_npxLineEdit->text().toDouble();
#endif

  return info;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void InitCondDialogCovariance:: setInitCond( LattFuncs const& info) 
{
 
  // FIXME !

 #if 0 
 Matrix const& covariance = info.covariance;

  QString s("%1"); 

      x_xLineEdit->setText(  s.arg( covariance(0,0)) );
    x_npxLineEdit->setText(  s.arg( covariance(0,1)) );
  npx_npxLineEdit->setText(  s.arg( covariance(1,1)) );

      y_yLineEdit->setText(  s.arg( covariance(2,2)) );
    y_npyLineEdit->setText(  s.arg( covariance(2,3)) );
  npy_npyLineEdit->setText(  s.arg( covariance(3,3)) );

      x_yLineEdit->setText(  s.arg( covariance(1,2)) );
    x_npyLineEdit->setText(  s.arg( covariance(1,3)) );
    y_npxLineEdit->setText(  s.arg( covariance(2,1)) );
#endif

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

