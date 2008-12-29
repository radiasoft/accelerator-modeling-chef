#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/thinpoles.h>
#include <basic_toolkit/Distribution.h>
#include <SteerUtil.h>
#include <myPositronBunch.h>
#include <beamline/Alignment.h>
#include <beamline/LinacCavity.h>
#include <beamline/rfcavity.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/marker.h>
#include <beamline/thinpoles.h>
//#include "SteeringAlarm.h" Later..

using std::ifstream;
using std::cout;
using std::endl;
using std::cerr;

const double SteerUtil::kFact = PH_CNV_brho_to_p;  

SteerUtil::SteerUtil(BmlPtr model,  
                     MLBPMs* bpms,  long seed,
		     bool withPlots ):
beamEnergyZero(model->Energy()),
myGauss(0., 1., seed),
theModel(model), 
theBPMs(bpms), 
nominalVoltage(23.4e6),
hasDoubleQuads(false),
numDim(bpms->GetNumBPMS()),
correctors(0),
quadList(0),
allCavities(0),
nominalCavityVoltage(0),
nominalCavityPhase(0),
refPDip(0),
refPQuad(0),
refBRhoQuad(0),
refPBPM(0),
refDispersionAtBPM(0),
refDispersionPrimeAtBPM(0),
refPosDispersionAtBPM(0),
dipoleOK(0),
totalNumberPulses(0),
theCurrentBunchMomentum(0.),
stateAtInjection(6),
bunchMatch(6,6),      // Obsolete, I suspect... 
bunchEpsilY(0.02e-6 *M_PI), // Default emittances at the ILC 
bunchEpsilX(0.02e-6 * 20.*M_PI), 
bunchDPoP(0.0278), 
bunchSigmaZ(0.3e-3), 
bunchYPrimeOffset(0.0), 
bunchXPrimeOffset(0.0), 
bunchYOffset(0.0), 
bunchXOffset(0.0), 
bunchDPoPOffset(0.0),
bunchRMEpsilY(0.02e-8 *M_PI), // Default emittances at the ILC, divided by 100
                               // Cheat!!! assuming bunch for response matrix 
			       // determination are quasi single particles.  
bunchRMEpsilX(0.02e-8 * 20.*M_PI), 
bunchRMDPoP(0.0278), 
bunchRMSigmaZ(0.3e-3), 
bunchRMYPrimeOffset(0.0), 
bunchRMXPrimeOffset(0.0), 
bunchRMYOffset(0.0), 
bunchRMXOffset(0.0), 
bunchRMDPoPOffset(0.0),
atOffEnergy(false),
numRequiredPulses(10),
pilotBunches(0), 
pilotBunchesOffE(0), 
pilotBunchesRM(0),
pilotBunchesRMOffE(0),
displayToken(""),
numCavPerKlystrons(24), // ILC default , RDR_V0
bpmsDisplayed(0),
fOutMovieScale(0),
wakeIsOn(false),
twissIn(),
gridFileNameUpdate("")
// twissIn not defined at this stage..
{ // This will fail if there are no bpms.. Oh well.. 

 
 startingSteeringTime = time(0);
 
 LinacCavityPtr rfcPtr;
 QuadrupolePtr quadPtr;

 theBPMs->setGaussianDist(&myGauss);
 
 // First, decide if we have the type of model where we have a thin2pole 
 // (actually, now a vkick...) 
 // in the middle of quadrupole (Benchmark test) 
 for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
    ElmPtr q = *dbi;	
    if( QuadrupolePtr quadPtr= boost::dynamic_pointer_cast<quadrupole> (q) ) {
       beamline::iterator dbiN = dbi++;
       ElmPtr qNext = *dbiN;
       if( VKickPtr dipPtrN = boost::dynamic_pointer_cast<vkick> (qNext) ) {
         beamline::iterator dbiNN = dbiN++;
         ElmPtr qNextNext = *dbiNN;
         if(QuadrupolePtr quadPtrNN = boost::dynamic_pointer_cast<quadrupole> (qNextNext) )  {
            hasDoubleQuads = true;
	    break;
	 }
	 if (hasDoubleQuads) break;
      }
      if (hasDoubleQuads) break;
   }
 }
 if (hasDoubleQuads) cout << " Split Quadrupole configuration detected " << endl;
 for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
   ElmPtr q = *dbi;
   if (MarkerPtr xxPtr= boost::dynamic_pointer_cast<marker> (q)) continue;	
   if (DriftPtr xxPtr= boost::dynamic_pointer_cast<drift> (q)) continue;	
   if (QuadrupolePtr quadPtr= boost::dynamic_pointer_cast<quadrupole> (q)) {	
      quadList.push_back(quadPtr);
   }
   else if(LinacCavityPtr rfcPtr= boost::dynamic_pointer_cast<LinacCavity> (q))  {
     allCavities.push_back(rfcPtr);
     // Keep a carbon copy of the initial state of the linac, in case we muck 
     // things up. 
     nominalCavityVoltage.push_back(rfcPtr->Strength());
     nominalCavityPhase.push_back(rfcPtr->getPhi());
     OffECavityVoltage.push_back(0.);
     OffECavityPhase.push_back(0.);
   }
   else if(VKickPtr dipPtr= boost::dynamic_pointer_cast<vkick> (q)) {
//      cerr << " Kick angle for dipole " << correctors.size() 
//           << " =  " << dipPtr->Strength() << endl;
      correctors.push_back(dipPtr);
      nominalsCorrStrength.push_back(dipPtr->Strength());
      dipoleOK.push_back(true);
   }
  }
  int numQuadReal = quadList.size();
  if (hasDoubleQuads) numQuadReal /= 2;
//  cout << " SteerUtil: Number of Cavities = " << allCavities.size() 
//       << " dipoles " << correctors.size() << " quadrupoles " <<  numQuadReal << endl;
  
// This is not enough: We want to make sure these vector are ordered by S. 
// This will be done in loadRefMomentum
 // Handy information: the element by element reference momentum for the perfect lattice. 
 
 loadRefMomentum();
 // Upload at state of particle at injection
 
   for (size_t k=0; k != 6; k++) stateAtInjection[k] = 0.;
   
   
 // And bunches 
 for (int ib=0; ib != bpms->GetNumBPMS(); ib++) {
   myPositronBunch *ab  = new myPositronBunch(beamEnergyZero);
   pilotBunches.push_back(ab);
   myPositronBunch *abOffE  = new myPositronBunch(beamEnergyZero);
   pilotBunchesOffE.push_back(abOffE);
   myPositronBunch *abRM  = new myPositronBunch(beamEnergyZero);
   pilotBunchesRM.push_back(abRM);
   myPositronBunch *abRMOffE  = new myPositronBunch(beamEnergyZero);
   pilotBunchesRMOffE.push_back(abRMOffE);
 }
 
 
 
}

SteerUtil::~SteerUtil() 
{
// cerr <<  " Deleting this steerUtil at " << this <<  endl; 
 for (int ib=0; ib != pilotBunches.size(); ib++) 
   delete pilotBunches[ib];
 for (int ib=0; ib != pilotBunchesRM.size(); ib++) 
   delete pilotBunchesRM[ib];
 for (int ib=0; ib != pilotBunchesOffE.size(); ib++) 
   delete pilotBunchesOffE[ib];
 for (int ib=0; ib != pilotBunchesRMOffE.size(); ib++) 
   delete pilotBunchesRMOffE[ib];
 delete fOutMovieScale;
}

void SteerUtil::setStateAtInjection(const Vector &stateIn) { 
  if (stateIn.Dim() != Particle::PSD) {
    std::cerr 
      << "SteerUtil::setStateAtInjection: Requires a 6D vector " << endl;
     return;
  }
  for (size_t k=0; k != Particle::PSD; ++k) stateAtInjection[k] = stateIn[k];

}

