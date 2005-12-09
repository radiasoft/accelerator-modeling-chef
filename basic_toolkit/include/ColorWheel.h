/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      ColorWheel.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab    
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
**************************************************************************
*************************************************************************/


#ifndef COLORWHEEL_H
#define COLORWHEEL_H

class ColorWheel
{
 public:
  ColorWheel( double /*.angle [deg].*/ = 0.0 );

  double red();
  double green();
  double blue();

  double angle();  // Returns [0..360), where
                   // 0 is pure red, 
                   // 120 is pure green, and
                   // 240 is pure blue.

  double setAngle( double /*.[deg].*/ );
  double setIncrement( double /*.[deg].*/ );
                   // Returns the old value 
                   // of the increment.
  
  double increment();
  double changeBy( double /*.[deg].*/ );
  void   changeMode();

 private:
  double _theta;
  double _delta;
  bool   _mode256;
};

#endif // COLORWHEEL_H
