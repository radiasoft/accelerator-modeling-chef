/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LBSage.cc
******  Version:   1.0
******                                                                
****** Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/LBSage.h>
#include <beamline/Particle.h>
#include <beamline/BeamlineIterator.h>
#include <basic_toolkit/GenericException.h>

#include <physics_toolkit/BmlUtil.h>

using FNAL::pcout;
using FNAL::pcerr;


using namespace std;


LBSage::Info::Info() 
  : arcLength(-1.0),
    beta_1x(0.0),
    beta_1y(0.0),
    beta_2x(0.0),
    beta_2y(0.0),
    alpha_1x(0.0),
    alpha_1y(0.0),
    alpha_2x(0.0),
    alpha_2y(0.0),
    u1(0.0),
    u2(0.0),
    u3(0.0),
    u4(0.0),
    nu_1(0.0),
    nu_2(0.0)
{
}


LBSage::Info::Info( const Info& x ) 
  : arcLength(x.arcLength),
    beta_1x(x.beta_1x),
    beta_1y(x.beta_1y),
    beta_2x(x.beta_2x),
    beta_2y(x.beta_2y),
    alpha_1x(x.alpha_1x),
    alpha_1y(x.alpha_1y),
    alpha_2x(x.alpha_2x),
    alpha_2y(x.alpha_2y),
    u1(x.u1),
    u2(x.u2),
    u3(x.u3),
    u4(x.u4),
    nu_1(x.nu_1),
    nu_2(x.nu_2)
{
}


// ============================================================== //

LBSage::LBSage( const beamline* x, bool doClone ) 
: Sage( x, doClone ), _calcs(0), _n(0)
{
}


LBSage::LBSage( const beamline& x, bool doClone ) 
: Sage( &x, doClone ), _calcs(0), _n(0)
{
}


LBSage::~LBSage()
{
  _deleteCalcs();
}

void LBSage::_deleteCalcs()
{
  if( 0 != _calcs ) {
    for( int i = 0; i < _n; i++ ) 
    { if( 0 != _calcs[i] ) {delete _calcs[i];} }
    delete [] _calcs;
    _calcs = 0;
    _n = 0;
  }
}


void LBSage::_createCalcs()
{
  if( 0 != _calcs ) { _deleteCalcs(); }
  _n = _arrayPtr->size();
  _calcs = new Info* [_n];
  for( int i = 0; i < _n; i++ ) { _calcs[i] = 0;  }
}

// ============================================================== //

void LBSage::eraseAll() 
{
  _myBeamlinePtr->dataHook.eraseAll( "LBSage" );

  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be;
  while((  be = dbi++  )) {
    be->dataHook.eraseAll( "LBSage" );
  }

  _deleteCalcs();
}


int LBSage::doCalc( const JetParticle* ptr_jp, beamline::Criterion& crit )
{
  // PRECONDITIONS:  The JetParticle must be on the closed
  //   orbit with the one-turn  mapping for its state.

  JetParticle* localPtr = ptr_jp->Clone();
  Particle* ptr_proton  = ptr_jp->ConvertToParticle();

  const int n   = Particle::PSD;
  const int x   = Particle::_x();
  const int y   = Particle::_y();
  const int cdt = Particle::_cdt();
  const int xp  = Particle::_xp(); 
  const int yp  = Particle::_yp(); 
  const int dpp = Particle::_dpop();

  MatrixC E(n,n), E2(n,n);
  
  //*********************************************************
  // NOTE:
  // ----- 
  //
  // For some reason, MatrixC::eigenvector() will be called if
  // Matrix::eigenvector() is not explicitly specified. It is
  // not clear wether this behavior is compliant or ...
  // a compiler bug. -jfo
  //
  // 
  //*********************************************************
 
  E = ( localPtr->State() ).Jacobian().Matrix::eigenVectors();
  BmlUtil::normalize( E );

  Mapping id( "identity" );
  localPtr->setState( id );

  _createCalcs();

  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be;
  int i = 0;
  double lng = 0.0;
  std::complex<double>  temp;

  while( (0 != ( be = dbi++ )) && (i<_n) ) {
    _calcs[i] = new LBSage::Info;

    lng += be->OrbitLength( *ptr_proton );
    _calcs[i]->arcLength = lng;
    
    be->propagate( *localPtr );
    E2 = (( localPtr->State() ).Jacobian())*E;
    BmlUtil::normalize( E2 );
    
    // beta_1x and beta_2y
    temp = E2(x,x);
    if( real(temp) > 0.0 ) {
      _calcs[i]->beta_1x = 2.0*real(temp)*real(temp);
    }
    else { 
      _deleteCalcs(); delete ptr_proton; delete localPtr; 
       throw GenericException( __FILE__, __LINE__, 
              "LBSage::doCalc(const JetParticle*, beamline::Criterion&)",
			       "beta_1x is negative !. This error is likely due to an unstable lattice." );
       return 1; 
    }
    temp = real(E2(y,y));
    if( real(temp) > 0.0 ) {
      _calcs[i]->beta_2y = 2.0*real(temp)*real(temp);
    }
    else { 
      _deleteCalcs(); delete ptr_proton; delete localPtr; 
       throw GenericException( __FILE__, __LINE__, 
             "LBSage::doCalc(const JetParticle*, beamline::Criterion&)",
			       "beta_2y is negative ! This error is likely due to an unstable lattice." );
        return 2; 
     }

    // alpha_1x and alpha_2y
    temp = E2(xp,x)*E2(x,x);
    _calcs[i]->alpha_1x = -2.0*real(temp);
    _calcs[i]->u1       = -2.0*imag(temp);

    temp = E2(yp,y)*E2(y,y);
    _calcs[i]->alpha_2y = -2.0*real(temp);
    _calcs[i]->u4       = -2.0*imag(temp);

    // beta_1y and beta_2x
    temp = E2(y,x);
    _calcs[i]->beta_1y = 2.0*real(temp*conj(temp));
    temp = E2(x,y);
    _calcs[i]->beta_2x = 2.0*real(temp*conj(temp));

    // alpha_1y and alpha_2x
    temp = E2(yp,x)*E2(y,x);
    _calcs[i]->alpha_1y = -2.0*real(temp);
    _calcs[i]->u2       = -2.0*imag(temp);

    temp = E2(xp,y)*E2(y,y);
    _calcs[i]->alpha_2x = -2.0*real(temp);
    _calcs[i]->u3       = -2.0*imag(temp);

    i++;
  }

  if( i != _n ) {
    *_errorStreamPtr
         << "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** WARNING *** int LBSage::doCalc( const JetParticle* ptr_jp, beamline::Criterion& crit )"
            "\n*** WARNING *** Count comes out wrong: i = " << i
                                            << ";  _n = " << _n
         << "\n*** WARNING *** "
         << endl;
  }

  delete ptr_proton; 
  delete localPtr;
  return 0;
}


const LBSage::Info* LBSage::getInfoPtr( int j )
{
  if( 0 == _calcs || j < 0 || _arrayPtr->size() <= j ) 
  { return 0; }

  return _calcs[j];
}
