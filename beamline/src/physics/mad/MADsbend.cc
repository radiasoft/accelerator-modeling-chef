#include "beamline.inc"

const double changeMADConvention = - 1.0;

void sbend::P_LikeMAD( bmlnElmnt* p_be, Particle& p ) {
  sbend* pbe = (sbend*) p_be;
  static  int     i, j;
  static  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  static  double  inState[ BMLN_dynDim ];
  double rho;
  
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      matrix[i][j] = 0.0;
    matrix[i][i] = 1.0;
  }
  
  if(pbe->strength != 0.0) {
    rho = p.BRho()/pbe->strength;

    matrix[0][0] = cos(pbe->angle);
    matrix[0][3] = sin(pbe->angle)*rho;
    matrix[0][5] = rho*(1.0 - cos(pbe->angle))/p.Beta();
    matrix[1][4] = pbe->length;
    matrix[2][0] = changeMADConvention*(-sin(pbe->angle)/p.Beta());
    matrix[2][3] = changeMADConvention*(-matrix[0][5]);
    matrix[2][5] = changeMADConvention *(pbe->length/p.Beta()/
                                         p.Gamma()/
					 p.Gamma() - 
					 (pbe->length - sin(pbe->angle)*rho)
					 /p.Beta());
    matrix[3][0] = -sin(pbe->angle)/rho;
    matrix[3][3] = matrix[0][0];
    matrix[3][5] = changeMADConvention*(-matrix[2][0]);
    // Finally .. the mapping ...............................
    for( i = 0; i < BMLN_dynDim; i++  ) {
      inState[i] = p.state[i];
    }
    for( i = 0; i < BMLN_dynDim; i++  ) {
      p.state[i] = 0.0;
      for( j = 0; j < BMLN_dynDim; j++  ) 
	p.state[i] += matrix[i][j]*inState[j];
    }

  } else {
    p.state[0] += pbe->length * p.state[3];
    p.state[1] += pbe->length * p.state[4];
    p.state[2] -= pbe->length/p.Beta()/p.Gamma()/p.Gamma() * p.state[5];
  }  
}

void sbend::J_LikeMAD( bmlnElmnt* p_be, JetParticle& p ) {
  sbend* pbe = (sbend*) p_be;
  Jet      inState  [BMLN_dynDim];
  Jet      outState [BMLN_dynDim];
  Jet      zero;
  Jet dummy;
  static  int     i, j;
  static  double  matrix [ BMLN_dynDim ][ BMLN_dynDim ];
  double rho;


  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      matrix[i][j] = 0.0;
    matrix[i][i] = 1.0;
  }
  
  if(pbe->strength != 0.0) {
    rho = p.BRho().standardPart()/pbe->strength;

    matrix[0][0] = cos(pbe->angle);
    matrix[0][3] = sin(pbe->angle)*rho;
    matrix[0][5] = rho*(1.0 - cos(pbe->angle))/p.Beta().standardPart();
    matrix[1][4] = pbe->length;
    matrix[2][0] = changeMADConvention*(-sin(pbe->angle)/p.Beta().standardPart());
    matrix[2][3] = changeMADConvention*(-matrix[0][5]);
    matrix[2][5] = changeMADConvention*(pbe->length/p.Beta().standardPart()/
                                        p.Gamma().standardPart()/
                                        p.Gamma().standardPart() - 
                                       (pbe->length - sin(pbe->angle)*rho)
                                       /p.Beta().standardPart());
    matrix[3][0] = -sin(pbe->angle)/rho;
    matrix[3][3] = matrix[0][0];
    matrix[3][5] = changeMADConvention*(-matrix[2][0]);
    
    // Finally .. the mapping ...............................
    for( i = 0; i < BMLN_dynDim; i++  ) {
      inState[i] = p.state(i);
    }
    zero = 0.0;
    // ??? REMOVE zero.fixReferenceAtStart( p.state );
    
    for( i = 0; i < BMLN_dynDim; i++  ) {
      outState[i] = zero;
      for( j = 0; j < BMLN_dynDim; j++  ) 
	outState[i] = outState[i] + matrix[i][j]*inState[j];
    }

    for( i = 0; i < BMLN_dynDim; i++  ) {
      ( p.state ).SetComponent( i, outState[i] );
    }

  } else {
    dummy = p.state(0) + pbe->length * p.state(3);
    ( p.state ).SetComponent( 0, dummy );
    dummy = p.state(1) + pbe->length * p.state(4);
    ( p.state ).SetComponent( 1, dummy );
    dummy = p.state(2) - pbe->length/p.Beta()/p.Gamma()/p.Gamma() * p.state(5);
    ( p.state ).SetComponent( 2, dummy );
  }
}
