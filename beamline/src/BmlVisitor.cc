/*
**
** File: BmlVisitor.cc
** 
** This is pretty minimal, but not much is needed here.
**
** --- Leo Michelotti
** --- August 21, 1997
**
*/

#include "bmlnElmnt.h"  // This include BmlVisitor.h internally.

void BmlVisitor::visitBeamline( beamline* x )
{
  dlist_iterator getNext ( *(dlist*) x );
  bmlnElmnt* p;
  while ( 0 != (  p = (bmlnElmnt*) getNext() ) ) {
    p->accept( *this );
  }
}


