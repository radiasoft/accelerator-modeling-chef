/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      LattFncData.cpp                                   ******
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
******                                                               ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
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

#include <sstream>
#include <sqlite/query.hpp>
#include <LattFncData.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFncData::LattFncData(   sqlite::connection& db, 
                            double const& horTune, double const& verTune ) 
: PlotData( db )
{

  char const*      plot_attributes[]  = { "Dispersion",   "Arc Length [m]", "Beta [m], Alpha ",  "Phase Advance" };        

  CurveData::curve_attribute  curve_attributes[] = 
             { {"Hor Beta",    CurveData::yLeft,    CurveData::Color(255,0,0)}, 
	       {"Ver Alpha",   CurveData::yLeft,    CurveData::Color(255,0,0)},
               {"Hor Psi",     CurveData::yRight,   CurveData::Color(255,0,0)},
               {"Ver Beta",    CurveData::yLeft,    CurveData::Color(0,0,255)},
               {"Hor Alpha",   CurveData::yLeft,    CurveData::Color(0,0,255)},
               {"Ver Psi",     CurveData::yRight,   CurveData::Color(0,0,255)} 
             };
  
  char const*  sql[] =  
   { "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT",
     "SELECT arclength, beta_x, alpha_x, psi_x,  beta_y, alpha_y, psi_y FROM "
     "REFERENCE_ORBIT, COURANT_SNYDER WHERE (REFERENCE_ORBIT.iseq = COURANT_SNYDER.iseq)" 
   };
 
  int ncrvs        = sizeof( curve_attributes ) / sizeof(CurveData::curve_attribute );
  int npltattribs  = sizeof( plot_attributes )  / sizeof(char const *);

  init( db, npltattribs, plot_attributes, ncrvs, curve_attributes, sql );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFncData::~LattFncData()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


