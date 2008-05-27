/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinMultipole.cc
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
******  Author: ostiguy@fnal.gov
******                                                               
**************************************************************************
*************************************************************************/

#include <beamline/ThinMultipolePropagators.h>
#include <beamline/thinMultipole.h>
#include <beamline/BmlVisitor.h>

using namespace std;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole () 
  : bmlnElmnt( "", 0.0, 0.0 ), poles_() 
{
  propagator_ = PropagatorPtr( new Propagator());
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole ( std::string const& name) 
  : bmlnElmnt( name, 0.0, 0.0 ), poles_() 
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole(  thinMultipole const& x ) 
  : bmlnElmnt( x ), poles_(x.poles_)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::~thinMultipole() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinMultipole::isMagnet() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  thinMultipole::setPole(int n, std::complex<double> const&  coeff)
{
  poles_[n] = coeff; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::const_iterator thinMultipole::begin() const
{
  return poles_.begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::const_iterator thinMultipole::end() const
{
  return poles_.end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::accept( BmlVisitor& v ) 
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

