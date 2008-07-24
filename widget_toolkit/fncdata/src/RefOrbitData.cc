/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      RefOrbitData.cpp                                  ******
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
#include <RefOrbitData.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RefOrbitData::RefOrbitData( sqlite::connection& db )
  : PlotData(db)
{
  
  char const*      plot_attributes[]  = { "Reference Orbit ", "Arc Length [m]", "Hor Orbit [mm]", "Ver Orbit [mm] "};        

  CurveData::curve_attribute  curve_attributes[] = 
             { 
               {"Hor Reference Orbit",           CurveData::yLeft,    CurveData::Color(0,0,0)},
               {"Ver Reference Orbit",           CurveData::yRight,   CurveData::Color(1,0,0)} 
             };
  
  char const*  sql[] =  
   { "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT",
     "SELECT arclength, xco*1.0e3, yco*1.0e3  FROM REFERENCE_ORBIT "
   };
 
  int ncrvs        = sizeof( curve_attributes ) / sizeof(CurveData::curve_attribute );
  int npltattribs  = sizeof( plot_attributes )  / sizeof(char const *);

  init( db, npltattribs, plot_attributes, ncrvs, curve_attributes, sql );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RefOrbitData::~RefOrbitData()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


