/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandEdwardsTeng.cpp                            ****** 
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
#include <CommandEdwardsTeng.h>

#include<string>
#include<vector>
#include<physics_toolkit/BeamlineContext.h>
#include<physics_toolkit/EdwardsTengSage.h>
#include<CHEFPlotMain.h>
#include<ETFncData.h>
#include<qwidget.h>
#include <qmessagebox.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QWidget* CommandEdwardsTeng::operator()( QWidget* parent, BmlContextPtr const& context ) 
{ 
  if( context->isTreatedAsRing() ) {
    CHEFPlotMain* plot =  new CHEFPlotMain(  parent, "plotWidget", Qt::WDestructiveClose );

    string caption = "CHEF: Lattice Functions (Factorization): " + string( context->name() );

    plot->setCaption( caption.c_str() );
    plot->setGeometry(0,0, parent->width(), parent->height() );
    plot->setAutoClear(true);

    ETFncData data(   context->getETArray()
                    , context->getHorizontalEigenTune()
                    , context->getVerticalEigenTune()
                    , context->cheatBmlPtr()             );
    plot->addData(data);
    return plot;
  }
  else {
    ostringstream uic;
    uic  <<   "Can only compute Edward-Teng lattice functions"
            "\nfor a periodic structure.";
    QMessageBox::critical(   0
                           , QString( "ERROR: NOT RING" )
                           , QString( uic.str().c_str() )
                           , QMessageBox::Ok
                           , QMessageBox::NoButton
                           , QMessageBox::NoButton        );
    throw GenericException( __FILE__, __LINE__,
                            "CommandEdwardsTeng::operator()(...)",
                            uic.str().c_str() );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
