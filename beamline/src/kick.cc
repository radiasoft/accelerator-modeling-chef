/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      kick.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <beamline/kick.h>

using namespace std;

// **************************************************
//   class vkick 
// **************************************************

vkick::vkick() : bmlnElmnt() {
}

vkick::vkick( double k ) : bmlnElmnt() {
                      // Unfortunately, this is a
 strength = k;        // special case. Normally a
                      // single "double" argument
}                     // indicates length.

vkick::vkick( const char* n ) : bmlnElmnt(n) {
}

vkick::vkick( const char* n, double k ) : bmlnElmnt(n) {
                      // Unfortunately, this is a
 strength = k;        // special case. Normally a
                      // single "double" argument
}                     // indicates length.

vkick::vkick( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

vkick::vkick( const vkick& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
}


vkick::vkick( const char* n, double l, double s ) 
: bmlnElmnt(n,l,s) 
{
}


vkick::~vkick() {
}


const char* vkick::Type() const 
{ 
  return "vkick"; 
}


// **************************************************
//   class hkick
// **************************************************

hkick::hkick() : bmlnElmnt() {
}

hkick::hkick( double k ) : bmlnElmnt() {
 strength = k;
}

hkick::hkick( const char* n ) : bmlnElmnt(n) {
}

hkick::hkick( const char* n, double k ) 
: bmlnElmnt( n ) 
{
 strength = k;      // Unfortunately, this is a
                    // special case. Normally a
}                   // single "double" argument
                    // indicates length.

hkick::hkick( const hkick& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}


hkick::hkick( bmlnElmntData& x ) : bmlnElmnt( x ) {
}


hkick::hkick( const char* n, double l, double s ) 
: bmlnElmnt(n,l,s) 
{
}


hkick::~hkick() {
}


const char* hkick::Type() const 
{ 
  return "hkick"; 
}


// ************************************************
//   class kick
// ************************************************

kick::kick() : bmlnElmnt() {
	horizontalKick = 0.0;
	verticalKick = 0.0;
}

kick::kick( const char* s ) : bmlnElmnt(s) {
}

kick::kick(double hStrength, double vStrength ) : bmlnElmnt() {
	horizontalKick = hStrength;
	verticalKick = vStrength;
}

kick::kick(const char* s, double hStrength, double vStrength) : bmlnElmnt(s) {
	horizontalKick = hStrength;
	verticalKick = vStrength;
}


// The following two constructors added 9/1/99.
// (L.M.)
kick::kick( double lng, double hStrength, double vStrength ) 
: bmlnElmnt( "NONAME", lng, hStrength + vStrength, 0 ),  // strength is arbitrary
  horizontalKick( hStrength ), 
  verticalKick  ( vStrength )
{
}

kick::kick( const char* s, double lng, double hStrength, double vStrength ) 
: bmlnElmnt( s, lng, hStrength + vStrength, 0 ),  // strength is arbitrary
  horizontalKick( hStrength ), 
  verticalKick  ( vStrength )
{
}



kick::kick(const kick& x) : bmlnElmnt( (bmlnElmnt&) x ) {
	horizontalKick = x.horizontalKick;
	verticalKick = x.verticalKick;
}

kick::~kick() {
}

ostream& kick::writeTo(ostream& os) {
	os << OSTREAM_DOUBLE_PREC << horizontalKick << "  " << verticalKick;
	os << "\n";
	return os;
}

istream& kick::readFrom(istream& is) {
	double a,b;
	is >> a >> b;
	horizontalStrength() = a;
	verticalStrength() = b;
	return is;
}


const char* kick::Type() const 
{ 
  return "kick"; 
}
