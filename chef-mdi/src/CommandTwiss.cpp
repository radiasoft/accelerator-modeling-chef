/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CommandTwiss.cpp                                  ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:    Leo Michelotti                                   ******
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

#include <CommandTwiss.h>

#include<string>
#include<vector>
#include<beamline/ParticleBunch.h>
#include<physics_toolkit/BeamlineContext.h>
#include<CHEFPlotMain.h>
#include<LattFncData.h>
#include<LFDataTable.h>
#include<qwidget.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QWidget* CommandTwiss::operator()( QWidget* parent, BmlContextPtr const& context ) 
{ 
   CHEFPlotMain* plot =  new CHEFPlotMain(  parent, "plotWidget", Qt::WDestructiveClose );

   string caption = "CHEF:  Twiss Functions (uncoupled): " + string( context->name() );

   plot->setCaption( caption.c_str() );
   plot->setGeometry(0,0, parent->width(), parent->height() );
   plot->setAutoClear(true);

   context->computeCourantSnyder();

   if( context->isTreatedAsRing() ) {

     LattFncData data(   context->dbConnection()

                       , context->getHTune()
                       , context->getVTune()
                       , context->cheatBmlPtr()           );
     plot->addData(data);
   }
   else {
     LattFncData data(   context->dbConnection()
                       , -1.0
                       , -1.0
                       , context->cheatBmlPtr()           );
     plot->addData(data);
   }


   //   ( new LFDataTable( context->dbConnection(), parent, "Twiss Functions (uncoupled)" ) )->show();

   return plot;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
