/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandPropagateDispersion.cpp                    ****** 
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
#include <CommandPropagateDispersion.h>

#include <DspnFncData.h>
#include <CHEFPlotMain.h>
#include <physics_toolkit/DispersionSage.h>

#include <string>
#include <vector>

using namespace std;

QWidget* CommandPropagateDispersion::operator()( QWidget* parent, BmlContextPtr& context, LattFuncSage::lattFunc const& initial) 

{

    DispersionSage::Info initialDispersion;
    initialDispersion.arcLength      = 0.0;
    initialDispersion.dispersion.hor = initial.dispersion.hor;
    initialDispersion.dispersion.ver = initial.dispersion.ver;
    initialDispersion.dPrime.hor     = initial.dPrime.hor;
    initialDispersion.dPrime.ver     = initial.dPrime.ver;

    context->setInitialDispersion( initialDispersion );

    DspnFncData dfd( context->getDispersionArray() );

    CHEFPlotMain* plotWidget = new CHEFPlotMain( parent, "DspnplotWidget", Qt::WDestructiveClose );
 
    string  theCaption("CHEF: Dispersion: " );
    
    theCaption += string( context->name() );
    
    plotWidget->setCaption( theCaption );
    plotWidget->setGeometry(0,0, parent->width(), parent->height() );
    plotWidget->setAutoClear(true);
    plotWidget->addData( dfd  );

    return plotWidget;
 }

