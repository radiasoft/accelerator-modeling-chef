#ifndef COMBINED_FUNCTION_H
#define COMBINED_FUNCTION_H

#include "mxyzptlk.h"
#include "beamline.inc"

enum WHICH_MULTIPOLE { DIPOLE_FIELD, QUADRUPOLE_FIELD, SEXTUPOLE_FIELD, OCTUPOLE_FIELD, DECAPOLE_FIELD, TWELVEPOLE_FIELD, FOURTEENPOLE_FIELD, SIXTEENPOLE_FIELD, EIGHTEENPOLE_FIELD };

class combinedFunction : public bmlnElmnt {
private:
  istream& readFrom(istream&);
  ostream& writeTo(ostream&);

 public:
  combinedFunction();
  combinedFunction(char * /* name */ );
  combinedFunction(char *, beamline&);
  combinedFunction(const combinedFunction&);
  virtual ~combinedFunction();
  void append(bmlnElmnt&);
  void eliminate() {delete this;}

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );
  void accept( BmlVisitor& v ) { v.visitCombinedFunction( this ); }
  
  void setField(WHICH_MULTIPOLE, double field);
  double Field(WHICH_MULTIPOLE);
  void setSkew(WHICH_MULTIPOLE, alignmentData&);
  alignmentData Skew(WHICH_MULTIPOLE);
  char* Type() { return "combinedFunction"; }
  bmlnElmnt* Clone() { return new combinedFunction( *this ); }
  void accept(BmlVisitor&);

};



#endif // COMBINED_FUNCTION_H
