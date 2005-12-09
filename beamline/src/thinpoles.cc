/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinpoles.cc
******  Version:   2.1
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

#include <thinpoles.h>

using namespace std;

// **************************************************
//   class thin2pole 
// **************************************************

thin2pole::thin2pole( double s /* strength = BL */ )
: bmlnElmnt( 0.0, s )
{
}


thin2pole::thin2pole( const char*  n /* name */, double s /* strength */ )
: bmlnElmnt( n, 0.0, s )
{
}


thin2pole::thin2pole( bmlnElmntData& x )
: bmlnElmnt( x )
{
}


thin2pole::thin2pole( const thin2pole& x )
: bmlnElmnt( (const bmlnElmnt&) x )
{
}


thin2pole::~thin2pole()
{
}


const char* thin2pole::Type() const
{
  return "thin2pole"; 
}


// **************************************************
//   class thin12pole 
// **************************************************

thin12pole::thin12pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin12pole::thin12pole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin12pole::thin12pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin12pole::thin12pole( const thin12pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin12pole::~thin12pole() {
}


const char* thin12pole::Type() const 
{ 
  return "thin12pole"; 
}


// **************************************************
//   class thin14pole 
// **************************************************

thin14pole::thin14pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin14pole::thin14pole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin14pole::thin14pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin14pole::thin14pole( const thin14pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin14pole::~thin14pole() {
}


const char* thin14pole::Type() const 
{ 
  return "thin14pole"; 
}


// **************************************************
//   class thin16pole 
// **************************************************

thin16pole::thin16pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin16pole::thin16pole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin16pole::thin16pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin16pole::thin16pole( const thin16pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin16pole::~thin16pole() {
}

const char* thin16pole::Type() const 
{ 
  return "thin16pole"; 
}


// **************************************************
//   class thin18pole 
// **************************************************

thin18pole::thin18pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin18pole::thin18pole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin18pole::thin18pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin18pole::thin18pole( const thin18pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin18pole::~thin18pole() {
}


const char* thin18pole::Type() const 
{ 
  return "thin18pole"; 
}


// **************************************************
//   class thinMultipole 
// **************************************************

thinMultipole::thinMultipole ( double s ) : bmlnElmnt( 0.0, s ) {
 static char firstCall = 1;

 if ( firstCall ) {
   printf( "\n*** SORRY:                                         " );
   printf( "\n*** SORRY: class thinMultipole is not implemented. " );
   printf( "\n*** SORRY: A marker will be substituted            " );
   printf( "\n*** SORRY: for each instance.   Length and tilt    " );
   printf( "\n*** SORRY: information are being ignored.          " );
   printf( "\n*** SORRY:                                       \n" );
   firstCall = 0;
 }

}

thinMultipole::thinMultipole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 static char firstCall = 1;

 if ( firstCall ) {
   printf( "\n*** SORRY:                                         " );
   printf( "\n*** SORRY: class thinMultipole is not implemented. " );
   printf( "\n*** SORRY: A marker will be substituted            " );
   printf( "\n*** SORRY: for each instance.   Length and tilt    " );
   printf( "\n*** SORRY: information are being ignored.          " );
   printf( "\n*** SORRY:                                       \n" );
   firstCall = 0;
 }

}


thinMultipole::thinMultipole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinMultipole::thinMultipole( const thinMultipole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinMultipole::~thinMultipole() {
}



const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}


