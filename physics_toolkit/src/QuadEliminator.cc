/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
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
: quadPtr_(), bmlPtr_()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QuadEliminator::~QuadEliminator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( beamline const& x )
{

  // Create the new beamline

  string nuNam = x.Name() + string("_QuadMerged");

  bmlPtr_ = BmlPtr( new beamline( nuNam.c_str() ) );
  bmlPtr_->setEnergy( x.Energy() );


  // Process the argument
 for (beamline::const_deep_iterator it  = x.deep_begin();
                                    it != x.deep_end(); ++it )  {
                                 
    (*it)->accept( *this );
  }


  // Append final quad, if it exists.
  if( quadPtr_ ) {
    bmlPtr_->append( QuadrupolePtr( quadPtr_->Clone() ) );
    quadPtr_ = QuadrupolePtr();
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( bmlnElmnt const& x )
{
  if( bmlPtr_ )  {  // Not paranoia!

    if( quadPtr_ ) {

      bmlPtr_->append( QuadrupolePtr( quadPtr_->Clone() ) );
      quadPtr_ = QuadrupolePtr();
    }

    bmlPtr_->append( ElmPtr( x.Clone() ) ); 
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QuadEliminator::visit( quadrupole const& x )
{
  if( !quadPtr_ ) {
    quadPtr_ = QuadrupolePtr( x.Clone() );
  }

  else if( ( quadPtr_->Alignment() == x.Alignment() ) &&
           ( quadPtr_->Strength()  == x.Strength()  )    ) {

    string newName =  quadPtr_->Name() + string("+") +  x.Name();
    quadPtr_->rename( newName.c_str() );
    quadPtr_->setLength( x.Length() + quadPtr_->Length() );
  }

  else {
     bmlPtr_->append( QuadrupolePtr( quadPtr_->Clone() ) );
     quadPtr_ = QuadrupolePtr( x.Clone()  );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr QuadEliminator::beamlinePtr()
{
  return bmlPtr_;
  // Invoking routine is responsible for
  // using this beamline before the 
  // QuadEliminator goes out of scope.
}