double SteerUtil::GetOneDipoleSetting(unsigned int corrIndex) const {

   if (corrIndex < correctors.size()) {
        // Get the component
	VKickPtr theDipole = correctors[corrIndex];
        return theDipole->Strength();
    } 
    return 0.;
}

bool SteerUtil::AdjustOneDipole(unsigned int corrIndex, double increment) {

    if ( correctors.size() >= corrIndex) {
        // Get the component
	VKickPtr theDipole = correctors[corrIndex];
        double cVal = theDipole->Strength();
//	double cValOld = cVal;
	cVal += increment;
        theDipole->setStrength(cVal);
//	cout << " Dipole " << corrIndex  << " changed from " << 
//	   cValOld << " to " << cVal << endl;
        return true;
    } 
    return false;
}

bool SteerUtil::SetOneDipole(unsigned int corrIndex, double value) {

   if (corrIndex < correctors.size()) {
        // Get the component
	VKickPtr theDipole = correctors[corrIndex];
        theDipole->setStrength(value);
//	cout << " Setting Dipole " << corrIndex << " to " << value << endl;
//	cout << " Reference momentum " << refPDip[corrIndex] << endl;
        return true;
    } 
    return false;
}

bool SteerUtil::SetAllDipoles(const char *fileName, double fact) {
  
// cerr << " In SetAllDipoles " << endl;
 std::ifstream fMis(fileName);
 if (!fMis) {
   cerr << " SteerUtil::SetAllDipoles: Can not open file " << fileName << endl;
   return false; 
 }
 int nLines = 0;
 bool isMyDumpTableCorrector = false;
 fMis.exceptions(std::ios_base::eofbit);
 
 int numElemInFile = 0; 
 int nDips = 0;
 const double factUnits = fact*1.0; // Rescale the motion, as suggested by Nikolai, for more 
                                 // more sensitive testing.. 
 // Use the fact that these are order in S 
 while (true) {
    try {
      std::string t1;
      getline(fMis, t1);
      nLines++;
      if ((nLines == 1) && 
          (t1.find("QuadY CDipVal") != std::string::npos)) {
	    isMyDumpTableCorrector =  true;
	    continue;
      } // Actually, not really needed for now, since the second 
      // word in Steering::DumpTableCorrector is the same 
      // quantity as in Jeff/Pt table from the BenchMark studies. 
      // reject comment line
      if (t1.find("!") != std::string::npos) continue;
      //
      double vars[2]; // siz of them, and they are:  s, correction;
      std::istringstream t1Str(t1);
      
      t1Str >> vars[0] >> vars[1];
      if (nDips >= correctors.size()) break; // no point reading further in the file..
      VKickPtr theDipole = correctors[nDips];
      theDipole->setStrength(factUnits*vars[1]);
//      cerr << " dipole " << nDips << " at z " << vars[0] 
//           << " correction (T.M) " << vars[1] << endl;
      nDips++;
  } catch (std::ios_base::failure) { 
       cout << " Eof of file " << fileName << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
          return false;
       }
       break;
     }
  }// reading one line  at a time 
 cout << " SteerUtil::SetAllDipoles, Adjusted " << nDips << " dipoles " <<endl;
 fMis.close();
 return true;
}
  

bool SteerUtil::MoveOneQuadrupole(unsigned int corrIndex, double deltaY) {

// Always relative to previous alignment.. 
    int corrIndexTrue = corrIndex;
    if (hasDoubleQuads) corrIndexTrue *= 2;
    if (corrIndex < quadList.size()) { 
           
	alignmentData alPrev = quadList[corrIndexTrue]->Alignment();
//	cerr << " Previous aligment for quad " << corrIndex << endl;
//	cerr << " Y = " << alPrev.yOffset << " X = " << alPrev.xOffset << endl;
//	cerr << " tilt " << alPrev.tilt << endl;       
	alPrev.yOffset += deltaY;
	quadList[corrIndexTrue]->setAlignment(alPrev);
	if (hasDoubleQuads) quadList[corrIndexTrue+1]->setAlignment(alPrev);
        return true;
    }
    return false;   
}

bool SteerUtil::MoveOneQuadrupoleRel(unsigned int corrIndex, double deltaY) {

// Always relative to previous alignment.. 
    int corrIndexTrue = corrIndex;
    if (hasDoubleQuads) corrIndexTrue *= 2;
    if (corrIndex < quadList.size()) { 
         int iErr = 0;
	 // Very slow due to change in user interface.. I have to iterate again
	 unsigned int n = 0; 
         for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	    ElmPtr q = *dbi;	
	    if (QuadrupolePtr quadPtr= boost::dynamic_pointer_cast<quadrupole> (q)) {
	      if (n == corrIndex) {
	         theModel->moveRelY(dbi, deltaY);
	         std::cerr << " I have moved Quadrupole " << corrIndex << " by " <<
	              deltaY << endl;
		 return true;
	    }
	    n++;
	  }
	}
//	theModel->moveRelY(quadList[corrIndexTrue], deltaY, iErr );
//	std::cerr << " I have moved Quadrupole " << corrIndex << " by " <<
//	 deltaY << endl;
	 
//        return true;
    }
    return false;   
}

bool SteerUtil::MoveAllQuadrupolesNormal(double deltaY) {

// Always relative to previous alignment.. 
    if ( quadList.size() == 0) return false;
    // Same as above, we move them by pairs.
    for (unsigned int ic = 0; ic != quadList.size(); ++ic) {
	alignmentData alPrev = quadList[ic]->Alignment();
	alPrev.yOffset += deltaY*myGauss.getValue();
	quadList[ic]->setAlignment(alPrev);
	if (hasDoubleQuads) {
	   quadList[ic+1]->setAlignment(alPrev);
	   ++ic;
	} 
    }
    return true;   
}

bool SteerUtil::SetAllQuadrupolesYNominal() {

    if ( quadList.size() == 0) return false;
    for (unsigned int ic = 0; ic != quadList.size(); ++ic) {
	alignmentData alPrev = quadList[ic]->Alignment();
	alPrev.yOffset = 0.;
	quadList[ic]->setAlignment(alPrev); 
    }
    return true;   
}
bool SteerUtil::SetAllQuadrupoleStrengthRandom(double sigma) {

    if ( quadList.size() == 0) return false;
    for (unsigned int ic = 0; ic != quadList.size(); ++ic) {
	double s = quadList[ic]->Strength();
	s *= 1. + sigma*myGauss.getValue();
	quadList[ic]->setStrength(s); 
    }
    return true;   
}

double SteerUtil::GetOneQuadrupoleYTrans(unsigned int corrIndex) const {
    if ( quadList.size() == 0) return 0.;
    int corrIndexTrue = corrIndex;
    if (hasDoubleQuads) corrIndexTrue *= 2;
    if (corrIndexTrue >= quadList.size()) return 0.;
//    std::cerr << " Getting to Quad misalignments " << endl;
    alignmentData alPrev = quadList[corrIndexTrue]->Alignment();
    
    return alPrev.yOffset;
    return 0.;
    
}

bool SteerUtil::MoveOneCavity(unsigned int corrIndex, double deltaY) {

// All quads are split, to accomodate for the correcotr which part of the package, 
//  and the BPM..  We need to move by pair 

    if (corrIndex >= allCavities.size()) return false;
    alignmentData alPrev = allCavities[corrIndex]->Alignment();
    alPrev.yOffset += deltaY;
    allCavities[corrIndex]->setAlignment(alPrev); 
    return true;
}

bool SteerUtil::MoveOneCavity(LinacCavityPtr aCav, double deltaY) {

// All quads are split, to accomodate for the correcotr which part of the package, 
//  and the BPM..  We need to move by pair 

    if (aCav == 0) return false;
    alignmentData alPrev = aCav->Alignment();
    alPrev.yOffset += deltaY;
    aCav->setAlignment(alPrev); 
    return true;
}

