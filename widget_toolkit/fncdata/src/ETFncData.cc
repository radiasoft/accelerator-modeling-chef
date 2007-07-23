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
**************************************************************************/

#include <ETFncData.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ETFncData::ETFncData( std::vector<EdwardsTengSage::Info> const& et_vec, ConstBmlPtr bml)
{

   std::vector<double>      azimuth;

   std::vector<double>       beta_H;
   std::vector<double>       beta_V;

   std::vector<double>      alpha_H;
   std::vector<double>      alpha_V;

  for ( std::vector<EdwardsTengSage::Info>::const_iterator it  = et_vec.begin();  
	                                                   it != et_vec.end();  ++it) {


        azimuth.push_back(     it->arcLength      );
         beta_H.push_back(     it->beta.hor       );
        alpha_H.push_back(     it->alpha.hor      );

         beta_V.push_back(     it->beta.ver       );
        alpha_V.push_back(     it->alpha.ver      );

  }


  CurveData c1( azimuth,  beta_H, "Horizontal Beta" );
  CurveData c2( azimuth,  beta_V, "Vertical Beta"   );

  c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1.setColor(  CurveData::Color( 0, 0, 0) ); //

  c2.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c2.setColor(  CurveData::Color( 255, 0, 0) ); //

  addCurve( c1 );
  addCurve( c2 );

  setXLabel( "Arc Length [m]"                    );
  setYLabel(  CurveData::yLeft, "Beta [m]"       );

  setBeamline( bml ); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ETFncData::~ETFncData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

