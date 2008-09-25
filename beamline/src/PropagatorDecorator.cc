/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PropagatorDecorator.h
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
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/PropagatorDecorator.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PropagatorDecorator::PropagatorDecorator( PropagatorPtr propagator)
  : propagator_(propagator)
{}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PropagatorDecorator::PropagatorDecorator( PropagatorDecorator const& o)
  : propagator_(o.propagator_)
{
    propagator_ = PropagatorPtr( propagator_->Clone() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PropagatorDecorator* PropagatorDecorator::Clone() const
{ 
  return new PropagatorDecorator(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PropagatorDecorator::~PropagatorDecorator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::setup( bmlnElmnt& elm)
{
  return propagator_->setup(elm);
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
void PropagatorDecorator::setAttribute( bmlnElmnt& elm, std::string const& name, 
                                                         boost::any const& value )
{
  return propagator_->setAttribute( elm, name, value );
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::operator()(  bmlnElmnt const& elm,  Particle& p)
{
  (*propagator_)(elm,p);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::operator()(  bmlnElmnt const& elm,  JetParticle& p) 
{
  (*propagator_)(elm,p);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::operator()(  bmlnElmnt const& elm,  ParticleBunch& b)  
{
  (*propagator_)(elm,b);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::operator()(  bmlnElmnt const& elm, JetParticleBunch& b)  
{
  (*propagator_)(elm,b);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool PropagatorDecorator::hasAlignment() const 
{
  return propagator_->hasAlignment();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool PropagatorDecorator::hasAperture()  const
{
  return propagator_->hasAperture();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  PropagatorDecorator::setAlignment( Vector const& translation, Vector const& rotation)
{
  propagator_->setAlignment( translation, rotation);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PropagatorDecorator::setAperture( bmlnElmnt::aperture_t type, double const& hor, double const& ver )
{
  propagator_->setAperture( type, hor, ver );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<bmlnElmnt::aperture_t, double, double>   PropagatorDecorator::aperture()   const
{
  return propagator_->aperture();
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<Vector,Vector>  PropagatorDecorator::alignment()   const
{
  return propagator_->alignment();
} 
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
