#ifndef SECTOR_H
#define SECTOR_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif


class sector : public bmlnElmnt
{
private:
  char   mapType;        // ??? Unnecessary. Get rid of this!
  Mapping    myMap;
  double betaH     [2];  // 0 = entry;  1 = exit
  double alphaH    [2];
  double deltaPsiH;
  double betaV     [2];
  double alphaV    [2];
  double deltaPsiV;
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];
  double (*DeltaT) ( double );
  Jet    (*JetDeltaT) ( const Jet& );

  ostream& writeTo ( ostream& );
  istream& readFrom( istream& );

public:
  sector( const char* = 0, double = 0.0 );
  sector( double* betaH,  double* alphaH,  double* psiH,
          double* betaV,  double* alphaV,  double* psiV, double length );
  sector( char*, double* betaH,  double* alphaH,  double* psiH,
                 double* betaV,  double* alphaV,  double* psiV, double length );

  sector( Jet*, double /* length */ );
  sector( char*, Jet*, double );
  sector( const Mapping&,  double /* length */ );
  sector( char*, const Mapping&,  double );
  sector( const sector& );
  ~sector();

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSector( this ); }

  void setFrequency( double (*)( double ) );
  void setFrequency( Jet (*)( const Jet& ) );
  void setLength( double );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sector") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new sector( *this ); }
} ;


#endif // SECTOR_H