bool SteerUtil::MoveOneBPM(unsigned int iBPM, double deltaY) {
         theBPMs->setAnOffset(iBPM, deltaY); 
}

bool SteerUtil::RotateOneCavity(unsigned int corrIndex, double deltaYAngle) {

    if (corrIndex >= allCavities.size()) return false;
    int iErr = -10;
    double aPitch = this->getCavityPitch(allCavities[corrIndex]);
    aPitch += deltaYAngle; 
	 // Very slow due to change in user interface.. I have to iterate again
	 unsigned int n = 0; 
         for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	    ElmPtr q = *dbi;	
	    if (LinacCavityPtr cavPtr= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	      if (n == corrIndex) {
	         theModel->pitch(dbi, deltaYAngle, 1.0);
	         std::cerr << " I rotated the cavity " << corrIndex << " by " <<
	              deltaYAngle << endl;
		 return true;
	    }
	    n++;
	  }
	}
    
//    theModel->pitch(allCavities[corrIndex], deltaYAngle, 1.0, iErr);
    return false;
}

bool SteerUtil::RotateOneCavity(LinacCavityPtr aCav, double deltaYAngle) {

    if (aCav == 0) return false;
    int iErr = -10;
/*    
    cerr << " Testing .. SteerUtil::RotateOneCavity " << endl;
    theModel->pitch(aCav, 0.00155, 0.5, &iErr);
    double aPitch = this->getCavityPitch(aCav);
    cerr << " First 1.55 mRad, check " << aPitch << endl;
    theModel->pitch(aCav, -0.00155, 0.5, &iErr);
    aPitch = this->getCavityPitch(aCav);
    cerr << " Second, revert check " << aPitch << endl;
    theModel->pitch(aCav, -0.000760, 0.5, &iErr);
    theModel->pitch(aCav, -0.000200, 0.5, &iErr);
    aPitch = this->getCavityPitch(aCav);
    cerr << " Third, -0.000760 and -0.000200 " << aPitch << endl;
    theModel->pitch(aCav, 0.000960, 0.5, &iErr);
    aPitch = this->getCavityPitch(aCav);
    cerr << " Fourth, revert check " << aPitch << endl;
    cerr << " Second, revert check " << aPitch << endl;
    theModel->pitch(aCav, 0.000760, 0.5, &iErr);
    theModel->pitch(aCav, 0.000200, 0.5, &iErr);
    aPitch = this->getCavityPitch(aCav);
    cerr << " Fifth, 0.000760 and 0.000200 " << aPitch << endl;
    theModel->pitch(aCav, -0.000960, 0.5, &iErr);
    aPitch = this->getCavityPitch(aCav);
    cerr << " Six, revert check " << aPitch << endl;
    exit(2);
*/    
//    double aPitch = this->getCavityPitch(aCav);
//    aPitch += deltaYAngle;   The model does increment for me !! 
	 // Very slow due to change in user interface.. I have to iterate again
	 int n = 0;
         for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	    ElmPtr q = *dbi;	
	    if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	      if (aCav == cavPtrTmp) {
	         theModel->pitch(dbi, deltaYAngle, 1.0);
	         std::cerr << " I rotated the cavity number " << n << " by " <<
	              deltaYAngle << endl;
		 return true;
	    }
	    n++;
	  }
	}
    
//    theModel->pitch(aCav, deltaYAngle, 1.0, iErr);
    return (false);
}

double SteerUtil::getCavityPitch(LinacCavityPtr aCav) const {
   if (aCav == 0) return 0.;
   Frame atmpFrame = aCav->getUpstreamPinnedFrame();
//   cout << " Frame " << atmpFrame << endl;
   return std::asin(atmpFrame.getzAxis()(1));
}
   
double SteerUtil::getAverageCavityPitch(LinacCavityPtr firstCav, size_t numCav) const  {
   
   double aa = 0.;
   bool go = false;
   size_t nCav = 0;
   for (size_t iCav = 0; iCav != allCavities.size(); iCav++ ) {
     if (allCavities[iCav] == firstCav) go = true;
     if (!go) continue;
     aa += this->getCavityPitch(allCavities[iCav]);
     nCav++;
     if (nCav == numCav) break;
   }
   if (nCav == 0) return 0.;
   return aa/nCav;
}

bool SteerUtil::setVoltageOnCavity(unsigned int cavIndex, double volt) { // in MeV/m
   if (cavIndex >= allCavities.size()) return false; 
    allCavities[cavIndex]->setStrength(1.e6*volt);
   return true;
}

bool SteerUtil::MoveAllElements(const char *fileName, bool moveQuad, 
                        bool moveBPM, bool moveCav, 
			unsigned int firstBPMMoved ){ 

// cerr << " In MoveAllElements " << endl;
 std::ifstream fMis(fileName);
 if (!fMis) {
   cerr << " SteerUtil::MoveAllElements: Can not open file " << fileName << endl;
   return false; 
 }
 int nLines = 0;
 fMis.exceptions(std::ios_base::eofbit);
 
 int numElemInFile = 0; 
 int nCavs = 0; 
 int nQuads = 0;
 unsigned int nBPMs = 0;
 bool firstNonZeroMis = false; // Checking the configuration of the file vs model. 
 std::vector<LinacCavityPtr>::const_iterator cavFr = allCavities.begin();
 std::vector<QuadrupolePtr>::const_iterator quadFr = quadList.begin();
 std::vector<VKickPtr>::const_iterator dipoleFr = correctors.begin();
 bool moveCavityOK = true;
 bool moveQuadOK = true;
 bool warningCavity= true;
 bool warningQuad = true;
 bool startMoving = false;
 const double factSteer = 1.0; // Rescale the motion, as suggested by Nikolai, for more 
                                 // more sensitive testing.. 
 if (!moveCav) cout << " NOT moving Cavities " << endl;
 // Use the fact that these are order in S 
 while (true) {
    try {
      std::string t1;
      getline(fMis, t1);
      nLines++;
      // reject comment line
      if (t1.find("!") != std::string::npos) continue;
      if (t1.find("%") != std::string::npos) continue;
      //
      double vars[6]; // siz of them, and they are:  s, dx, dy, dxang, dyang, dtilt;
      for (int kVar=0; kVar != 6; ++kVar) {
       std::string varString(t1.substr(kVar*15, (kVar+1)*15)); // fixed spacing
       vars[kVar] = atof(varString.c_str()); 
      }
      if ((std::abs(vars[2]) > 1.0e-6) && (!firstNonZeroMis)) {
        firstNonZeroMis = true;
	cout << " First non zero line reached at line " << nLines;
        cout << " Number of cavities in files " << nCavs << endl;
        cout << " Number of Bpm in files " << nBPMs << endl;
        cout << " Number of quads in files " << nQuads << endl;
      }
//      if ((nLines > 5100) && (nLines < 5105)) {
//        cout << " at line " << nLines << " s = " << vars[0] << " dy " 
//	     << vars[2] << " dyang " << vars[4] << endl;
//      }
      // count the number of real emlement.
      if (t1.find("CAV") != std::string::npos) {
        if (cavFr == allCavities.end()) moveCavityOK = false;
	
	if (moveCavityOK && startMoving && moveCav) {
//	  alignmentData alPrev = (*cavFr)->Alignment();   
//          if (std::abs(vars[1]) > 1.e-8)  alPrev.xOffset = vars[1]*factSteer;
//          if (std::abs(vars[2]) > 1.e-8)  alPrev.yOffset = vars[2]*factSteer;
//	  (*cavFr)->setAlignment(alPrev);
//	   cerr << " Moving cavity at S = " << vars[0] << endl;   
	  if (std::abs(vars[1]) > 1.0e-10) {
             for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	        ElmPtr q = *dbi;	
	        if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	           if ((*cavFr) == cavPtrTmp) {
	         if (std::abs(vars[1]*factSteer) > 1.0e-9) 
		     theModel->moveRelX(dbi, vars[1]*factSteer);
		 break;
	    }
	   }
	 }
//	     theModel->moveRelX((*cavFr), vars[1]*factSteer, iErr );
	  }
//          if (iErr != 0) {
//	     cout << " not moving element at s = " << vars[0] << " dx = " << vars[1] << endl;
//	     cout << " .... all subsequent cavity misalingnment ignored" << endl;
//	     moveCavityOK = false;
//	  }
	  
          int iErr = 0;
// !!!!!!! Temporary test, remove cavity shift...
// Next two lines were commented out on Nov 9 2006  
// See result in DM2D20ANoGMNegWAbs1em4Dip2NoCavShift
	  if (std::abs(vars[2]) > 1.0e-10) { 
            for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	        ElmPtr q = *dbi;	
	        if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	           if ((*cavFr) == cavPtrTmp) {
	         theModel->moveRelY(dbi, vars[2]*factSteer);
		 break;
	        }
	      } 
//	      theModel->moveRelY((*cavFr), vars[2]*factSteer, iErr );
	    }
// Reverting to default code, shift and tilt	  
//          if (iErr != 0) {
//	     cout << " not moving element at s = " << vars[0] << " dy = " << vars[2] << endl;
//	     cout << " .... all subsequent cavity misalingnment ignored" << endl;
//	     moveCavityOK = false;
	  }
          if (std::abs(vars[3]) > 1.1e-10) { 
            for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	        ElmPtr q = *dbi;	
	        if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	           if ((*cavFr) == cavPtrTmp) {
	         theModel->yaw(dbi, vars[3]*factSteer, 0.5);
		 break;
	        }
	      }
	    } 
