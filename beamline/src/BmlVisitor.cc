/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BmlVisitor.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******  REVISION HISTORY:
******  
******  Jan-Mar 2007       ostiguy@fnal.gov
******
******  - converted to new-style STL compatible beamline iterators    
******  - take advantage of dynamic typing
******  - use reference-based argument rather than ptr 
******  - use safe casting to dispatch to visit( bmlnElmnt& )default behavior   
******                                                            
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


/*
**
** File: BmlVisitor.cc
** 
** This is pretty minimal, but not much is needed here.
**
** --- Leo Michelotti
** --- August 21, 1997
**
** Added ConstBmlVisitor class.
**
** --- Leo Michelotti
** --- August 21, 2001
*/

#include <beamline/beamline_elements.h>  
#include <beamline/beamline.h>  
#include <beamline/bmlnElmnt.h>  
#include <beamline/BmlVisitor.h>


using namespace std;


void BmlVisitor::visit( beamline& x )  // THIS IS BROKEN !
{

  for (beamline::iterator it = x.begin();   it != x.end(); ++it ) {
    (*it)->accept( *this );
  }

}


void BmlVisitor::visit( hkick& x )
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( octupole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinrfcavity& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( rfcavity& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( srot& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( vkick& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( monitor& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( marker& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( drift& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( rbend& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( sbend& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( sector& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( quadrupole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thin2pole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinQuad& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinSextupole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinOctupole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinDecapole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thin12pole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thin14pole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thin16pole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thin18pole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinMultipole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( sextupole& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( BBLens& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinSeptum& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( thinLamb& x ) 
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( combinedFunction& x )
               {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( Pinger& x )
		 {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( HPinger& x )
		 {visit( static_cast<bmlnElmnt&>(x) ); }

void BmlVisitor::visit( VPinger& x )
		 {visit( static_cast<bmlnElmnt&>(x) ); } 

void BmlVisitor::visit( kick& x)
		 {visit( static_cast<bmlnElmnt&>(x));  }

void BmlVisitor::visit( Slot& x)
		 {visit( static_cast<bmlnElmnt&>(x));  }

void BmlVisitor::visit( CF_rbend& x)
		 {visit( static_cast<bmlnElmnt&>(x));  }

void BmlVisitor::visit( CF_sbend& x)
		 {visit( static_cast<bmlnElmnt&>(x));  }


//---------------------------------------------------------------------------------

void ConstBmlVisitor::visit( beamline const& x )  // THIS IS BROKEN !
{

  for (beamline::const_iterator it = x.begin();  it != x.end(); ++it ) {
    (*it)->accept( *this );
  }

}

void ConstBmlVisitor::visit( hkick const& x )
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( octupole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinrfcavity const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( rfcavity const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( srot const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit(  vkick const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit(  monitor const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit(  marker const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( drift const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( rbend const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( sbend const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( sector const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( quadrupole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thin2pole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinQuad const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinSextupole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinOctupole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinDecapole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thin12pole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thin14pole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thin16pole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thin18pole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinMultipole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( sextupole const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( BBLens const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinSeptum const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( thinLamb const& x ) 
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( combinedFunction const& x )
               {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( Pinger const& x )
		 {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( HPinger const& x )
		 {visit( static_cast<bmlnElmnt const&>(x) ); }

void ConstBmlVisitor::visit( VPinger const& x )
		 {visit( static_cast<bmlnElmnt const&>(x) ); } 

void ConstBmlVisitor::visit( kick const& x)
		 {visit( static_cast<bmlnElmnt const&>(x));  }

void ConstBmlVisitor::visit( Slot const& x)
		 {visit( static_cast<bmlnElmnt const&>(x));  }

void ConstBmlVisitor::visit( CF_rbend const& x)
		 {visit( static_cast<bmlnElmnt const&>(x));  }

void ConstBmlVisitor::visit( CF_sbend const& x)
		 {visit( static_cast<bmlnElmnt const&>(x));  }

