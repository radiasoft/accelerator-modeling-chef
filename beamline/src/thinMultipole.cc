/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinMultipole.cc
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
**************************************************************************
*************************************************************************/

#include <beamline/thinMultipole.h>
#include <beamline/BmlVisitor.h>

using namespace std;



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



thinMultipole::thinMultipole(  thinMultipole const& x ) 
: bmlnElmnt( x ){}

thinMultipole::~thinMultipole() {}



const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}


bool thinMultipole::isMagnet() const
{
  return false;
}


void thinMultipole::accept( BmlVisitor& v ) 
{
  v.visit(*this);
}

void thinMultipole::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}
