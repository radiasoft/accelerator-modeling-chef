#ifdef HAS_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      MADJetQuad.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#include "JetQuadrupole.h"

using namespace std;

JetQuadrupole::MAD_Prop   JetQuadrupole::LikeMAD;

int JetQuadrupole::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) {
  JetQuadrupole* pbe = (JetQuadrupole*) p_be;
  int     i, j;
  double  arg, factor;
  double  inState[ BMLN_dynDim ], outState[ BMLN_dynDim ];
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];
  double realStrength; 
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      mapMatrix[i][j] = 0.0;
    mapMatrix[i][i] = 1.0;
  }
  realStrength = pbe->Strength() / p.BRho();
 if ( realStrength == 0.0 )         // Zero-strength quad acts like a drift
 {
   mapMatrix[0][0] = 1.0;  mapMatrix[0][3] = pbe->Length();
   mapMatrix[3][0] = 0.0;  mapMatrix[3][3] = 1.0;

   mapMatrix[1][1] = 1.0;  mapMatrix[1][4] = pbe->Length();
   mapMatrix[4][1] = 0.0;  mapMatrix[4][4] = 1.0;
 }
 else if ( realStrength < 0.0 )     // Defocussing horizontally
 {                              // Focussing   vertically
   factor = sqrt( - realStrength );
   arg    = factor * pbe->Length();

   mapMatrix[0][0] = mapMatrix[3][3] = cosh( arg );
   mapMatrix[0][3] = mapMatrix[3][0] = sinh( arg );
   mapMatrix[0][3] /=  factor;
   mapMatrix[3][0] *=  factor;

   mapMatrix[1][1] = mapMatrix[4][4] = cos( arg );
   mapMatrix[1][4] = mapMatrix[4][1] = sin( arg );
   mapMatrix[1][4] /=  factor;
   mapMatrix[4][1] *= -factor;
 }
 else                      // Defocussing vertically
 {                         // Focussing   horizontally
   factor = sqrt( realStrength );
   arg    = factor * pbe->Length();

   mapMatrix[0][0] = mapMatrix[3][3] = cos( arg );
   mapMatrix[0][3] = mapMatrix[3][0] = sin( arg );
   mapMatrix[0][3] /=  factor;
   mapMatrix[3][0] *= -factor;

   mapMatrix[1][1] = mapMatrix[4][4] = cosh( arg );
   mapMatrix[1][4] = mapMatrix[4][1] = sinh( arg );
   mapMatrix[1][4] /=  factor;
   mapMatrix[4][1] *=  factor;
 }

 mapMatrix[2][5] = -pbe->Length()/p.Beta()/p.Gamma()/p.Gamma();
 

    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** MADJetQuad::MAD_Prop::operator() "
              "*** ERROR *** This was written for a phase     "
              "*** ERROR *** space dimension of 6.            "
              "*** ERROR ***                                  "
           << endl;
      exit(1);
    }

    inState[0] = p.get_x();
    inState[1] = p.get_y();
    inState[2] = p.get_cdt();
    inState[3] = p.get_npx();
    inState[4] = p.get_npy();
    inState[5] = p.get_ndp();

 for( i = 0; i < BMLN_dynDim; i++  ) {
   outState[i] = 0.0;
   for( j = 0; j < BMLN_dynDim; j++  ) 
     outState[i] += mapMatrix[i][j]*inState[j];
 }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  return 0;
}


int JetQuadrupole::MAD_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) {
  JetQuadrupole* pbe = (JetQuadrupole*) p_be;
  Jet    inState  [BMLN_dynDim];
  Jet    outState [BMLN_dynDim];
  Jet    zero;
  int    i, j;
  Jet arg, factor;
  Jet realStrength;
  Jet mapMatrix[BMLN_dynDim][BMLN_dynDim];
 
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      mapMatrix[i][j] = 0.0;
    mapMatrix[i][i] = 1.0;
  }
  realStrength = pbe->getJetStrength() / p.ReferenceBRho();
  if ( realStrength.standardPart() == 0.0 ) // Zero-strength quad acts like a drift
    {
      mapMatrix[0][0] = 1.0;  mapMatrix[0][3] = pbe->Length();
      mapMatrix[3][0] = 0.0;  mapMatrix[3][3] = 1.0;
      
      mapMatrix[1][1] = 1.0;  mapMatrix[1][4] = pbe->Length();
      mapMatrix[4][1] = 0.0;  mapMatrix[4][4] = 1.0;
    }
  else if ( realStrength.standardPart() < 0.0 ) // Defocussing horizontally
    {                              // Focussing   vertically
      factor = sqrt( - realStrength );
      arg    = factor * pbe->Length();
      
      mapMatrix[0][0] = mapMatrix[3][3] = cosh( arg );
      mapMatrix[0][3] = mapMatrix[3][0] = sinh( arg );
      mapMatrix[0][3] /=  factor;
      mapMatrix[3][0] *=  factor;
      
      mapMatrix[1][1] = mapMatrix[4][4] = cos( arg );
      mapMatrix[1][4] = mapMatrix[4][1] = sin( arg );
      mapMatrix[1][4] /=  factor;
      mapMatrix[4][1] *= -factor;
    }
  else                      // Defocussing vertically
    {                         // Focussing   horizontally
      factor = sqrt( realStrength );
      arg    = factor * pbe->Length();
      
      mapMatrix[0][0] = mapMatrix[3][3] = cos( arg );
      mapMatrix[0][3] = mapMatrix[3][0] = sin( arg );
      mapMatrix[0][3] /=  factor;
      mapMatrix[3][0] *= -factor;
      
      mapMatrix[1][1] = mapMatrix[4][4] = cosh( arg );
      mapMatrix[1][4] = mapMatrix[4][1] = sinh( arg );
      mapMatrix[1][4] /=  factor;
      mapMatrix[4][1] *=  factor;
    }

  
  mapMatrix[2][5] = -pbe->Length()/p.ReferenceBeta()/p.ReferenceGamma()/
    p.ReferenceGamma();

  zero = 0.0;


    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** MADJetQuad::MAD_Prop::operator() "
              "*** ERROR *** This was written for a phase     "
              "*** ERROR *** space dimension of 6.            "
              "*** ERROR ***                                  "
           << endl;
      exit(1);
    }

    inState[0] = p.get_x();
    inState[1] = p.get_y();
    inState[2] = p.get_cdt();
    inState[3] = p.get_npx();
    inState[4] = p.get_npy();
    inState[5] = p.get_ndp();

  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + mapMatrix[i][j]*inState[j];
  }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  return 0;
}
