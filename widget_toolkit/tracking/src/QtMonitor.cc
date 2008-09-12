/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      QtMonitor.cc
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
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
****** REVISION HISTORY
****** Mar 2007 ostiguy@fnal.gov
****** - eliminated references to slist/dlist
****** - use new style STL-compatible beamline iterators 
****** Sep 2008 ostiguy@fnal.gov
****** - modified to work without virtual localPropagate method. 
******                                                               
**************************************************************************
*************************************************************************/


// ************************************
// 
// File: QtMonitor.cc
// 
// Author: Leo Michelotti
// Date: September 17, 2001
// 
// ************************************

#include <typeinfo>

#include "QtMonitor.h"
#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/BasePropagator.h>

class QtMonitor::Propagator: public BasePropagator {

 public:
 
  Propagator* Clone() const;

  void  setup( bmlnElmnt& );
  void  operator()( bmlnElmnt const& elm,    Particle& p);
  void  operator()( bmlnElmnt const& elm, JetParticle& p);
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QtMonitor::Propagator* QtMonitor::Propagator::Clone() const 
{  
   return new Propagator(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QtMonitor::Propagator::setup(  bmlnElmnt& arg ) 
{  
  ElmPtr& elm = bmlnElmnt::core_access::get_ElmPtr(arg);
  
  elm = ElmPtr( new monitor( arg.Name(), arg.Length() ));

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QtMonitor::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
   ConstElmPtr mon = bmlnElmnt::core_access::get_ElmPtr(elm);
   mon->propagate(p);

   QtMonitor& qtmon = const_cast<QtMonitor&>( static_cast< QtMonitor const&>(elm));
   qtmon.emit ping( qtmon.getAzimuth(), p.State() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QtMonitor::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
   ConstElmPtr mon = bmlnElmnt::core_access::get_ElmPtr(elm);
   mon->propagate(p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// -------------------------------
// Implementation: class QtMonitor
// -------------------------------

QtMonitor::QtMonitor( char const * identifier )
: monitor( identifier ), azimuth_(0.0)
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QtMonitor::QtMonitor( QtMonitor const& x )
: QObject(this), monitor( x ), azimuth_( x.azimuth_ )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QtMonitor::~QtMonitor()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* QtMonitor::Type() const
{
  return "QtMonitor";
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int QtMonitor::setAzimuth( beamline  const& x )
{
 
 int n    = 0;
 double s = 0.0;

 for (beamline::const_deep_iterator it  = x.deep_begin();
	                            it != x.deep_end();   ++it) {
       
    s += (*it)->Length();
    if(typeid(**it) == typeid(QtMonitor)) {
      ++n;
      boost::static_pointer_cast<QtMonitor>(*it)->azimuth_ = s;
    }
  }

  return n;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double const&  QtMonitor::getAzimuth() const
{
  return azimuth_;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QtMonitor* QtMonitor::Clone() const 
{
 return new QtMonitor( *this ); 
}
