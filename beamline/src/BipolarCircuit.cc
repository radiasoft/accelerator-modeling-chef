#include "BipolarCircuit.h"

BipolarCircuit::BipolarCircuit() : circuit () {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(const char* n) : circuit (n) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(bmlnElmnt* q) : circuit (q) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(const char* n, bmlnElmnt* q) : circuit (n,q) {
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
