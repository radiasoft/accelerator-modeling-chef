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

#include <fstream>

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/bmlnElmnt.h>

class BoolNode;


class ModifierVisitor : public virtual BmlVisitor
{
 public:
  ModifierVisitor();
  ModifierVisitor( bmlnElmnt& );        // Modifies one element
  ModifierVisitor( bmlnElmnt* );
  ModifierVisitor( const BoolNode& );   // Modifies elements satisfying criterion
  ModifierVisitor( const BmlPtrList& ); // Modifies elements in a list
  // This list is assumed to be sorted.
  // Elements must appear
  //   in the same order in which
  //   they appear in the beamline
  //   to be visited.
  // They also must appear only once
  //   in the line.
  ~ModifierVisitor();

  const BmlPtrList& getDoneList() const;
  int getNumberModified() const;
  int getNumberRemaining() const;

 protected:
  BoolNode*   _queryPtr;     // owned
  BmlPtrList  _toDoList;     // list is owned, not the pointers
  BmlPtrList  _doneList;  
  bmlnElmnt*  _currentPtr;   // not owned
};


class AlignVisitor : public ModifierVisitor
{
 public: 
  AlignVisitor( const alignmentData&, bmlnElmnt* = 0 );
  AlignVisitor( const alignmentData&, const BoolNode& );
  AlignVisitor( const alignmentData&, const BmlPtrList& );
  // This list is assumed to be sorted.
  // Elements must appear
  // in the same order in which
  // they appear in the beamline
  // to be visited.
  ~AlignVisitor();

  void visitBmlnElmnt( bmlnElmnt* );

 private:
  alignmentData _myAlign;
  void _doAlign( bmlnElmnt* );
};

#endif // MODIFIERVISITOR_H
