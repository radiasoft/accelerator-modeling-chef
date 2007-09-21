/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      InitCondDialogLF.cpp                              ****** 
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

#include <InitCondDialogLF.h>
#include <LattFuncSage.h>
#include <qvalidator.h>
#include <qlineedit.h>

InitCondDialogLF::InitCondDialogLF(QWidget* parent, const char* name, WFlags f)
 : InitCondDialogLFBase(parent,name,f)
{
  QDoubleValidator* positiveValidator = new QDoubleValidator(0.0, 1.0e9, 6, this, "betaValidator");  
  QDoubleValidator* dbleValidator     = new QDoubleValidator(this, "dbleValidator");  

  hBetaLineEdit->setValidator( positiveValidator );
  vBetaLineEdit->setValidator( positiveValidator );

  hAlphaLineEdit->setValidator( dbleValidator );
  vAlphaLineEdit->setValidator( dbleValidator );

  hEtaLineEdit->setValidator( dbleValidator );
  vEtaLineEdit->setValidator( dbleValidator );

  hEtaPrimeLineEdit->setValidator( dbleValidator );
  vEtaPrimeLineEdit->setValidator( dbleValidator );

  hPsiLineEdit->setValidator( dbleValidator );
  vPsiLineEdit->setValidator( dbleValidator );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFuncSage::lattFunc InitCondDialogLF::getInitCond() const
{

  LattFuncSage::lattFunc lf;

  lf.beta.hor       = hBetaLineEdit->text().toDouble();
  lf.beta.ver       = vBetaLineEdit->text().toDouble();
  lf.alpha.hor      = hAlphaLineEdit->text().toDouble();
  lf.alpha.ver      = vAlphaLineEdit->text().toDouble();

  lf.dispersion.hor = hEtaLineEdit->text().toDouble();
  lf.dispersion.ver = vEtaLineEdit->text().toDouble();

  lf.dPrime.hor     = hEtaPrimeLineEdit->text().toDouble();
  lf.dPrime.ver     = vEtaPrimeLineEdit->text().toDouble();

  lf.psi.hor        = hPsiLineEdit->text().toDouble();
  lf.psi.ver        = vPsiLineEdit->text().toDouble();  

  return lf;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void InitCondDialogLF:: setInitCond( LattFuncSage::lattFunc const& lf) 
{

  QString s("%1"); 

  hBetaLineEdit->setText(  s.arg(lf.beta.hor) );
  vBetaLineEdit->setText(  s.arg(lf.beta.ver) );
  hAlphaLineEdit->setText( s.arg(lf.alpha.hor) );
  vAlphaLineEdit->setText( s.arg(lf.alpha.ver) );
						       
  hEtaLineEdit->setText( s.arg(lf.dispersion.hor) );
  vEtaLineEdit->setText( s.arg(lf.dispersion.ver) );
				
  hEtaPrimeLineEdit->setText( s.arg(lf.dPrime.hor) );
  vEtaPrimeLineEdit->setText( s.arg(lf.dPrime.ver) );
						       
  hPsiLineEdit->setText( s.arg( lf.psi.hor ) );
  vPsiLineEdit->setText( s.arg( lf.psi.ver ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
InitCondDialogLF::dispersionMode()
{


 hBetaLineEdit->setEnabled( false );
 vBetaLineEdit->setEnabled( false );

 hAlphaLineEdit->setEnabled( false);
 vAlphaLineEdit->setEnabled( false);

 hEtaLineEdit->setEnabled(true);
 vEtaLineEdit->setEnabled(true);

 hEtaPrimeLineEdit->setEnabled(true);
 vEtaPrimeLineEdit->setEnabled(true );

 hPsiLineEdit->setEnabled(false);
 vPsiLineEdit->setEnabled(false);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
InitCondDialogLF::ULFMode()
{

 hBetaLineEdit->setEnabled( true );
 vBetaLineEdit->setEnabled( true );
 hAlphaLineEdit->setEnabled( true);
 vAlphaLineEdit->setEnabled( true);

 hEtaLineEdit->setEnabled(false);
 vEtaLineEdit->setEnabled(false);

 hEtaPrimeLineEdit->setEnabled(false);
 vEtaPrimeLineEdit->setEnabled(false );

 hPsiLineEdit->setEnabled(false);
 vPsiLineEdit->setEnabled(false);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
InitCondDialogLF::momentsMode()
{

 hBetaLineEdit->setEnabled( true );
 vBetaLineEdit->setEnabled( true );
 hAlphaLineEdit->setEnabled( true );
 vAlphaLineEdit->setEnabled( true);

 hEtaLineEdit->setEnabled(false);
 vEtaLineEdit->setEnabled(false);

 hEtaPrimeLineEdit->setEnabled(false);
 vEtaPrimeLineEdit->setEnabled(false );

 hPsiLineEdit->setEnabled(false);
 vPsiLineEdit->setEnabled(false);

}
