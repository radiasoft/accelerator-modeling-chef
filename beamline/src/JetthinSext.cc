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

JetthinSext::JetthinSext ( char* n, 
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
