#ifndef _h_SteeringAlarm
#define _h_SteeringAlarm 1
#include <vector>
#include <iostream>

namespace Steering { 
  
     // So far, only a simple aperture trigger. 
     // Will grow as need be... 
  class Alarm {
    public:
      Alarm(double rCutMax);
      
      void reset();
      inline bool isOn() const { 
          return (std::sqrt(xBeam*xBeam + yBeam*yBeam) > rCut);}

    protected:
	  
     double rCut; // Maximum radius.. 
     
     
     double sLocation;
     double xBeam;
     double yBeam;
     
     
     
     
  };
} //Steering 

#endif

