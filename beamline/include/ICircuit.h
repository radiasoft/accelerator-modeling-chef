#ifndef ICIRCUIT_H
#define ICIRCUIT_H

#ifndef CIRCUIT_H
#include "circuit.h"
#endif

class ICircuit : public circuit {
protected:
  double current;
public:
  ICircuit();
  ~ICircuit();
  ICircuit( const char* );
  ICircuit( bmlnElmnt* );
  ICircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
};

#endif // ICIRCUIT_H
