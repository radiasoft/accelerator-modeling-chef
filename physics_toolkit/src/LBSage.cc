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
******  Copyright (c) 2004  Universities Research Association, Inc.   
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include "LBSage.h"
#include "Particle.h"
#include "BeamlineIterator.h"

#include "BmlUtil.h"  // Temporary line

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
  E = ( localPtr->State() ).Jacobian().eigenVectors();
  BmlUtil::normalize( E );

  Mapping id( "identity" );
  localPtr->setState( id );

  _createCalcs();

  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be;
  int i = 0;
  double lng = 0.0;
  FNAL::Complex temp;

  while( (0 != ( be = dbi++ )) && (i<_n) ) {
    lng += be->OrbitLength( *ptr_proton );
    _calcs[i]->arcLength = lng;
    
    be->propagate( *localPtr );
    E2 = (( localPtr->State() ).Jacobian())*E;
    
    // beta_1x and beta_2y
    temp = E2(x,x);
    if( real(temp) > 0.0 ) {
      _calcs[i]->beta_1x = real(temp)*real(temp);
    }
    else { 
      _deleteCalcs(); delete ptr_proton; delete localPtr; return 1; 
    }
    temp = real(E2(y,y));
    if( real(temp) > 0.0 ) {
      _calcs[i]->beta_2y = real(temp)*real(temp);
    }
    else { 
      _deleteCalcs(); delete ptr_proton; delete localPtr; return 2; 
    }

    // beta_1y and beta_2x
    temp = E2(y,x);
    _calcs[i]->beta_1y = real(temp*conj(temp));
    temp = E2(x,y);
    _calcs[i]->beta_2x = real(temp*conj(temp));

    // And that's all I'll bother with for now
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


const LBSage::Info* LBSage::get_LBFuncPtr( int j )
{
  if( 0 == _calcs || j < 0 || _arrayPtr->size() <= j ) 
  { return 0; }

  return _calcs[j];
}