//	     theModel->yaw((*cavFr), vars[3]*factSteer, 0.5, iErr);   
	  }
//          if (iErr != 0) {
//	     cout << " not moving element at s = " << vars[0] << " does not exist in model." << endl;
//	     cout << " .... all subsequent cavity misalingnment ignored" << endl;
//	     moveCavityOK = false;
//	  }
          if (std::abs(vars[4]) > 1.1e-10) {
	     // Flipping sign of the pitch..
            for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	        ElmPtr q = *dbi;	
	        if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	           if ((*cavFr) == cavPtrTmp) {
	         theModel->pitch(dbi, -vars[4]*factSteer, 0.5);
		 break;
	        }
	      }
	    } 
	     
//	     theModel->pitch((*cavFr), -vars[4]*factSteer, 0.5, iErr);
            // Testing cavity rotation.. 
//            rfcavity *aCav = (*cavFr);
//	    cerr << " Testing getUpstreamPinnedFrame .. " << endl;
//	    Frame atmpFrame = ((bmlnElmnt *) aCav)->getUpstreamPinnedFrame();
//	    cerr << " Frame axis 2 " << atmpFrame.getzAxis() << endl;
//            double cosPitch = atmpFrame.getzAxis()(2);
//            cerr << " angle set " <<  -vars[4] << " from Frame " << std::acos(cosPitch) << std::endl;
//	     cerr << " pitched cavity at S = " << vars[0] << " pitch " 
//	          << getCavityPitch((*cavFr)) << endl;   
//             if (iErr != 0) {
//	       cout << " not moving element at s = " << vars[0] << " does not exist in model." << endl;
//	       cout << " .... all subsequent cavity misalingnment ignored" << endl;
//	       moveCavityOK = false;
//	    }
	 } // pitching the cavity... 
	}
        nCavs++;
	numElemInFile++;
	cavFr++;
	
      } else if (t1.find("QUAD") != std::string::npos ) {
        if (quadFr == quadList.end()) moveQuadOK = false;
	// skip the second 1/2 quad, does not exists in XtFF version.
	// except, or course, if we have the quad slip configuration  
	/* No longer needed, Jeff has updated his file to have only one quad. 
	if ((!hasDoubleQuads) && (nHalfQuads%2 == 1)) {
	   nHalfQuads ++;
	   continue;
	}
	*/
 	if (moveQuadOK && startMoving && moveQuad) {
	  alignmentData alPrev = (*quadFr)->Alignment();   
          if (std::abs(vars[1]) > 1.e-8)  alPrev.xOffset = vars[1]*factSteer;
          if (std::abs(vars[2]) > 1.e-8)  alPrev.yOffset = vars[2]*factSteer;
	  (*quadFr)->setAlignment(alPrev);
	  // Quad Rolls are missing !.... Oh well, next time 
	}
        nQuads ++;
	numElemInFile++;
	quadFr++;
     } else if (t1.find("BPM") != std::string::npos) {
           startMoving = (nBPMs >= firstBPMMoved);
           if (moveBPM && startMoving)	   
	       theBPMs->setAnOffset(nBPMs, vars[2]*factSteer); 
           nBPMs++;
	   
	         numElemInFile++;
     }   
  } catch (std::ios_base::failure) { 
       cout << " Eof of file " << fileName << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
          return false;
       }
       break;
     }
  }// reading one line  at a time 

 
   cout << " Total # of Elements in file " << numElemInFile << endl;
   cout << " Number of cavities in files " << nCavs << endl;
   cout << " Number of Bpm in files " << nBPMs << endl;
   cout << " Number of 1/2 quads in files " << nQuads << endl;
 
 // ???????? More stuff.
 fMis.close();
 return true;
}
bool SteerUtil::MoveAllElementsILCMLAlgo(const char *fileName ){ 
	// Read in misalignment file

/*
	ifstream ifs(fileName);
	if(!ifs) {
		cerr<<"problems openning file lattice/nick23p4_misxy_1.txt"<<endl;
		abort();
	}
	string s;
	getline(ifs,s); // ignore first line

	AcceleratorModel::Beamline linac = theModel->GetBeamline();

	vector<ComponentFrame*> frames;
//	linacModel.ExtractComponents("TWRFStructure.*|Quadrupole.*|BPM.*",frames);
	theModel->ExtractComponents("*",frames);
	cout<<"\n"<<frames.size()<<" components found"<<endl;
        double sInModel = 0.;
	vector<double> zLocYCor;
	for(size_t n=0; n<frames.size(); n++) {
	         
		 sInModel += (*frames[n]).GetComponent().GetLength();
	         std::string nameInModel = (*frames[n]).GetComponent().GetName();
		if(nameInModel.find("YCOR") != std::string::npos) {
		    zLocYCor.push_back(sInModel);
		   continue; 
		 }
		if(nameInModel.find("DCME") != std::string::npos) continue; 
		if(nameInModel.find("DICS") != std::string::npos) continue; 
		if(nameInModel.find("DUQP") != std::string::npos) continue; 
		if(nameInModel.find("DDQP") != std::string::npos) continue; 
		if(nameInModel.find("XCOR") != std::string::npos) continue; 
		if(nameInModel.find("DIMS") != std::string::npos) continue; 
//		if(nameInModel.find("BPM") != std::string::npos) continue; 
		double z,x,y,xa,ya,tilt;
		ifs>>z>>x>>y>>xa>>ya>>tilt;
		ifs>>s;  // class
		ifs>>s;  // name
		if(!ifs) {
			cerr<<"Error reading file lattice/nick23p4_misxy_1.txt"<<endl;
			abort();
		}
		// check name
		if(nameInModel != s) {
			cerr<<"name mismatch: ";
			cerr<<s<<" != "<<(*frames[n]).GetComponent().GetName()<<endl;
			abort();
		}
		// check Z 
                if (std::abs(z + (*frames[n]).GetComponent().GetLength() - sInModel) > 0.01) {
			cerr<<" Z Error : ";
			cerr<<s<<" != "<< nameInModel << endl;
                        cerr << " Z in file " << z + (*frames[n]).GetComponent().GetLength() 
		              << " In Model " <<  sInModel   << endl;
			abort();		
		} 
		// set alignment
		frames[n]->Translate(x,y,0);
		frames[n]->RotateY(xa);
		frames[n]->RotateX(-ya); // sign?
		frames[n]->RotateZ(tilt);
	}
*/
}
// 
// !!! Of real limited use, e.g., straight FoDo, 
// Assumptions: there is one dipole ahead or in each Quadrupole/corrector pack
// However, we start with dipole corrector one, related to how the benchmark test were 
// done
// 
void SteerUtil::SetAllDipolesFromQuadYTrans() {
  
  for (size_t iDip=1; iDip != correctors.size()-1; ++iDip) { 
    if ((iDip) >= quadList.size()) break;
    double yTrans = GetOneQuadrupoleYTrans(iDip);    
    QuadrupolePtr theQuad;
    if (hasDoubleQuads) theQuad = quadList[(iDip-1)*2]; // Here we make use of the assumption 
                                  // of dipole to quadrupole mapping, one to one.
    else theQuad = quadList[(iDip-1)];			  
    double qStrength = theQuad->Strength();
    double lengthQuad = theQuad->Length();
    double setting = 2.0 * lengthQuad * yTrans * qStrength; // Why two ? 
    SetOneDipole(iDip, setting);      
  }
  
} 

