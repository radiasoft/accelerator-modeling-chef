#ifndef BIPOLARCIRCUIT_H
#define BIPOLARCIRCUIT_H

#ifndef CIRCUIT_H
#include "circuit.h"
#endif

struct PolarityBarn : public BarnacleData {
  double polarity;
  PolarityBarn(double x) : polarity(x) {}
  PolarityBarn(const PolarityBarn& x){polarity = x.polarity;}
  ~PolarityBarn() {}
};

class BipolarCircuit : public circuit {
protected:
  double field;
public:
  BipolarCircuit();
  ~BipolarCircuit();
  BipolarCircuit( const char* );
  BipolarCircuit( bmlnElmnt* );
  BipolarCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void append( bmlnElmnt* );

};

#endif // BIPOLARCIRCUIT_H
