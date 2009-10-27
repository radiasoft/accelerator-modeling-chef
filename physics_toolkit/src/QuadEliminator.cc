/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      QuadEliminator.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
****** REVISION HISTORY
****** 
****** Mar 2007       ostiguy@fnal.gov 
******
******  - reference counted elements/beamlines 
******  - eliminated references to slist/dlist
******  - use new-style STL compatible beamline iterators
******
****** Aug 2008           ostiguy@fnal.gov
****** - revised to conform to the conventional semantics 
******   of the Visitor pattern. QuadEliminator now modifies the 
******   beamline in-place rather than return a new one through
******   a private member accessor. 
****** - uncessary cloning eliminated: beamline::append( x ) always
******   _clones_ x if x is a reference or value type. Shared 
******   semantics is obtained when x is an ElmPtr. 
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * File: QuadEliminator.cc
 * 
 * Source for class QuadEliminator
 * 
 * Concatenates quads.
 * 
 * Leo Michelotti
 * March 11, 2002
 * 
 */

#include <iostream>
#include <string>

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/QuadEliminator.h>
#include <beamline/quadrupole.h>
#include <beamline/beamline.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QuadEliminator::QuadEliminator()
: bml_(), quadPtr_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QuadEliminator::~QuadEliminator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( beamline& x )
{

  bml_ = BmlPtr( new beamline( x.Name()+ string("_QuadMerged") ));
  bml_->setMomentum( x.Momentum() );
  bml_->setLineMode( x.getLineMode() );
  
  // Process the argument

  for (beamline::const_deep_iterator it  = x.deep_begin();
                                    it != x.deep_end(); ++it )  {
                                 
    (*it)->accept( *this );
  }

  // Append final quad, if it exists.

  if( quadPtr_ ) {
    bml_->append( quadPtr_ );
    quadPtr_ = QuadrupolePtr();
  }

  x = *bml_; 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( BmlnElmnt& x )
{
  if( !bml_ ) return;

  if( quadPtr_ ) {

      bml_->append( quadPtr_ );
      quadPtr_ = QuadrupolePtr();
   }

  bml_->append( x ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( quadrupole& x )
{
  if( !quadPtr_ ) {
    quadPtr_ = QuadrupolePtr( x.clone() );
  }

  else if( ( quadPtr_->alignment() == x.alignment() ) &&
           ( quadPtr_->Strength()  == x.Strength()  )    ) {

    string newName =  quadPtr_->Name() + string("+") +  x.Name();
    quadPtr_->rename( newName.c_str() );
    quadPtr_->setLength( x.Length() + quadPtr_->Length() );
  }

  else {
     bml_->append( quadPtr_  );
     quadPtr_ = QuadrupolePtr( x.clone() );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


