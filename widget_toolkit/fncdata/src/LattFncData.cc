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


    CurveData c1( azimuth, beta_H,  "Horizontal Beta"    );
    CurveData c2( azimuth, alpha_H, "Horizontal Alpha"   );
    //CurveData c3( azimuth, psi_H,   "Horizontal Psi"     );

    CurveData c4( azimuth, beta_V,  "Vertical Beta"      );
    CurveData c5( azimuth, alpha_V, "Vertical Alpha"     );
    //CurveData c6( azimuth, psi_V,   "Vertical Psi"       );


    c1.setAxes( CurveData::xBottom, CurveData::yLeft  );
    c1.setColor(CurveData::Color(0,0,0) );

    c2.setAxes( CurveData::xBottom, CurveData::yRight  );
    c2.setColor(CurveData::Color(255,0,0) );

    //c3.setAxes( CurveData::xBottom, CurveData::yLeft  );
    //c3.setColor(CurveData::Color(0,255,0) );


    c4.setAxes( CurveData::xBottom, CurveData::yLeft );
    c4.setColor(CurveData::Color(0,0,0) );

    c5.setAxes( CurveData::xBottom, CurveData::yRight );
    c5.setColor(CurveData::Color(255,0,0) );

    //c6.setAxes( CurveData::xBottom, CurveData::yRight );
    //c6.setColor(CurveData::Color(0,255,0) );

    addCurve( c1 );
    addCurve( c2 );
    //addCurve( c3 );

    addCurve( c4 );
    addCurve( c5 );
    //addCurve( c6 );

    setXLabel( "Arc Length [m]"                      );
    setYLabel(  CurveData::yLeft,   "Beta  [m]"      );
    setYLabel(  CurveData::yRight,  "Alpha"          );

    // setScaleMag(  CurveData::yRight, 5.0 ); // scale magnification ... unused at this point

    setBeamline( bml ); 

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFncData::~LattFncData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


