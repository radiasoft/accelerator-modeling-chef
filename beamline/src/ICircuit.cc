#include "ICircuit.h"

ICircuit::ICircuit() : circuit () {
  current = 0.0;
}

ICircuit::ICircuit(const char* n) : circuit (n) {
  current = 0.0;
}

ICircuit::ICircuit(bmlnElmnt* q) : circuit (q) {
  current = q->Current();
}

ICircuit::ICircuit(const char* n, bmlnElmnt* q) : circuit (n,q) {
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

