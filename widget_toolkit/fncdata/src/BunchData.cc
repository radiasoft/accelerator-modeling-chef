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
#include <beamline/TBunch.h>

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

  int npts = b.size();
  CurveData  c1( npts,  "x vs y"  );

  CurveData::iterator  it1 = c1.begin();

  for ( ParticleBunch::const_iterator it  = b.begin();
	  it != b.end(); ++it ) {
     
     Vector const& state = it->State(); 

     double   x = state[0]; 
     double   y = state[1]; 
     double   z = state[2]; 
     double npx = state[3]; 
     double npy = state[4]; 
     double npz = state[5]; 

    *(it1++) = CurveData::Point(x,y);

  }


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

