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
******  File:      srot.cc
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


#include "srot.h"

using namespace std;

// **************************************************
//   class srot
// **************************************************

srot::srot() : bmlnElmnt() {
}

srot::srot( double s) : bmlnElmnt(0.0,s) {
}

srot::srot( char* n ) : bmlnElmnt(n) {
}

srot::srot( char* n, double s) : bmlnElmnt( n, 0.0,s) {
}

srot::srot( const srot& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
}

srot::~srot() {
}


const char* srot::Type() const 
{ 
  return "srot"; 
}


