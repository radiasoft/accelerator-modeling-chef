/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      CF_sbend_MADPropagator.cc
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Nov 2013           michelotti@fnal.gov
******  - initial implementation of class CF_sbend_MADPropagator. This
******  is a prototype version, primarily intended for further
******  testing.  Use cautiously otherwise.  Planned future versions
******  will have a different interface (esp. by using templates to
******  expand the application beyond CF_sbend to other beamline
******  elements).
******
**************************************************************************
*************************************************************************/

#include <iostream>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/CF_sbend_MADPropagator.h>


// DGN: temporary kludge to allow access
// DGN: to coefficients.

double CF_sbend_MADPropagator::rre[7][7];
double CF_sbend_MADPropagator::tte[7][7][7];


namespace
{

// Preliminary stuff
// -------------------
#if 1
int const i_x   = Particle::xIndex;
int const i_y   = Particle::yIndex;
int const i_cdt = Particle::cdtIndex;
int const i_npx = Particle::npxIndex;
int const i_npy = Particle::npyIndex;
int const i_ndp = Particle::ndpIndex;
#endif
#if 0
Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;
#endif


#if 1
int to_MAD  [] = { 1, 3, 5, 2, 4, 6 };
int to_CHEF [] = { 0, i_x, i_npx, i_y, i_npy, i_cdt, i_ndp };
#endif
#if 0
int to_MAD[6];
int to_CHEF[7];

to_MAD[i_x]   = 1;
to_MAD[i_npx] = 2;
to_MAD[i_y]   = 3;
to_MAD[i_npy] = 4;
to_MAD[i_cdt] = 5;
to_MAD[i_ndp] = 6;

to_CHEF[0]    = 0;   // definitely not used
to_CHEF[1]    = i_x;
to_CHEF[2]    = i_npx;
to_CHEF[3]    = i_y;
to_CHEF[4]    = i_npy;
to_CHEF[5]    = i_cdt;
to_CHEF[6]    = i_ndp;
#endif


#if 1
template<typename Particle_t>
void propagate( CF_sbend& elm, Particle_t& p, CF_sbend_MADPropagator const* ptr_mlp )
{
#endif
#if 0
template<typename Particle_t>
void propagate( CF_sbend& elm, Particle_t& p, double[7][7] const re_, double[7][7][7] const te_ )
{
#endif
  // I am bypassing consideration of the dipole
  // error field for now. No calculation uses ek_
  // --------------------------------------------

  // Preliminary stuff
  // -----------------

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();
  State_t  MAD_in_state(7);
  State_t  MAD_out_state(7);

  #if 1
  MAD_in_state[::to_MAD[i_x]]   = p.get_x();
  MAD_in_state[::to_MAD[i_y]]   = p.get_y();
  MAD_in_state[::to_MAD[i_npx]] = p.get_npx() / p.get_npz();
  MAD_in_state[::to_MAD[i_npy]] = p.get_npy() / p.get_npz();
  MAD_in_state[::to_MAD[i_cdt]] = p.get_cdt();  // ???
  MAD_in_state[::to_MAD[i_ndp]] = p.get_ndp();  // ???
  #endif
  #if 0
  for( int i = 1; i <= 6; ++i ) {
    MAD_in_state[i] = state[ ::to_CHEF[i] ];
  }
  #endif



  // Applying the mapping
  // --------------------

  for( int i = 1; i <= 6; ++i ) {
    MAD_out_state[i] = 0.0;

    for( int j = 1; j <= 6; ++j ) {
      // ... linear part
      // ---------------
      // MAD_out_state[i] += ptr_mlp->re_[i][j]*MAD_in_state[j];
      MAD_out_state[i] = MAD_out_state[i] + ptr_mlp->re_[i][j]*MAD_in_state[j];

      for( int k = 0; k <= 6; ++k ) {
        // ... quadratic part
        // ------------------
        // MAD_out_state[i] += ptr_mlp->te_[i][j][k] * ( MAD_in_state[j]*MAD_in_state[k] );
        MAD_out_state[i] = MAD_out_state[i] + ptr_mlp->te_[i][j][k] * ( MAD_in_state[j]*MAD_in_state[k] );
      }
    }
  }



  // Writing new state to particle
  // -----------------------------

  #if 1
  Component_t numerator( p.Momentum()*p.Momentum() );
  Component_t denominator(   MAD_out_state[::to_MAD[i_npx]]*MAD_out_state[::to_MAD[i_npx]] 
                           + MAD_out_state[::to_MAD[i_npy]]*MAD_out_state[::to_MAD[i_npy]] );
  denominator = denominator + 1.0;

  Component_t pz( numerator / denominator );
  pz = sqrt(pz);

  p.set_x(   MAD_out_state[::to_MAD[i_x]] );
  p.set_y(   MAD_out_state[::to_MAD[i_y]] );
  p.set_npx( MAD_out_state[::to_MAD[i_npx]] * ( pz / p.ReferenceMomentum() ) );
  p.set_npy( MAD_out_state[::to_MAD[i_npy]] * ( pz / p.ReferenceMomentum() ) );
  p.set_cdt( MAD_out_state[::to_MAD[i_cdt]] );  // ???
  p.set_ndp( MAD_out_state[::to_MAD[i_ndp]] );  // ???
  #endif
  #if 0
  for( int i = 1; i <= 6; ++i ) {
    state[ ::to_CHEF[i] ] = MAD_out_state[i];
  }
  #endif

  
  // ??? state[i_cdt] -= elm.getReferenceTime(); ???
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( CF_sbend& elm,    Particle& p, CF_sbend_MADPropagator const* ptr_mlp );
template void propagate( CF_sbend& elm, JetParticle& p, CF_sbend_MADPropagator const* ptr_mlp );

#endif

} // end anonymous namespace


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double const CF_sbend_MADPropagator::zero     =  0.0;
double const CF_sbend_MADPropagator::one      =  1.0;
double const CF_sbend_MADPropagator::two      =  2.0;
double const CF_sbend_MADPropagator::three    =  3.0;
double const CF_sbend_MADPropagator::four     =  4.0;
double const CF_sbend_MADPropagator::six      =  6.0;
double const CF_sbend_MADPropagator::nine     =  9.0;
double const CF_sbend_MADPropagator::twelve   = 12.0;
double const CF_sbend_MADPropagator::fifteen  = 15.0;
double const CF_sbend_MADPropagator::twty     = 20.0;
double const CF_sbend_MADPropagator::twty2    = 22.0;
double const CF_sbend_MADPropagator::twty4    = 24.0;
double const CF_sbend_MADPropagator::thty     = 30.0;
double const CF_sbend_MADPropagator::foty2    = 42.0;
double const CF_sbend_MADPropagator::fvty6    = 56.0;
double const CF_sbend_MADPropagator::svty2    = 72.0;
double const CF_sbend_MADPropagator::httwty   = 120.0;
double const CF_sbend_MADPropagator::c1       = one;
double const CF_sbend_MADPropagator::c2       = one/two;
double const CF_sbend_MADPropagator::c3       = one/twty4;
double const CF_sbend_MADPropagator::c4       = one/720.0;
double const CF_sbend_MADPropagator::s1       = one;
double const CF_sbend_MADPropagator::s2       = one/six;
double const CF_sbend_MADPropagator::s3       = one/httwty;
double const CF_sbend_MADPropagator::s4       = one/5040.0;
double const CF_sbend_MADPropagator::cg0      = one/twty;
double const CF_sbend_MADPropagator::cg1      = 5.0/840.0;
double const CF_sbend_MADPropagator::cg2      = 21.0/60480.0;
double const CF_sbend_MADPropagator::ch0      = one/fvty6;
double const CF_sbend_MADPropagator::ch1      = 14.0/4032.0;
double const CF_sbend_MADPropagator::ch2      = 147.0/443520.0;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend_MADPropagator::CF_sbend_MADPropagator()
{
  // Initialize mapping arrays.
  // -------------------------
  for( int i = 0; i <= 6; ++i ) {
    ek_[i] = 0.0;
    for( int j = 0; j <= 6; ++j ) {
      re_[i][j] = 0.0;
      for( int k = 0; k <= 6; ++k ) {
        te_[i][j][k] = 0.0;
      }
    }
  }

  // Linear coefficients initialized to
  // the unit matrix, as in MAD's tmsect.f90.
  // ----------------------------------------
  for( int i = 1; i <= 6; ++i ) {
    re_[i][i] = 1.0;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend_MADPropagator::CF_sbend_MADPropagator( CF_sbend_MADPropagator const& x )
{
  // Initialize mapping arrays.  (probably not necessary)
  // -------------------------
  for( int i = 0; i <= 6; ++i ) {
    ek_[i] = x.ek_[i];
    for( int j = 0; j <= 6; ++j ) {
      re_[i][j] = x.re_[i][j];
      for( int k = 0; k <= 6; ++k ) {
        te_[i][j][k] = x.te_[i][j][k];
      }
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend_MADPropagator::tmfoc( double el, double sk1,
                                double& c, double& s, double& d, double& f )
{
  //----------------------------------------------------------------------
  // Purpose:                                                             
  //   Compute linear focussing functions.                                
  // Input:                                                               
  //   el        (double)  element length.                                
  //   sk1       (double)  quadrupole strength.                           
  // Output:                                                              
  //   c         (double)  cosine-like function.             c(k,l)       
  //   s         (double)  sine-like function.               s(k,l)       
  //   d         (double)  dispersion function.              d(k,l)       
  //   f         (double)  integral of dispersion function.  f(k,l)       
  //----------------------------------------------------------------------

  double qk, qkl, qkl2;
  double const zero=0.0, one=1.0, two=2.0, six=6.0,
               twelve=12.0, twty=20.0, thty=30.0, foty2=42.0;

  //---- Initialize.
  qk = sqrt(abs(sk1));
  qkl = qk * el;
  qkl2 = sk1 * el*el;
  if ( abs(qkl2) <= 1.0e-2 ) {
     c = (one - qkl2 * (one - qkl2 / twelve) /  two);
     s = (one - qkl2 * (one - qkl2 / twty) /  six) * el;
     d = (one - qkl2 * (one - qkl2 / thty) / twelve) * pow(el,2) / two;
     f = (one - qkl2 * (one - qkl2 / foty2) / twty) * pow(el,3) / six;
  }
  else {
     if ( qkl2 > zero ) {
        c = cos(qkl);
        s = sin(qkl) / qk;
     }
     else {
        c = cosh(qkl);
        s = sinh(qkl) / qk;
     }
     d = (one - c) / sk1;
     f = (el  - s) / sk1;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend_MADPropagator::setup( CF_sbend& arg )
{
  // NOTE: This propagator is not using Bend::bend_core_access,
  // NOTE: and thus will use only public information.
  
  // Estimate the reference momentum
  // from bend parameters.
  // --------------------------------------

  double angle  = arg.getBendAngle();
  double el     = arg.Length();
  
  double rho_0  = el / angle;
  double h      = 1.0/rho_0;
  double brho_0 = arg.Strength() * rho_0;

  double p_ref  = PH_CNV_brho_to_p * brho_0;

  double sk1    = arg.getQuadrupole() / ( el*brho_0 );       // = B'/brho
  double sk2    = arg.getSextupole() * 2.0 / ( el*brho_0 );  // = B''/brho


  // Now I assume that the reference particle is a proton.
  // This can be generalized by writing a setup routine
  // that accepts a particle as in put argument, which is
  // probably the way it should be done in the first place.
  // ----------------------------------------------------
  Proton prtn( PH_NORM_mp );   // argument value does not matter, as it
  prtn.SetReferenceMomentum( p_ref );          // is reset right here.

  double beta   = prtn.Beta();
  double gamma  = prtn.Gamma();
  double dtbyds = 1.0/(PH_MKS_c*beta);  // ???

  double bi     = one/beta;
  double bi2    = bi*bi;
  double bi2gi2 = one /pow(beta*gamma,2);



  // This then follows the code established
  // in direct_comparison_2.cc, which mimics
  // what is in MAD's tmsect subroutine.
  // ---------------------------------------
  
  double xksq  = h*h + sk1;
  double xk    = sqrt(abs(xksq));
  double xkl   = xk * el;
  double xklsq = xksq * el*el;

  double cx, sx, dx, fx, gx, hx=0;
  if ( abs(xklsq) < 1.0e-2 ) 
  {
     cx = (c1 - xklsq * (c2 - xklsq*c3));
     sx = (s1 - xklsq * (s2 - xklsq*s3)) * el;
     dx = (c2 - xklsq * (c3 - xklsq*c4)) * el*el;
     fx = (s2 - xklsq * (s3 - xklsq*s4)) * pow(el,3);
     gx = (cg0 - xklsq * (cg1 - xklsq*cg2)) * pow(el,5);
     hx = (ch0 - xklsq * (ch1 - xklsq*ch2)) * pow(el,7);
  }
  else 
  {
     if (xklsq > zero ) {
        cx = cos(xkl);
        sx = sin(xkl) / xk;
     }
     else {
        cx = cosh(xkl);
        sx = sinh(xkl) / xk;
     }
     dx = (one - cx) / xksq;
     fx = (el  - sx) / xksq;
     gx = (three*el - sx*(four-cx)) / (two*xksq*xksq);
     hx = (fifteen*el - sx*(twty2-nine*cx+two*cx*cx)) / (six*pow(xksq,3));
  }

  re_[1][1] = cx;
  re_[1][2] = sx;
  re_[1][6] = h * dx * bi;
  re_[2][1] = - xksq * sx;
  re_[2][2] = cx;
  re_[2][6] = h * sx * bi;
  re_[5][2] = - re_[1][6];
  re_[5][1] = - re_[2][6];
  re_[5][6] = el*bi2gi2 - (h*h)*fx*bi2;


  // I am bypassing consideration of the dipole
  // error field for now.
  // ------------------------------------------
  #if 1
  ek_[1] = zero;
  ek_[2] = zero;
  ek_[5] = el*dtbyds;
  #endif
  #if 0
  ek_[1] = - dh*dx;  ???
  ek_[2] = - dh*sx;  ???
  ek_[5] =   h*dh*fx*bi + el*dtbyds;  ???
  #endif



  // Vertical.
  // --------
  double yksq  = - sk1;
  double yk    = sqrt(abs(yksq));
  double ykl   = yk*el;
  double yklsq = yksq*el*el;

  double cy, sy=0;
  if (abs(yklsq) < 1.0e-2) {
     cy = (c1 - yklsq * (c2 - yklsq*c3));
     sy = (s1 - yklsq * (s2 - yklsq*s3)) * el;
  }
  else if (yklsq > zero) {
     cy = cos(ykl);
     sy = sin(ykl) / yk;
  }
  else {
     cy = cosh(ykl);
     sy = sinh(ykl) / yk;
  }

  re_[3][3] = cy;
  re_[3][4] = sy;
  re_[4][3] = - yksq * sy;
  re_[4][4] = cy;
  ek_[3]    = zero;
  ek_[4]    = zero;


  // ---------------------------------------------------
  // Change the signs of MAD coefficients involving time
  // ---------------------------------------------------

  for( int i = 1; i <= 6; ++i ) {
    for( int j = 1; j <= 6; ++j ) {
      if( 5 == i ) { re_[i][j] = - re_[i][j]; }
      if( 5 == j ) { re_[i][j] = - re_[i][j]; }
    }
  }


  // Second-order terms.
  // -----------------

  // Pure horizontal terms.
  // ---------------------
  double xs6 = (sk2 + two*h*sk1) / six;
  double ys2 = (sk2 +     h*sk1) / two;
  double h2  = h / two;

  double t116 = xs6 * (three*sx*fx - dx*dx) - h * sx*sx;
  double t126 = xs6 * (sx*dx*dx - two*cx*gx) - h * sx * dx;
  double t166 = xs6 * (pow(dx,3) - two*sx*gx) - h2 * dx*dx;
  double t216 = xs6 * (three*cx*fx + sx*dx);
  double t226 = xs6 * (three*sx*fx + dx*dx);
  double t266 = xs6 * (sx*dx*dx - two*cx*gx);
  double t516 = h * xs6 * (three*dx*fx - four*gx) + (sk1/two) * (fx + sx*dx);
  double t526 = h * xs6 * (pow(dx,3) - two*sx*gx) + (sk1/two) * dx*dx;
  double t566 = h * xs6 * (three*hx - two*dx*gx) + (sk1/two) * gx - fx;

  double t1 = (sk1/two) * (dx*dx - sx*fx) - dx;
  double t2 = (sk1/two) * (el*dx - fx);
  double t5 = fx - sk1 * (gx - fx*dx / two);


  te_[1][1][1] = - xs6 * (sx*sx + dx) - h2*xksq*sx*sx;
  te_[1][1][2] = (- xs6*dx + h2*cx) * sx;
  te_[1][2][2] = (- xs6*dx + h2*cx) * dx;
  te_[1][1][6] = (- h2*t116 + (sk1/four)*el*sx) * bi;
  te_[1][2][6] = (- h2*t126 + (sk1/four) * (el*dx - fx) - sx/two) *bi;
  te_[1][6][6] = (- h*h*t166 + h*t1) * bi2 - h2 * dx * bi2gi2;

  te_[2][1][1] = - xs6 * (one + two*cx) * sx;
  te_[2][1][2] = - xs6 * (one + two*cx) * dx;
  te_[2][2][2] = - (two*xs6*dx + h2) * sx;
  te_[2][1][6] = (- h2*t216 - (sk1/four) * (sx - el*cx)) * bi;
  te_[2][2][6] = (- h2*t226 + (sk1/four) * el * sx) * bi;
  te_[2][6][6] = (- h*h*t266 + h*t2) * bi2 - h2 * sx * bi2gi2;

  te_[5][1][1] = (h2*xs6 * (sx*dx + three*fx) - (sk1/four) * (el - cx*sx)) * bi;
  te_[5][1][2] = (h2*xs6*dx*dx + (sk1/four)*sx*sx) * bi;
  te_[5][2][2] = (h*xs6*gx - sk1 * (fx - sx*dx) / four - sx/two) * bi;
  te_[5][1][6] = h2 * ((t516 - sk1 * (el*dx - fx) / two) * bi2 + sx * bi2gi2);
  te_[5][2][6] = h2 * ((t526 - sk1 * (dx*dx - sx*fx) / two) * bi2 + dx * bi2gi2);
  te_[5][6][6] = (h*h * (t566 + t5) * bi2 + (three/two) * (h*h*fx - el) * bi2gi2) * bi;


  // Mixed terms.
  // ------------
  double y2ksq = four * yksq;

  double cyy = 0.0, syy = 0.0, dyy = 0.0, fyy = 0.0;

  tmfoc(el,y2ksq,cyy,syy,dyy,fyy);

  double y2klsq = y2ksq * el*el;

  double y0, y1, y2, zc, zs, zd, zf;
  double dd;
  double sumsq, sifsq;
  double difsq, cp, sp, dp, fp, dm, fm, cm, sm;

  if ( max(abs(y2klsq),abs(xklsq)) <= 1.0e-2 ) {
     y0 = one;
     y1 = xklsq + y2klsq;
     y2 = xklsq*xklsq + xklsq*y2klsq + y2klsq*y2klsq;
     zc = (y0 - (y1 - y2 / thty) / twelve) * pow(el,2) / two;
     zs = (y0 - (y1 - y2 / foty2) / twty) * pow(el,3) /  six;
     zd = (y0 - (y1 - y2 / fvty6) / thty) * pow(el,4) /  twty4;
     zf = (y0 - (y1 - y2 / svty2) / foty2) * pow(el,5) / httwty;
  }
  else if (xksq <= zero  || yksq <= zero) {
     dd = xksq - y2ksq;
     zc = (cyy - cx) / dd;
     zs = (syy - sx) / dd;
     zd = (dyy - dx) / dd;
     zf = (fyy - fx) / dd;
  }
  else {
     sumsq = pow( (xk/two + yk), 2 );
     difsq = pow( (xk/two - yk), 2 );
     tmfoc( el, sumsq, cp, sp, dp, fp ); // ???
     tmfoc( el, difsq, cm, sm, dm, fm ); // ???
     zc = sp * sm / two;
     zs = (sp*cm - cp*sm) / (four*xk*yk);
     if ( xksq > y2ksq ) {
        zd = (dyy - zc) / xksq;
        zf = (fyy - zs) / xksq;
     }
     else {
        zd = (dx - zc) / y2ksq;
        zf = (fx - zs) / y2ksq;
     }
  }

  double t336 = sk2 * (cy*zd - two*sk1*sy*zf) + h * sk1 * fx * sy;
  double t346 = sk2 * (sy*zd - two*cy*zf) + h * fx * cy;
  double t436 = two * ys2 * fx * cy - sk2 * sk1 * (sy*zd - two*cy*zf);
  double t446 = two * ys2 * fx * sy - sk2 * (cy*zd - two*sk1*sy*zf);

  te_[1][3][3] = + sk2*sk1*zd + ys2*dx;
  te_[1][3][4] = + sk2*zs/two;
  te_[1][4][4] = + sk2*zd - h2*dx;

  te_[2][3][3] = + sk2*sk1*zs + ys2*sx;
  te_[2][3][4] = + sk2*zc/two;
  te_[2][4][4] = + sk2*zs - h2*sx;

  te_[3][1][3] = + sk2*(cy*zc/two - sk1*sy*zs) + h2*sk1*sx*sy;
  te_[3][1][4] = + sk2*(sy*zc/two - cy*zs) + h2*sx*cy;
  te_[3][2][3] = + sk2*(cy*zs/two - sk1*sy*zd) + h2*sk1*dx*sy;
  te_[3][2][4] = + sk2*(sy*zs/two - cy*zd) + h2*dx*cy;
  te_[3][3][6] = (h2*t336 - sk1*el*sy/four) * bi;
  te_[3][4][6] = (h2*t346 - (sy + el*cy) / four) * bi;

  te_[4][1][3] = sk2*sk1*(cy*zs - sy*zc/two) + ys2*sx*cy;
  te_[4][1][4] = sk2*(sk1*sy*zs - cy*zc/two) + ys2*sx*sy;
  te_[4][2][3] = sk2*sk1*(cy*zd - sy*zs/two) + ys2*dx*cy;
  te_[4][2][4] = sk2*(sk1*sy*zd - cy*zs/two) + ys2*dx*sy;
  te_[4][3][6] = (h2*t436 + sk1 * (sy - el*cy) / four) * bi;
  te_[4][4][6] = (h2*t446 - sk1*el*sy/four) * bi;

  te_[5][3][3] = (- h*sk2*sk1*zf - h*ys2*fx + sk1*(el-cy*sy)/four)*bi;
  te_[5][3][4] = (- h*sk2*zd/two - sk1*sy*sy/four) * bi;
  te_[5][4][4] = (- h*sk2*zf + h*h2*fx - (el + sy*cy)/four) * bi;



  // Change the signs of MAD coefficients involving time
  // ---------------------------------------------------

  for( int i = 1; i <= 6; ++i ) {
    for( int j = 1; j <= 6; ++j ) {
      for( int k = 1; k <= 6; ++k ) {
        if( 5 == i ) { te_[i][j][k] = - te_[i][j][k]; }
        if( 5 == j ) { te_[i][j][k] = - te_[i][j][k]; }
        if( 5 == k ) { te_[i][j][k] = - te_[i][j][k]; }
      }
    }
  }


  // call tmsymm(te)  ???
  // My best guess for what this is supposed to do.
  // -----------------------------------------------------
  for( int i = 1; i <= 6; ++i ) {
    for( int j = 1; j <= 6; ++j ) {
      for( int k = j; k <= 6; ++k ) {
        if( j != k ) {
          te_[i][k][j] = te_[i][j][k];
        }
      }
    }
  }

  // DGN: temporary kludge for diagnostic purpose only
  

  for( int i = 0; i < 7; ++i ) {
    for( int j = 0; j < 7; ++j ) {
      CF_sbend_MADPropagator::rre[i][j] = re_[i][j];
      for( int k = 0; k < 7; ++k ) {
        CF_sbend_MADPropagator::tte[i][j][k] = te_[i][j][k];
      }
    }
  }

  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend_MADPropagator::operator()( CF_sbend& elm, Particle& p)
{
  ::propagate( elm, p, this );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend_MADPropagator::operator()( CF_sbend& elm, JetParticle& p)
{
  ::propagate( elm, p, this );
}
