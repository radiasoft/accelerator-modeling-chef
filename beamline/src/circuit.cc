#include "beamline.inc"
#ifndef __VISUAL_CPP__
#include <string.h>
#endif

// **************************************************
//   class circuit
//   Author: James Holt
//   Aug 27, 1992
// **************************************************

//Constructors

circuit::circuit() {
   onOffSwitch  = 1;
   ident = strdup( "NONAME" );
   numElm = 0;
}

circuit::circuit( char* n ) {
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

circuit::circuit( char* n, bmlnElmnt* q ) {
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

ICircuit::ICircuit() : circuit () {
  current = 0.0;
}

ICircuit::ICircuit(char* n) : circuit (n) {
  current = 0.0;
}

ICircuit::ICircuit(bmlnElmnt* q) : circuit (q) {
  current = q->Current();
}

ICircuit::ICircuit(char* n, bmlnElmnt* q) : circuit (n,q) {
  current = q->Current();
}
ICircuit::~ICircuit() {
}
void ICircuit::switchOn() {
  onOffSwitch = 1;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setCurrent( current );
  }
}

void ICircuit::switchOff() {
  onOffSwitch = 0;
  double dummy = 0.0;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setStrength( dummy );
  }
}

void ICircuit::set(void* x) {
  double* curr = (double*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  current = *curr;
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setCurrent( *curr );
  }
}

void ICircuit::get(void* x) {
  double* curr = (double*)x;
  *curr = current;
}

FCircuit::FCircuit() : circuit () {
  field = 0.0;
}

FCircuit::FCircuit(char* n) : circuit (n) {
  field = 0.0;
}

FCircuit::FCircuit(bmlnElmnt* q) : circuit (q) {
  field = q->Strength();
}

FCircuit::FCircuit(char* n, bmlnElmnt* q) : circuit (n,q) {
  field = q->Strength();
}
FCircuit::~FCircuit() {
}
void FCircuit::switchOn() {
  onOffSwitch = 1;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setStrength( field );
  }

}

void FCircuit::switchOff() {
  onOffSwitch = 0;
  double dummy = 0.0;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setStrength( dummy );
  }
}

void FCircuit::set(void* x) {
  double* curr = (double*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  field = *curr;
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setStrength( *curr );
  }
}

void FCircuit::get(void* x) {
  double* curr = (double*)x;
  *curr = field;
}

void FCircuit::getCurrent(void* x) {
  double* curr = (double*)x;
  double  ItoFieldvalue;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  // Take as a representative element, the first one and use it's ItoField.
  p = (bmlnElmnt*) getNext();
  ItoFieldvalue = p->IToField();

  if(ItoFieldvalue != 0.0) {
    *curr = field/ItoFieldvalue;
    return;
  } else {
    cerr << "FCircuit::getCurrent(): ItoField conversion is zero for " 
         << getName() << endl;
    *curr = 0.0;
    return;
  }
}

void FCircuit::setCurrent(void* x) {
  double* current = (double *)x;
  double  ItoFieldvalue;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  // Take as a representative element, the first one and use it's ItoField.
  p = (bmlnElmnt*) getNext();
  ItoFieldvalue = p->IToField();

  if(ItoFieldvalue != 0.0) {
    field = *current * ItoFieldvalue;
  } else {
    cerr << "FCircuit::setCurrent(): ItoField conversion is zero for " 
         << getName() << endl;
    *current = 0.0;
    return;
  }
  // Now set all of the elements in the circuit to the same current.
  getNext.Reset();
  while(p = (bmlnElmnt*) getNext()) {
    p->setCurrent(*current);
    cout << p->Name() << " Strength= " << p->Strength() << " Current= "
         << *current  << " I2F= " << p->IToField() << endl;
  };
}

void FCircuit::append( bmlnElmnt* q ) {
   dlist::append( q );
   field = q->Strength();
   numElm++;
}

mover::mover() : circuit (), align() {
}

mover::mover(char* n) : circuit (n), align() {
}

mover::mover(bmlnElmnt* q) : circuit (q), align(q->Alignment()) {
}

mover::mover(char* n, bmlnElmnt* q) : circuit (n,q), align(q->Alignment()) {
}

mover::~mover() {}

void mover::append( bmlnElmnt* q ) {
   dlist::append( q );
   if ( align.xOffset == 0 && align.yOffset == 0 && align.tilt == 0 ) {
     align = q->Alignment();
   }
   numElm++;
}

void mover::switchOn() {
  onOffSwitch = 1;
  set(&align);
}

void mover::switchOff() {
  onOffSwitch = 0;
  alignmentData dummy;
  set(&dummy);
}

void mover::set(void* x) {
  alignmentData* curr = (alignmentData*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  align = *curr;
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setAlignment( *curr );
  }
}

void mover::get(void* x) {
   alignmentData* curr = (alignmentData*)x;
  *curr = align;
}

BipolarCircuit::BipolarCircuit() : circuit () {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(char* n) : circuit (n) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(bmlnElmnt* q) : circuit (q) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(char* n, bmlnElmnt* q) : circuit (n,q) {
  field = 0.0;
}
BipolarCircuit::~BipolarCircuit() {
}
void BipolarCircuit::switchOn() {
  onOffSwitch = 1;
  set(&field);
}

void BipolarCircuit::switchOff() {
  onOffSwitch = 0;
  double dummy = 0.0;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;

  while ( (p = (bmlnElmnt*) getNext()) ) {
    p -> setStrength( dummy );
  }
}

void BipolarCircuit::set(void* x) {
  double* curr = (double*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  PolarityBarn* pol;
  
  field = *curr;
  while ( (p = (bmlnElmnt*) getNext()) ) {
    pol = (PolarityBarn*)p->dataHook.find("Polarity");
    if(pol == 0) {
      cerr << "BipolarCircuit: no polarity information for " <<
	p->Name() << " Assuming +1 " << endl;
      p -> setStrength( *curr );

    } else 
      p -> setStrength( (*curr)* pol->polarity );
  }
}

void BipolarCircuit::get(void* x) {
  double* curr = (double*)x;
  *curr = field;
}

void BipolarCircuit::append( bmlnElmnt* q ) {
   dlist::append( q );
   field = q->Strength();
   numElm++;
}
