#ifndef COMBINED_FUNCTION_H
#define COMBINED_FUNCTION_H

#include "mxyzptlk.h"
#include "beamline.inc"

enum WHICH_MULTIPOLE { DIPOLE_FIELD, QUADRUPOLE_FIELD, SEXTUPOLE_FIELD, OCTUPOLE_FIELD, DECAPOLE_FIELD, TWELVEPOLE_FIELD, FOURTEENPOLE_FIELD, SIXTEENPOLE_FIELD, EIGHTEENPOLE_FIELD };

class combinedFunction : public bmlnElmnt 
{
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

  virtual void enterLocalFrame( Particle&    p ) const
    { bmlnElmnt::enterLocalFrame( p ); }
  virtual void enterLocalFrame( JetParticle& p ) const
    { bmlnElmnt::enterLocalFrame( p ); }
  virtual void leaveLocalFrame( Particle&    p ) const
    { bmlnElmnt::leaveLocalFrame( p ); }
  virtual void leaveLocalFrame( JetParticle& p ) const
    { bmlnElmnt::leaveLocalFrame( p ); }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitCombinedFunction( this ); }
  
  void setField(WHICH_MULTIPOLE, double field);
  double Field(WHICH_MULTIPOLE);
  void setSkew(WHICH_MULTIPOLE, alignmentData&);
  void setLength( double x )
    { length = x; }
  alignmentData Skew(WHICH_MULTIPOLE);
  char* Type() const;
  bmlnElmnt* Clone() const { return new combinedFunction( *this ); }

  double AdjustPosition( const Proton& );
  double AdjustPosition( const JetProton& );
};

#endif // COMBINED_FUNCTION_H
