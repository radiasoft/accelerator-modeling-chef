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
******  File:      circuit.cc
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


#include "circuit.h"
#include <string>

using namespace std;

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
 cerr << "*** ERROR ***                               \n"
         "*** ERROR *** Circuit copy construction     \n"
         "*** ERROR *** is not allowed!               \n"
         "*** ERROR ***                                 "
      << endl;
 exit(1);
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
   cerr << "*** ERROR *** circuit::switchOn(): This doesn't set anything!!" << endl;
}
void circuit::switchOff() {
   onOffSwitch = 0;
   cerr << "*** ERROR *** circuit::switchOff():  This doesn't set anything!!" << endl;
}

void circuit::set( void* ) {
  cerr << " This should never be called!! " << endl;
}

char* circuit::getName() {
 return strdup( ident );
}

void circuit::get(void* ) {
  cerr << " This should never be called!! " << endl;
}

