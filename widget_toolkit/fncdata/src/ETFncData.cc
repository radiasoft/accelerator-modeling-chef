/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      ETFFncData.cpp                                    ****** 
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
#include <ETFncData.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ETFncData::ETFncData(   sqlite::connection& db, double const& horTune, double const& verTune )
: PlotData( db )
{
  setTunes( horTune, verTune );

  std::ostringstream sql;

  sql << "SELECT COUNT(*) AS NELMS FROM REFERENCE_ORBIT" << ends;

  sqlite::query q1(db,sql.str() );

  sqlite::result_type res1 = q1.emit_result();

  int nelms =  res1 ? res1->get_int(0) : 0;

  CurveData c1( nelms, "Horizontal Beta" );
  CurveData c2( nelms, "Vertical Beta"   );

  sql.str("");

  sql << "SELECT arclength, beta_h, beta_v, alpha_h, alpha_v FROM "
      << "REFERENCE_ORBIT, EDWARDS_TENG WHERE (REFERENCE_ORBIT.iseq = EDWARDS_TENG.iseq)" 
      << ends;

  sqlite::query q2(db,sql.str() ); 
  sqlite::result_type res2 = q2.emit_result();

  CurveData::iterator it1 = c1.begin();
  CurveData::iterator it2 = c2.begin();

  if (res2) { 
    do { 

      double azimuth = res2->get_double(0);
      double betaH   = res2->get_double(1); // beta_x;
      double  betaV  = res2->get_double(2); // beta_y;
      double alphaH  = res2->get_double(3); // alpha_x;
      double alphaV  = res2->get_double(4); // alpha_y;

      *(it1++) = CurveData::Point(azimuth, betaH);
      *(it2++) = CurveData::Point(azimuth, betaV);

    } while ( res2->next_row() );
  };


  c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1.setColor(  CurveData::Color( 0, 0, 0) ); //

  c2.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c2.setColor(  CurveData::Color( 255, 0, 0) ); //

  addCurve( c1 );
  addCurve( c2 );

  setXLabel( "Arc Length [m]"                    );
  setYLabel(  CurveData::yLeft, "Beta [m]"       );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ETFncData::~ETFncData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

