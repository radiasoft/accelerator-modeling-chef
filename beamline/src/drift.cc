#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      drift.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "drift.h"

using namespace std;

// **************************************************
//   class drift 
// **************************************************

drift::drift() : bmlnElmnt() {
}

drift::drift( char* n ) : bmlnElmnt(n) {
}

drift::drift( double l ) : bmlnElmnt(l) {
}

drift::drift( char* n, double l ) : bmlnElmnt( n, l ) {
}

drift::drift( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

drift::drift( const drift& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
}

drift::~drift() {
}


const char* drift::Type() const 
{ 
  return "drift"; 
}
