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
******  Copyright (c) 1990 Universities Research Association, Inc./Fermilab    
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                              
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

#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/MathConstants.h> 

class DLLEXPORT Frame
{

 public:

   Frame();
   Frame( Frame const& );
  ~Frame();

   Frame& operator=( Frame const& x );

   static Frame const& identityFrame();

   int setOrigin   ( Vector const& );
   int setAxis     ( int, Vector const&  );
   int setDualAxis ( int, Vector const&  );

   bool setOrthonormalAxes( MatrixD const& );

   Frame dual() const;
   bool  isOrthonormal() const;
   bool  isRightHanded() const;


   static int xAxisIndex();
   static int yAxisIndex();
   static int zAxisIndex();

   Vector         getOrigin()        const;
   Vector         getAxis( int )     const;
   Vector         getDualAxis( int ) const;
   Vector         getxAxis()         const;
   Vector         getyAxis()         const;
   Vector         getzAxis()         const;
   MatrixD const& getAxes()          const;

   int rotate    ( double,        // rotation angle [ radians ]
                         const Vector&, // axis of rotation (right-hand sense)
                         bool = true    // false -> do not rotate origin
                       );
   int translate ( Vector const&  // displacement of origin
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

   void convertInPlace( Vector& p, Vector& v ) const;
                                  // Converts the input arguments - which
                                  //  model an element of a tangent bundle;
                                  //  i.e. the vector v is "attached" to 
                                  //  the point p - to their values as
                                  //  viewed by the frame.

   friend std::ostream& operator<< ( std::ostream& os, const Frame& );
   friend std::istream& operator>> ( std::istream& is, Frame& );


 private:

   Vector  o_;
   MatrixD e_;

};


inline int Frame::xAxisIndex()
{ 
  return 0; 
}

inline int Frame::yAxisIndex()
{ 
  return 1; 
}

inline int Frame::zAxisIndex()
{ 
  return 2; 
}

inline MatrixD const& Frame::getAxes() const
{
  return e_;
}


#endif  // FRAME_H
