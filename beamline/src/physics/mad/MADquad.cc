// This version of the LikeMAD propagators
// was written by Mike Martens, who corrected
// higher order errors present in the 
// previous version.
// 
// Nov.4, 1998

#include "beamline.inc"

void quadrupole::P_LikeMAD( bmlnElmnt* p_be, Particle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;

  double length = pbe->length;

  if ( pbe->strength == 0.0 ) {

    double D, p3divpbar, xpr, ypr;

    p3divpbar = sqrt( ( 1.0 + p.state[5] ) * ( 1.0 + p.state[5] )
		      - p.state[3]*p.state[3] 
		      - p.state[4]*p.state[4] );

    xpr = p.state[3] / p3divpbar;
    ypr = p.state[4] / p3divpbar; 
    
    p.state[0] += length * xpr;
    p.state[1] += length * ypr;
    
    D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
    
    p.state[2] += ( D / p.Beta() ) - ( length / p.beta );

  } else {


    // Scale variable so 
    // 1) the reference momentum is q_o = p_0* (1.0 + delp/p)
    // 2) the "temporary" delp = 0

    double q0divp0 = 1.0 + p.state[5]; // factor to convert reference momentum 
    double K1 = pbe->strength / p.ReferenceBRho() / q0divp0;
    double Beta = p.Beta();   // this is reference beta with changed ref p. 
    double Gamma = p.Gamma();


    p.state[3] = p.state[3]/q0divp0;   // p_x/p_0 * p_0/q_0 = p_x/q_0 
    p.state[4] = p.state[4]/q0divp0;   // p_y/p_0 * p_0/q_0 = p_y/q_0 



    double outState[6];

    // Now compute terms in second order expansion as per MAD manual
    double arg, factor, kxsqr, kysqr, cx, sx, cy, sy;
    double T200, T203, T233, T211, T214, T244;
    
    kxsqr = K1;
    kysqr = -K1;
    factor = ( K1 > 0.0 )? sqrt( K1 ): sqrt( -K1 );
    arg = factor * pbe->length;

    if ( K1 > 0.0 )  {            // Focussing horizontally
      cx = cos( arg );
      sx = sin( arg )/factor;
      cy = cosh( arg );
      sy = sinh( arg )/factor;
    } else {                      // Defocussing horizontally
      cx = cosh( arg );
      sx = sinh( arg )/factor;
      cy = cos( arg );
      sy = sin( arg )/factor;
    } 
  
    T200 =   ( length - sx * cx );
    T203 = -  sx * sx;
    T233 =   (length + sx * cx );
    T211 = - ( length - sy * cy );
    T214 =   sy * sy;
    T244 =   ( length + sy * cy );
 
    outState[0] =          cx * p.state[0]  +  sx * p.state[3];
    outState[3] = -kxsqr * sx * p.state[0]  +  cx * p.state[3];
    outState[1] =          cy * p.state[1]  +  sy * p.state[4];
    outState[4] = -kysqr * sy * p.state[1]  +  cy * p.state[4];
    
    double Dist;
    Dist = ( 
	    ( T200 * p.state[0] + 2.0 * T203 * p.state[3] ) * K1 * p.state[0]
	    + T233 * p.state[3] * p.state[3]
	    + ( T211 * p.state[1] + 2.0 * T214 * p.state[4] ) * K1 * p.state[1]
	    + T244 * p.state[4] * p.state[4]
	    )/4.0;
  
    Dist += length;

    p.state[2] += (Dist/Beta - length/p.ReferenceBeta() );


    p.state[0] = outState[0];          
    p.state[1] = outState[1];          
    p.state[3] = outState[3] * q0divp0;          
    p.state[4] = outState[4] * q0divp0;          

  }
}

void quadrupole::J_LikeMAD( bmlnElmnt* p_be, JetParticle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;

  double length = pbe->length;

  if ( pbe->strength == 0.0 ) {

    Jet D, p3divpbar;
    Jet xpr, ypr;
    Jet dummy;

    p3divpbar = sqrt( ( 1.0 + p.state(5) ) * ( 1.0 + p.state(5) )
		      - p.state(3)*p.state(3) 
		      - p.state(4)*p.state(4) );

    xpr = p.state(3) / p3divpbar;
    ypr = p.state(4) / p3divpbar; 
    
    dummy = p.state(0) + length * xpr;
    ( p.state ).SetComponent( 0, dummy );
    dummy = p.state(1) + length * ypr;
    ( p.state ).SetComponent( 1, dummy );

    D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
    
    dummy =  p.state(2) + ( D / p.Beta() ) - ( length / p.beta );
    ( p.state ).SetComponent( 2, dummy );
    
  } else {

    Jet q0divp0;
    Jet K1;
    Jet Beta, Gamma;

    q0divp0 = 1.0 + p.state(5); 
    K1 = pbe->strength / p.ReferenceBRho() / q0divp0;
    Beta = p.Beta();   
    Gamma = p.Gamma();


    p.state(3) = p.state(3)/q0divp0;   // p_x/p_0 * p_0/q_0 = p_x/q_0 
    p.state(4) = p.state(4)/q0divp0;   // p_y/p_0 * p_0/q_0 = p_y/q_0 



    Jet outState[6];

    // Now compute terms in second order expansion as per MAD manual
    Jet arg, factor, kxsqr, kysqr, cx, sx, cy, sy;
    Jet T200, T203, T233, T211, T214, T244;
    
    kxsqr = K1;
    kysqr = -K1;
    factor = ( pbe->strength > 0.0 )? sqrt( K1 ): sqrt( -K1 );
    arg = factor * pbe->length;

    if ( pbe->strength > 0.0 )  {            // Focussing horizontally
      cx = cos( arg );
      sx = sin( arg )/factor;
      cy = cosh( arg );
      sy = sinh( arg )/factor;
    } else {                      // Defocussing horizontally
      cx = cosh( arg );
      sx = sinh( arg )/factor;
      cy = cos( arg );
      sy = sin( arg )/factor;
    } 
  
    T200 =   ( length - sx * cx );
    T203 = -  sx * sx;
    T233 =   (length + sx * cx );
    T211 = - ( length - sy * cy );
    T214 =   sy * sy;
    T244 =   ( length + sy * cy );
 
    outState[0] =          cx * p.state(0)  +  sx * p.state(3);
    outState[3] = -kxsqr * sx * p.state(0)  +  cx * p.state(3);
    outState[1] =          cy * p.state(1)  +  sy * p.state(4);
    outState[4] = -kysqr * sy * p.state(1)  +  cy * p.state(4);
    
    Jet Dist;
    Dist = ( 
	    ( T200 * p.state(0) + 2.0 * T203 * p.state(3) ) * K1 * p.state(0)
	    + T233 * p.state(3) * p.state(3)
	    + ( T211 * p.state(1) + 2.0 * T214 * p.state(4) ) * K1 * p.state(1)
	    + T244 * p.state(4) * p.state(4)
	    )/4.0;
  
    Dist += length;

    outState[2] = p.state(2) + ( Dist/Beta - length/p.ReferenceBeta() );

    outState[3] *= q0divp0;          
    outState[4] *= q0divp0;          

    ( p.state ).SetComponent( 0, outState[0] );    
    ( p.state ).SetComponent( 1, outState[1] );    
    ( p.state ).SetComponent( 2, outState[2] );    
    ( p.state ).SetComponent( 3, outState[3] );    
    ( p.state ).SetComponent( 4, outState[4] );    
  }
}
