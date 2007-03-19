/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
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
#include "bmlnElmnt.h"
#include "Particle.h"
#include <beamline/beamline.h>

// -------------------------------
// Implementation: class QtMonitor
// -------------------------------

QtMonitor::QtMonitor( const char* identifier )
: monitor( (char*) identifier ), azimuth_(0.0)
{
}

QtMonitor::QtMonitor( const QtMonitor& x )
: QObject(this), monitor( x ), azimuth_( x.azimuth_ )
{
}

QtMonitor::~QtMonitor()
{
}

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


void QtMonitor::localPropagate( Particle& p )
{
  this->monitor::localPropagate( p );
  emit ping( azimuth_, p.State() );
}
