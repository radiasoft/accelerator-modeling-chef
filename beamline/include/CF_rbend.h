#ifndef CF_RBEND_H
#define CF_RBEND_H

#include "beamline.h"

class CF_rbend : public bmlnElmnt
{
  public:

  CF_rbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // Pole face angle [ radians ]
            int = 4 );
 

  CF_rbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // Pole face angle [ radians ]
            int = 4 );


  CF_rbend( double,         // length     [ meter    ]
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


  CF_rbend( const char*,    // name
            double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            int = 4 );


  CF_rbend( const CF_rbend& );

  ~CF_rbend();
  void eliminate();


  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) 
    { v.visitCF_rbend( this ); }
  
  char* Type() const;
  bmlnElmnt* Clone() const 
    { return new CF_rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );

  double PoleFaceAngle()    
    { return _poleFaceAngle; }
  double getPoleFaceAngle() 
    { return _poleFaceAngle; }

  double AdjustPosition( const Proton& );
  double AdjustPosition( const JetProton& );


  int setQuadrupole( double );  
  int setSextupole( double );  
  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B"l/2 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  double getQuadrupole();
  double getSextupole();
  // Returns integrated multipole strengths
  // i.e., .getQuadrupole() returns B'l
  //       .getSextupole()  returns B"l/2


 private:

  bmlnElmnt** _u;
  bmlnElmnt** _v;

  double _poleFaceAngle;

  ostream& writeTo(ostream&);
  istream& readFrom(istream&);
};


#endif // CF_RBEND
