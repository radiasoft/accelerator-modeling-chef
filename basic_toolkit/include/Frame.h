/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Frame.h
******  Version:   4.1
******  Date:      March 15, 2005
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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
#include "TMatrix.h"

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

   bool setOrthonormalAxes( const MatrixD& );

   Frame dual() const;
   bool  isOrthonormal() const;

   static int xAxisIndex();
   static int yAxisIndex();
   static int zAxisIndex();

   Vector  getOrigin()        const;
   Vector  getAxis( int )     const;
   Vector  getDualAxis( int ) const;
   Vector  getxAxis()         const;
   Vector  getyAxis()         const;
   Vector  getzAxis()         const;
   MatrixD getAxes()          const;

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
   static Frame tween( const Frame&, const Frame&, double, bool=true );
                                  // Returns a Frame located in between
                                  //  its first two arguments. 
                                  // The third argument should lie
                                  //  within [0,1].  
                                  // The last argument determines whether
                                  //  all paranoid checks are done.
                                  // It is called "tween" because of
                                  //  its similarity to the "tweening"
                                  //  done in cartoon animation.

   friend std::ostream& operator<< ( std::ostream& os, /* const */ Frame& );
   friend std::istream& operator>> ( std::istream& is, /* const */ Frame& );
};



#include "Frame.icc"


#endif  // FRAME_H
