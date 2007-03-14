/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******  Version:   1.0
******
******  File:      ModifierVisitor.h
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
******  REVISION HISTORY
******
****** Mar 2007     ostiguy@fnal.gov
******  - support for reference counted elements
******  - eliminated references to slist/dlist
******  
**************************************************************************
*************************************************************************/


/*
 * File: ModifierVisitor.h
 *
 * Header for class ModifierVisitor.
 * Base class for visitors that modify
 * attributes of beamline elements.
 *
 * Leo Michelotti
 * October 16, 2003
 *
 */

#ifndef MODIFIERVISITOR_H
#define MODIFIERVISITOR_H

#include <list>
#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/bmlnElmnt.h>

class BoolNode;

class ModifierVisitor : public virtual BmlVisitor
{
 public:

  ModifierVisitor();
  ModifierVisitor( ElmPtr const& );                 // Modifies one element
  ModifierVisitor( BoolNode const& );               // Modifies elements satisfying criterion
  ModifierVisitor( std::list<ElmPtr> const& );      // Modifies elements in a list

  // This list is assumed to be sorted.
  // Elements must appear
  //   in the same order in which
  //   they appear in the beamline
  //   to be visited.
  // They also must appear only once
  //   in the line.

  ~ModifierVisitor();

  std::list<ElmPtr>&           getDoneList();
  int                          getNumberRemaining() const;

 protected:

  BoolNode*               queryPtr_;     // owned
  std::list<ElmPtr>       toDoList_;     
  ElmPtr                 currentPtr_;   // not owned
};


#endif // MODIFIERVISITOR_H
