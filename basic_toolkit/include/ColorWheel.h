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
