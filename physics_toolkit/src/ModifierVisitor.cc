/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      MofidiferVisitor.cc
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/

/*
 * File: ModifierVisitor.cc
 *
 * Source for class ModifierVisitor.
 * Base class for visitors that modify
 * attributes of beamline elements.
 *
 * Leo Michelotti
 * October 16, 2003
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <physics_toolkit/ModifierVisitor.h>
#include <beamline/BeamlineExpressionTree.h>
#include <beamline/bmlnElmnt.h>



ModifierVisitor::ModifierVisitor()
: _queryPtr(0), _toDoList(), _doneList(), _currentPtr(0)
{
}


ModifierVisitor::ModifierVisitor( const BoolNode& x )
: _queryPtr(0), _toDoList(), _doneList(), _currentPtr(0)
{
  _queryPtr = x.Clone();
}


ModifierVisitor::ModifierVisitor( const BmlPtrList& x )
: _queryPtr(0), _toDoList(x), _doneList(), _currentPtr(0)
{
  // This list is assumed to be sorted.
  // Elements must appear
  // in the same order in which
  // they appear in the beamline
  // to be visited.

  _currentPtr = _toDoList.get();  // Will be null if list is empty.
}


ModifierVisitor::ModifierVisitor( bmlnElmnt& x )
: _queryPtr(0), _toDoList(), _doneList(), _currentPtr(&x)
{
}


ModifierVisitor::ModifierVisitor( bmlnElmnt* x )
: _queryPtr(0), _toDoList(), _doneList(), _currentPtr(x)
{
}


ModifierVisitor::~ModifierVisitor()
{
  delete _queryPtr;
  _currentPtr = 0;
}


const BmlPtrList& ModifierVisitor::getDoneList() const
{
  return _doneList;
}


int ModifierVisitor::getNumberModified() const
{
  return _doneList.size();
}


int ModifierVisitor::getNumberRemaining() const
{
  return _toDoList.size();
}


// *********************************************
// ***** class AlignVisitor
// *********************************************

AlignVisitor::AlignVisitor( const alignmentData& a, bmlnElmnt* q )
: ModifierVisitor(q), _myAlign(a)
{
}


AlignVisitor::AlignVisitor( const alignmentData& a, const BoolNode& q )
: ModifierVisitor(q), _myAlign(a)
{
}


AlignVisitor::AlignVisitor( const alignmentData& a, const BmlPtrList& q )
: ModifierVisitor(q), _myAlign(a)
{
}


AlignVisitor::~AlignVisitor()
{
}


void AlignVisitor::visitBmlnElmnt( bmlnElmnt* q )
{
  if( 0 == q ) { return; }

  if( _queryPtr ) {
    if( _queryPtr->evaluate( q ) ) {
      _doAlign(q);
      _doneList.append(q);
    }
  }

  else if( _currentPtr ) {
    if( q == _currentPtr ) {
      _doAlign(q);
      _doneList.append(q);
      _currentPtr = _toDoList.get();
    }
  }
}


void AlignVisitor::_doAlign( bmlnElmnt* q )
{
  q->setAlignment(_myAlign);
}
