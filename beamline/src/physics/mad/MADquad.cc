// This version of the LikeMAD propagators
// was written by Mike Martens, who corrected
// higher order errors present in the 
// previous version.
// 
// Nov.4, 1998

#include "quadrupole.h"

quadrupole::MAD_Prop   quadrupole::LikeMAD;

int quadrupole::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;

  double length = pbe->Length();

  if ( pbe->Strength() == 0.0 ) {

    double D, p3divpbar, xpr, ypr;

    p3divpbar = sqrt( ( 1.0 + p.get_ndp() ) * ( 1.0 + p.get_ndp() )
		      - p.get_npx()*p.get_npx() 
		      - p.get_npy()*p.get_npy() );

    xpr = p.get_npx() / p3divpbar;
    ypr = p.get_npy() / p3divpbar; 
    
    p.set_x( p.get_x() + length * xpr );
    p.set_y( p.get_y() + length * ypr );
    
    D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
    
    p.set_cdt( p.get_cdt() + ( D / p.Beta() ) - ( length / p.ReferenceBeta() ) );

  } else {


    // Scale variable so 
    // 1) the reference momentum is q_o = p_0* (1.0 + delp/p)
    // 2) the "temporary" delp = 0

    double q0divp0 = 1.0 + p.get_ndp(); // factor to convert reference momentum 
    double K1 = pbe->Strength() / p.ReferenceBRho() / q0divp0;
    double Beta = p.Beta();   // this is reference beta with changed ref p. 
    double Gamma = p.Gamma();


    p.set_npx( p.get_npx()/q0divp0 );   // p_x/p_0 * p_0/q_0 = p_x/q_0 
    p.set_npy( p.get_npy()/q0divp0 );   // p_y/p_0 * p_0/q_0 = p_y/q_0 



    double outState[6];

    // Now compute terms in second order expansion as per MAD manual
    double arg, factor, kxsqr, kysqr, cx, sx, cy, sy;
    double T200, T203, T233, T211, T214, T244;
    
    kxsqr = K1;
    kysqr = -K1;
    factor = ( K1 > 0.0 )? sqrt( K1 ): sqrt( -K1 );
    arg = factor * pbe->Length();

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
 
    outState[0] =          cx * p.get_x()  +  sx * p.get_npx();
    outState[3] = -kxsqr * sx * p.get_x()  +  cx * p.get_npx();
    outState[1] =          cy * p.get_y()  +  sy * p.get_npy();
    outState[4] = -kysqr * sy * p.get_y()  +  cy * p.get_npy();
    
    double Dist;
    Dist = ( 
	    ( T200 * p.get_x() + 2.0 * T203 * p.get_npx() ) * K1 * p.get_x()
	    + T233 * p.get_npx() * p.get_npx()
	    + ( T211 * p.get_y() + 2.0 * T214 * p.get_npy() ) * K1 * p.get_y()
	    + T244 * p.get_npy() * p.get_npy()
	    )/4.0;
  
    Dist += length;

    p.set_cdt( p.get_cdt() + (Dist/Beta - length/p.ReferenceBeta() ) );


    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_npx( outState[3] * q0divp0 );
    p.set_npy( outState[4] * q0divp0 );

  }

  return 0;
}

int quadrupole::MAD_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) {
  quadrupole* pbe = (quadrupole*) p_be;

  double length = pbe->Length();

  if ( pbe->Strength() == 0.0 ) {

    Jet D, p3divpbar;
    Jet xpr, ypr;
    Jet dummy;

    p3divpbar = sqrt( ( 1.0 + p.get_ndp() ) * ( 1.0 + p.get_ndp() )
		      - p.get_npx()*p.get_npx() 
		      - p.get_npy()*p.get_npy() );

    xpr = p.get_npx() / p3divpbar;
    ypr = p.get_npy() / p3divpbar; 
    
    p.set_x( p.get_x() + length * xpr );
    p.set_y( p.get_y() + length * ypr );

    D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
    
    p.set_cdt( p.get_cdt() + ( D / p.Beta() ) - ( length / p.ReferenceBeta() ) );
    
  } else {

    Jet q0divp0;
    Jet K1;
    Jet Beta, Gamma;

    q0divp0 = 1.0 + p.get_ndp(); 
    K1 = pbe->Strength() / p.ReferenceBRho() / q0divp0;
    Beta = p.Beta();   
    Gamma = p.Gamma();


    p.set_npx( p.get_npx()/q0divp0 );   // p_x/p_0 * p_0/q_0 = p_x/q_0 
    p.set_npy( p.get_npy()/q0divp0 );   // p_y/p_0 * p_0/q_0 = p_y/q_0 



    Jet outState[6];

    // Now compute terms in second order expansion as per MAD manual
    Jet arg, factor, kxsqr, kysqr, cx, sx, cy, sy;
    Jet T200, T203, T233, T211, T214, T244;
    
    kxsqr = K1;
    kysqr = -K1;
    factor = ( pbe->Strength() > 0.0 )? sqrt( K1 ): sqrt( -K1 );
    arg = factor * pbe->Length();

    if ( pbe->Strength() > 0.0 )  {            // Focussing horizontally
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
 
    outState[0] =          cx * p.get_x()  +  sx * p.get_npx();
    outState[3] = -kxsqr * sx * p.get_x()  +  cx * p.get_npx();
    outState[1] =          cy * p.get_y()  +  sy * p.get_npy();
    outState[4] = -kysqr * sy * p.get_y()  +  cy * p.get_npy();
    
    Jet Dist;
    Dist = ( 
	    ( T200 * p.get_x() + 2.0 * T203 * p.get_npx() ) * K1 * p.get_x()
	    + T233 * p.get_npx() * p.get_npx()
	    + ( T211 * p.get_y() + 2.0 * T214 * p.get_npy() ) * K1 * p.get_y()
	    + T244 * p.get_npy() * p.get_npy()
	    )/4.0;
  
    Dist += length;

    outState[2] = p.get_cdt() + ( Dist/Beta - length/p.ReferenceBeta() );

    outState[3] *= q0divp0;          
    outState[4] *= q0divp0;          

    p.set_x   ( outState[0] );
    p.set_y   ( outState[1] );
    p.set_cdt ( outState[2] );
    p.set_npx ( outState[3] );
    p.set_npy ( outState[4] );
  }

  return 0;
}
