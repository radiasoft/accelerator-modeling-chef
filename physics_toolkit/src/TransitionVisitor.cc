/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      TransitionVisitor.cc
******  Version:   2.2
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

#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/TransitionVisitor.h>
#include <beamline/rbend.h>
#include <beamline/sbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <physics_toolkit/LattFuncSage.h>
#include <typeinfo>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// Static error codes

const int  TransitionVisitor::NUMERRS    = 10;
const int  TransitionVisitor::OKAY       = 0;
const int  TransitionVisitor::NEGLEVEL   = 1;
const int  TransitionVisitor::ZEROLENGTH = 2;
const int  TransitionVisitor::NEGCOMP    = 3;
const int  TransitionVisitor::DOUBLED    = 4;
const int  TransitionVisitor::NOPARTICLE = 5;
const int  TransitionVisitor::NOLATTFUNC = 6;
const int  TransitionVisitor::VERDISP    = 7;
const int  TransitionVisitor::NOELEMENTS = 8;
const int  TransitionVisitor::NULLBMLPTR = 9;
const char* TransitionVisitor::errorMessage_ [] = 
{ "OKAY: No errors.",
  "NEGLEVEL: Negative beamline levels reached.",
  "ZEROLENGTH: No calculation done: beamline length = 0.",
  "NEGCOMP: Momentum compaction is negative.",
  "DOUBLED: Attempt to visit two beamlines with same TransitionVisitor.",
  "NOPARTICLE: No particle was instantiated.",
  "NOLATTFUNC: Dispersion has not been calculated.",
  "VERDISP: Vertical dispersion detected.  I'm not a Sage, you know!",
  "NOELEMENTS: The beamline is empty.",
  "NULLBMLPTR: Asked to visit a null pointer."
};


