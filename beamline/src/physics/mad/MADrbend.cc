#include "beamline.inc"

const double changeMADConvention = - 1.0;

void rbend::P_LikeMAD( bmlnElmnt* p_be, Particle& p ) {
  rbend* pbe = (rbend*) p_be;
  int     i, j;
  double  rho;
  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  double  edgeMatrix [ BMLN_dynDim ][BMLN_dynDim];
  double  inState[ BMLN_dynDim ];

  rho = p.BRho()/pbe->strength;
  double angle = 2.0 * asin(pbe->Length()/(2.0*rho));

  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ ){
      matrix[i][j] = 0.0;
      edgeMatrix[i][j] = 0.0;
    }
    matrix[i][i] = 1.0;
    edgeMatrix[i][i] = 1.0;
  }
  
  matrix[0][0] = cos(angle);
  matrix[0][3] = sin(angle)*rho;
  matrix[0][5] = rho*(1.0 - cos(angle))/p.Beta();
  matrix[1][4] = pbe->length;
  matrix[2][0] = -sin(angle)/p.Beta();
  matrix[2][3] = -matrix[0][5];
  matrix[2][5] = pbe->length/p.Beta()/p.Gamma()/p.Gamma() - 
    (pbe->length - sin(angle)*rho)/p.Beta();
  matrix[3][0] = -sin(angle)/rho;
  matrix[3][3] = matrix[0][0];
  matrix[3][5] = -matrix[2][0];
  
  edgeMatrix[3][0] = tan( angle/2.0 )/(rho);
  edgeMatrix[4][1] = -edgeMatrix[3][0];
 
  // Finally .. the mapping ...............................
  // First do the entrance edge
  
  if(pbe->align != 0) 
    pbe->align->misalign(p,pbe->geometry,inState);
  else {
    for( i = 0; i < BMLN_dynDim; i++  ) 
      inState[i] = p.state[i];
  }
  for( i = 0; i < BMLN_dynDim; i++  ) {
    p.state[i] = 0.0;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      p.state[i] += edgeMatrix[i][j]*inState[j];
  }
  
  // Next do the bend
  
  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state[i];
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    p.state[i] = 0.0;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      p.state[i] += matrix[i][j]*inState[j];
  }
  
  // Finally do the exit edge
  
  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state[i];
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    p.state[i] = 0.0;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      p.state[i] += edgeMatrix[i][j]*inState[j];
  }
  if(pbe->align != 0) {
    pbe->align->align(p,pbe->geometry,inState);
    for( i = 0; i < BMLN_dynDim; i++  ) 
      p.state[i] = inState[i];
  }
}

void rbend::J_LikeMAD( bmlnElmnt* p_be, JetParticle& p ) {
  rbend* pbe = (rbend*) p_be;
  Jet      inState  [BMLN_dynDim];
  Jet      outState [BMLN_dynDim];
  Jet      zero;
  int     i, j;
  Jet  rho;
  Jet  matrix [ BMLN_dynDim ][BMLN_dynDim];
  Jet  edgeMatrix [ BMLN_dynDim ][BMLN_dynDim];

  rho = p.BRho()/pbe->strength;
  double angle = 2.0 * asin(pbe->Length()/(2.0*rho.standardPart()));

  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ ){
      matrix[i][j] = 0.0;
      edgeMatrix[i][j] = 0.0;
    }
    matrix[i][i] = 1.0;
    edgeMatrix[i][i] = 1.0;
  }
  
  matrix[0][0] = cos(angle);
  matrix[0][3] = sin(angle)*rho;
  matrix[0][5] = rho*(1.0 - cos(angle))/p.Beta();
  matrix[1][4] = pbe->length;
  matrix[2][0] = -sin(angle)/p.Beta();
  matrix[2][3] = -matrix[0][5];
  matrix[2][5] = pbe->length/p.Beta()/p.Gamma()/p.Gamma() - 
    (pbe->length - sin(angle)*rho)/p.Beta();
  matrix[3][0] = -sin(angle)/rho;
  matrix[3][3] = matrix[0][0];
  matrix[3][5] = -matrix[2][0];
  
  edgeMatrix[3][0] = tan( angle/2.0 )/(rho);
  edgeMatrix[4][1] = -edgeMatrix[3][0];
 
 // Finally .. the mapping ...............................
 // First do the entrance edge
 
  if(pbe->align != 0) 
    pbe->align->misalign(p,pbe->geometry,inState);
  else {
    for( i = 0; i < BMLN_dynDim; i++  ) 
      inState[i] = p.state(i);
  }
  zero = 0.0;
//  zero.fixReferenceAtStart( p.state );
 
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + edgeMatrix[i][j]*inState[j];
    ( p.state ).SetComponent( i, outState[i] );
  }
 
 // Next do the bend
 
 for( i = 0; i < BMLN_dynDim; i++  ) 
   inState[i] = p.state(i);
 
 zero = 0.0;
// zero.fixReferenceAtStart( p.state );
 
 for( i = 0; i < BMLN_dynDim; i++  ) {
   outState[i] = zero;
   for( j = 0; j < BMLN_dynDim; j++  ) 
     outState[i] = outState[i] + matrix[i][j]*inState[j];
   ( p.state ).SetComponent( i, outState[i] );
 }
 
 // Finally do the exit edge
 
 for( i = 0; i < BMLN_dynDim; i++  ) 
   inState[i] = p.state(i);
 
  zero = 0.0;
//  zero.fixReferenceAtStart( p.state );
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + edgeMatrix[i][j]*inState[j];
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

