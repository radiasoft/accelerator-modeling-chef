#ifndef FRAME_H
#define FRAME_H

#include "MXBBML_bool.h" // For Sun compilers: bool

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

   Frame dual() const;
   bool  isOrthonormal() const;

   Vector getOrigin()        const;
   Vector getAxis( int )     const;
   Vector getDualAxis( int ) const;

   short int rotate    ( double,        // rotation angle [ radians ]
                         const Vector&, // axis of rotation (right-hand sense)
                         bool = true    // 0 -> do not rotate origin
                       );
   short int translate ( const Vector&  // displacement of origin
                       );
   void      reset();

   friend ostream& operator<< ( ostream& os, /* const */ Frame& );
};


#endif  // FRAME_H
