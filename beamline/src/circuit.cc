/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      circuit.cc
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

#include <string>

#include <basic_toolkit/iosetup.h>
#include <beamline/circuit.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


// **************************************************
//   class circuit
//   Original Author: James Holt
//   Aug 27, 1992
// 
//   Subsequent authors: Elliott McCrory
//                       Leo Michelotti
// **************************************************

//Constructors

circuit::circuit() {
   onOffSwitch  = 1;
   ident = strdup( "NONAME" );
   numElm = 0;
}

circuit::circuit( const char* n ) {
   onOffSwitch  = 1;
   numElm = 0;
   ident  = strdup( n );
}

circuit::circuit( bmlnElmnt* q ) {
   onOffSwitch  = 1;
   dlist::append(q);
   ident = strdup( q->Name() );
   numElm++;
}

circuit::circuit( const char* n, bmlnElmnt* q ) {
   onOffSwitch  = 1;
   dlist::append(q);
   numElm++;
   ident  = strdup( n );
}

circuit::circuit( const circuit& ) 
{
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "circuit::circuit( const circuit& ) ", 
        "Cannot copy a circuit." ) );
}

// Destructor

circuit::~circuit() {
  clear();
  delete [] ident;
}

// Methods

void circuit::append( bmlnElmnt* q ) {
   dlist::append( q );
   numElm++;
}

void circuit::switchOn() {
   onOffSwitch = 1;
   (*pcerr) << "*** ERROR *** circuit::switchOn(): This doesn't set anything!!" << endl;
}
void circuit::switchOff() {
   onOffSwitch = 0;
   (*pcerr) << "*** ERROR *** circuit::switchOff():  This doesn't set anything!!" << endl;
}

void circuit::set( void* ) {
  (*pcerr) << " This should never be called!! " << endl;
}

char* circuit::getName() {
 return strdup( ident );
}

void circuit::get(void* ) {
  (*pcerr) << " This should never be called!! " << endl;
}

