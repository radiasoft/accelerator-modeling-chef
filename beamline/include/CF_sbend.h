#ifndef CF_SBEND_H
#define CF_SBEND_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class CF_sbend : public bmlnElmnt
{
  public:

  CF_sbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // angle   [ radians ]
            int = 4 );
 

  CF_sbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // angle   [ radians ]
            int = 4 );


  CF_sbend( double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            // Normal and skew multipoles alternate.
            // xmlt(0)       = b_0    xmlt(1)     = a_0
            // xmlt(2)       = b_1    xmlt(3)     = a_1
            // xmlt(4)       = b_2    xmlt(5)     = a_2
            // ...
            // xmlt(2*m)     = b_[m]  xmlt(2*m+1) = a_[m]
            int = 4 );


  CF_sbend( const char*,    // name
            double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            int = 4 );


  CF_sbend( const CF_sbend& );

  ~CF_sbend();
  void eliminate();


  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) { v.visitCF_sbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCF_sbend( this ); }
  
  void peekAt( double& s, Particle* = 0 );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "CF_sbend") != 0 )
    return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const 
    { return new CF_sbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );


  int setQuadrupole  ( double );  
  int setSextupole   ( double );  
  int setOctupole    ( double );  
  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B''l/2 )
  //       .setOctupole   ( B'''l/6 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  int setDipoleField ( double );  
  // Here the argument is the dipole field, 
  // NOT the integrated dipole field.

  double getQuadrupole() const;
  double getSextupole()  const;
  double getOctupole()   const;
  // Returns integrated multipole strengths
  // i.e., .getQuadrupole() returns B'l
  //       .getSextupole()  returns B''l/2
  //       .getOctupole()   returns B'''l/6

  double getDipoleField() const;
  // Returns the dipole field,
  // NOT the integrated dipole field.

  double getAngle() const;
  double Angle()    const;

 private:

  bmlnElmnt** _u;
  bmlnElmnt** _v;

  double _angle; // total bend angle  [ radians ]

  ostream& writeTo(ostream&);
  istream& readFrom(istream&);
};


#endif // CF_SBEND
