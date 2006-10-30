/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      ChromaticityAdjuster.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 *  File: ChromaticityAdjuster.cc
 *  
 *  Implementation for a sage that adjusts
 *  chromaticities given a collection of "correction"
 *  sextupoles
 *  
 *  Leo Michelotti
 *  February 1, 1999
 */


#include <basic_toolkit/GenericException.h>
#include <beamline/Particle.h>
#include <beamline/sextupole.h>
#include <physics_toolkit/ChromaticityAdjuster.h>
#include <physics_toolkit/LattFuncSage.h>

using namespace std;

ChromaticityAdjuster::ChromaticityAdjuster( const beamline* x, bool doClone ) 
: Sage( x, doClone ), _numberOfCorrectors(0), _c(2,1)
{
  _correctors = 0;
  _f = 0;
}

ChromaticityAdjuster::ChromaticityAdjuster( const beamline& x, bool doClone ) 
: Sage( &x, doClone ), _numberOfCorrectors(0), _c(2,1)
{
  _correctors = 0;
  _f = 0;
}

ChromaticityAdjuster::~ChromaticityAdjuster() 
{
  if( _correctors ) delete [] _correctors;
  if( _f ) delete _f;
}


void ChromaticityAdjuster::_addCorrector( const bmlnElmnt* x,
                                          double a, double b )
{
  int         i;
  bmlnElmnt** oldCorrectors = 0;
  MatrixD*    old_f = 0;

  if( _numberOfCorrectors > 0 ) 
  {
    oldCorrectors = new bmlnElmnt* [ _numberOfCorrectors ];
    old_f = new MatrixD( _numberOfCorrectors, 2 );

    for( i = 0; i < _numberOfCorrectors; i++ ) 
    {
      oldCorrectors[i] = _correctors[i];

      (*old_f)(i,0) = (*_f)(i,0);
      (*old_f)(i,1) = (*_f)(i,1);
    }

    delete [] _correctors;
    _correctors = 0;   // Not really necessary.
    delete _f;
    _f = 0;            // Not really necessary.
  }

  _numberOfCorrectors++;
  _correctors = new bmlnElmnt* [ _numberOfCorrectors ];
  _f = new MatrixD( _numberOfCorrectors, 2 );
  for( i = 0; i < _numberOfCorrectors-1; i++ ) {
    _correctors[i] = oldCorrectors[i];
    (*_f)(i,0) = (*old_f)(i,0);
    (*_f)(i,1) = (*old_f)(i,1);
  }
  _correctors[ _numberOfCorrectors-1 ] = (bmlnElmnt*) x;
  (*_f)( _numberOfCorrectors-1, 0 )    = a;
  (*_f)( _numberOfCorrectors-1, 1 )    = b;


  if( oldCorrectors ) delete [] oldCorrectors;
  if( old_f )         delete old_f;
}


void ChromaticityAdjuster::addCorrector( const sextupole& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void ChromaticityAdjuster::addCorrector( const sextupole* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}

void ChromaticityAdjuster::addCorrector( const thinSextupole& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void ChromaticityAdjuster::addCorrector( const thinSextupole* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}


int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetParticle& jp )
{
  int j;
  double delta_H = x;
  double delta_V = y;

  _myBeamlinePtr->dataHook.eraseAll( "Ring" );
  _myBeamlinePtr->eraseBarnacles( "Ring" );

  // 
  JetParticle* jprPtr  = jp.Clone();
  JetParticle* jpr2Ptr = jp.Clone(); // Necessary????
  JetParticle* jpr3Ptr = jp.Clone(); // Necessary????

  // Calculate current lattice functions
  LattFuncSage lfs( _myBeamlinePtr );
 
  _myBeamlinePtr->propagate( *jprPtr );
  // lfs.Fast_CS_Calc( jprPtr  );
  lfs.Slow_CS_Calc( jprPtr  );
  lfs.Disp_Calc   ( jpr2Ptr );
 
  int N = this->numberOfCorrectors();
  MatrixD beta      (2,N);
  MatrixD delta_xi  (2,1);
  MatrixD w         (N,1);
  double  dsp;
  // delta_xi = beta*_f*c
  // w = _f*c
  // delta strength_k = 2 pi * brho * w_k / dps_k
 
  LattFuncSage::lattFunc* ptr;
  bmlnElmnt* q; 
  bool gotDispersion;
  bool gotBetas;
  for( j = 0; j < N; j++ ) 
  {
    gotDispersion = false;
    gotBetas      = false;

    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Dispersion" );
    if(!ptr) { 
      ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    }
    if(ptr) { 
      dsp =   ptr->dispersion.hor;
      gotDispersion = true;
    }
    else {
      dsp = 0.0;  // Just to give it a value.
    }

    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    // NOTE: possibly 2nd time this is done.
    if(ptr) {
      beta(0,j) =   ptr->beta.hor * dsp;
      beta(1,j) = - ptr->beta.ver * dsp;
      gotBetas = true;
    }

    if( !(gotBetas && gotDispersion) ) {
      delete jprPtr;  jprPtr  = 0;
      delete jpr2Ptr; jpr2Ptr = 0;
      delete jpr3Ptr; jpr3Ptr = 0;
      throw( GenericException( __FILE__, __LINE__, 
             "int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetParticle& jp )", 
             "Lattice functions or dispersion not yet calculated." ) );
    }
  }
  

  // Adjust tunes and recalculate
  delta_xi(0,0) = delta_H;
  delta_xi(1,0) = delta_V;
 
  double brho = jprPtr->ReferenceBRho();
  _c = (M_TWOPI*brho)*( (beta*(*_f)).inverse() * delta_xi );
  w = (*_f)*_c;
 
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isaThinSextupole(q) ) {
      q->setStrength( q->Strength() + w(j,0) );
    }
    else {
      q->setStrength( q->Strength() + (w(j,0)/q->Length()) );
    }
  }

 
  // Clean up ...
  // REMOVE: _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  // REMOVE: _myBeamlinePtr->eraseBarnacles( "Twiss" );
  delete jprPtr;  jprPtr  = 0;
  delete jpr2Ptr; jpr2Ptr = 0;
  delete jpr3Ptr; jpr3Ptr = 0;

  return 0;
}


void ChromaticityAdjuster::eraseAll()
{
}
