#ifndef FRAME_H
#define FRAME_H

#include "VectorD.h"
#include "Matrix.h"

class Frame
{
 private:
   Vector  o;
   MatrixD e;
 public:
   Frame();
   Frame( const Frame& );
   ~Frame();

   Frame& operator=( const Frame& );
   
   short int setOrigin   ( const Vector& );
   short int setAxis     ( int, const Vector& );
   short int setDualAxis ( int, const Vector& );

   Vector getOrigin()        const;
   Vector getAxis( int )     const;
   Vector getDualAxis( int ) const;

   short int rotate    ( double,        // rotation angle [ radians ]
                         const Vector&  // axis of rotation (right-hand sense)
                       );
   short int translate ( const Vector&  // displacement of origin
                       );

   Frame dual() const;
};

#endif  // FRAME_H
