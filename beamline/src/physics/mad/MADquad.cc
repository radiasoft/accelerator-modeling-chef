#include "beamline.inc"

void quadrupole::P_LikeMAD( bmlnElmnt* p_be, Particle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;
  int     i, j;
  double  arg, factor;
  double  inState[ BMLN_dynDim ];
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];
  double realStrength; 
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      mapMatrix[i][j] = 0.0;
    mapMatrix[i][i] = 1.0;
  }
  realStrength = pbe->strength / p.BRho();
 if ( realStrength == 0.0 )         // Zero-strength quad acts like a drift
 {
   mapMatrix[0][0] = 1.0;  mapMatrix[0][3] = pbe->length;
   mapMatrix[3][0] = 0.0;  mapMatrix[3][3] = 1.0;

   mapMatrix[1][1] = 1.0;  mapMatrix[1][4] = pbe->length;
   mapMatrix[4][1] = 0.0;  mapMatrix[4][4] = 1.0;
 }
 else if ( realStrength < 0.0 )     // Defocussing horizontally
 {                              // Focussing   vertically
   factor = sqrt( - realStrength );
   arg    = factor * pbe->length;

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
   arg    = factor * pbe->length;

   mapMatrix[0][0] = mapMatrix[3][3] = cos( arg );
   mapMatrix[0][3] = mapMatrix[3][0] = sin( arg );
   mapMatrix[0][3] /=  factor;
   mapMatrix[3][0] *= -factor;

   mapMatrix[1][1] = mapMatrix[4][4] = cosh( arg );
   mapMatrix[1][4] = mapMatrix[4][1] = sinh( arg );
   mapMatrix[1][4] /=  factor;
   mapMatrix[4][1] *=  factor;
 }

 mapMatrix[2][5] = -pbe->length/p.Beta()/p.Gamma()/p.Gamma();
 
  if(pbe->align != 0) 
    pbe->align->misalign(p,pbe->geometry,inState);
  else {
    for( i = 0; i < BMLN_dynDim; i++  ) 
      inState[i] = p.state[i];
  }
 for( i = 0; i < BMLN_dynDim; i++  ) {
   p.state[i] = 0.0;
   for( j = 0; j < BMLN_dynDim; j++  ) 
     p.state[i] += mapMatrix[i][j]*inState[j];
 }
  if(pbe->align != 0) {
    pbe->align->align(p,pbe->geometry,inState);
    for( i = 0; i < BMLN_dynDim; i++  ) 
      p.state[i] = inState[i];
  }
}


void quadrupole::J_LikeMAD( bmlnElmnt* p_be, JetParticle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;
  Jet    inState  [BMLN_dynDim];
  Jet    outState [BMLN_dynDim];
  Jet    zero;
  int    i, j;
  Jet    arg, factor;
  Jet    realStrength;
  Jet    mapMatrix[BMLN_dynDim][BMLN_dynDim];
 
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      mapMatrix[i][j] = 0.0;
    mapMatrix[i][i] = 1.0;
  }
  realStrength = pbe->strength / p.BRho();
  
  if ( realStrength.standardPart() == 0.0 )         // Zero-strength quad acts like a drift
    {
      mapMatrix[0][0] = 1.0;  mapMatrix[0][3] = pbe->length;
      mapMatrix[3][0] = 0.0;  mapMatrix[3][3] = 1.0;
      
      mapMatrix[1][1] = 1.0;  mapMatrix[1][4] = pbe->length;
      mapMatrix[4][1] = 0.0;  mapMatrix[4][4] = 1.0;
    }
  else if ( realStrength.standardPart() < 0.0 )     // Defocussing horizontally
    {                              // Focussing   vertically
      factor = sqrt( - realStrength );
      arg    = factor * pbe->length;
      
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
      arg    = factor * pbe->length;
      
      mapMatrix[0][0] = mapMatrix[3][3] = cos( arg );
      mapMatrix[0][3] = mapMatrix[3][0] = sin( arg );
      mapMatrix[0][3] /=  factor;
      mapMatrix[3][0] *= -factor;
      
      mapMatrix[1][1] = mapMatrix[4][4] = cosh( arg );
      mapMatrix[1][4] = mapMatrix[4][1] = sinh( arg );
      mapMatrix[1][4] /=  factor;
      mapMatrix[4][1] *=  factor;
    }

  
  mapMatrix[2][5] = -pbe->length/p.ReferenceBeta()/p.ReferenceGamma()/
    p.ReferenceGamma();
  zero = 0.0;
  // ??? REMOVE zero.fixReferenceAtStart( p.state );

  if(pbe->align != 0) 
    pbe->align->misalign(p,pbe->geometry,inState);
  else {
    for( i = 0; i < BMLN_dynDim; i++  ) 
      inState[i] = p.state(i);
  }

  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + mapMatrix[i][j]*inState[j];
  }
  if(pbe->align != 0) {
    pbe->align->align(outState,pbe->geometry,inState);
    for( i = 0; i < BMLN_dynDim; i++  ) 
      ( p.state ).SetComponent( i, inState[i] );
  } else {
    for( i = 0; i < BMLN_dynDim; i++  ) 
      ( p.state ).SetComponent( i, outState[i] );
  }
}
