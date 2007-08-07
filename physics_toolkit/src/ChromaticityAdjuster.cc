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
****** REVISION HISTORY:
****** 
******  January  2007:  Jean-Francois Ostiguy
******                  ostiguy@fnal.gov
****** 
******  - reference counted elements
******  - use value semantics for matrix members
******  - use new style STL compatible iterators
******
********************************************************************************
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
#include <beamline/JetParticle.h>
#include <beamline/sextupole.h>
#include <physics_toolkit/ChromaticityAdjuster.h>
#include <physics_toolkit/LattFuncSage.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ChromaticityAdjuster::ChromaticityAdjuster( BmlPtr x ) 
: Sage( x ) {}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ChromaticityAdjuster::ChromaticityAdjuster( beamline const& x ) 
: Sage( x ) {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ChromaticityAdjuster::~ChromaticityAdjuster() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  ChromaticityAdjuster::numberOfCorrectors() const
{
  return correctors_.size();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool ChromaticityAdjuster::isaThinSextupole( bmlnElmnt const& q ) const
{
  return ( 0 == strcmp( q.Type(), "thinSextupole" ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD& ChromaticityAdjuster::getControls()
{
  return c_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ChromaticityAdjuster::addCorrector_private(  ElmPtr x, double a, double b ) 
{

  MatrixD old_f = f_;

  correctors_.push_back(x);

  int ncor =  correctors_.size();

  f_ = MatrixD(ncor,2);

  for( int i= 0;  i < ncor-1; ++i) {
    f_(i,0) = old_f(i,0);
    f_(i,1) = old_f(i,1);
  }

  f_( ncor-1, 0 )    = a;
  f_( ncor-1, 1 )    = b;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  ChromaticityAdjuster::addCorrector(  ElmPtr x, double a, double b ) 
{
  // This form is used by the GUI code. It MUST go away !


  SextupolePtr     ps;  
  ThinSextupolePtr pts;  

  if ( ps  = boost::dynamic_pointer_cast<sextupole>(x)     ) 
  {
      addCorrector_private( x, a, b );
      return;
  }

  if ( pts = boost::dynamic_pointer_cast<thinSextupole>(x) ) 
      addCorrector_private( x, a, b );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ChromaticityAdjuster::addCorrector( sextupole const& x, double a, double b )
{
  addCorrector_private( ElmPtr( x.Clone() ), a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ChromaticityAdjuster::addCorrector( SextupolePtr x, double a, double b )
{
  addCorrector_private( x, a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ChromaticityAdjuster::addCorrector( thinSextupole const& x, double a, double b )
{
  addCorrector_private( ElmPtr( x.Clone() ), a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ChromaticityAdjuster::addCorrector( ThinSextupolePtr x, double a, double b )
{
  addCorrector_private( x, a, b );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetParticle& jp )
{
  double delta_H = x;
  double delta_V = y;

  myBeamlinePtr_->dataHook.eraseAll( "Ring" );
  myBeamlinePtr_->eraseBarnacles( "Ring" );

  // 

  JetParticle  jpr(jp); 
  JetParticle  jpr2(jp);
  JetParticle  jpr3(jp);

  // Calculate current lattice functions
  LattFuncSage lfs( myBeamlinePtr_ );
 
  myBeamlinePtr_->propagate(jpr);

  // lfs.Fast_CS_Calc( jprPtr  );

  lfs.Slow_CS_Calc( jpr);
  lfs.Disp_Calc   ( jpr2);
 
  int N = correctors_.size();

  MatrixD beta      (2,N);
  MatrixD delta_xi  (2,1);
  double  dsp;

  // delta_xi = beta*_f*c
  // w = _f*c
  // delta strength_k = 2 pi * brho * w_k / dps_k
 
  bool gotDispersion = false;
  bool gotBetas      = false;

  for( int j = 0; j < N; ++j) 
  {
    gotDispersion = false;
    gotBetas      = false;

    BarnacleList::iterator ptr = correctors_[j]->dataHook.find( "Dispersion" );
    if( ptr ==  correctors_[j]->dataHook.end() ) { 
      ptr = correctors_[j]->dataHook.find( "Twiss" );
    }

    if( ptr != correctors_[j]->dataHook.end() ) { 
      dsp           =   boost::any_cast<LattFuncSage::lattFunc>(ptr->info).dispersion.hor;
      gotDispersion = true;
    }
    else {
      dsp = 0.0;  // Just to give it a value.
    }

    ptr = correctors_[j]->dataHook.find( "Twiss" );
    // NOTE: possibly 2nd time this is done.
    if(ptr !=  correctors_[j]->dataHook.end() ) {
      beta(0,j) =   boost::any_cast<LattFuncSage::lattFunc>(ptr->info).beta.hor * dsp;
      beta(1,j) = - boost::any_cast<LattFuncSage::lattFunc>(ptr->info).beta.ver * dsp;
      gotBetas = true;
    }

    if( !(gotBetas && gotDispersion) ) {
      throw( GenericException( __FILE__, __LINE__, 
             "int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetParticle& jp )", 
             "Lattice functions or dispersion not yet calculated." ) );
    }
  }
  
  // Adjust tunes and recalculate

  delta_xi(0,0) = delta_H;
  delta_xi(1,0) = delta_V;
 
  double brho = jpr.ReferenceBRho();
  c_ = (M_TWOPI*brho)*( (beta*f_).inverse() * delta_xi );

  MatrixD w  = f_* c_;
 
  int j=0;
  for( std::vector<ElmPtr>::iterator it = correctors_.begin(); it != correctors_.end(); ++it, ++j) 
  {

    if( isaThinSextupole(**it) ) {
      (*it)->setStrength( (*it)->Strength() + w(j,0) );
    }
    else {
      (*it)->setStrength( (*it)->Strength() + (w(j,0)/(*it)->Length()) );
    }
  }
 
  return 0;
  }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void ChromaticityAdjuster::eraseAll()
{ }
