/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandPropagateMoments.cpp                       ****** 
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
#include <CommandPropagateMoments.h>

#include <MomentsFncData.h>
#include <CHEFPlotMain.h>

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/CovarianceSage.h>
#include <physics_toolkit/BmlUtil.h>

#include <string>
#include <vector>

using namespace std;

QWidget* CommandPropagateMoments::operator()( QWidget* parent, BmlContextPtr& context, LattFuncSage::lattFunc const& initial )
{

    CovarianceSage::Info initialCovariance;

    initialCovariance.arcLength = 0.0;
    initialCovariance.beta.hor  = initial.beta.hor;
    initialCovariance.beta.ver  = initial.beta.ver;
    initialCovariance.alpha.hor = initial.alpha.hor;
    initialCovariance.alpha.ver = initial.alpha.ver;

    // ------------------------------------------------------------------------------------------------------------
    // * The conversion from lattice function to covariance assumes equal horizontal and vertical emittance.
    // * these emiitances (or their ratio, should be part of the use input )
    // * FIXME !!!!!
    //---------------------------------------------------------------------------------------------------------------
 
    int errcode = BmlUtil::makeCovariance( initialCovariance, context->getParticle() );

    if ( errcode != 0 ) throw GenericException( __FILE__, __LINE__, " CommandPropagateMoments::operator()",  "Call to makeCovariance failed." );
        

    context->setInitialCovariance( initialCovariance );
 
    MomentsFncData data( context->getCovarianceArray(),  context->cheatBmlPtr() );

    CHEFPlotMain* plotWidget = new CHEFPlotMain( parent, "MMplotWidget", Qt::WDestructiveClose );

    string theCaption("CHEF: Lattice Functions (covariance): " );

    theCaption += string( context->name() );

    plotWidget->setCaption( theCaption );
    plotWidget->setGeometry(0,0, parent->width(), parent->height() );
    plotWidget->setAutoClear(true);
    plotWidget->addData( data );
  
    return plotWidget;

}
