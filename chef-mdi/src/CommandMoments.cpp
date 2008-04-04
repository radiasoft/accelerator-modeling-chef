/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandMoments.cpp                                ****** 
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
#include <CommandMoments.h>

#include<string>
#include<vector>
#include<basic_toolkit/GenericException.h>
#include<physics_toolkit/BeamlineContext.h>
#include<CHEFPlotMain.h>
#include<MomentsFncData.h>
#include<qwidget.h>
#include <qmessagebox.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QWidget* CommandMoments::operator()( QWidget* parent, BmlContextPtr const& context ) 
{ 
  if( context->isTreatedAsRing() ) {
    CHEFPlotMain* plot =  new CHEFPlotMain(  parent, "plotWidget", Qt::WDestructiveClose );

    string caption = "CHEF:  Lattice Functions (Moments): " + string( context->name() );

    plot->setCaption( caption.c_str() );
    plot->setGeometry(0,0, parent->width(), parent->height() );
    plot->setAutoClear(true);

      context->computeCovariance();

      MomentsFncData data(   context->dbConnection()
                         , context->getHTune()
                         , context->getVTune()
                         , context->cheatBmlPtr()            );

    plot->addData( data );
    return plot;
  }
  else {
    ostringstream uic;
    uic  <<   "This command is meant for a periodic structure.";
    QMessageBox::critical(   0
                           , QString( "ERROR: NOT RING" )
                           , QString( uic.str().c_str() )
                           , QMessageBox::Ok
                           , QMessageBox::NoButton
                           , QMessageBox::NoButton        );
    throw GenericException( __FILE__, __LINE__,
                            "CommandMoments::operator()(...)",
                            uic.str().c_str() );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
