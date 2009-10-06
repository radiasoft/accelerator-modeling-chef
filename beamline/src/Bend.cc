/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Bend.cc
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
******  Author: Jean-Francois Ostiguy  ostiguy@fnal.gov
******                                                                
****** REVISION HISTORY 
******
****** May 2008           ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/Bend.h>
#include <beamline/BendPropagators.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Bend::Bend( const char* n, double const& l, double const& s, double const& alpha, 
                           double const& usAngle,     double const& dsAngle, 
                           double const& usFaceAngle, double const& dsFaceAngle, Bend::BendType bend_type) 
  : BmlnElmnt( n, l, s ),
    angle_(alpha),
    usAngle_(usAngle),
    dsAngle_(dsAngle),
    usFaceAngle_(usFaceAngle),
    dsFaceAngle_(dsFaceAngle),
    type_(bend_type)
{
  propagator_ = PropagatorPtr( new Propagator(*this) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Bend::Bend( Bend const& x )
:   BmlnElmnt( x ),      
      angle_(x.angle_),
    usAngle_(x.usAngle_),
    dsAngle_(x.dsAngle_),
usFaceAngle_(x.usFaceAngle_),
dsFaceAngle_(x.dsFaceAngle_),
       type_(x.type_)
{
  propagator_->ctor(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Bend::~Bend() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Bend& Bend::operator=(Bend const& rhs)
{
  if (this == &rhs) return *this;

  BmlnElmnt::operator=(rhs);
  angle_       = rhs.angle_;
  usAngle_     = rhs.usAngle_;
  dsAngle_     = rhs.dsAngle_;
  usFaceAngle_ = rhs.usFaceAngle_;
  dsFaceAngle_ = rhs.dsFaceAngle_;
         type_ = rhs.type_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Bend::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Bend::isThin()    const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Bend::isPassive() const
{
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Bend::isDriftSpace() const
{
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* Bend::Type() const 
{ 
  return "Bend"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Bend::BendType Bend::getBendType() const 
{
  return type_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Bend::accept( BmlVisitor& v ) 
{ 
 v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Bend::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& Bend::writeTo(std::ostream& os)
{ 
  return os;
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::istream& Bend::readFrom(std::istream& is) 
{ 
  return is; 
}
