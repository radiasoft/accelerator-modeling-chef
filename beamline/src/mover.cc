#include "mover.h"

mover::mover() : circuit (), align() {
}

mover::mover(const char* n) : circuit (n), align() {
}

mover::mover(bmlnElmnt* q) : circuit (q), align(q->Alignment()) {
}

mover::mover(const char* n, bmlnElmnt* q) : circuit (n,q), align(q->Alignment()) {
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

