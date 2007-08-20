/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:  BunchData.cpp                                         ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:                                                      ******
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

#include <BunchData.h>
#include <beamline/ParticleBunch.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchData::BunchData( ParticleBunch const& b)
: PlotData() 
{
    std::vector<double>    x;
    std::vector<double>    y;
    std::vector<double>    z;
    std::vector<double>    px;
    std::vector<double>    py;
    std::vector<double>    pz;

    for ( ParticleBunch::const_iterator it  = b.begin();
	  it != b.end(); ++it ) {
     
     Vector const& state = it->State(); 

      x.push_back( state[0] ); 
      y.push_back( state[1] ); 
      z.push_back( state[2] ); 
     px.push_back( state[3] ); 
     py.push_back( state[4] ); 
     pz.push_back( state[5] ); 

    }

  CurveData  c1(  x,  y,  "x vs y"  );

  c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1.setColor( CurveData::Color( 0, 0, 0));
  c1.setConnected(false);

  addCurve( c1 );

  setXLabel( "X vs Y "                     );
  setYLabel( CurveData::yLeft,  "X [m]"    );
  setYLabel( CurveData::yRight, "Y [m]"    );


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchData::~BunchData() 
{ } 

