#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      TransitionVisitor.cc
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


/*
 * File: TransitionVisitor.cc
 * 
 * class TransitionVisitor
 * 
 * Calculates transition energy or gamma-t.
 * 
 * Leo Michelotti
 * May 15, 2002
 *
 */


#include "TransitionVisitor.h"

#include "rbend.h"
#include "sbend.h"
#include "CF_rbend.h"
#include "CF_sbend.h"

#include "LattFuncSage.h"
#include <typeinfo>

// Static error codes

const int  TransitionVisitor::NUMERRS    = 10;
const int  TransitionVisitor::OKAY       = 0;
const int  TransitionVisitor::NEGLEVEL   = 1;
const int  TransitionVisitor::ZEROLENGTH = 2;
const int  TransitionVisitor::NEGCOMP    = 3;
const int  TransitionVisitor::DOUBLED    = 4;
const int  TransitionVisitor::NOPROTON   = 5;
const int  TransitionVisitor::NOLATTFUNC = 6;
const int  TransitionVisitor::VERDISP    = 7;
const int  TransitionVisitor::NOELEMENTS = 8;
const int  TransitionVisitor::NULLBMLPTR = 9;
const char* TransitionVisitor::_errorMessage [] = 
{ "OKAY: No errors.",
  "NEGLEVEL: Negative beamline levels reached.",
  "ZEROLENGTH: No calculation done: beamline length = 0.",
  "NEGCOMP: Momentum compaction is negative.",
  "DOUBLED: Attempt to visit two beamlines with same TransitionVisitor.",
  "NOPROTON: No proton was instantiated.",
  "NOLATTFUNC: Dispersion has not been calculated.",
  "VERDISP: Vertical dispersion detected.  I'm not a Sage, you know!",
  "NOELEMENTS: The beamline is empty.",
  "NULLBMLPTR: Asked to visit a null pointer."
};

using namespace std;

// Constructors

TransitionVisitor::TransitionVisitor()
: _s(0.0),
  _alpha(0.0),
  _gamma_t(0.0),
  _errorCode(NOPROTON),
  _protonPtr(0),
  _level(0),
  _D(0.0),
  _Dprime(0.0),
  _calcDone(false),
  _coeff(4,4),
  _b(1,4),
  _f(4,1)
{
  _coeff(0,0) = 1.0;
  _coeff(1,1) = 1.0;
  _coeff(2,0) = 1.0;
  _coeff(3,1) = 1.0;
}


TransitionVisitor::TransitionVisitor( const TransitionVisitor& x )
: _s(x._s),
  _alpha(x._alpha),
  _gamma_t(x._gamma_t),
  _errorCode(x._errorCode),
  _level(x._level),
  _D(x._D),
  _Dprime(x._Dprime),
  _calcDone(x._calcDone),
  _coeff(x._coeff),
  _b(x._b),
  _f(x._f)
{
  if( x._protonPtr ) {
    _protonPtr = new Proton( *(x._protonPtr) );
    _errorCode = OKAY;
  }
  else {
    _protonPtr = 0;
    _errorCode = NOPROTON;
  }
}


TransitionVisitor::~TransitionVisitor()
{
  if( _protonPtr ) { delete _protonPtr; }
  _protonPtr = 0;
}


// Visiting functions

void TransitionVisitor::visitBeamline( beamline* x )
{
  // Some filters
  if( x == 0 ) {
    _errorCode = NULLBMLPTR;
    return;
  }
  if( _calcDone ) {
    _errorCode = DOUBLED;
    return;
  }
  if( !_protonPtr ) {
    _protonPtr = new Proton( x->Energy() );
    _errorCode = OKAY;
  }
  if( _errorCode != OKAY ) {
    return;
  }

  // Inititiate, if appropriate
  if( _level == 0 ) 
  {
    bmlnElmnt* l = x->lastElement();
    if( l == 0) { _errorCode = NOELEMENTS; return; }
    while( typeid(*l) == typeid(beamline) ) {
      l = ((beamline*) l)->lastElement();
      if( l == 0) { _errorCode = NOELEMENTS; return; }
    }
    this->_set_prev(l);
  }


  // Enter the beamline
  _level++;
  this->BmlVisitor::visitBeamline(x);
  // dlist_iterator getNext ( *(dlist*) x );
  // bmlnElmnt* p;
  // while ( 0 != (  p = (bmlnElmnt*) getNext() ) ) {
  //   p->accept( *this );
  // }
  _level--;
  

  // Finish the calculation, if appropriate
  if( _level == 0 ) {
    if( _s > 0.0 ) {
      _alpha = _alpha / _s;
      _calcDone = true;
    }
    else {
      _errorCode = ZEROLENGTH;
      cerr << "\n*** ERROR ***                              "
           << "\n*** ERROR *** " 
           << _errorMessage[_errorCode]
           << "\n*** ERROR *** " 
           << endl;
    }
  }
  
  else if( _level < 0 ) {
    cerr << "\n*** ERROR ***                              "
            "\n*** ERROR *** TransitionVisitor::visitBeamline  "
            "\n*** ERROR *** Impossible! _level = " << _level << " < 0."
            "\n*** ERROR ***                              "
         << endl;
    _errorCode = NEGLEVEL;
  }
}


void TransitionVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  if( !_calcDone && (_errorCode == OKAY) ) 
  {
    _s += x->OrbitLength( *(_protonPtr) );
    this->_set_prev(x);
  }
}


void TransitionVisitor::_visit_bend( bmlnElmnt* x )
{
  if( !_calcDone && (_errorCode == OKAY) ) 
  {
    // Filters
    LattFuncSage::lattFunc* lfPtr =
      dynamic_cast<LattFuncSage::lattFunc*>(x->dataHook.find("Twiss"));
    if( lfPtr == 0 ) {
      _errorCode = NOLATTFUNC;
      return;
    }
    
    if( fabs(lfPtr->dispersion.ver) > 1.0e-9 || 
        fabs(lfPtr->dPrime.ver    ) > 1.0e-9    ) 
    {
      _errorCode = VERDISP;
      return;
    }

    // The calculation
    double lng = x->OrbitLength( *(_protonPtr) );
    _coeff(2,1) = lng;
    _coeff(2,2) = lng*lng;
    _coeff(2,3) = _coeff(2,2)*lng;
    _coeff(3,2) = 2.0*lng;
    _coeff(3,3) = 3.0*lng*lng;
  
    double temp;
    temp  = lng; _b(0,0) = temp;
    temp *= lng; _b(0,1) = temp/2.0;
    temp *= lng; _b(0,2) = temp/3.0;
    temp *= lng; _b(0,3) = temp/4.0;

    double str  = x->Strength();
    double brho = _protonPtr->ReferenceBRho();
    _f(0,0) = str*_D/brho;
    _f(1,0) = str*_Dprime/brho;
    _f(2,0) = str*(lfPtr->dispersion.hor)/brho;
    _f(3,0) = str*(lfPtr->dPrime.hor)/brho;

    _alpha += ( _b*_coeff.inverse()*_f )(0,0);  // inefficient use of inverse()

    _s += lng;
    this->_set_prev(x);
  }
}


void TransitionVisitor::visitRbend( rbend* x )
{
  this->_visit_bend(x);
}


void TransitionVisitor::visitSbend( sbend* x )
{
  this->_visit_bend(x);
}


void TransitionVisitor::visitCF_rbend( CF_rbend* x )
{
  this->_visit_bend(x);
}


void TransitionVisitor::visitCF_sbend( CF_sbend* x )
{
  this->_visit_bend(x);
}


void TransitionVisitor::_set_prev( const bmlnElmnt* x )
{
  // Filters
  LattFuncSage::lattFunc* lfPtr =
    dynamic_cast<LattFuncSage::lattFunc*>(x->dataHook.find("Twiss"));
  if( lfPtr == 0 ) {
    _errorCode = NOLATTFUNC;
    return;
  }
  
  if( fabs(lfPtr->dispersion.ver) > 1.0e-9 || 
      fabs(lfPtr->dPrime.ver    ) > 1.0e-9    ) 
  {
    _errorCode = VERDISP;
    return;
  }


  // Body
  _D      = lfPtr->dispersion.hor;
  _Dprime = lfPtr->dPrime.hor;
}


// Evaluators

double TransitionVisitor::getTransitionGamma() const
{
  if( _calcDone && (_errorCode == OKAY) && (_alpha > 0.0) ) {
    return sqrt(1.0/_alpha);
  }
  return -1.0;  // Error flag.
}


double TransitionVisitor::getMomentumCompaction() const
{
  if( _calcDone && (_errorCode == OKAY) ) {
    return _alpha;
  }
  return 0.0; 
}


int TransitionVisitor::getErrorCode() const
{
  return _errorCode;
}


const char* TransitionVisitor::getErrorMessage( int i ) const
{
  if( 0 <= i  &&  i < NUMERRS ) {
    return _errorMessage[i];
  }
  return 0;
}


const char* TransitionVisitor::getErrorMessage() const
{
  return _errorMessage[_errorCode];
}
