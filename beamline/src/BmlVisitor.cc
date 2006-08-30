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

#include <beamline/bmlnElmnt.h>  // This include BmlVisitor.h internally.

using namespace std;


void BmlVisitor::visitBeamline( beamline* x )
{
  dlist_iterator getNext ( *(dlist*) x );
  bmlnElmnt* p;
  while ( 0 != (  p = (bmlnElmnt*) getNext() ) ) {
    p->accept( *this );
  }
}


void ConstBmlVisitor::visitBeamline( const beamline* x )
{
  dlist_iterator getNext ( *(dlist*) x );
  bmlnElmnt* p;
  while ( 0 != (  p = (bmlnElmnt*) getNext() ) ) {
    p->accept( *this );
  }
}


