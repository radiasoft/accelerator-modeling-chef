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
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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

#include "bmlnElmnt.h"  // This include BmlVisitor.h internally.

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


