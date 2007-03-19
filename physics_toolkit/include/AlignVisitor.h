/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******  Version:   1.0
******
******  File:      AlignVisitor.h
******
******  Copyright (c) 2003  Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
****** REVISION HISTORY:
******
******  Dec 2006 - Jan 2007  ostiguy@fnal.gov 
******
****** - header split from ModifierVisitor
****** - use STL rather than custom list container
****** - visit function(s) use element reference as parameter
******
******
**************************************************************************
*************************************************************************/

#ifndef ALIGNVISITOR_H
#define ALIGNVISITOR_H

#include <list>
#include <basic_toolkit/globaldefs.h>
#include <beamline/Alignment.h>
#include <beamline/BmlPtr.h>
#include <physics_toolkit/ModifierVisitor.h>


//----------------------------------------------------------------------------------
// Note: the list appearing in the constructor's argument is assumed to be sorted, that
// is, elements must appear in the **same order in which they appear in the beamline**
// to be visited.
//---------------------------------------------------------------------------------

class AlignVisitor : public ModifierVisitor

{
 public: 

  AlignVisitor( alignmentData const&, ElmPtr elm = ElmPtr() );
  AlignVisitor( alignmentData const&, BoolNode const& );
  AlignVisitor( alignmentData const&, std::list<ElmPtr> const& );
 ~AlignVisitor();

  void visit( bmlnElmnt& );

 private:

  void          doAlign( bmlnElmnt& );

  alignmentData myAlign_;

};

#endif // ALIGNVISITOR_H

