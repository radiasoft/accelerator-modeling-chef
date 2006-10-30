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
#include <beamline/bmlnElmnt.h>  
#include <beamline/BmlVisitor.h>
#include <beamline/BeamlineIterator.h>

using namespace std;


void BmlVisitor::visitBeamline( beamline* x )
{

  BeamlineIterator iter ( *x );
  bmlnElmnt* p;
  while ( (p=iter++) ) {
    p->accept( *this );
  }
}



void BmlVisitor::visitHkick( hkick* x )
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitOctupole( octupole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinrfcavity( thinrfcavity* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitRfcavity( rfcavity* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitSrot( srot* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitVkick( vkick* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitMonitor( monitor* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitMarker( marker* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitDrift( drift* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitRbend( rbend* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitSbend( sbend* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitSector( sector* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitQuadrupole( quadrupole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThin2pole( thin2pole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinQuad( thinQuad* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinSextupole( thinSextupole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinOctupole( thinOctupole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinDecapole( thinDecapole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThin12pole( thin12pole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThin14pole( thin14pole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThin16pole( thin16pole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThin18pole( thin18pole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinMultipole( thinMultipole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitSextupole( sextupole* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitBBLens( BBLens* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinSeptum( thinSeptum* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitThinLamb( thinLamb* x ) 
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitCombinedFunction( combinedFunction* x )
               { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitPinger( Pinger* x )
		 { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitHPinger( HPinger* x )
		 { visitBmlnElmnt( (bmlnElmnt*) x ); }
void BmlVisitor::visitVPinger( VPinger* x )
		 { visitBmlnElmnt( (bmlnElmnt*) x ); } 
void BmlVisitor::visitKick( kick* x)
		 { visitBmlnElmnt( (bmlnElmnt*) x);  }
void BmlVisitor::visitSlot( Slot* x)
		 { visitBmlnElmnt( (bmlnElmnt*) x);  }
void BmlVisitor::visitCF_rbend( CF_rbend* x)
		 { visitBmlnElmnt( (bmlnElmnt*) x);  }
void BmlVisitor::visitCF_sbend( CF_sbend* x)
		 { visitBmlnElmnt( (bmlnElmnt*) x);  }


//---------------------------------------------------------------------------------

void ConstBmlVisitor::visitBeamline( beamline const* x )  // THIS IS BROKEN !
{

  BeamlineIterator iter ( const_cast<beamline&>(*x) );
  bmlnElmnt* p;
  while ( (p=iter++) ) {
    p->accept( *this );
  }
}

void ConstBmlVisitor::visitHkick( const hkick* x )
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitOctupole( const octupole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinrfcavity( const thinrfcavity* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitRfcavity( const rfcavity* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitSrot( const srot* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitVkick( const vkick* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitMonitor( const monitor* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitMarker( const marker* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitDrift( const drift* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitRbend( const rbend* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitSbend( const sbend* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitSector( const sector* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitQuadrupole( const quadrupole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThin2pole( const thin2pole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinQuad( const thinQuad* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinSextupole( const thinSextupole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinOctupole( const thinOctupole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinDecapole( const thinDecapole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThin12pole( const thin12pole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThin14pole( const thin14pole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThin16pole( const thin16pole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThin18pole( const thin18pole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinMultipole( const thinMultipole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitSextupole( const sextupole* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitBBLens( const BBLens* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinSeptum( const thinSeptum* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitThinLamb( const thinLamb* x ) 
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitCombinedFunction( const combinedFunction* x )
               { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitPinger( const Pinger* x )
		 { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitHPinger( const HPinger* x )
		 { visitBmlnElmnt( (const bmlnElmnt*) x ); }

void ConstBmlVisitor::visitVPinger( const VPinger* x )
		 { visitBmlnElmnt( (const bmlnElmnt*) x ); } 

void ConstBmlVisitor::visitKick( const kick* x)
		 { visitBmlnElmnt( (const bmlnElmnt*) x);  }

void ConstBmlVisitor::visitSlot( const Slot* x)
		 { visitBmlnElmnt( (const bmlnElmnt*) x);  }

void ConstBmlVisitor::visitCF_rbend( const CF_rbend* x)
		 { visitBmlnElmnt( (const bmlnElmnt*) x);  }

void ConstBmlVisitor::visitCF_sbend( const CF_sbend* x)
		 { visitBmlnElmnt( (const bmlnElmnt*) x);  }