// Constructors

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TransitionVisitor::TransitionVisitor()
: s_(0.0),
  alpha_(0.0),
  gamma_t_(0.0),
  errorCode_(NOPARTICLE),
  particlePtr_(0),
  level_(0),
  D_(0.0),
  Dprime_(0.0),
  calcDone_(false),
  coeff_(4,4),
  b_(1,4),
  f_(4,1)
{
   coeff_(0,0) = 1.0;
   coeff_(1,1) = 1.0;
   coeff_(2,0) = 1.0;
   coeff_(3,1) = 1.0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TransitionVisitor::TransitionVisitor( const TransitionVisitor& x )
: s_(x.s_),
  alpha_(x.alpha_),
  gamma_t_(x.gamma_t_),
  errorCode_(x.errorCode_),
  level_(x.level_),
  D_(x.D_),
  Dprime_(x.Dprime_),
  calcDone_(x.calcDone_),
  coeff_(x.coeff_),
  b_(x.b_),
  f_(x.f_)
{
  if( x.particlePtr_ ) {
    particlePtr_ = (x.particlePtr_)->Clone();
    errorCode_   = OKAY;
  }
  else {
    particlePtr_ = 0;
    errorCode_   = NOPARTICLE;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TransitionVisitor::TransitionVisitor( const Particle& x )
: s_(0.0),
  alpha_(0.0),
  gamma_t_(0.0),
  errorCode_(OKAY),
  particlePtr_( x.Clone() ),
  level_(0),
  D_(0.0),
  Dprime_(0.0),
  calcDone_(false),
  coeff_(4,4),
  b_(1,4),
  f_(4,1)
{
  coeff_(0,0) = 1.0;
  coeff_(1,1) = 1.0;
  coeff_(2,0) = 1.0;
  coeff_(3,1) = 1.0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TransitionVisitor::~TransitionVisitor()
{
  if( particlePtr_ ) { delete particlePtr_; }
   particlePtr_ = 0;
}


// Visiting functions

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( beamline& x )
{
  if( calcDone_ ) {
    errorCode_ = DOUBLED;
    return;
  }
  if( !particlePtr_ ) {
     errorCode_ = NOPARTICLE;
  }
  if(  errorCode_ != OKAY ) {
    return;
  }

  // Inititiate, if appropriate
  if( level_ == 0 ) 
  {
    bmlnElmnt* l = x.lastElement().get();                   //////////////FIXME !!!!!
     if( l == 0) { errorCode_ = NOELEMENTS; return; }
    while( typeid(*l) == typeid(beamline) ) {
      l = ((beamline*) l)->lastElement().get();              //////////////FIXME !
      if( l == 0) { errorCode_ = NOELEMENTS; return; }
    }
    set_prev(*l);
  }


  // Enter the beamline
  ++level_;
  BmlVisitor::visit(x);
  --level_;
  

  // Finish the calculation, if appropriate
  if( level_ == 0 ) {
    if( s_ > 0.0 ) {
       alpha_    = alpha_ / s_;
       calcDone_ = true;
    }
    else {
      errorCode_ = ZEROLENGTH;
      (*pcerr) << "\n*** ERROR ***                              "
           << "\n*** ERROR *** " 
           << errorMessage_[ errorCode_]
           << "\n*** ERROR *** " 
           << endl;
    }
  }
  
  else if( level_ < 0 ) {
    (*pcerr) << "\n*** ERROR ***                              "
            "\n*** ERROR *** TransitionVisitor::visitBeamline  "
            "\n*** ERROR *** Impossible! level_ = " << level_ << " < 0."
            "\n*** ERROR ***                              "
         << endl;
    errorCode_ = NEGLEVEL;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( bmlnElmnt& x )
{
  if( ! calcDone_ && ( errorCode_ == OKAY) ) 
  {
    s_ += x.OrbitLength( *(particlePtr_) );
    set_prev(x);
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit_bend( bmlnElmnt& x )
{
  if( !calcDone_ && ( errorCode_ == OKAY) ) 
  {
    // Filters


    BarnacleList::iterator it = x.dataHook.find("Twiss");

    if( it ==  x.dataHook.end() ) {
      errorCode_ = NOLATTFUNC;
      return;
    }

    LattFuncSage::lattFunc lf = boost::any_cast<LattFuncSage::lattFunc>( it->info );
    
    if( fabs(lf.dispersion.ver) > 1.0e-9 || 
        fabs(lf.dPrime.ver    ) > 1.0e-9    ) 
    {
      errorCode_ = VERDISP;
      return;
    }

    // The calculation
    double lng = x.OrbitLength( *(particlePtr_) );
    coeff_(2,1) = lng;
    coeff_(2,2) = lng*lng;
    coeff_(2,3) = coeff_(2,2)*lng;
    coeff_(3,2) = 2.0*lng;
    coeff_(3,3) = 3.0*lng*lng;
  
    double temp;
    temp  = lng; b_(0,0) = temp;
    temp *= lng; b_(0,1) = temp/2.0;
    temp *= lng; b_(0,2) = temp/3.0;
    temp *= lng; b_(0,3) = temp/4.0;

    double str  = x.Strength();
    double brho = particlePtr_->ReferenceBRho();
    f_(0,0) = str * D_/brho;
    f_(1,0) = str * Dprime_/brho;
    f_(2,0) = str * (lf.dispersion.hor)/brho;
    f_(3,0) = str * (lf.dPrime.hor)/brho;

    alpha_ += ( b_* coeff_.inverse()*f_ )(0,0);  // inefficient use of inverse()

    s_ += lng;
    set_prev(x);
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( rbend& x )
{
  visit_bend(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( sbend& x )
{
  visit_bend(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( CF_rbend& x )
{
  visit_bend(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::visit( CF_sbend& x )
{
  visit_bend(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TransitionVisitor::set_prev( bmlnElmnt const& x )
{
  // Filters

  BarnacleList::const_iterator it = x.dataHook.find("Twiss");  
 
  if( it == x.dataHook.end() ) {
    errorCode_ = NOLATTFUNC;
    return;
  }
  
 LattFuncSage::lattFunc lf = boost::any_cast<LattFuncSage::lattFunc>(it->info);


  if( fabs(lf.dispersion.ver) > 1.0e-9 || 
      fabs(lf.dPrime.ver    ) > 1.0e-9    ) 
  {
    errorCode_ = VERDISP;
    return;
  }


  // Body
  D_      = lf.dispersion.hor;
  Dprime_ = lf.dPrime.hor;
}


// Evaluators

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TransitionVisitor::getTransitionGamma() const
{
  if( calcDone_ && ( errorCode_ == OKAY) && ( alpha_ > 0.0) ) {
    return sqrt(1.0/ alpha_);
  }
  return -1.0;  // Error flag.
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TransitionVisitor::getMomentumCompaction() const
{
  if( calcDone_ && ( errorCode_ == OKAY) ) {
    return alpha_;
  }
  return 0.0; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int TransitionVisitor::getErrorCode() const
{
  return errorCode_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* TransitionVisitor::getErrorMessage( int i ) const
{
  if( 0 <= i  &&  i < NUMERRS ) {
    return errorMessage_[i];
  }
  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* TransitionVisitor::getErrorMessage() const
{
  return errorMessage_[ errorCode_ ];
}