// We need to keep track of reference momentum.. for almost all steering algorythm
void SteerUtil::loadRefMomentum() {

 refPDip.clear();
 refPBPM.clear();
 refPQuad.clear();
 refBRhoQuad.clear();
 
 resetLinacGradient();
 
 Particle* prtPtr = new Positron( beamEnergyZero );

 double s = 0.;
 double pMom;
 for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
    ElmPtr q = *dbi;
    q->propagate((*prtPtr));
    s += q->Length();
    pMom = (*prtPtr).Momentum();
    if(boost::dynamic_pointer_cast<monitor> (q)) {
       refPBPM.push_back(pMom);
// Set the reference Dispersion to zero. 
//  We'll fix that in fillReferenceDispersion in SteerAddUtil.cc 
       refDispersionAtBPM.push_back(0.);
       refDispersionPrimeAtBPM.push_back(0.);
       refPosDispersionAtBPM.push_back(0.);
    }
    if(boost::dynamic_pointer_cast<vkick> (q)) {
        refPDip.push_back(pMom);
    }
    if(boost::dynamic_pointer_cast<quadrupole> (q)) {
        refPQuad.push_back(pMom);
	refBRhoQuad.push_back(prtPtr->ReferenceBRho());
	// Set the reference dispersion to zero. 
    }
//    if( 0 == strcmp( "quadrupole", q->Type() ) ) 
//       cerr << " At quad S = " << s << " pMom = " << pMom << endl; 
 } // Loop through the lattice...
// cerr << " End of recording nominal momentum... P final " << pMom << endl; 
 if (refPDip.size() != correctors.size()) {
        cerr << " Inconsistent geometry, while getting reference momentum, found " 
	     << refPDip.size() << " dipoles, expected  " << correctors.size() <<  endl;
	exit(2);
 }
 if (refPBPM.size() != theBPMs->GetNumBPMS()) {
        cerr << " Inconsistent geometry, while getting reference momentum, found " 
	     << refPBPM.size() << " BPMs, expected  " 
	     << theBPMs->GetNumBPMS() <<  endl;
	exit(2);
 }
 
 delete prtPtr;
}

size_t SteerUtil::getBPMPtrs(MLBPMs& bpms, size_t first, size_t num) const {
    size_t nn = 0;
    bpms.clear();
    for (vector<MLBPMPtr>::const_iterator it = theBPMs->begin(); 
                                      it != theBPMs->end(); ++it) {
       if (nn >= first) {
          const MLBPMPtr abp = *it;
          bpms.addOne(abp);
          if (bpms.size() == num) return num;
       }
       nn++;
    }
    return bpms.size();
}			    

// Action routines here:  Given the state of the accelerator, 
// assuming we got beam permit for a pilot bunch, we send it. 
// Note: previous BPM data will be lost, new reading will be made available.
// We drop a virtual beam dump at zMax

void SteerUtil::sendPilotBeam(const Vector& stateIn, double zMax) {
  IncrementPulseCounter();
  Positron pilot(beamEnergyZero, stateIn);
  theCurrentBunchMomentum = beamEnergyZero;
  double s = 0;
  theBPMs->reset(); 
  for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
    ElmPtr q = *dbi; 
    q->propagate(pilot);
    s += q->Length();
//    if( 0 == strcmp( "monitor", q->Type() ) ) { 
//      Vector sTemp = pilot.getState();
//      cout << " s = " << s << " y " << sTemp[1] << " y' " << sTemp[4] 
//           << " E Ref " << pilot.ReferenceEnergy() << endl;
//    }   
    if (s > zMax) break;
  }
}
// We send multiple bunches, with beam gitter. 
bool SteerUtil::sendRealBunches(int numBunches,
                               int numParticles, double zMax, 
                               bool printDebug, int bpmStart) {

// We take the assumption that the bunch offsets mean in this context an r.m.s spread. 
  // Debugging ground motion.. opening the file  
   double YPrimeOffsetSigma = this->bunchYPrimeOffset; 
   double YOffsetSigma = this->bunchYOffset;
   double XPrimeOffsetSigma = this->bunchXPrimeOffset;
   double XOffsetSigma = this->bunchXOffset;
   double DPoPOffsetSigma = this->bunchDPoPOffset;
   theBPMs->reset(); // 
   theBPMs->setToIntegrate(); // 
   bool ok = true;
   for (int i=0; i != numBunches; i++) {
     this->bunchYPrimeOffset = YPrimeOffsetSigma*this->myGauss.getValue();
     this->bunchYOffset = YOffsetSigma*this->myGauss.getValue();
     this->bunchXPrimeOffset = XPrimeOffsetSigma*this->myGauss.getValue();
     this->bunchXOffset = XOffsetSigma*this->myGauss.getValue();
     this->bunchDPoPOffset = DPoPOffsetSigma*this->myGauss.getValue();
     if (klystronJitter > 1.0e-9) this->jitterKlystrons();
     ok = sendRealBunch(numParticles, zMax, printDebug, bpmStart, false); 
     
     if (!ok) break;
  }
  
  theBPMs->computeAverages(numBunches); 
  theBPMs->setToOverWrite(); //  Restore to default.. in case ingle bunches 
                             // are send one by one... 
  
  this->bunchYPrimeOffset = YPrimeOffsetSigma;
  this->bunchYOffset = YOffsetSigma;
  this->bunchXPrimeOffset = XPrimeOffsetSigma;
  this->bunchXOffset = XOffsetSigma;
  this->bunchDPoPOffset = DPoPOffsetSigma;
  return ok;
}
//  We send a real bunch.  We make a local copy first.. 
// Note : we simulate also the Wakefield, So, I must use my Positron bunch..

