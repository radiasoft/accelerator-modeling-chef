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

  if( queryPtr_ ) {
    if( queryPtr_->evaluate( &q ) ) {
       doAlign(q);
    }
  }

  // NOTE: THIS FUNCTION IS BROKEN. IT WILL NOT WORK WITH SHARED ELEMENTS

  else if( currentPtr_ ) {
    if( &q == &(*currentPtr_) ) {
       doAlign(q);
       currentPtr_ = toDoList_.front(); 
       toDoList_.erase( toDoList_.begin() );
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AlignVisitor::doAlign( bmlnElmnt& q )
{
  q.setAlignment(myAlign_);
}
