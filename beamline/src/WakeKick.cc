/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKick.cc
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******
******  REVISION HISTORY
******
******  May 2008           ostiguy@fnal.gov
******  - setStrength() now dispatched to propagator by base class
******    (no longer virtual)
******  - added explicit implementation for assignment operator
******
**************************************************************************
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/BmlVisitor.h>
#include <beamline/WakeFunctions.h>
#include <beamline/BunchProjector.h>
#include <iomanip>
#include <iostream>
#include <boost/bind.hpp>

using namespace std;


WakeKick::WakeKick( std::string const& name )
  : bmlnElmnt(name, 0.0, 0.0)
{ 
   propagator_ = PropagatorPtr( new Propagator( 256, 12 * 300.0e-6 ) );    
   propagator_->setup(*this);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::WakeKick( WakeKick const& x )
  : bmlnElmnt(x)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::~WakeKick() 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick& WakeKick::operator=( WakeKick const& rhs) 
{ 
  if ( this == & rhs) return *this;
  bmlnElmnt::operator=(rhs);
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* WakeKick::Type() const 
{ 
  return "WakeKick"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     WakeKick::isMagnet() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     WakeKick::isThin() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     WakeKick::isPassive() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     WakeKick::isDriftSpace() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::accept( BmlVisitor& v ) 
{ 
   v.visit(*this ); 
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::accept( ConstBmlVisitor& v ) const 
{ 
   v.visit(*this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

