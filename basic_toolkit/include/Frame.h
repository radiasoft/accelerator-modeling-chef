/*
 * File: Frame.h
 * Header file for class Frame
 * 
 * Version 1.0       July, 1998
 * 
 * Author: Leo Michelotti
 * michelotti@fnal.gov
 * 
 */

#ifndef FRAME_H
#define FRAME_H

#include "VectorD.h"
#include "Matrix.h"

// #include "MXBBML_bool.h" // For Sun compilers: bool
#include "MathConstants.h" // For Sun compilers: bool

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

   int xAxisIndex();
   int yAxisIndex();
   int zAxisIndex();

   Vector getOrigin()        const;
   Vector getAxis( int )     const;
   Vector getxAxis()         const;
   Vector getyAxis()         const;
   Vector getzAxis()         const;
   Vector getDualAxis( int ) const;

   short int rotate    ( double,        // rotation angle [ radians ]
                         const Vector&, // axis of rotation (right-hand sense)
                         bool = true    // false -> do not rotate origin
                       );
   short int translate ( const Vector&  // displacement of origin
                       );
   void      reset();

   Frame relativeTo( const Frame& ) const;   // Returns "this" Frame
                                  // "as seen by" the argument Frame.
                                  // Assumes an orthonormal Frame.
   Frame patchedOnto( const Frame& ) const;   // Patches "this" Frame
                                  // onto the argument Frame: the
                                  // inverse operation of relativeTo.
                                  // Assumes an orthonormal Frame.

   friend ostream& operator<< ( ostream& os, /* const */ Frame& );
   friend istream& operator>> ( istream& is, /* const */ Frame& );
};



#include "Frame.icc"


#endif  // FRAME_H
