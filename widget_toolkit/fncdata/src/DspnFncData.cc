/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      GeneratePlotData.cpp                              ******
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
#include <boost/scoped_ptr.hpp>
#include <DspnFncData.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DspnFncData::DspnFncData( sqlite::connection& db )
  : PlotData(db)
{
  
  char const*      plot_attributes[]  = { "Dispersion",      "Arc Length [m]", "Dispersion [m]",  "Dispersion Derivative" };        

  CurveData::curve_attribute  curve_attributes[] = 
             { {"Hor Dispersion",                CurveData::yLeft,   CurveData::Color(0,0,0)}, 
	       {"Ver Dispersion",                CurveData::yLeft,   CurveData::Color(0,0,0)},
               {"Hor Dispersion Derivative",     CurveData::yRight,  CurveData::Color(0,0,0)},
               {"Ver Dispersion Derivative",     CurveData::yRight,  CurveData::Color(0,0,0)}
             };
  
  char const*  sql[] =  
   { "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT",
     "SELECT arclength, etax, etay, etapx, etapy FROM REFERENCE_ORBIT, DISPERSION WHERE ( REFERENCE_ORBIT.iseq = DISPERSION.iseq )"
   };
 
  int ncrvs        = sizeof( curve_attributes ) / sizeof(CurveData::curve_attribute );
  int npltattribs  = sizeof( plot_attributes )  / sizeof(char const *);

  init( db, npltattribs, plot_attributes, ncrvs, curve_attributes, sql );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DspnFncData::~DspnFncData()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


