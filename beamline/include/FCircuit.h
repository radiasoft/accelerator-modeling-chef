#ifndef FCIRCUIT_H
#define FCIRCUIT_H

#ifndef CIRCUIT_H
#include "circuit.h"
#endif

class FCircuit : public circuit {
protected:
  double field;
public:
  FCircuit();
  ~FCircuit();
  FCircuit( const char* );
  FCircuit( bmlnElmnt* );
  FCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void setCurrent( void* );
  void getCurrent( void* );
  void append( bmlnElmnt* );
};

#endif // FCIRCUIT_H
