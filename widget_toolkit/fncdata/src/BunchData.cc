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
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>

using namespace std;

namespace {

  typedef PhaseSpaceIndexing::index index;

  index i_x   = Particle::i_x;
  index i_y   = Particle::i_y;
  index i_cdt = Particle::i_cdt;
  index i_npx = Particle::i_npx;
  index i_npy = Particle::i_npy;
  index i_ndp = Particle::i_ndp;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchData::BunchData( ParticleBunch const& b)
: PlotData() 
{
    std::vector<double>    x;
    std::vector<double>    npx;

    std::vector<double>    y;
    std::vector<double>    npy;

    std::vector<double>    z;
    std::vector<double>    npz;

  int npts = b.size();
  CurveData  c1( npts,  "x vs y"  );

  CurveData::iterator  it1 = c1.begin();

  for ( ParticleBunch::const_iterator it  = b.begin();
	  it != b.end(); ++it ) {
     
     Vector const& state = it->state(); 

     double   x = state[i_x]; 
     double   y = state[i_y]; 
     double   z = state[i_cdt]; 
     double npx = state[i_npx]; 
     double npy = state[i_npy]; 
     double npz = state[i_ndp]; 

    *(it1++) = CurveData::Point(x,y);

  }


  c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1.setColor( CurveData::Color( 0, 0, 0));
  c1.setConnected(false);

  addCurve( c1 );

  setXLabel( "x vs x' "                    );
  setYLabel( CurveData::yLeft,  "x [m]"    );
  setYLabel( CurveData::yRight, "x'[m]"    );
  //  setYLabel( CurveData::yRight, "Y [m]"    );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchData::~BunchData() 
{ } 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
