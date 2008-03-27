#include <cmath>
#include <iostream>
#include "MLBPM.h"
#include <Particle.h>
#include <ParticleBunch.h>

using std::abs;
using std::cerr;
using std::cout;
using std::endl;

#define DEFAULTRESOLUTION 1.0e-7
#define DEFAULTSCALE 1.

MLBPM::MLBPM(int index, double zp, bool isHorizontal):
     id(index), integrate(false), zPos(zp), ct(0), value(0.), 
     error(0.), offset(0.), horizontal(false), resolution(DEFAULTRESOLUTION),
     scale(DEFAULTSCALE)
      {;}
MLBPM::MLBPM():
     id(0), integrate(false), zPos(0), ct(0), value(0.), error(0.),
      offset(0.), horizontal(false), resolution(DEFAULTRESOLUTION),
      scale(DEFAULTSCALE)
      {;}

Gaussian MLBPM::myGauss = Gaussian();
             
MLBPM::~MLBPM() {;}

void MLBPM::SetResolution(double v, double s) {
    resolution = v;
    scale = s;
 }

//Filling in our 
void MLBPM::localPropagate (Particle& p) {
   
   double tmpVal = 0.;
   
   if (horizontal) tmpVal = p.get_x();
   else tmpVal = p.get_y();
   
   // Adding the scale factor..
   tmpVal *= scale;
   
   // Adding resolution 
   tmpVal += resolution*myGauss.getValue();		
   error = 1.0e6*resolution;
   // Adding my own offset 
   tmpVal += offset;
   ct = zPos;
   if (!integrate) 
   tmpVal *= 1.e6; // now in microns
    

   if (integrate) value += tmpVal;
   else value = tmpVal;
//   cerr << " Saw BPM data for id = " << id << " ct " << ct 
//       << " zp " << zPos   <<  " value " <<  
//       value << " error " << error << endl;
   
// Ignore othe effects for now..     		

}

void MLBPM::localPropagate (ParticleBunch& ps) {
	
    if (ps.size() == 0) return;
    double tmpVal = 0.;
    double posZ = 0.;
    for (ParticleBunch::const_iterator pPtr = ps.begin(); 
           pPtr != ps.end(); ++pPtr) {
         double vv = 0.;
         if (horizontal) vv = scale*pPtr->get_x(); 
	 else vv = scale*pPtr->get_y();
	 tmpVal += vv;
	 posZ +=  pPtr->get_cdt();
    }
    tmpVal /= ps.size();
    posZ /= ps.size();	
  // Adding resolution
   double errMeasure =  resolution*myGauss.getValue(); 
   tmpVal += errMeasure;
   // Adding my own offset 
   tmpVal += offset;
//   cerr << " BPM Add " << (this) << " tmpVal = " << tmpVal << " +- " << errMeasure << endl;		
   error = 1.0e6*resolution;
   ct = zPos + posZ;
   tmpVal *= 1.e6; // now in microns 
   if (integrate) value += tmpVal;
   else value = tmpVal;
//   cerr << " Saw BPM data for id = " << id << " ct " << ct 
//       << " zp " << zPos   <<  " value " <<  
//       value << " error " << error << endl;


}