bool SteerUtil::sendRealBunch(int numParticles, double zMax, 
                               bool printDebug, int bpmStart, bool withUpload) {

// Generating the bunch   
   myPositronBunch bunch(beamEnergyZero);
  this->prepareRealBunch(numParticles, bunch, bpmStart);
 
//  if (bpmStart != 0) cerr <<  " sendRealBunch, Starting at BPM " << bpmStart << endl;
  
    if (printDebug) {
        cerr << 
	" Real Bunch at injection, Y position (microns) " << 1.0e6*bunch.getYPosition() << 
	 " X " << 1.0e6*bunch.getXPosition() << endl;
        cerr << 
	 " Y prime " << bunch.getYPrime() << " xprime " << bunch.getXPrime() << 
	 " Momentum "  << bunch.getAverageRealMomentum() << endl;
	 cerr << " bpmStart  " << bpmStart << endl;
    }
  
  
  std::ofstream *fOutYDipVal = 0;
  int firstBPMDump = 0; 
  if (displayToken.size() > 3) {
    std::cerr << " Dumping unexpected YD file.. displayToken  " 
              << displayToken << endl;
    size_t iLocD = displayToken.find("/d");
    if (iLocD != std::string::npos) { 
      std::string dipValStr = displayToken.substr(iLocD+2);
      std::istringstream dipValStrStr(dipValStr); dipValStrStr >> firstBPMDump;
      std::string fNameYDipOut(displayToken);
      std::ostringstream ofst1; ofst1 << "YD_p" << totalNumberPulses << ".txt"; 
      fNameYDipOut += ofst1.str();
      fOutYDipVal = new std::ofstream(fNameYDipOut.c_str());
      (*fOutYDipVal) << " BPM s Y DipVal P " << endl;
    }
  }

  double s = 0;
  int iBPM = 0;
  bool jump = false;
  beamline::iterator dbi = theModel->begin();
  ElmPtr q = *dbi; 
//  if (theWake->isItOn()) cerr << " Real Bunch, with wake " << endl;
//  else cerr << " Real Bunch, without wake " << endl;
 
  if (bpmStart != 0 && (!this->isBunchEmptyAtBPM(bpmStart))) {
    while(dbi != theModel->end()) {
      q = *dbi; 
      s += q->Length();
      if( MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) )  iBPM++;
      if (iBPM-1 == bpmStart) break;
      ++dbi; 
    }
    q->propagate(bunch);
    if (printDebug) cerr << " True propagation starts at s " << s << endl;
    jump = true;
    if (printDebug) cerr << " Y Position " << bunch.getYPosition() 
                   << " Average P " << bunch.getAverageRealMomentum() << endl;
  }
  // We don't reset the beamline iterator. 
  while(dbi != theModel->end()) {
    q = *dbi; 
//    if (jump && printDebug) cerr << " Jumping propagation, at s = " << s << endl;
//    if (printDebug) cerr << " At s = " << s << endl;
    q->propagate(bunch);
//    if (jump && printDebug) cerr << " Y Position " << bunch.getYPosition() 
//                   << " Average P " << bunch.getAverageRealMomentum() << endl;
    
    s += q->Length();
// Wake is now done internally...     
//    if ((theWake != 0) && (( 0 == strcmp( "rfcavity", q->Type() ) ))) 
//         theWake->DoItAll(bunch, q);
//    if (jump && ( 0 == strcmp( "rfcavity", q->Type() )))
//        cerr << " Y Position, after cavity propagation...   " << bunch.getYPosition() 
//                   << " Average P " << bunch.getAverageRealMomentum() << endl;
    if( MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) )  {
       if (printDebug) {
        cerr << 
	" At S = " << s << " Y position (microns) " << 1.0e6*bunch.getYPosition() << 
	 " X " << 1.0e6*bunch.getXPosition() << endl;
        cerr << 
	 " Y prime " << bunch.getYPrime() << " xprime " << bunch.getXPrime() << 
	 " Momentum "  << bunch.getAverageRealMomentum() << endl;
      }
//   Check for aperture cut.  Hardtype for now!  
//  Right thing to do will be use the alarm and limits sub-package.  
// 
      double yp = bunch.getYPosition();
      double ypp = bunch.getYPrime();
      double xp = bunch.getXPosition();
      double averP = bunch.getAverageRealMomentum();
      if ((fOutYDipVal != 0) && (iBPM >= firstBPMDump)) 
        (*fOutYDipVal) << iBPM << " " << s << " " << 1.0e6*yp 
	               << " " << (1.e7*this->GetOneDipoleSetting(iBPM)) 
		       << " " << averP << endl; 
      if ((yp*yp + xp*xp) > 1.225e-3) {
         std::cerr << " Beam found at BPM = " << iBPM << " at " <<  
	               1000.0*std::sqrt(yp*yp + xp*xp) << std::endl;
		       
	 std::cerr << " yp (meters) " << yp << " xp " << xp << 	       
		       " mm from nominal axis, abort propagation " << std::endl;	       
         return false; // Iris cut..
      }
      if (withUpload) this->uploadBunchAtBPM(iBPM, bunch);
      // Display part, to make dynamical movies.
      bool doDisplayDump = false; 
      if (bpmsDisplayed.size() > 0 ) { 
        for (size_t kd =0; kd != bpmsDisplayed.size(); kd++) { 
	  if (iBPM == bpmsDisplayed[kd]) doDisplayDump = true; 
          if (doDisplayDump) break;
	}
	if (doDisplayDump) { //We have a fixed scale for all the plots
	                    // Could deduce this from the nominal beam
			    // emittance, but it will be most of the time off. 
	  double bigScaleY = 1000. ; // 
	  double smallScaleY = -1.; // Variable size... 
	  double bigScaleYp = bigScaleY/50. ; // aver. 50 m beta func.
	  double smallScaleYp = 1./50.;
	  std::string ffName(displayToken);
	  ostringstream ofst2; ofst2 << "YYp_b" << iBPM; ffName += ofst2.str();
	  ostringstream ofst3; ofst3 << "_p" << totalNumberPulses; 
	  ffName += ofst3.str();
	  std::string finalToken(ffName);
	  std::string fToken("_MB"); finalToken += fToken;
	  bunch.dumpItForMovie(finalToken, bigScaleY, bigScaleYp, 250, 250);
          finalToken = ffName;
	  fToken = "_MS"; finalToken += fToken;
	  double yScale = 
	     bunch.dumpItForMovie(finalToken, smallScaleY, smallScaleYp, 250, 250);
	  this->updateFileOutMovieScale ( iBPM, 1.0e6*yp, 1.0e6*ypp, yScale);
	} 
      }
      iBPM++;
    }
    dbi++;  
    if (s > zMax) break;
  }
  
  if (fOutYDipVal != 0) fOutYDipVal->close();
  delete fOutYDipVal;
  return true;
/*
  for (vector<MLBPM*>::const_iterator it = theBPMs->begin(); 
                                      it != theBPMs->end(); ++it) {
	cerr << " BPM  " <<  (*it)->GetId() << " address " << (*it) 
	     << " reading " << (*it)->GetMeasurement() << endl;			       
  }
*/
}

