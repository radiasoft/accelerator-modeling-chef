/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      JetthinSext.cc
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

#include "sextupole.h"

using namespace std;

// **************************************************
//   class JetthinSext
// **************************************************

JetthinSext::JetthinSext ( double s, int index ) 
: bmlnElmnt() {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
 K2L.setVariable( s, index );
 K2L += s;
 strength = s;
 strengthIndex = index;
}

JetthinSext::JetthinSext ( const char* n, 
			   double s,
			   int index) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
 K2L.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}


JetthinSext::JetthinSext( bmlnElmntData& x ) : bmlnElmnt( x ) {
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "JetthinSext::JetthinSext( bmlnElmntData& x ) : bmlnElmnt( x ) {", 
        "Sorry. You can't do this." ) );
}

JetthinSext::JetthinSext( const JetthinSext& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  K2L = x.K2L;
  strengthIndex = x.strengthIndex;
}

JetthinSext::~JetthinSext() {
}

void JetthinSext::setStrength( double s ) {
 K2L.setVariable( s, strengthIndex );
 strength = s;
}


void JetthinSext::setStrength( double s, int index ) {
 K2L.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

void JetthinSext::eliminate() {
 delete this;
}


const char* JetthinSext::Type() const 
{ 
  return "JetthinSext"; 
}


ostream& JetthinSext::writeTo(ostream& os)
{
  os << strengthIndex << "\n";
  return os;
}

istream& JetthinSext::readFrom(istream& is)
{
  is >> strengthIndex;
  return is;
}
