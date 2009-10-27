/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
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
 * Concatenates Drifts and removes passive
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
#include <beamline/Drift.h>
#include <beamline/beamline.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DriftEliminator::DriftEliminator()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DriftEliminator::~DriftEliminator()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DriftEliminator::visit( beamline& x )
{

  BmlPtr bml( new beamline( x.Name()+ string("_Condensed") ));
  bml->setMomentum( x.Momentum() );
  bml->setLineMode( x.getLineMode() );
  
  double length = 0.0; 

  for ( beamline::const_deep_iterator it = x.deep_begin(); it != x.deep_end(); ++it ) {
     
    if ( !(*it)->isPassive() ) { 
      if (length > 0.0 ) { bml->append( Drift( "concat_drift", length ) ); length = 0.0; }
      bml->append( *it);
    }
    else {
      length += (*it)->Length();
    }
  }

  // Append final Drift, if it exists.

  if (length > 0.0 ) { bml->append( Drift( "concat_drift", length ) ); length = 0.0; }
  x = *bml;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


