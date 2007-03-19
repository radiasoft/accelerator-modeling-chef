/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      LattFncData.cpp                                   ******
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

#include <LattFncData.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFncData::LattFncData( std::vector<LattFuncSage::lattFunc> const& twiss_vec, ConstBmlPtr bml ) 
{

   std::vector<double>      azimuth;
   std::vector<double>       beta_H;
   std::vector<double>      alpha_H;
   std::vector<double>        psi_H;
   std::vector<double>       beta_V;
   std::vector<double>      alpha_V;
   std::vector<double>        psi_V;
   std::vector<double>   inv_beta_H;
   std::vector<double>   inv_beta_V;
   std::vector<double>  root_beta_H;
   std::vector<double>  root_beta_V;
   std::vector<double>       disp_H;
   std::vector<double>       disp_V;

  for ( std::vector<LattFuncSage::lattFunc>::const_iterator it =  twiss_vec.begin();  
                                                            it !=  twiss_vec.end(); ++it ) {

       azimuth.push_back( it->arcLength      );
        beta_H.push_back(  it->beta.hor      );
       alpha_H.push_back( it->alpha.hor      );
         psi_H.push_back(   it->psi.hor      );
        beta_V.push_back(  it->beta.ver      );
       alpha_V.push_back( it->alpha.ver      );
         psi_V.push_back(   it->psi.ver      );
    inv_beta_H.push_back(  1.0/it->beta.hor  );
    inv_beta_V.push_back(  1.0/it->beta.ver  );
   root_beta_H.push_back( sqrt(it->beta.hor) );
   root_beta_V.push_back( sqrt(it->beta.ver) );
        disp_H.push_back( it->dispersion.hor );
        disp_V.push_back( it->dispersion.ver );

    }


    CurveData c1( azimuth, beta_H, "Horizontal Beta"        );
    CurveData c2( azimuth, beta_V, "Vertical Beta"          );
    CurveData c3( azimuth, disp_H, "Horizontal Dispersion"  );
    CurveData c4( azimuth, disp_V, "Vertical Dispersion"    );

    //c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
    //c2->setPen( QPen( "red",   1, Qt::SolidLine ) );
    //c3->setPen( QPen( "black", 0, Qt::DashLine ) );
    //c4->setPen( QPen( "red",   0, Qt::DashLine ) );

    c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c2.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c3.setAxes( CurveData::xBottom, CurveData::yRight );
    c4.setAxes( CurveData::xBottom, CurveData::yRight );

    addCurve( c1 );
    addCurve( c2 );
    addCurve( c3 );
    addCurve( c4 );

    setXLabel( "Arc Length [m]"                      );
    setYLabel(  CurveData::yLeft,   "Beta [m]"       );
    setYLabel(  CurveData::yRight,  "Dispersion [m]" );

    setScaleMag(  CurveData::yRight, 5.0 ); 
    setBeamline( bml ); 

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFncData::~LattFncData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


