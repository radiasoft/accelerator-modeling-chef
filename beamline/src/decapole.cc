/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      decapole.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <decapole.h>

using namespace std;

// **************************************************
//   class thinDecapole 
// **************************************************

thinDecapole::thinDecapole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // (1/4!)*B''''l/Brho  in  meters^-2
}

thinDecapole::thinDecapole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/4!)*B''''l/Brho  in  meters^-2
}


thinDecapole::thinDecapole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinDecapole::thinDecapole( const thinDecapole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinDecapole::~thinDecapole() {
}



const char* thinDecapole::Type() const 
{ 
  return "thinDecapole"; 
}


bool thinDecapole::isMagnet() const
{
  return true;
}
