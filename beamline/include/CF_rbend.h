#ifndef CF_RBEND_H
#define CF_RBEND_H

#include "mxyzptlk.h"
#include "beamline.inc"

#include "combinedFunction.h"

class CF_rbend : public bmlnElmnt
{
 public:
  CF_rbend( double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&   // multipoles [ meter^-n ]
            // Normal and skew multipoles alternate.
            // xmlt(0)       = b_0    xmlt(1)     = a_0
            // xmlt(2)       = b_1    xmlt(3)     = a_1
            // xmlt(4)       = b_2    xmlt(5)     = a_2
            // ...
            // xmlt(2*m)     = b_[m]  xmlt(2*m+1) = a_[m]
          );
  CF_rbend( char*,          // name
            double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&   // multipoles [ meter^-n ]
          );
  CF_rbend( const CF_rbend& );

  ~CF_rbend();

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) 
    { v.visitCF_rbend( this ); }
  
  char* Type() const 
    { return "CF_rbend"; }
  bmlnElmnt* Clone() const 
    { return new CF_rbend( *this ); }


 private:
  bmlnElmnt* u[52];
};

#endif // CF_RBEND
