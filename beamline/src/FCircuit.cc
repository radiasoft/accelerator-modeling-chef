#include "FCircuit.h"

FCircuit::FCircuit() : circuit () {
  field = 0.0;
}

FCircuit::FCircuit(const char* n) : circuit (n) {
  field = 0.0;
}

FCircuit::FCircuit(bmlnElmnt* q) : circuit (q) {
  field = q->Strength();
}

FCircuit::FCircuit(const char* n, bmlnElmnt* q) : circuit (n,q) {
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
  double ItoFieldvalue;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while(p = (bmlnElmnt*) getNext()) {
    double current = *((double *)x);
    ItoFieldvalue = p->IToField();
    if(ItoFieldvalue != 0.0) {
      field = current * ItoFieldvalue;
    } else {
      cerr << "FCircuit::setCurrent(): ItoField conversion is zero for " 
	   << p->Name() << endl;
      current = field;
      return;
    }
    p->setCurrent(current);
    cout << p->Name() << " Strength= " << p->Strength() << " Current= "
         << current  << " I2F= " << p->IToField() << endl;
  };
}

void FCircuit::append( bmlnElmnt* q ) {
   dlist::append( q );
   field = q->Strength();
   numElm++;
}

