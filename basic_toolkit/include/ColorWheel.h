/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      ColorWheel.h
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
