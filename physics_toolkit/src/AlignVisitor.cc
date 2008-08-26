/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      AlignVisitor.cc
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
****** REVISION HISTORY:
******
******  Dec 2006 - Jan 2007  ostiguy@fnal.gov 
******
****** - implementation of AlignVisitor moved from ModifierVisitor.cc to this file.
****** - use STL rather than custom list container
****** - visit function(s) use element reference as parameter
******
******  
**************************************************************************
*************************************************************************/

#include <physics_toolkit/AlignVisitor.h>
#include <beamline/BeamlineExpressionTree.h>

AlignVisitor::AlignVisitor( alignmentData const& a, ElmPtr q)
: ModifierVisitor(q), myAlign_(a)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignVisitor::AlignVisitor( alignmentData const& a, BoolNode const& q )
: ModifierVisitor(q), myAlign_(a)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignVisitor::AlignVisitor( alignmentData const& a, std::list<ElmPtr> const& q )
: ModifierVisitor(q), myAlign_(a)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignVisitor::~AlignVisitor()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AlignVisitor::visit( bmlnElmnt& q )
{
   
  // NOTE: The code below will not work when elements are 
  //       "shared" that is, two or more shared_ptrs are
  //       correspond to the same raw pointer. 

  if( queryPtr_ ) {
    if( queryPtr_->evaluate( q ) ) {
       doAlign(q);
    }
  }

  if (toDoList_.empty() ) return; 

  if( &q == &(*toDoList_.front()) ) {
       doAlign(q);
       toDoList_.pop_front();
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AlignVisitor::doAlign( bmlnElmnt& q )
{
  q.setAlignment(myAlign_);
}
