#include "circuit.h"
#ifndef __VISUAL_CPP__
#include <string.h>
#endif

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

