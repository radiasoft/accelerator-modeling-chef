#ifndef _h_SteerAnalysis
#define _h_SteerAnalysis 1
#include <vector>
#include <iostream>
#include <fstream>
#include <MathConstants.h>
#include "bmlnElmnt.h"
#include <PhysicsConstants.h>
#include <Distribution.h>
#include <beamline.h>
#include "SteerUtil.h"
#include "myPositronBunch.h"

// Steering Analysis utility, to feed back results from previous runs to current one. 

namespace Steering {
//! \brief BPM + Wire Scan data container. 
/*!   At each BPM, this simulation package computes ( Steering::StudyPerformance) 
*   the position of the beam, either directly or via the BPM reading, and various estimates 
* of the emittances, and dump it to a file. At a later stage, such file can be read in and 
* studied. A small struct contains the data for one stations.. 
* Subject to change!... And actually not used in this set of tests. 
*/
  class MonitData {  // Data from a CHEF monitoring station, i.e. a DFS BPM 
  
    public:
      explicit MonitData(); // all initialized to 0, BPM to 999999
      int BPM;  
      double z, y, yp, epsilY, epsilNDY, epsilYM, epsilNDYM, Disp, DispP, BpmYOff, QuadOff, DipVal;
      double final_y, final_epsilY, final_epsilNDY, final_epsilYM, final_epsilNDYM; // unused for now. 
  };
//! \brief Dispersion Matched Steering Performance analysis 
/*!   At each beam steering simlation, a set of data reflecting the performance of 
      the corresponding tuning is gathered. Not currently used in these examples. 
*/
  class  DFSRun {
    public:
      // the Directory name incluse the run label, job number (if any) and the process number.  
     explicit DFSRun(const std::string &fDirName, 
                     int macroIter, int fDipole, bool finalPulse=false);
     explicit DFSRun(const std::string &fName, 
                      bool finalPulse=false);
    private:
     std::string label;  // include the file number
     int jobNum;        // Condor Job number 
     int procNum;      // Condor Process number 
     int macroIter;    // Macro iteration 
     int firstDipole;  // first dipole 
     bool finalResult;
     std::vector<MonitData> data;
     
     void fillFromFile(const std::string &fName, bool finalPulse=false);
     
    public:
     
     inline std::string getLabel() { return label;}
     inline int getJobNumber() { return  jobNum; }
     inline int getProcNumber() { return  procNum; }
     inline int getMacroIterationNumber() { return  macroIter; }
     inline int getFirstDipole() { return  firstDipole; }
     inline bool isFinal() { return  finalResult; }
     
     inline size_t getNumberBPMS() {return data.size();}
     inline double getZ(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].z;}
     inline double getYPos(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].y;}
     inline double getYPrime(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].yp;}
     inline double getEpsilY(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].epsilY;}
     inline double getEpsilNDY(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].epsilNDY;}
     inline double getEpsilYMerlin(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].epsilYM;}
     inline double getEpsilNDYMerlin(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].epsilNDYM;}
     inline double getDispersion(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].Disp;}
     inline double getDispersionPrime(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].DispP;}
     inline double getDipoleValue(size_t iBPM) {
         if (iBPM > data.size()) return 0.;
	 return data[iBPM].DipVal;}
	
     void loadCorrectors(SteerUtil* myUtility);	
	  
};

void   StudyDispersionDMSAA1(SteerUtil* aSteerUtil);  // generic name, study, analysis whatever.. 
void   StudyDispersionDMSAA2(SteerUtil* aSteerUtil);

} // Steering namespace


#endif
