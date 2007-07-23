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

#include <MomentsFncData.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsFncData::MomentsFncData( std::vector<CovarianceSage::Info> const& covar_vec, ConstBmlPtr bml )
: PlotData() 
{
    std::vector<double>    azimuth;
    std::vector<double>    beta_H;
    std::vector<double>    alpha_H;
    std::vector<double>    beta_V;
    std::vector<double>    alpha_V;

  for (std::vector<CovarianceSage::Info>::const_iterator it  = covar_vec.begin(); 
                                                         it != covar_vec.end(); ++it ) {

       azimuth.push_back(	it->arcLength       );

        beta_H.push_back(       it->beta.hor        );
        beta_V.push_back(       it->beta.ver        );

       alpha_H.push_back(       it->alpha.hor       );
       alpha_V.push_back(       it->alpha.ver       );

  }

  CurveData  c1(  azimuth, beta_H,  "Horizontal Beta"  );
  CurveData  c2(  azimuth, beta_V,  "Vertical Beta"    );

  CurveData  c3(  azimuth, alpha_H, "Horizontal Alpha" );
  CurveData  c4(  azimuth, alpha_V, "Vertical Alpha"   );

  c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c1.setColor( CurveData::Color( 0, 0, 0));

  c2.setAxes( CurveData::xBottom, CurveData::yLeft  );
  c2.setColor( CurveData::Color( 255, 0, 0));

  c3.setAxes( CurveData::xBottom, CurveData::yRight );
  c3.setColor( CurveData::Color( 0, 0, 0));

  c4.setAxes( CurveData::xBottom, CurveData::yRight );
  c4.setColor( CurveData::Color(  255, 0, 0));

  addCurve( c1 );
  addCurve( c2 );
  addCurve( c3 );
  addCurve( c4 );

  setXLabel( "Arc Length [m]"              );
  setYLabel( CurveData::yLeft,  "Beta [m]" );
  setYLabel( CurveData::yRight, "Alpha"    );

  setBeamline( bml ); 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MomentsFncData::~MomentsFncData() 
{ } 

