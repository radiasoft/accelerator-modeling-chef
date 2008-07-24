/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandEigenModes.cpp                             ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
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
#include <CommandEigenmodes.h>

#include<string>
#include<vector>
#include<basic_toolkit/GenericException.h>
#include<physics_toolkit/BeamlineContext.h>
#include<physics_toolkit/LBSage.h>
#include<CHEFPlotMain.h>
#include<LBFncData.h>
#include<qwidget.h>
#include <qmessagebox.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QWidget* CommandEigenmodes::operator()( QWidget* parent, BmlContextPtr bml ) 
{ 

  bml->periodicCourantSnyder4D();

  CHEFPlotMain* plot =  new CHEFPlotMain(  parent, "plotWidget", Qt::WDestructiveClose );

  string caption = "CHEF:  Lattice Functions (EigenModes): " + string( bml->Name() );

  plot->setCaption( caption.c_str() );
  plot->setGeometry(0,0, parent->width(), parent->height() );
  plot->setAutoClear(true);


  LBFncData data(   bml->dbConnection(), 
                    bml->getHTune(),
                    bml->getVTune() );
  plot->addData( data );

  return plot;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
