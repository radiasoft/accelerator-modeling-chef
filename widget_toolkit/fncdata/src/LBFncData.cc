/**************************************************************************
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
**************************************************************************/


#include <LBFncData.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LBFncData::LBFncData( std::vector<LBSage::Info> const&  lb_vec, ConstBmlPtr bml ) 
{

  std::vector<double>   azimuth;

  std::vector<double>   beta_1x;
  std::vector<double>   beta_1y;

  std::vector<double>   beta_2x;
  std::vector<double>   beta_2y;

  for ( std::vector<LBSage::Info>::const_iterator it  = lb_vec.begin(); 
                                                  it != lb_vec.end(); ++it ) {

       azimuth.push_back( it->arcLength );

       beta_1x.push_back( it->beta_1x   );
       beta_1y.push_back( it->beta_1y   );

       beta_2x.push_back( it->beta_2x   );
       beta_2y.push_back( it->beta_2y   );

  }

  CurveData  c1x( azimuth, beta_1x,  "<hor|beta|hor>"   );
  CurveData  c1y( azimuth, beta_1y,  "<ver|beta|hor>"   );
  CurveData  c2x( azimuth, beta_2x,  "<hor|beta|ver>"   );
  CurveData  c2y( azimuth, beta_2y,  "<ver|beta|ver>"   );

  c1x.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1x.setColor( CurveData::Color(0,0,0) );

  c2x.setAxes( CurveData::xBottom, CurveData::yRight );
  c2x.setColor(  CurveData::Color(255, 0, 0) );

  c2y.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c2y.setColor(  CurveData::Color(255, 0, 0) );


  c1y.setAxes( CurveData::xBottom, CurveData::yRight );
  c1y.setColor( CurveData::Color(0,0,0) );

  addCurve( c1x );
  addCurve( c2x );
  addCurve( c1y );
  addCurve( c2y );
  

  setXLabel( "Arc Length [m]" );
  setYLabel(  CurveData::yLeft,  "Beta [m]" );
  setYLabel(  CurveData::yRight, "Beta [m]" );

  setBeamline( bml ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LBFncData::~LBFncData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

