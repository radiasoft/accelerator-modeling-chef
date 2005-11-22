/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******  File:      srot.cc
******                                                                
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


#include <srot.h>

using namespace std;

// **************************************************
//   class srot
// **************************************************

srot::srot() : bmlnElmnt() {
}

srot::srot( double s) : bmlnElmnt(0.0,s) {
}

srot::srot( const char* n ) : bmlnElmnt(n) {
}

srot::srot( const char* n, double s) : bmlnElmnt( n, 0.0,s) {
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


