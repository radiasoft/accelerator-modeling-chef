#ifndef BBLENS_H
#define BBLENS_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class BBLens : public bmlnElmnt
{
private:
  double emittance[3];   // One sigma (noninvariant) emittance / pi
  double gamma;          // Relativistic gamma
  double beta;           // Relativistic beta
  double num;            // Number of proton charges; if the bunch
                         // is negatively charged, then this number
                         // is negative.
  double sigma[3];       // This will depend on position in the 
                         // lattice.  The third component is zero
                         // for now.
public:
  BBLens( const char*   = 0
             /* name */,
          double        = 1.0
             /* length [m]: the length of the bunch
                in its rest frame */,
          double        = 3.3e11
             /* strength : total number of proton
                charges in the bunch */,
          double        = 1000.0
             /* gamma of the bunch, in the lab frame */,
          const double* = 0
             /* pointer to an array containing
                three values for 6 X invariant emittance [m] / pi  */
        );
  BBLens( const BBLens& );
  ~BBLens();

  char useRound;         // By default = 1
                         // If 1: then round beam approximation
                         // used when horizontal and vertical 
                         // sigmas approximately equal.

  void Kludge( double  /* num    */, 
               double  /* gamma  */, 
         const double* /* sigma  */ );
  void KludgeNum( double ); 
  void KludgeSigma( const double* );
  void AdjustSigma();

  Vector NormalizedEField( double x, double y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  JetVector NormalizedEField( const Jet& x, const Jet& y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitBBLens( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitBBLens( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "BBLens") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new BBLens( *this ); }
  Vector Beta();
  void GetSigma( double* );
};

#endif // BBLENS_H
