#include "rbend.h"

rbend::MAD_Prop   rbend::LikeMAD;

const double changeMADConvention = - 1.0;

int rbend::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) {
  rbend* pbe = (rbend*) p_be;
  int     i, j;
  double  rho;
  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  double  edgeMatrix [ BMLN_dynDim ][BMLN_dynDim];
  double  inState[ BMLN_dynDim ], outState[ BMLN_dynDim ];

  rho = p.BRho()/pbe->Strength();
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
  matrix[1][4] = pbe->Length();
  matrix[2][0] = -sin(angle)/p.Beta();
  matrix[2][3] = -matrix[0][5];
  matrix[2][5] = pbe->Length()/p.Beta()/p.Gamma()/p.Gamma() - 
    (pbe->Length() - sin(angle)*rho)/p.Beta();
  matrix[3][0] = -sin(angle)/rho;
  matrix[3][3] = matrix[0][0];
  matrix[3][5] = -matrix[2][0];
  
  edgeMatrix[3][0] = tan( angle/2.0 )/(rho);
  edgeMatrix[4][1] = -edgeMatrix[3][0];
 
  // Finally .. the mapping ...............................
  // First do the entrance edge
  
    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** sbend::MAD_Prop::operator()      "
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
      outState[i] += edgeMatrix[i][j]*inState[j];
  }
  
  // Next do the bend
  
  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = outState[i];
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = 0.0;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] += matrix[i][j]*inState[j];
  }
  
  // Finally do the exit edge
  
  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = outState[i];
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = 0.0;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] += edgeMatrix[i][j]*inState[j];
  }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  return 0;
}

int rbend::MAD_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) {
  rbend* pbe = (rbend*) p_be;
  Jet      inState  [BMLN_dynDim];
  Jet      outState [BMLN_dynDim];
  Jet      zero;
  int     i, j;
  Jet  rho;
  Jet  matrix [ BMLN_dynDim ][BMLN_dynDim];
  Jet  edgeMatrix [ BMLN_dynDim ][BMLN_dynDim];

  rho = p.BRho()/pbe->Strength();
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
  matrix[1][4] = pbe->Length();
  matrix[2][0] = -sin(angle)/p.Beta();
  matrix[2][3] = -matrix[0][5];
  matrix[2][5] = pbe->Length()/p.Beta()/p.Gamma()/p.Gamma() - 
    (pbe->Length() - sin(angle)*rho)/p.Beta();
  matrix[3][0] = -sin(angle)/rho;
  matrix[3][3] = matrix[0][0];
  matrix[3][5] = -matrix[2][0];
  
  edgeMatrix[3][0] = tan( angle/2.0 )/(rho);
  edgeMatrix[4][1] = -edgeMatrix[3][0];
 
 // Finally .. the mapping ...............................
 // First do the entrance edge
  
    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** sbend::MAD_Prop::operator()      "
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

  zero = 0.0;
  //  zero.fixReferenceAtStart( p.state );
 
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + edgeMatrix[i][j]*inState[j];
  }
 
 // Next do the bend
 
 for( i = 0; i < BMLN_dynDim; i++  ) 
   inState[i] = outState[i];
 
 zero = 0.0;
 // zero.fixReferenceAtStart( p.state );
 
 for( i = 0; i < BMLN_dynDim; i++  ) {
   outState[i] = zero;
   for( j = 0; j < BMLN_dynDim; j++  ) 
     outState[i] = outState[i] + matrix[i][j]*inState[j];
 }
 
 // Finally do the exit edge
 
 for( i = 0; i < BMLN_dynDim; i++  ) 
   inState[i] = outState[i];
 
  zero = 0.0;
  //  zero.fixReferenceAtStart( p.state );
  
  for( i = 0; i < BMLN_dynDim; i++  ) {
    outState[i] = zero;
    for( j = 0; j < BMLN_dynDim; j++  ) 
      outState[i] = outState[i] + edgeMatrix[i][j]*inState[j];
  }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  return 0;
}

