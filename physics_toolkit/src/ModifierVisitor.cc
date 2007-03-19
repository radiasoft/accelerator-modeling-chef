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
******* REVISION HISTORY
****** 
******  Mar 2007     ostiguy@fnal.gov
******
****** - support for reference counted elements/beamlines 
****** - visitor interface takes advantage of (reference) dynamic type
****** - initialization optimizations
*****                                                                
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
#include <beamline/beamline_elements.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/Alignment.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ModifierVisitor::ModifierVisitor()
: queryPtr_(0),  toDoList_(), currentPtr_()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ModifierVisitor::ModifierVisitor( BoolNode const& x )
: queryPtr_(0), toDoList_(), currentPtr_()
{
  queryPtr_ = x.Clone();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ModifierVisitor::ModifierVisitor( std::list<ElmPtr> const& x )
: queryPtr_(0), toDoList_(x), currentPtr_()
{
  // This list is assumed to be sorted.
  // Elements must appear
  // in the same order in which
  // they appear in the beamline
  // to be visited.

  if ( toDoList_.empty() ) return;

  currentPtr_ = toDoList_.front();  
  toDoList_.pop_front();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ModifierVisitor::ModifierVisitor( ElmPtr const& x )
: queryPtr_(), toDoList_(), currentPtr_(x)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ModifierVisitor::~ModifierVisitor()
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int ModifierVisitor::getNumberRemaining() const
{
  return toDoList_.size();
}


