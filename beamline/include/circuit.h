#ifndef CIRCUIT_H
#define CIRCUIT_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class circuit : public dlist
{
protected:
  char onOffSwitch;     // switch to turn current on and off
  char* ident;          // name of circuit
  int numElm;           // number of elements in the circuit
public:
  circuit();
  circuit( const char* );
  circuit( bmlnElmnt* );
  circuit( const char*, bmlnElmnt* );
  circuit(const circuit&);
  virtual ~circuit();
  virtual void switchOn();
  virtual void switchOff();
  virtual void set( void* );
  virtual void get( void* );
  char* getName();

  void append( bmlnElmnt* q );
} ;

#endif // CIRCUIT_H
