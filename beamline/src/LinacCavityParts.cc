/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityParts.cc
******
******  Copyright (c) Fermi Alliance / Fermilab    
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
******  Author:    Jean-Francois Ostiguy                       
******             ostiguy@fnal.gov
******
****** REVISION HISTORY
******
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** May 2008   ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/LinacCavityPartsPropagators.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/BmlVisitor.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream::LCavityUpstream( std::string const& name, double const& length, double const& rfreq,         
                                                      double const& volts,  double const& phis)
  : bmlnElmnt(name,length,volts*1.0e-9), w_rf_(2.0*M_PI*rfreq), phi_s_(phis)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream::LCavityUpstream( LCavityUpstream const& o)
  : bmlnElmnt(o), w_rf_(o.w_rf_), phi_s_(o.phi_s_)

{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream* LCavityUpstream::Clone() const 
{ 
  return new LCavityUpstream( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream::~LCavityUpstream()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream& LCavityUpstream::operator=(  LCavityUpstream const& rhs)
{
  if (this == &rhs) return *this;

  bmlnElmnt::operator=( rhs);

  w_rf_   = rhs.w_rf_;
  phi_s_  = rhs.phi_s_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LCavityUpstream::accept( BmlVisitor&      v )
{
 v.visit( *this ); 
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::accept( ConstBmlVisitor& v ) const
{
 v.visit( *this ); 
} 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LCavityUpstream::getPhi()  const
{
  return phi_s_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LCavityUpstream::getRadialFrequency()  const
{
  return w_rf_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double LCavityUpstream::getDesignEnergyGain()   const
{
  return Strength()* cos(phi_s_); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* LCavityUpstream::Type() const
{
  return "LCavityUpstream"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    LCavityUpstream::isMagnet() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void    LCavityUpstream::setFrequency( double const& f)
{
  w_rf_ = 2*M_PI*f; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::setPhi( double const& radians)
{
   phi_s_ = radians;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//----------------------------------------------------------------------------------
//  DOWSTREAM LCAVITY SECTION
//----------------------------------------------------------------------------------


LCavityDnstream::LCavityDnstream( std::string const&  name, double const& length, double const& rfreq,         
                                                      double const& volts,  double const& phis)
  : bmlnElmnt(name,length,volts*1.0e-9), w_rf_(2.0*M_PI*rfreq), phi_s_(phis)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream::LCavityDnstream( LCavityDnstream const& o)
  : bmlnElmnt(o), w_rf_(o.w_rf_), phi_s_(o.phi_s_)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream* LCavityDnstream::Clone() const 
{ 
  return new LCavityDnstream( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream::~LCavityDnstream()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream& LCavityDnstream::operator=(  LCavityDnstream const& rhs)
{
  if (this == &rhs) return *this;

  bmlnElmnt::operator=( rhs);

  w_rf_   = rhs.w_rf_;
  phi_s_  = rhs.phi_s_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LCavityDnstream::accept( BmlVisitor&      v )
{
 v.visit( *this ); 
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::accept( ConstBmlVisitor& v ) const
{
 v.visit( *this ); 
} 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LCavityDnstream::getPhi()  const
{
  return phi_s_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LCavityDnstream::getRadialFrequency()  const
{
  return w_rf_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double LCavityDnstream::getDesignEnergyGain()   const
{
  return Strength()* cos(phi_s_); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* LCavityDnstream::Type() const
{
  return "LCavityDnstream"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    LCavityDnstream::isMagnet() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void    LCavityDnstream::setFrequency( double const& f)
{
  w_rf_ = 2*M_PI*f; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::setPhi( double const& radians)
{
   phi_s_ = radians;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

