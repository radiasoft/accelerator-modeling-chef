/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DspnFncData.cc                                    ****** 
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


#include <DspnFncData.h>


using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DspnFncData::DspnFncData( std::vector<DispersionSage::Info> const& disp_vec, ConstBmlPtr bml )
{

   std::vector<double>     azimuth;
   std::vector<double>       clo_H;
   std::vector<double>       clo_V;
   std::vector<double>      disp_H;
   std::vector<double>      disp_V;
   std::vector<double>    dPrime_H;
   std::vector<double>    dPrime_V;

  const double mm = 0.001; 
 
  for ( std::vector<DispersionSage::Info>::const_iterator it  =  disp_vec.begin(); 
                                                          it !=  disp_vec.end(); ++it ) 
  {

       azimuth.push_back(   it->arcLength             );
         clo_H.push_back(   it->closedOrbit.hor / mm  );
         clo_V.push_back(   it->closedOrbit.ver / mm  );
        disp_H.push_back(   it->dispersion.hor        );
        disp_V.push_back(   it->dispersion.ver        );
      dPrime_H.push_back(   it->dPrime.hor            );
      dPrime_V.push_back(   it->dPrime.ver            );

  }

  CurveData c1( azimuth, disp_H,   "Hor Dispersion"            );
  CurveData c2( azimuth, disp_V,   "Ver Dispersion"            );
  CurveData c3( azimuth, dPrime_H, "Ver Dispersion"            );
  CurveData c4( azimuth, dPrime_V, "Hor Dispersion Derivative" );
  CurveData c5( azimuth, clo_H,    "Hor Closed Orbit"          );
  CurveData c6( azimuth, clo_V,    "Ver Closed Orbit"          );


    //c1->setPen( QPen( "black",  1, Qt::SolidLine ) );
    //c2->setPen( QPen( "blue",   1, Qt::SolidLine ) );
    //c3->setPen( QPen( "cyan",   0, Qt::SolidLine ) );
    //c4->setPen( QPen( "magenta",0, Qt::SolidLine ) );
    //c5->setPen( QPen( "red",    0, Qt::SolidLine ) );
    //c6->setPen( QPen( "green",  0, Qt::SolidLine ) );


    c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c2.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c3.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c4.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c5.setAxes( CurveData::xBottom, CurveData::yRight );
    c6.setAxes( CurveData::xBottom, CurveData::yRight );


    addCurve( c1 );
    addCurve( c2 );
    addCurve( c3 );
    addCurve( c4 );
    addCurve( c5 );
    addCurve( c6 );

    setXLabel( "Arc Length [m]" );
    setYLabel( CurveData::yLeft,  "Dispersion [m]"    );
    setYLabel( CurveData::yRight, "Closed Orbit [mm]" );

    setScaleMag(  CurveData::yRight, 5.0 ); 
    setBeamline( bml ); 

  }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DspnFncData::~DspnFncData()
{}

