/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:  LBFncData.cpp                                         ****** 
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

#include<sqlite/query.hpp> 

#include<DataBrowser.h>   

#include<LBFncData.h>   // WARNING qobject #defines emit ! 
#include<LFDataTable.h> // WARNING qobject #defines emit ! 

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LBFncData::LBFncData( sqlite::connection& db, double const& horTune, double const& verTune ) 
: PlotData( db )
{

  setTunes( -1.0, -1.0);
  // setTunes( horTune, verTune );

  char const*      plot_attributes[]  = { "Generalized (4D) Courant-Snyder Lattice Functions", "Arc Length [m]", "Beta [m]",  "Beta [m]" }; 

  CurveData::curve_attribute  curve_attributes[] = 
          { {"<hor|beta|hor> (beta_1x ) ",                CurveData::yLeft,   CurveData::Color(255,0,0)}, 
	    {"<ver|beta|hor> (beta_1y ) ",                CurveData::yLeft,   CurveData::Color(0,0,0)  },
            {"<hor|beta|ver> (beta_2x ) ",                CurveData::yRight,  CurveData::Color(0,0,0)  },
            {"<ver|beta|ver> (beta_2y ) ",                CurveData::yRight,  CurveData::Color(255,0,0)}  };
  
  char const*  sql[] =  
   { "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT",
     "SELECT arclength, beta1x, beta1y, beta2x, beta2y, alpha1x, alpha1y, psi1, alpha2x, alpha2y, psi2 "
     "FROM REFERENCE_ORBIT, COURANT_SNYDER_4D WHERE (REFERENCE_ORBIT.iseq = COURANT_SNYDER_4D.iseq)" 
   };
 
  int ncrvs        = sizeof( curve_attributes ) / sizeof(CurveData::curve_attribute );
  int npltattribs  = sizeof( plot_attributes )  / sizeof(char const *);

  init( db, npltattribs, plot_attributes, ncrvs, curve_attributes, sql );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LBFncData::~LBFncData()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