void SteerUtil::prepareRealBunch(int numParticles, 
                                   myPositronBunch& bunch, int aBPMStart) {

// Generating the bunch   
  IncrementPulseCounter();
  bunch.setNominalEnergy(beamEnergyZero);
  
// Use the store beam... if it exist.... Otherwise, generate the bunch..
// Note: this last check is actually needed, the response matrix don't know if there are 
// appropriate loaded bunch or not.  
  if (aBPMStart != 0 && (!this->isBunchEmptyAtBPM(aBPMStart))) {
    this->downloadBunchAtBPM (aBPMStart, bunch);
    if (wakeIsOn) {
       bunch.setOriginalBunchCharge(2.0e10);  // for my old wake
       bunch.setIntensity(2.0e10); // new wake from CHEF
       for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
	  ElmPtr q = *dbi;
          if(LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (*dbi)) {
	  //  cavPtr->setWakeOn(true);
	     if (cavPtr->wakeOn()) std::cerr << " Wake is On !!! " <<  endl;
	     else std::cerr << " Wake is Off !!! " <<  endl;
	  }
       }    
    } else {
       bunch.setIntensity(2.0e6); // new wake from CHEF 
/*
       for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
	  ElmPtr q = *dbi;
          if(LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (*dbi)) 
	    cavPtr->setWakeOn(false);
       }    
*/
    }
    return;
  }
  if (twissIn.defined) 
    bunch.generate(twissIn, numParticles, bunchEpsilX, bunchEpsilY, 
                      bunchSigmaZ, bunchDPoP);
  else 
    bunch.generate(bunchMatch, numParticles, bunchEpsilX, bunchEpsilY, 
                      bunchSigmaZ, bunchDPoP);
  bunch.setOriginalBunchCharge(2.0e10);
  if (wakeIsOn) {
     bunch.setOriginalBunchCharge(2.0e10);  // for my old wake
     bunch.setIntensity(2.0e10); // new wake from CHEF 
/*
       for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
	  ElmPtr q = *dbi;
          if(LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (*dbi)) {
	  //  cavPtr->setWakeOn(true);
//	     if (cavPtr->wakeOn()) std::cerr << " Wake is On !!! " <<  endl;
//	     else std::cerr << " Wake is Off !!! " <<  endl;
	  }
       }    
*/
  } else {
     bunch.setOriginalBunchCharge(2.0e6);  // for my old wake
     bunch.setIntensity(2.0e6); // new wake from CHEF 
  }
  
   
  Vector displacement(6);
  Vector stateInInj = this->getStateAtInjection(); 
  
  displacement(0) = bunchXOffset;
  displacement(1) = bunchYOffset;
  displacement(2) = 0.;  // no jitter in ct, does not make much sense... 
  displacement(3) = bunchXPrimeOffset;
  displacement(4) = bunchYPrimeOffset;
  displacement(5) = bunchDPoPOffset;
  if (aBPMStart == 0) {  // True only for Kirti's lattice 
   displacement(1) += stateInInj[1];
   displacement(4) += stateInInj[4];
  }
  if (atOffEnergy) { 
    // Take into account the reduced energy for the Bunch Compressor. 
    displacement(5) += bunchDPoPOffsetBC;
    // And match it
    if (twissIn.defined) { 
      displacement(1) += twissIn.DY * displacement(5);
      displacement(4) += twissIn.DYp * displacement(5);
//      cerr <<  " Prepare Real Bunch, Displaced bunch in Y = " << displacement(1) << endl;
//      cerr <<  " ......... in Yp = " << displacement(4) << endl;
    }
  }
  bunch.shiftCenter(displacement);
  
}

void SteerUtil::setLinacGradient(double factor) {
  size_t iCav = 0;
//  std::cerr << " SetLinacGradient " << endl;
//  for (std::vector<LinacCavityPtr>::iterator itCav=allCavities.begin(); 
//            itCav != allCavities.end(); itCav++) {
  for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
	ElmPtr q = *dbi;
        if(LinacCavityPtr monPtr = boost::dynamic_pointer_cast<LinacCavity> (q)) {
	   
//	double v = factor*nominalCavityVoltage[iCav];
	double vv = q->Strength();    
//	(*itCav)->setStrength(1.0e9*vv*factor);
	q->setStrength(1.0e9*vv*factor);
//	if (iCav < 200) std::cerr << " Nom " << nominalCavityVoltage[iCav] 
//	               << " current " << vv <<  " new " << (*itCav)->Strength() << endl;
//	(*itCav)->setStrength(v);
	iCav++;   
     } 
  }
  // Reset also the Bunch Compressor to it's full energy. 
  // we do this by altering the state of the injected bunch
  //bunchDPoPOffsetBC = 0.;
  // Not here !.... This was a bug!.... 
}

void SteerUtil::resetLinacGradient(){  // Set it back to nominal 

   atOffEnergy = false;
   size_t iCav = 0;	
  for (std::vector<LinacCavityPtr>::iterator itCav=allCavities.begin(); 
            itCav != allCavities.end(); itCav++) {
       (*itCav)->setStrength(1.0e9*nominalCavityVoltage[iCav]);
//       (*itCav)->setStrength(nominalCavityVoltage[iCav]);
       (*itCav)->setPhi(nominalCavityPhase[iCav]); 
     iCav++;
  }
  // Reset also the Bunch Compressor to it's full energy. 
  // we do this by altering the state of the injected bunch
  bunchDPoPOffsetBC = 0.;
}
//
// Mostly relevant in the context of Benchmark 2, when we were extremly confused about 
// the use of a Proton instead of a positron to registering the beamline.. 
// No longer expeted to be relevant.  
//
void SteerUtil::SetAllQuadStrengthByFixedOpticalK(std::vector<double>& kStrengthIn) {


  if (refPQuad.size() != quadList.size()) {
    cerr << " Inconsistency in SteetUtil, SetAllQuadStrengthByFixedOpticalK "
         << endl;    
  }
  size_t iQq = 0;
  for (size_t iQPtr = 0; iQPtr != quadList.size(); iQPtr++) {
    double strength = quadList[iQPtr]->Strength();
    std::cerr << " Quad " << iQq 
              << " refPQuad " << 
	      refPQuad[iQPtr] << 
	      " kStrengthIn " << kStrengthIn[iQPtr]*refBRhoQuad[iQPtr]*(1.0e-9*PH_MKS_c)/5. << 
	      " kStrengthCHEF " << quadList[iQPtr]->Strength() << std::endl;
      double newStrength = kStrengthIn[iQPtr]*refBRhoQuad[iQPtr]*(1.0e-9*PH_MKS_c)/beamEnergyZero;
//      quadList[iQPtr]->setStrength(newStrength);
      
    iQq++;
  }

}

void SteerUtil::clearAllStoredBunches() {
  
 for (int ib=0; ib != pilotBunches.size(); ib++) {
   if (pilotBunches[ib]->size() > 2) {
      double p = pilotBunches[ib]->getReferenceMomentum();
      delete pilotBunches[ib];
      pilotBunches[ib] = new myPositronBunch(p);
   }   
 }  
 for (int ib=0; ib != pilotBunchesRM.size(); ib++) 
   if (pilotBunchesRM[ib]->size() > 2) {
      double p = pilotBunchesRM[ib]->getReferenceMomentum();
      delete pilotBunchesRM[ib];
      pilotBunchesRM[ib] = new myPositronBunch(p);
   }   
 for (int ib=0; ib != pilotBunchesOffE.size(); ib++) 
   if (pilotBunchesOffE[ib]->size() > 2) {
      double p = pilotBunchesOffE[ib]->getReferenceMomentum();
      delete pilotBunchesOffE[ib];
      pilotBunchesOffE[ib] = new myPositronBunch(p);
   }   
 for (int ib=0; ib != pilotBunchesRMOffE.size(); ib++) 
   if (pilotBunchesRMOffE[ib]->size() > 2) {
      double p = pilotBunchesRMOffE[ib]->getReferenceMomentum();
      delete pilotBunchesRMOffE[ib];
      pilotBunchesRMOffE[ib] = new myPositronBunch(p);
   }   
} 

