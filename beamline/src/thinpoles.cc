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

#include <beamline/thinpoles.h>

using namespace std;

// **************************************************
//   class thin2pole 
// **************************************************

thin2pole::thin2pole()
: bmlnElmnt( 0.0, 0.0 )
{
}


thin2pole::thin2pole( double const& s /* strength = BL */ )
: bmlnElmnt( 0.0, s )
{
}


thin2pole::thin2pole( const char*  n /* name */, double const& s /* strength */ )
: bmlnElmnt( n, 0.0, s )
{
}



thin2pole::thin2pole( const thin2pole& x )
: bmlnElmnt( x ){}


thin2pole::~thin2pole()
{}


const char* thin2pole::Type() const
{
  return "thin2pole"; 
}


bool thin2pole::isMagnet() const
{
  return true;
}


// **************************************************
//   class thin12pole 
// **************************************************

thin12pole::thin12pole () : bmlnElmnt( 0.0, 0.0 ) {
}

thin12pole::thin12pole ( double const& s ) : bmlnElmnt( 0.0, s ) {
}

thin12pole::thin12pole ( const char* n, double const& s ) : bmlnElmnt( n, 0.0, s ) {
}


thin12pole::thin12pole( const thin12pole& x ) 
: bmlnElmnt( x ){}

thin12pole::~thin12pole() {
}


const char* thin12pole::Type() const 
{ 
  return "thin12pole"; 
}


bool thin12pole::isMagnet() const
{
  return true;
}

// **************************************************
//   class thin14pole 
// **************************************************

thin14pole::thin14pole () : bmlnElmnt( 0.0, 0.0 ) {
}

thin14pole::thin14pole ( double const& s ) : bmlnElmnt( 0.0, s ) {
}

thin14pole::thin14pole ( const char* n, double const& s ) : bmlnElmnt( n, 0.0, s ) {
}



thin14pole::thin14pole( const thin14pole& x ) 
: bmlnElmnt( x ){}

thin14pole::~thin14pole() {
}


const char* thin14pole::Type() const 
{ 
  return "thin14pole"; 
}


bool thin14pole::isMagnet() const
{
  return true;
}


// **************************************************
//   class thin16pole 
// **************************************************

thin16pole::thin16pole () : bmlnElmnt( 0.0, 0.0 ) {
}

thin16pole::thin16pole ( double const& s ) : bmlnElmnt( 0.0, s ) {
}

thin16pole::thin16pole ( const char* n, double const& s ) : bmlnElmnt( n, 0.0, s ) {
}


thin16pole::thin16pole( const thin16pole& x ) 
: bmlnElmnt( x ){}

thin16pole::~thin16pole() {
}

const char* thin16pole::Type() const 
{ 
  return "thin16pole"; 
}


bool thin16pole::isMagnet() const
{
  return true;
}


// **************************************************
//   class thin18pole 
// **************************************************

thin18pole::thin18pole () : bmlnElmnt( 0.0, 0.0 ) {
}

thin18pole::thin18pole ( double const& s ) : bmlnElmnt( 0.0, s ) {
}

thin18pole::thin18pole ( const char* n, double const& s ) : bmlnElmnt( n, 0.0, s ) {
}



thin18pole::thin18pole( const thin18pole& x ) 
: bmlnElmnt( x ){}

thin18pole::~thin18pole() {
}


const char* thin18pole::Type() const 
{ 
  return "thin18pole"; 
}


bool thin18pole::isMagnet() const
{
  return true;
}


// **************************************************
//   class thinMultipole 
// **************************************************

thinMultipole::thinMultipole () : bmlnElmnt( 0.0, 0.0 ) {
 static char firstCall = 1;

 if ( firstCall ) {
   printf( "\n*** SORRY:                                         " );
   printf( "\n*** SORRY: class thinMultipole is not implemented. " );
   printf( "\n*** SORRY: A marker will be substituted            " );
   printf( "\n*** SORRY: for each instance.                      " );
   printf( "\n*** SORRY:                                       \n" );
   firstCall = 0;
 }

}

thinMultipole::thinMultipole ( double const& s ) : bmlnElmnt( 0.0, s ) {
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

thinMultipole::thinMultipole ( const char* n, double const& s ) : bmlnElmnt( n, 0.0, s ) {
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



thinMultipole::thinMultipole( const thinMultipole& x ) 
: bmlnElmnt( x ){}

thinMultipole::~thinMultipole() {}



const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}


bool thinMultipole::isMagnet() const
{
  return true;
}


