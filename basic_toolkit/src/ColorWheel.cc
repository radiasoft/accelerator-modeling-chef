#include "ColorWheel.h"

// REMOVE #define RED    0.
// REMOVE #define GREEN  120.
// REMOVE #define BLUE   240.

ColorWheel::ColorWheel( double ang )
: _theta(ang), _delta(0.), _mode256(false)
{
  while( _theta >= 360. ) _theta -= 360.0;
  while( _theta <  0.   ) _theta += 360.0;
}


// REMOVE double ColorWheel::red()
// REMOVE {
// REMOVE   double ret = 0.;
// REMOVE 
// REMOVE   if(_mode256) 
// REMOVE   {
// REMOVE     if( _theta >= RED && _theta < GREEN ) {
// REMOVE       ret = 255.0*((GREEN - _theta)/120.);
// REMOVE     }
// REMOVE     else if( _theta >= BLUE && _theta < (RED+360) ) {
// REMOVE       ret = 255. - this->blue();
// REMOVE     }
// REMOVE   }
// REMOVE   else 
// REMOVE   {
// REMOVE     if( _theta >= RED && _theta < GREEN ) {
// REMOVE       ret = (GREEN - _theta)/120.;
// REMOVE     }
// REMOVE     else if( _theta >= BLUE && _theta < (RED+360) ) {
// REMOVE       ret = 1.0 - this->blue();
// REMOVE     }
// REMOVE   }
// REMOVE 
// REMOVE   return ret;
// REMOVE }
// REMOVE 
// REMOVE double ColorWheel::green()
// REMOVE {
// REMOVE   double ret = 0.;
// REMOVE 
// REMOVE   if(_mode256)
// REMOVE   {
// REMOVE     if( _theta >= GREEN && _theta < BLUE ) {
// REMOVE       ret = 255.0*((BLUE - _theta)/120.);
// REMOVE     }
// REMOVE     else if( _theta >= RED && _theta < GREEN ) {
// REMOVE       ret = 255. - this->red();
// REMOVE     }
// REMOVE   }
// REMOVE   else
// REMOVE   {
// REMOVE     if( _theta >= GREEN && _theta < BLUE ) {
// REMOVE       ret = (BLUE - _theta)/120.;
// REMOVE     }
// REMOVE     else if( _theta >= RED && _theta < GREEN ) {
// REMOVE       ret = 1.0 - this->red();
// REMOVE     }
// REMOVE   }
// REMOVE 
// REMOVE   return ret;
// REMOVE }
// REMOVE 
// REMOVE double ColorWheel::blue()
// REMOVE {
// REMOVE   double ret = 0;
// REMOVE 
// REMOVE   if(_mode256) 
// REMOVE   {
// REMOVE     if( _theta >= BLUE && _theta < (RED+360) ) {
// REMOVE       ret = 255.0*(((RED+360) - _theta)/120.);
// REMOVE     }
// REMOVE     else if( _theta >= GREEN && _theta < BLUE ) {
// REMOVE       ret = 255. - this->green();
// REMOVE     }
// REMOVE   }
// REMOVE   else 
// REMOVE   {
// REMOVE     if( _theta >= BLUE && _theta < (RED+360) ) {
// REMOVE       ret = ((RED+360) - _theta)/120.;
// REMOVE     }
// REMOVE     else if( _theta >= GREEN && _theta < BLUE ) {
// REMOVE       ret = 1. - this->green();
// REMOVE     }
// REMOVE   }
// REMOVE 
// REMOVE   return ret;
// REMOVE }

double ColorWheel::red()
{
  double ret = 0.;

  if(_mode256) 
  {
    if( _theta >= 300.0 || _theta <= 60.0 ) {
      ret = 255.0;
    }
    else if( _theta > 240.0 ) {
      ret = 255.0*((_theta - 240.0)/60.0);
    }
    else if( _theta < 120.0 ) {
      ret = 255.0*((120.0 - _theta)/60.0);
    }
  }

  else 
  {
    if( _theta >= 300.0 || _theta <= 60.0 ) {
      ret = 1.0;
    }
    else if( _theta > 240.0 ) {
      ret = (_theta - 240.0)/60.0;
    }
    else if( _theta < 120.0 ) {
      ret = (120.0 - _theta)/60.0;
    }
  }

  return ret;
}

double ColorWheel::green()
{
  double ret = 0.;

  if(_mode256) 
  {
    if( _theta >= 60.0 && _theta <= 180.0 ) {
      ret = 255.0;
    }
    else if( _theta < 60.0 ) {
      ret = 255.0*((_theta)/60.0);
    }
    else if( _theta > 180.0 && _theta < 240.0 ) {
      ret = 255.0*((240.0 - _theta)/60.0);
    }
  }

  else 
  {
    if( _theta >= 60.0 && _theta <= 180.0 ) {
      ret = 1.0;
    }
    else if( _theta < 60.0 ) {
      ret = (_theta)/60.0;
    }
    else if( _theta > 180.0 && _theta < 240.0 ) {
      ret = (240.0 - _theta)/60.0;
    }
  }

  return ret;
}

double ColorWheel::blue()
{
  double ret = 0;

  if(_mode256) 
  {
    if( _theta >= 180.0 && _theta <= 300.0 ) {
      ret = 255.0;
    }
    else if( _theta > 120.0 && _theta < 180.0 ) {
      ret = 255.0*((_theta - 120.0 )/60.0);
    }
    else if( _theta > 300.0 && _theta < 360.0 ) {
      ret = 255.0*((360.0 - _theta)/60.0);
    }
  }

  else 
  {
    if( _theta >= 180.0 && _theta <= 300.0 ) {
      ret = 1.0;
    }
    else if( _theta > 120.0 && _theta < 180.0 ) {
      ret = (_theta - 120.0 )/60.0;
    }
    else if( _theta > 300.0 && _theta < 360.0 ) {
      ret = (360.0 - _theta)/60.0;
    }
  }

  return ret;
}

double ColorWheel::angle()
{
  return _theta;
}


double ColorWheel::setAngle( double u )
{
  static double ret;
  ret = _theta;
  _theta = u;
  return ret;
}

double ColorWheel::setIncrement( double u )
{
  static double ret;
  ret = _delta;
  _delta = u;
  return ret;
}

double ColorWheel::increment()
{
  static double ret;
  ret = _theta;
  _theta += _delta;
  while( _theta >= 360.0 ) _theta -= 360.0;
  return ret;
}

double ColorWheel::changeBy( double x )
{
  static double ret;
  ret = _theta;
  _theta += x;
  while( _theta >= 360.0 ) _theta -= 360.0;
  return ret;
}


void ColorWheel::changeMode()
{
  _mode256 = !_mode256;
}