void SteerUtil::uploadRMBunchAtBPM (int aBPM, myPositronBunch& abunch) {
  myPositronBunch* to = 0;
  if (!this->atOffEnergy) { 
    if ((aBPM < 0) || (aBPM >= pilotBunchesRM.size())) return; 
    to = pilotBunchesRM[aBPM];
   } else {
    if ((aBPM < 0) || (aBPM >= pilotBunchesRMOffE.size())) return; 
    to = pilotBunchesRMOffE[aBPM];
   }
  if (to == 0) return;
  to->clear();
/*
  delete to;
  to = new myPositronBunch(abunch.getNominalEnergy());
  if (!this->atOffEnergy) { 
    pilotBunchesRM[aBPM] = to;
  } else {
    pilotBunchesRMOffE[aBPM] = to;;
  }
*/
  double nominalE = 0.;
  for (ParticleBunch::const_iterator pPtr = abunch.begin(); 
	        pPtr != abunch.end(); ++pPtr) {
    Positron copyP(pPtr->ReferenceEnergy(), pPtr->State());
    to->append(copyP); 
    if (nominalE < 1.e-9) {
      double nominalP = copyP.ReferenceMomentum();
      nominalE = std::sqrt(PH_NORM_me*PH_NORM_me + nominalP*nominalP);
    }
  }
  to->setNominalEnergy(nominalE);
  to->setIntensity(abunch.Intensity());     
}
void SteerUtil::uploadBunchAtBPM (int aBPM, myPositronBunch& abunch) {
  myPositronBunch* to = 0;
  if (!this->atOffEnergy) { 
    if ((aBPM < 0) || (aBPM >= pilotBunches.size())) return; 
    to = pilotBunches[aBPM];
  } else {
    if ((aBPM < 0) || (aBPM >= pilotBunchesOffE.size())) return; 
    to = pilotBunchesOffE[aBPM];
  }
  if (to == 0) return;
  to->clear();
/*
  delete to;
  to = new myPositronBunch(abunch.getNominalEnergy());
  if (!this->atOffEnergy) { 
    pilotBunches[aBPM] = to;
  } else {
    pilotBunchesOffE[aBPM] = to;;
  }
*/
  double nominalE = 0.;
  for (ParticleBunch::const_iterator pPtr = abunch.begin(); 
	        pPtr != abunch.end(); ++pPtr) {
    Positron copyP(pPtr->ReferenceEnergy(), pPtr->State());
//    cerr << " Positron at y = " << copyP.get_y() << " y prime " << copyP.get_npy() << endl;
    to->append(copyP); 
    if (nominalE < 1.e-9) {
      double nominalP = copyP.ReferenceMomentum();
      nominalE = std::sqrt(PH_NORM_me*PH_NORM_me + nominalP*nominalP);
    }
  }  
  to->setNominalEnergy(nominalE);
//  cerr << " Uploaded bunch with " << to->size() << " particles" 
//       << " Nominal E " << nominalE << endl;
  to->setIntensity(abunch.Intensity());     
}
void SteerUtil::downloadRMBunchAtBPM (int aBPM,  myPositronBunch& abunch) const {

  myPositronBunch* from = 0;
  if (!this->atOffEnergy) { 
    if ((aBPM < 0) || (aBPM >= pilotBunchesRM.size())) return; 
    from = pilotBunchesRM[aBPM];
  } else { 
    if ((aBPM < 0) || (aBPM >= pilotBunchesRMOffE.size())) return; 
    from = pilotBunchesRMOffE[aBPM];
  }
  if (from == 0) return;
  abunch.clear();
  double nominalE = 0.;
  for (ParticleBunch::const_iterator pPtr = from->begin(); 
	        pPtr != from->end(); ++pPtr) {
    Positron copyP(pPtr->ReferenceEnergy(), pPtr->State());
    abunch.append(copyP); 
    if (nominalE < 1.e-9) {
      double nominalP = copyP.ReferenceMomentum();
      nominalE = std::sqrt(PH_NORM_me*PH_NORM_me + nominalP*nominalP);
    }
  }
  abunch.setNominalEnergy(nominalE);
  abunch.setNumMacroParticleAtCreation(from->size());
  abunch.setIntensity(from->Intensity());
//  cerr << " Downloaded bunch with " << abunch.size() << " particles" 
//       << " Nominal E " << nominalE << endl;

}
void SteerUtil::downloadBunchAtBPM (int aBPM,  myPositronBunch& abunch) const {

  myPositronBunch* from = 0;
  if (!this->atOffEnergy) { 
    if ((aBPM < 0) || (aBPM >= pilotBunches.size())) return; 
    from = pilotBunches[aBPM];
  } else { 
    if ((aBPM < 0) || (aBPM >= pilotBunchesOffE.size())) return; 
    from = pilotBunchesOffE[aBPM];
  }
  if (from == 0) return;
//   cerr << " ...with "<< from->size() << " particles" << endl;
  if (from->size() == 0) {
    cerr << " SteerUtil::downloadBunchAtBPM, " << aBPM << " no particle in buffer !!! " << endl;
    exit(2);
  }
  abunch.clear();
  double nominalE = 0.;
  for (ParticleBunch::const_iterator pPtr = from->begin(); 
	        pPtr != from->end(); ++pPtr) {
    Positron copyP(pPtr->ReferenceEnergy(), pPtr->State());
//    cerr << " Positron at y = " << copyP.get_y() << " y prime " << copyP.get_npy() << 
//            " Reference momentum " << copyP.ReferenceMomentum() << endl;
    abunch.append(copyP); 
    if (nominalE < 1.e-9) {
      double nominalP = copyP.ReferenceMomentum();
      nominalE = std::sqrt(PH_NORM_me*PH_NORM_me + nominalP*nominalP);
    }
  }
  abunch.setNominalEnergy(nominalE);
  abunch.setNumMacroParticleAtCreation(from->size());
  abunch.setIntensity(from->Intensity());
//   cerr << " downloaded bunch with " << abunch.size() << " particles" << endl;
}

// Temporary fix for the tilt cavity inconsistency, 
// Merlin/


void SteerUtil::correctVerticalOffsetInCav(const ElmPtr q, Particle *ptr) {
   // Get the tilt angle...
   LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity>(q);
   if (!aCav) return;    
   
   double tilt = this->getCavityPitch(aCav);
   double EField = q->Strength();
   double deltaPy = EField*tilt*q->Length()*std::sin(aCav->getPhi());
   double dy = deltaPy*q->Length()/(ptr->Momentum() * 4.); 
      // Don't ask why I divide by 4 !!!
      // and yes, I am aware ofthe difference between Pz and P. 
      // correction (to be done ) on top of corrections..
   std::cerr << " correctVerticalOffsetInCav, tilt " << tilt << 
             " Efield " << EField << " dy " << dy << endl;
   double y = ptr->get_y() + dy;
   ptr->set_y(y);

}
				  
void SteerUtil::correctVerticalOffsetInCav(const ElmPtr q, 
                                              myPositronBunch &aBunch) {
   // Get the tilt angle...
   LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity>(q);
   if (!aCav) return;    
   double tilt = this->getCavityPitch(aCav);
   if (std::abs(tilt) < 1.0e-9) return;
   double EField = q->Strength();
   double deltaPy = EField*tilt*q->Length()*std::sin(aCav->getPhi());
   for (ParticleBunch::iterator ptr = aBunch.begin(); 
	        ptr != aBunch.end(); ++ptr) {
     double dy = deltaPy*q->Length()/(ptr->Momentum() * 4.); 
      // Don't ask why I divide by 4 !!!
      // and yes, I am aware ofthe difference between Pz and P. 
      // correction (to be done ) on top of corrections..
//     std::cerr << " correctVerticalOffsetInCav, tilt " << tilt << 
//             " Efield " << EField << " dy " << dy << endl;
     double y = ptr->get_y() + dy;
     ptr->set_y(y);
   }

}				  


// To be upgraded to R, if need be.. 

#ifdef USEROOT

 void SteerUtil::PlotDipoleSettingvsQuadYTrans(
      const std::string& label, size_t firstDipole, size_t lastDipole) {
 
   size_t nPts = (size_t) lastDipole - firstDipole;   
   double xx[nPts];
   double yy[nPts];
   
   for (size_t iDip = firstDipole; iDip != lastDipole; ++iDip) { 
     size_t i = iDip - firstDipole;
     yy[i] = GetOneDipoleSetting( iDip);
     xx[i] = GetOneQuadrupoleYTrans(iDip-1);
  }
  TCanvas* aCanvas = new TCanvas("BPM-Merlin Plots ", "Merlin Plots");
  TGraph *tg = new TGraph(nPts,xx, yy);
  std::string title(" Dipole Setting vs Quad Offset, - ");
  title += label;
  aCanvas->Clear();
  tg->SetTitle(title.c_str());
  tg->SetMarkerColor(2);
  tg->SetMarkerStyle(20);
  tg->Draw("AP");
  aCanvas->Modified();
  aCanvas->Update();
  std::string fName("DipSetvsQuad-");
  fName += label;
  fName += ".eps";
  aCanvas->Print(fName.c_str());
  delete tg;
  delete aCanvas;
}

#endif
