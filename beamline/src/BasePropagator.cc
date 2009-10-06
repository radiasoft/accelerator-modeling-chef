/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BasePropagator.cc
******
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
******
**************************************************************************
*************************************************************************/

#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>
#include <beamline/BasePropagator.h>
#include <beamline/BmlnElmnt.h>
#include <beamline/beamline.h>
#include <iostream>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::BasePropagator()
  : ctRef_(0.0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::BasePropagator( BmlnElmnt const& elm )
  : ctRef_(0.0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::ctor( BmlnElmnt const& elm) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::BasePropagator( BasePropagator const& o)
  : ctRef_(o.ctRef_)
{
  if( o.bml_ ) { bml_ = BmlPtr(o.bml_->clone()); }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator& BasePropagator::operator=( BasePropagator const& o)
{
  if ( this == &o ) return *this; 
  
  return *this; //default  
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::~BasePropagator() {}; 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::operator()(  BmlnElmnt const& elm,    ParticleBunch& b) 
{
 for (  ParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    (*this)( elm, *it ); 
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::operator()(  BmlnElmnt const& elm, JetParticleBunch& b)  
{
 for (  JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    (*this)( elm, *it ); 
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   BasePropagator::setReferenceTime( double ct ) 
{
  ctRef_ = ct;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BasePropagator::getReferenceTime() const 
{
  return ctRef_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   BasePropagator::setAttribute( BmlnElmnt& elm, std::string const& attribute, boost::any const& value )
{ 
  // default: the propagator reconstructs itself.  
 
     ctor( elm ); 

  // if this member is overriden in a derived class, the code should 
  // call ctor() for any attribute change that is not explicitly handled.

  // std::cout << "element   " << elm.Type() << "  " << elm.Name() << std::endl;   
  // std::cout << "attribute " << attribute << " has been modified." << std::endl;   
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BasePropagator::isComposite() const
{ 
 return bml_;
} 
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BasePropagator::hasAperture() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BasePropagator::hasAlignment() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BasePropagator::setAlignment( Vector const& translation, Vector const& rotation, bool )
{
  /** do nothing **/ 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BasePropagator::setAperture( BmlnElmnt::aperture_t const, double const& hor, double const& ver )
{
  /** do nothing **/ 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<BmlnElmnt::aperture_t, double, double>  BasePropagator::aperture()   const
{
  /** do nothing **/ 
  return boost::tuple<BmlnElmnt::aperture_t, double, double>(BmlnElmnt::infinite, 0.0, 0.0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
boost::tuple<Vector,Vector>  BasePropagator::alignment()   const
{
  /** do nothing **/ 
  return boost::tuple<Vector,Vector>(Vector(),Vector());
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BasePropagator::propagateReference( Particle& particle, double initialBRho, bool scaling )
{}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
