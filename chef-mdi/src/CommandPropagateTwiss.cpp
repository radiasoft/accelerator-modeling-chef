/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandPropagateTwiss.cpp                         ****** 
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

#include <CommandPropagateTwiss.h>
#include <physics_toolkit/LattFuncSage.h>
#include <vector>
#include <physics_toolkit/LattFuncSage.h>
#include <LattFncData.h>
#include <CHEFPlotMain.h>

using namespace std;

QWidget* CommandPropagateTwiss::operator()( QWidget* parent, BmlContextPtr& context, LattFuncSage::lattFunc const& initialConditions ) 
{
    context->setInitialTwiss( initialConditions );

    CHEFPlotMain* plotWidget = new CHEFPlotMain( parent, "PlotWidget", Qt::WDestructiveClose );

    if( context->isTreatedAsRing() ) {
      LattFncData lfd(   context->getTwissArray()
                       , context->getHorizontalFracTune()
                       , context->getVerticalFracTune()
                       , context->cheatBmlPtr()            );
      plotWidget->addData( lfd  );
    }
    else {
      LattFncData lfd(   context->getTwissArray()
                       , -1.0
                       , -1.0
                       , context->cheatBmlPtr()            );
      plotWidget->addData( lfd  );
    }

    string theCaption("CHEF: Uncoupled Lattice Functions: ");
    theCaption += string( context->name() );
    plotWidget->setCaption( theCaption.c_str() );

    plotWidget->setGeometry(0,0, parent->width(), parent->height() );
    plotWidget->show();

    return plotWidget;
}
