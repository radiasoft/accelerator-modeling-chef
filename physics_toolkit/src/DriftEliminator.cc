/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      DriftEliminator.cc
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
****** Mar 2006   ostiguy@fnal.gov
******
******  - reference counted elements/beamlines 
******  - eliminated references to slist/dlist
******  - use new-style STL compatible beamline iterators
******
**************************************************************************
*************************************************************************/


/*
 * File: DriftEliminator.cc
 * 
 * Source for class DriftEliminator
 * 
 * Concatenates drifts and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * May 17, 2001
 * 
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

#include <physics_toolkit/DriftEliminator.h>

#include <basic_toolkit/GenericException.h>
#include <beamline/drift.h>
#include <beamline/marker.h>
#include <beamline/monitor.h>
#include <beamline/Slot.h>
#include <beamline/beamline.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DriftEliminator::DriftEliminator()
  : bmlPtr_(), driftPtr_()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DriftEliminator::~DriftEliminator()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DriftEliminator::visit( beamline const& x )
{
  string nuName =  x.Name()+ string("_Condensed");

  bmlPtr_ = BmlPtr( new beamline( nuName.c_str()  ) );
  bmlPtr_->setEnergy( x.Energy() );


  // Process the argument


  for ( beamline::const_deep_iterator it= x.deep_begin(); it != x.deep_end(); ++it ) {
    (*it)->accept( *this );
  }


  // Append final drift, if it exists.
  if(  driftPtr_ ) {
    if( driftPtr_->Length() >0.0 ) {
      bmlPtr_->append( DriftPtr( driftPtr_->Clone() ) );
    }
    driftPtr_ = DriftPtr();
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void DriftEliminator::visit( bmlnElmnt const& x )
{

  if( !bmlPtr_ ) {  // Not paranoia!
       handlePassiveElement( x );
       return;
  }

  if( x.Strength() != 0.0  ) {
      if( driftPtr_ ) {
        if( driftPtr_->Length() > 0.0 ) {
          bmlPtr_->append( DriftPtr( driftPtr_->Clone() ) );
	}
        driftPtr_ = DriftPtr();
      }
      bmlPtr_->append( ElmPtr( x.Clone() ) ); 
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void DriftEliminator::visit( Slot const& x )
{
  if(  !bmlPtr_ ) {  // Not paranoia!
    return;
  }
 
  if( driftPtr_ ) {
      if( driftPtr_->Length() > 0.0 ) {
        bmlPtr_->append( DriftPtr( driftPtr_->Clone() ) );
      }
      driftPtr_ = DriftPtr();
  }
  bmlPtr_->append( ElmPtr( x.Clone() ) ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void DriftEliminator::handlePassiveElement( bmlnElmnt const& x )
{
  if( !driftPtr_ ) {
    driftPtr_ = DriftPtr( new drift( (char*) "concat_drift", x.Length() ) );
  }
  else {
    driftPtr_->setLength( x.Length() + driftPtr_->Length()  );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void DriftEliminator::visit( monitor const& x )
{
  if( 0.0 == x.Strength() ) {
    handlePassiveElement(x);
  } 
  else {
    visit( static_cast<bmlnElmnt const&>(x) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void DriftEliminator::visit( marker const& x )
{
  // Do nothing
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DriftEliminator::visit( drift const& x )
{
   handlePassiveElement(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr DriftEliminator::beamlinePtr()
{
  return bmlPtr_;

}




