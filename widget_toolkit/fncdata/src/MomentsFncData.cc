/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:  CHEFGUI.cpp                                           ****** 
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

#include <sqlite/query.hpp>
#include <MomentsFncData.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsFncData::MomentsFncData(   sqlite::connection& db,
                                  double const& horTune, double const& verTune )
: PlotData( db) 
{
  setTunes( horTune, verTune );


  char const*      plot_attributes[]  = { "Covariance",      "Arc Length [m]", "Beta [m]",  "Alpha" };        

  CurveData::curve_attribute  curve_attributes[] = 
             { {"Horizontal Beta",    CurveData::yLeft,   CurveData::Color(0,0,0)}, 
	       {"Vertical Beta",      CurveData::yLeft,   CurveData::Color(0,0,0)},
               {"Horizontal Alpha",   CurveData::yRight,  CurveData::Color(0,0,0)},
               {"Vertical Alpha",     CurveData::yRight,  CurveData::Color(0,0,0)},
             };
  
  char const*  sql[] =  
   { "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT",
     "SELECT arclength, beta_x, beta_y, alpha_x, alpha_y  FROM "
     "REFERENCE_ORBIT, COVARIANCE WHERE (REFERENCE_ORBIT.iseq = COVARIANCE.iseq)" 
   };
 
  int ncrvs        = sizeof( curve_attributes ) / sizeof(CurveData::curve_attribute );
  int npltattribs  = sizeof( plot_attributes )  / sizeof(char const *);

  init( db, npltattribs, plot_attributes, ncrvs, curve_attributes, sql );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsFncData::~MomentsFncData() 
{ } 

