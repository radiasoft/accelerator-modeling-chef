#include <cmath>
#include <iostream>
#include <fstream>
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"
#include <time.h>

// file SteerOnetoOne
// 
// Part of a collection of algorithm, using SteerUitl and 
// SteeringResponseMatrix 
//

// Most simplistic, assume same number of dipoles and BPMs
// assume we have a valid SteerUtil instance, i.e., we can control the Linac  

using std::cerr;
using std::endl;
using std::cout;
using std::ofstream;
using std::ifstream;

Steering::DMSPlan::DMSPlan():
firstDipole(0),
numDipoles(20), // reasonable of sub-section.
numDipoleTailRej(0), // Last one not always rejected. 
numCavitiesPerKlystrons(24), 
                              // 3 cryo-modules per r.f. units, 8 cav. per cryo-modules
			      // or, 12 cavity cryo-module, every two module. 
firstCavity(0),
currentMacroIteration(0),
numKlystrons(3), 
gradientFraction(0.975),  // for curved Linac.
numPseudoParticles(1000), // Number of particles while tracking. 
numPseudoPartRP(1000), // Number of particles while tracking, computing Response Matrix 
doLastSegment(false),
preSteer(false),
rescaleLattice(false),
byRFPhase(false),
minChangeDipole(2.50e-5),
gainChangeDipole(0.50),
minWeightSVD(0.001),
weight1to1DMS(0.07),
weightAbsolute(0.00333),    // Turn this off... or not ??  
weightDMS(0.07),         
weightCorr(1.0e-10), 
numMaxLocalIteration(25),
macroNumMacroIter(1),
macroFirstDipole(0),
macroNumDipoles(20), // The number of dipole ( or with above, the length of
	    // of the section to DFS Steer
macroNumDipIncrement(macroNumDipoles/2),
macroNumExtraBPMs(0),
macroNumKlystrons(3),
macroGradientFraction(0.975),  // Fraction of the nominal gradient 
scaleResponseMatrix(1.0),  // Fraction of the nominal gradient 
usePerfectResponse(true),
skipInitialPerfMeas(true),
useCryoTilt(false),
usePreSteer(false),
useDFSMerlin(true),
useReducedFieldInSteeredSection(true),
useReducedFieldInBunchCompressor(true),
usePerfectDispKnow(true),
doDispersionBump(false),
doDynamicalPrelim(false), // Used in SteeingWeightedDispersion and PitchModule
maxNumPulseTotal(9999999), // No cuts.. on total number of pulses. 
                           // Note: on the farm doing 150 pulses an hour..
maxNumSecondsTotal(172800), // Two days max.. 
numPulseBetweenLocalIteration(0), // Wait, simulated tuning process 
numPulseBetweenGlobalIteration(1*5*60), // Wait, simulated tuning process 
doMovie(false),
responsesPerfect(0) 
{ }

// Assuming we have a perfect lattice, and run plan on how to define the sections, 
// number of BPMs, and so forth, we can pre-compute these matrices for 

void Steering::DMSPlan::fillResponsesPerfect(std::string& fName, std::string& labelSteerG, 
                                               bool fromFile) {

  if (theSteerUtil == 0) {
    cerr << " No Steering utility activated... must be defined to proceed..." << endl;
    exit(2);
  }
  size_t numTotalDip = theSteerUtil->GetNumCorrectorDipoles();
  size_t numTotalBPMs = theSteerUtil->GetNumBPMs();
  responsesPerfect.clear();
  theSteerUtil->ResetPulseCounter();
  std::ostringstream labelG;
  firstDipole = macroFirstDipole;
  numDipoles = macroNumDipoles;  // fixed, part of the global plan. 
  numExtraBPMs = macroNumExtraBPMs; // Two extra BPMs 
  bool dipoleLeft = (firstDipole + numDipoles < (numTotalDip - 1 - numExtraBPMs));
  firstCavity = 0; // Always starting at the beginning of the Linac.
  numKlystrons = macroNumKlystrons; // Number of klystrons at reduced voltage per local iteration,
  if (useReducedFieldInBunchCompressor && (macroFirstDipole == 0))
      numKlystrons = 0;
  gradientFraction = macroGradientFraction;
  size_t iterLocal = 0;
  theSteerUtil->clearAllStoredBunches();
  // Assume the user kept track of the file correctly. Major disaster if not...  
  if (fromFile)  {
    ifstream fIn(fName.c_str());
    Matrix mct;
    int fdip=0;
    int nCols = 0;
    int nRows = 0;
    while (dipoleLeft) {
      fIn >> fdip >> nCols >> nRows;
      Matrix mct(nRows, nCols);
      for (int i = 0; i != nRows; i++) {
        for (int j = 0; j != nCols; j++) fIn >> mct(i,j);
      }
      responsesPerfect.push_back(mct);
      if ((!useDFSMerlin) &&  (weight1to1DMS > 1.e-15)) {
         fIn >> fdip >> nCols >> nRows;
         Matrix mct(nRows, nCols);
         for (int i = 0; i != nRows; i++) {
           for (int j = 0; j != nCols; j++) fIn >> mct(i,j);
         }
         respPerfectOneToOne.push_back(mct);
      }
      firstDipole += macroNumDipIncrement;
      dipoleLeft = (firstDipole + numDipoles < (numTotalDip - 1 - numExtraBPMs));
/*
      if (firstDipole == 5) { // debugging.. 
        cerr << " Response Matrix for First Dipole 5 " << endl;
        
        for (int i = 0; i != mct.rows(); i++) {
          for (int j = 0; j != mct.cols(); j++) cerr << " " << mct(i,j);
	  cerr << endl;
        }
      
      }
*/
      numKlystrons += macroNumDipIncrement; 
      dipoleLeft = IsDipoleLeftAndUpatePlanAtEnd(this); // might change the local plan if at end 
   }
    fIn.close();
    cerr << " Read Response Matrics from file " << fName << endl;
    return;
  }
  
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) ilcTestDataChr = (char *) defDataDir.c_str();    
  std::string topDir(ilcTestDataChr);
  topDir += std::string("/Test4/");
//  std::ostringstream gItt; gItt << topDir <<  
//                                  << labelSteerG << "/R00/";
  std::string fOutDirPath(topDir);
//  std::string cmd("/bin/mkdir "); cmd += fOutDirPath;
//  system(cmd.c_str());
  ofstream* fOut = 0;
  if ((!fromFile) && (fName.size() > 3)) fOut = new ofstream(fName.c_str());
  size_t kDip = 0;
  while (dipoleLeft) {
       std::string topDir(ilcTestDataChr);
       topDir += std::string("/Test4/");
//        std::ostringstream gIttDip; gIttDip << topDir 
//             << labelSteerG << "/R00/" << "/d" << firstDipole << "/";
       std::string fOutDirPathDip(topDir);     
//       std::string cmd =  "/bin/mkdir ";  cmd += fOutDirPathDip; 
//       system(cmd.c_str());
//       theSteerUtil->setDisplayToken(fOutDirPathDip);
       if (firstDipole + numDipoles + numExtraBPMs > numTotalBPMs) 
           numExtraBPMs = numTotalBPMs - (firstDipole + numDipoles - 1);
       rescaleLattice = false;
       byRFPhase = false;
       std::string labelMat("tmp");    
       size_t numCavTmp = numKlystrons * numCavitiesPerKlystrons;
       size_t numBPMs = numDipoles + numExtraBPMs;  // NOT square matrix problem. 
       if ((firstDipole + numDipoles) == (theSteerUtil->GetNumCorrectorDipoles()-2))
          numBPMs = numDipoles;
       // Define the initial state from the Bunch Compressor. 
       // Valid only for the Main Linac !!!! 
       Vector stateInBC(6);
       Vector stateInSinglePart = theSteerUtil->getStateAtInjection();
       for (size_t k=0; k != 6; k++) stateInBC[k] = 0.;
       if (useReducedFieldInBunchCompressor) {
          stateInBC[5] = gradientFraction - 1.0;
	  stateInBC[1] += theSteerUtil->getInputVerticalDispersion() * stateInBC[5];
          stateInBC[4] += theSteerUtil->getInputVerticalDispersionPrime() * stateInBC[5];
	  stateInBC[1] += stateInSinglePart[1]; // Kirti/Pt offset... 
	  stateInBC[4] += stateInSinglePart[4]; // Kirti/Pt offset... 
       }
       // Let us ge the matrix..
//       cerr << " Loading Perfect Matrix  at Dipole " << firstDipole << " NumBPMs " << numBPMs << endl;
       if (!useDFSMerlin)  {
           Steering::ResponseMatrix<Steering::ECHANGE> m0tmp(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
				     stateInBC,
                                     (firstDipole+1), numBPMs, gradientFraction,
				     firstCavity, numCavTmp,
				     firstDipole, numDipoles,
				     rescaleLattice, byRFPhase,
				     0, scaleResponseMatrix, 1000);
				        // hard type constant.. to be improved... 
         // Not sure if I have to make a copy... Just do it to play safe. 
         if (fOut != 0) { 
            fOut->precision(15);
            (*fOut) << firstDipole << " " << m0tmp.cols() << " " << m0tmp.rows() << endl;
            for (int i = 0; i != m0tmp.rows(); i++) {
              for (int j = 0; j != m0tmp.cols(); j++) (*fOut) << " " << m0tmp(i,j);
	      (*fOut) << endl;
           }
         }
         responsesPerfect.push_back(m0tmp);
       // Mixed One to one... 
         if (weight1to1DMS > 1.e-15) {
           m0tmp.setEnergy(false, false); // Reset Linac 
	   Steering::ResponseMatrix<Steering::CORDIP> m0tmp2(labelMat, 
	                             theSteerUtil->getTheModelPtr(), 
                                    (firstDipole+1), numBPMs, firstDipole, numDipoles);

           if (fOut != 0) { 
             fOut->precision(15);
             (*fOut) << firstDipole << " " << m0tmp2.cols() << " " << m0tmp2.rows() << endl;
             for (int i = 0; i != m0tmp2.rows(); i++) {
             for (int j = 0; j != m0tmp2.cols(); j++) (*fOut) << " " << m0tmp2(i,j);
	       (*fOut) << endl;
           }
          }
          respPerfectOneToOne.push_back(m0tmp2);
	 }
       } else { // Merlin matrixes, pre inverted.
//	   int numPulsesRespFunc = 1000; // default, sensible.. 
	   int numPulsesRespFunc = 1;
//	   std::cerr << "  Pure DMS loaded !!!! " << std::endl;
//           Steering::ResponseMatrix<Steering::ECHANGEPUREDMS> m0tmp(labelMat, 
           Steering::ResponseMatrix<Steering::ECHANGEMERLIN> m0tmp(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
				     stateInBC,
                                     (firstDipole+1), numBPMs, gradientFraction,
				     firstCavity, numCavTmp,
				     firstDipole, numDipoles,
				     weightAbsolute, weightDMS, weightCorr,
				     byRFPhase,
				     0, scaleResponseMatrix, numPulsesRespFunc,
				     usePerfectDispKnow);				     
         // Not sure if I have to make a copy... Just do it to play safe. 
         if (fOut != 0) { 
            fOut->precision(15);
            (*fOut) << firstDipole << " " << m0tmp.cols() << " " << m0tmp.rows() << endl;
            for (int i = 0; i != m0tmp.rows(); i++) {
              for (int j = 0; j != m0tmp.cols(); j++) (*fOut) << " " << m0tmp(i,j);
	      (*fOut) << endl;
           }
         }
         responsesPerfect.push_back(m0tmp);
	 
       }
       // Pust some nominal perfect bunches up to this point.. 
       //
       // I don't see the point of doing this any longer. 
       // Bunch savings occurs after steering through one section..
       //  
       /*
       ???????.setEnergy(false, true);
	 // We send one more bunch, and this time we load this good beam, 
	 // as the beam line is tuned up to some point... 
       MLBPMs bpmsTmp; // just to get a local copy
       int numBPMsTmp = numBPMs+1;
       if (numBPMsTmp > theSteerUtil->GetNumBPMs()) numBPMsTmp = theSteerUtil->GetNumBPMs();
       theSteerUtil->getBPMPtrs(bpmsTmp, firstDipole, numBPMsTmp); // 
       double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
       // Keep only the upstream sub-section
       int bpmStartThisIt = firstDipole - 2;
       int bpmStartThisItOffE = firstDipole - 3;
       if (bpmStartThisIt < 0) bpmStartThisIt = 0;
       if (bpmStartThisItOffE < 0) bpmStartThisItOffE = 0;
	      
       theSteerUtil->sendRealBunch(1000, zMax, 
	                                 false, bpmStartThisItOffE, true);
       responsesPerfect[kDip].setEnergy(false, false);
	 // We send one more bunch, and this time we load this good beam, 
	 // as the beam line is tuned up to some point...
       theSteerUtil->sendRealBunch(1000, zMax, 
	                                 false, bpmStartThisIt, true);
       */
       firstDipole += macroNumDipIncrement;
       numKlystrons += macroNumDipIncrement; 
       if (this->usePreSteer) {
         this->numKlystrons = this->firstDipole - 2;
       }
      dipoleLeft = IsDipoleLeftAndUpatePlanAtEnd(this); // might change the local plan if at end 
       kDip++;
                          // Keep the entie Linac down when running off-energy. 
                        // Assume one klystron per dipole
  } // Looping dipole by dipole...
  theSteerUtil->ResetPulseCounter(); 
  theSteerUtil->clearAllStoredBunches();
  if (fOut != 0) {
     fOut->close();
     delete fOut;
  }
   
}

void Steering::DMSPlan::printPlan() const {
  cout << endl;
  cout << " ---- Plan for Dispersion Matched Steering  -- " << endl;
  cout << " Macro Part .. " << endl;
  
  cout << " Number of Macro Iterations " << macroNumMacroIter << endl;
  cout << " firstDipole " <<  macroFirstDipole << " numDipoles " <<  macroNumDipoles 
       <<  " Extra BPMs " << macroNumExtraBPMs << endl;
  cout << " Nominal number of Kylstron modified per section " << macroNumKlystrons << 
          " Gradient Fraction " << macroGradientFraction << endl;
  cout << " At beginning, first local part .. " << endl;
  cout << " firstDipole " <<  macroFirstDipole << " numDipoles " 
        << macroNumDipoles << " Increment " << macroNumDipIncrement
	<< " Tail rej. " << numDipoleTailRej << endl; 
  if (useCryoTilt) cout << " Allowing for cryo tilting " << endl;
  else 	cout << " Cryo module can not be tilted " << endl;
  if (usePreSteer) cout << " Pre-steering to orbit before DMS  " << endl;
  cout << " Relative weight between 1 to 1 steering and DMS " << weight1to1DMS << endl;
  if (useDFSMerlin) {
      cout << " Using combined weight in the Response Matrix, ala Merlin " << endl;
      cout << " Weight Absolute " << weightAbsolute << " Weight DFS " 
           << weightDMS << " Corrector " << weightCorr << endl;
      if (useReducedFieldInSteeredSection) 
         cout << " Using using Reduced field in the section being steered " << endl;
      } else {
         cout << " Relative weight between 1 to 1 steering and DMS " << weight1to1DMS << endl;
  }
  cout << " Details.. " << endl;
  cout << " Minimum Weight for SVD pseudoInversion " << minWeightSVD << endl;
  cout << " numCavitiesPerKlystrons " << numCavitiesPerKlystrons << 
          " firstCavity " << firstCavity << " numKlystrons " << numKlystrons << endl;
  cout << " Local Gradient Fraction " << gradientFraction <<  
          " numPseudoParticles " << numPseudoParticles 
	  << " in Response Matrix " << numPseudoPartRP << endl;
  if (!rescaleLattice) cout << " Not rescaling the lattice for off-energy " << endl;
  else cout << " !!! rescaling the lattice for off-energy " << endl;
  if (byRFPhase) cout << " Changing rf phase instead of voltage  " << endl;
  else cout << " Reducing  voltage, with fixed r.f. phase when measuring dispersion " << endl;
  cout << " minChangeDipole " << minChangeDipole << " gainChangeDipole " << 
        gainChangeDipole << " numMaxLocalIteration " << numMaxLocalIteration << endl;
  if (usePerfectResponse) cout 
      << "  Using Response Matrices from perfectly aligned lattice " << endl;
  else cout << " Re-measuring Response matrices at each local iteration " << endl;
  if (useReducedFieldInBunchCompressor) cout << " Reducing Energy in Bunch Compressor" << endl;
  if (usePerfectDispKnow) cout << " Dispersion measured from Perfect Lattice is used." << endl;
  cout << " Scale for Response Matrix " << scaleResponseMatrix << endl; 
  cout << " ------------------------------ " << endl;

}

// At the end of the channel, we want to modify the plan to include the last section, 
// which has a different length. 

bool Steering::IsDipoleLeftAndUpatePlanAtEnd(DMSPlan* aPlan) { // The plan will change ! 


//  cout << " Steering::IsDipoleLeftAndUpatePlanAtEnd: Reaching the end of lattice " << endl;
//  cout << " Before, first dip " <<  aPlan->firstDipole 
//	         <<  " num dip " << aPlan->numDipoles   << endl;   
  size_t numTotalDip = theSteerUtil->GetNumCorrectorDipoles();
  int lastDipole = aPlan->firstDipole + aPlan->numDipoles;
  // very ugly... 
  if (lastDipole == (numTotalDip - 2 + aPlan->macroNumDipIncrement)) return false;
  if (lastDipole < (numTotalDip-1)) return true;
  int firstDipoleOver = aPlan->firstDipole;
  aPlan->firstDipole = numTotalDip - aPlan->macroNumDipoles;
  aPlan->numDipoles = numTotalDip - aPlan->firstDipole - 2;
  int deltaDip =  firstDipoleOver - aPlan->firstDipole;
	    // Back the On-Off klystron pointer.. 
  if (aPlan->numCavitiesPerKlystrons == 24) 
              aPlan->numKlystrons -= deltaDip; 
  else if (aPlan->numCavitiesPerKlystrons == 8)
              aPlan->numKlystrons -= 3 * deltaDip; 
  // Now update the the number of BPM.. 
  aPlan->numExtraBPMs = 0;
  aPlan->doLastSegment = true;
//  cout << " After, first dip " <<  aPlan->firstDipole 
//	         <<  " num dip " << aPlan->numDipoles  << " delta Dip " <<  deltaDip << endl;   

  return true;
}
Steering::localIterData::localIterData(const DMSPlan& aPlan, int it):
iterNum(it),
dipValues(aPlan.numDipoles),
suggestedChanges(aPlan.numDipoles),
isOK(aPlan.numDipoles),
actualChanges(aPlan.numDipoles)
{
  for (size_t i=0; i != aPlan.numDipoles; i++) {
    size_t iDip = aPlan.firstDipole + i;
    dipValues[i] = theSteerUtil->GetOneDipoleSetting(iDip);
    isOK[i] = true;
    suggestedChanges[i] = 0.;
    actualChanges[i] = 0.;
  }
}

bool Steering::localIterData::areWeDone(const DMSPlan& aPlan, bool withPrint) const {
      double maxDipChange = -1.;
      size_t iMax = 0;
      size_t numMaxDipChange = aPlan.numDipoles - aPlan.numDipoleTailRej;
      for (size_t i=0; i != numMaxDipChange; ++i) {
        if (!isOK[i]) continue;
        double absDipChange =  std::abs(suggestedChanges[i]);
	if (absDipChange > maxDipChange)  {
	    maxDipChange = absDipChange;
	    iMax = i;
	}
      }
      maxDipChange *= theSteerUtil->getRefMomentumAtDipole(0);
      maxDipChange /= theSteerUtil->getRefMomentumAtDipole(iMax+aPlan.firstDipole);
      if (withPrint) cout << " localIterData::areWeDone, it = " << iterNum 
           << " Relevant Maximum change " << maxDipChange << " At dipole " << 
          iMax+aPlan.firstDipole << " At P = " << 
	  theSteerUtil->getRefMomentumAtDipole(iMax+aPlan.firstDipole) << " GeV/c " << endl;
      return (maxDipChange < aPlan.minChangeDipole);
}
//
// Looking back at history...appraise the change, decide if we have to change 
// the strategy. If a change is suspicious, we don't do it.  If an early dipole becomes 
// suspicious, turn it off temporarely fo rthe duration of the sector iteration. 
// 
bool Steering::localIterData::appraise(const DMSPlan& aPlan,
                const std::vector<localIterData> &itPrevious) {
   if (itPrevious.size() < 2) return true; // no changes, too early to tell		
//   cerr << " In localIterData::appraise, on " << itPrevious.size() << " iterations " << endl;
   size_t numBadVariables = 0;
   size_t numMaxDipChange = aPlan.numDipoles - aPlan.numDipoleTailRej;
   for (size_t id=0; id != numMaxDipChange; id++) {
      double absDipChange =  std::abs(suggestedChanges[id]);
      double absDipChangeop = 
         absDipChange/theSteerUtil->getRefMomentumAtDipole(id+aPlan.firstDipole);
      if (absDipChangeop < aPlan.minChangeDipole) continue; // this one is o.k. 
//      cerr << " Analysing suggested changes for dipole " 
//           << id+aPlan.firstDipole  << " suggested change " << suggestedChanges[id] << endl;
      int numSameSign = 0;   // consecutive same signs in fact... 
      int numSuspicious = 0;
      double cP, cmPr;
      for (size_t itP=1; itP != itPrevious.size(); itP++) {
        double cPm1 = itPrevious[itP-1].suggestedChanges[id];
        cP = itPrevious[itP].suggestedChanges[id];
	cmPr = cP*cPm1;
	if (cmPr > 0.) numSameSign++;
	if ((std::abs(cP) > std::abs(cPm1)) && (cmPr > 0.)) numSuspicious++; 
	// More criteria here....???  
      }
      cmPr = suggestedChanges[id]*cP;
      if (cmPr > 0.) numSameSign++;
      if ((std::abs(suggestedChanges[id]) > std::abs(cP)) && (cmPr > 0.)) numSuspicious++;  
      
      bool lookGood = true;
//      if (numSameSign >= (itPrevious.size()-1) && (itPrevious.size() > 10)) lookGood = false;  // constants to be tuned, eventually.
      if ((numSuspicious > itPrevious.size()/2) && (itPrevious.size() > 6)) lookGood = false;
      // Comment out warning messages... 
      if (!lookGood) {
//        cout << " ...  numSameSign " << numSameSign << " Suspicious " 
//           << numSuspicious << endl;
//        cout << " ... Leaving alone dipole for now dipole " << id+aPlan.firstDipole << endl; 
//        cout << " ... Keep working with this dipole " << id+aPlan.firstDipole << endl; 
//	// Bad idea to set it to 0.
//	theSteerUtil->SetOneDipole(id+aPlan.firstDipole, 0.);     
//        isOK[id] = false;
//        numBadVariables++; 
//        cerr << " And quit for debugging purposes ! "  << endl; 
//        exit(2);
      }  
   }
   if (numBadVariables > 2*aPlan.numDipoles/3) return false;
   return true;	
}

void Steering::localIterData::applyChanges(const DMSPlan& aPlan) {

//     cout << " In localIterData::applyChanges, on " << aPlan.numDipoles << " dipoles " << endl;
     size_t numMaxDipChange = aPlan.numDipoles - aPlan.numDipoleTailRej;
     // Not sure this is wise. To be investigated.. 
//     if (numBPMs > mydmsPlan.numDipoles - numBadDip) numMaxDipChange = mydmsPlan.numDipoles;
      for (size_t i=0; i != numMaxDipChange; ++i) {
        size_t iDip =  aPlan.firstDipole + i;
	actualChanges[i] = 0.;
	if ((theSteerUtil->isDipoleOK(iDip)) && isOK[i]) {
	  actualChanges[i] = aPlan.gainChangeDipole*suggestedChanges[i]; ;
          theSteerUtil->AdjustOneDipole(iDip, actualChanges[i]);
//          cerr << " Adjusting dipole " << iDip << " by " << actualChanges[i] << " to " 
//	       << theSteerUtil->GetOneDipoleSetting(iDip) << endl;
	}
      }
}

// From numrec for SVD 
/*
extern "C" {
  void dsvdcmp(double **a, int m, int n, double w[], double **v);
  void dsvbksb(double **u, double w[], double **v, int m, int n, double b[],
	double x[]);
}
*/ // no longer needed. 

int Steering::SteerPureDMS(BmlPtr beamLine, std::string &label, Steering::DMSPlan &aPlan    ) {

  if (theSteerUtil == 0) {
    cerr << " No Steering utility activated... must be defined to proceed..." << endl;
    exit(2);
  }
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) ilcTestDataChr = (char *) defDataDir.c_str();    
  int nTotalIter = 0;
  aPlan.printPlan();
  size_t numTotalDip = theSteerUtil->GetNumCorrectorDipoles();
  size_t numTotalBPMs = theSteerUtil->GetNumBPMs();
  theSteerUtil->ResetPulseCounter();
  std::ostringstream labelG;
  int iGradientFract = (int) ( 100.*aPlan.macroGradientFraction);
//  labelG << label << "_Ma" << aPlan.macroNumMacroIter << "_Gf" 
//         << ((int) (100.0* aPlan.macroGradientFraction)) 
//         <<   "_NEB" <<  aPlan.macroNumExtraBPMs; 
       if (!aPlan.skipInitialPerfMeas) {
           
           std::string fTopDir(ilcTestDataChr);
           fTopDir += std::string("/Test4/");
	   std::string prelimLabel("Prelim");
           std::string noPart("");
           StudyPerformance(beamLine, fTopDir,  prelimLabel, noPart);
       }
  for (size_t itMacro=0; itMacro != aPlan.macroNumMacroIter; itMacro++ ) {
  
    aPlan.firstDipole = aPlan.macroFirstDipole;
    aPlan.numDipoles = aPlan.macroNumDipoles;  // fixed, part of the global plan. 
    aPlan.numExtraBPMs = aPlan.macroNumExtraBPMs; // Two extra BPMs 
    bool dipoleLeft = (aPlan.firstDipole + aPlan.numDipoles < 
                         (numTotalDip - 1 - aPlan.macroNumExtraBPMs));
    aPlan.firstCavity = 0;
    aPlan.numKlystrons = aPlan.macroNumKlystrons; // Number of klystrons at reduced voltage per local iteration,
    if (aPlan.useReducedFieldInBunchCompressor && (aPlan.macroFirstDipole == 0))
      aPlan.numKlystrons = 0;
    aPlan.gradientFraction = aPlan.macroGradientFraction;
    // We start steering after three klystrons.. 
    cerr << " Steering::SteerPureDMS At Macro iteration " << itMacro << std::endl;
    size_t iterLocal = 0;
    double origGain = aPlan.gainChangeDipole;
    size_t numRetry = 0; 
    char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
    std::string defDataDir("./data");
    if (ilcTestDataChr == 0) ilcTestDataChr = (char *) defDataDir.c_str();    
    std::string topDir(ilcTestDataChr);
    topDir += std::string("/Test4/");
    std::string fOutDirPath(topDir);
    theSteerUtil->clearBPMsForDisplay();
//    ????? Play with the gain here... 
    while (dipoleLeft) {
       bool keepGoingLocal = true;
       cerr << endl << " Local Iteration for first dipole at  " 
            << aPlan.firstDipole << " ..... Pulse count= " 
	    << theSteerUtil->GetPulseCount() << endl;
       std::ostringstream gDipStr; gDipStr << "d" << aPlan.firstDipole << "/";
       std::string fOutDirPathDip(fOutDirPath); fOutDirPathDip += gDipStr.str();
       std::string cmd("/bin/mkdir "); cmd += fOutDirPathDip;
       system(cmd.c_str());
       if (aPlan.useCryoTilt) {
         cerr << " Obsolete version of Steering with Dispersion. Quit " << endl;
	 exit(2);
       }
       
        
       if (aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs > numTotalBPMs) 
           aPlan.numExtraBPMs = numTotalBPMs - (aPlan.firstDipole + aPlan.numDipoles - 1);
       aPlan.rescaleLattice = false;
       aPlan.byRFPhase = false;
       std::vector<double> dipBefore;
       for (size_t ii=aPlan.firstDipole; 
                   ii != aPlan.firstDipole+aPlan.numDipoles; ii++) 
		dipBefore.push_back(theSteerUtil->GetOneDipoleSetting(ii)); 
       if (aPlan.doMovie) { // choice of BPM a bit arbitrary.. 
	  theSteerUtil->clearBPMsForDisplay();
	  theSteerUtil->addBPMForDisplay(aPlan.firstDipole + aPlan.numDipoles - 1);
	  std::string ffLabelFinal(fOutDirPathDip);
	  theSteerUtil->setDisplayToken(ffLabelFinal);	  
	  theSteerUtil->openFileOutMovieScale ();	  
       }	
       int nnIter = Steering::SteerPureDMSLocal(beamLine, aPlan);
       if (aPlan.doMovie) theSteerUtil->closeFileOutMovieScale ();
       nTotalIter += nnIter;
       if (nnIter == aPlan.numMaxLocalIteration+1) {
          nTotalIter --;
         cerr << " Not converging on this section... Go Back to the beginning!!!... " << endl;
//           std::string labelIter(labelG.str().c_str());
           std::string badIterLabel("Bad");
           std::string noPart("");
	   int lastBPM = aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs + 1;
           StudyPerformance(beamLine, fOutDirPathDip, badIterLabel, noPart, lastBPM);
	   // Now restore here... 
//           for (size_t ii=aPlan.firstDipole; 
//                   ii != aPlan.firstDipole+aPlan.numDipoles; ii++) 
//             theSteerUtil->SetOneDipole(ii, dipBefore[ii-aPlan.firstDipole]);  
// Debug.. 
//           cerr << " And quit in this exercise!!!! " << endl;
//          exit(2);	      
//	 break; 
//  reduce the gain... 
//          aPlan.gainChangeDipole *= 0.75;
	  numRetry ++;
// Or just go one dipole backward.. and reduce the gain... 
	  if (numRetry == 3) {
//            aPlan.firstDipole -= 2;
//            aPlan.numKlystrons -= 2;  // as many dipoles/quadrupoles/r.f. unit. 
//	    if (aPlan.firstDipole <= aPlan.macroFirstDipole) {
	       cerr << " Complete failure, had to go back to beginnning... Quit Now " << endl;
	       return -1;
	    }
            keepGoingLocal = false;
	  }
       if (!keepGoingLocal) break; 
       numRetry = 0;
       aPlan.gainChangeDipole = origGain;
       cout << endl << " Local iteration at section " << iterLocal 
            << " done, number of iteration " 
            << nnIter << " Total Pulse Count " << theSteerUtil->GetPulseCount() << endl;
/// Noo! we keep reducing the whole Linac. 
//       aPlan.firstCavity += 1*24; // Assume one dipole per 24 cavities. 
// debugging with cavity moves...		      
//       dipoleLeft = (aPlan.firstDipole  < 6);
        {
           std::string noPart("");
	   std::string okLabel("OK");
	   int lastBPM = aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs + 1;
           StudyPerformance(beamLine, fOutDirPathDip, okLabel, noPart, lastBPM);
       }
       aPlan.firstDipole += aPlan.macroNumDipIncrement;
       aPlan.numKlystrons += aPlan.macroNumDipIncrement;  // as many dipoles/quadrupoles/r.f. unit. 
       // Stop for debugging at dipole 8 
//       if (aPlan.firstDipole == 7) {
//        cerr << " Starting at Dipole 7, we'll stop here for debugging w/wo moving cavities " << endl;
//	exit(2);
//       }
       dipoleLeft = IsDipoleLeftAndUpatePlanAtEnd(&aPlan); 
       iterLocal++;
    }  // moving from section to section. 
    std::string noPart("");
    int lastBPM = aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs + 1;
    std::string lastMacroLabel("final");
    StudyPerformance(beamLine, fOutDirPath, lastMacroLabel, noPart, lastBPM);
    cout << endl << "--------------------- Macro Iteration " << itMacro << "finished  ----------------" << endl;
    cout << " ..... Number of pulses used " << theSteerUtil->GetPulseCount() << endl; 
    theSteerUtil->clearAllStoredBunches();
  } // On macro iteration. 
  return nTotalIter;
}


// Straight and strict DFS, using simple quare, linear solving. 
// Extended to include extra BPMs ==> using SVD..

int Steering::SteerPureDMSLocal(BmlPtr beamLine, 
                            const DMSPlan& mydmsPlan) {
   
    mydmsPlan.printPlan();
    std::string labelMat("SteerDFSPureLocal_v0");
    const double epsSVDSafe = 0.001;
    
    Vector stateInBC = theSteerUtil->getStateAtInjection(); 
    
    if (mydmsPlan.useReducedFieldInBunchCompressor) 
          stateInBC[5] = mydmsPlan.gradientFraction - 1.0;
    
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t numBPMs = mydmsPlan.numDipoles + mydmsPlan.numExtraBPMs;  // NOT square matrix problem. 
    size_t firstBPM = mydmsPlan.firstDipole+1;
    size_t indexBPMMax =  firstBPM + numBPMs;
    size_t numCav = mydmsPlan.numKlystrons*mydmsPlan.numCavitiesPerKlystrons;
    if (indexBPMMax >= theSteerUtil->getNumBPMs()) {
      cerr << " Steering::SteerDFSPure: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
         numBPMs = theSteerUtil->getNumBPMs() - firstBPM - 1;
    }
    
    std::vector<localIterData> itResults;
    size_t numConsecutiveBad = 0;
    Matrix* am0 = 0; 
    // Define one steering matrix no matter what, as we will use the ancillary 
    // information in it to measure the deviations.
    size_t numBadDip = 0;
    for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;

//    cerr << "  Cavity Status begin local " << mydmsPlan.firstCavity << " numCav " << numCav << endl;
    Steering::ResponseMatrix<Steering::ECHANGE> *am0Cor = 0; 
    if (mydmsPlan.usePerfectResponse) { // indepedent of iterations ! 
        size_t indexRepository = mydmsPlan.firstDipole - mydmsPlan.macroFirstDipole;
	indexRepository /= mydmsPlan.macroNumDipIncrement;
	am0 = new Matrix(mydmsPlan.responsesPerfect[indexRepository]);
	am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, -2);
    } else {
       am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
				     stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, 
				     mydmsPlan.numPseudoPartRP);
//       am0Cor->studyStability(-2);				     
       am0 = am0Cor;
//       cout << " SteerPureDMSLocal, current Missing energy = " << am0Cor->getEnergyOffset() << endl;
    }  
    int bpmStartThisIt = mydmsPlan.firstDipole - 2;
    // If off energy, must start one upstream, as we will store the good beam with 
    // the off energy configuration of this iteration.. 
    // Optimization for speed not very robust.. 
    int bpmStartThisItOffE = mydmsPlan.firstDipole - 3;
    if (bpmStartThisIt < 0) bpmStartThisIt = 0;
    if (bpmStartThisItOffE < 0) bpmStartThisItOffE = 0;
    for (size_t itSteer = 0; itSteer != mydmsPlan.numMaxLocalIteration; itSteer++ ) { 
     
//      cerr << " Steering::SteerPureDMSLocal At iteration.. " << itSteer << endl; 
     
      // count the number of broken dipoles. 
      // Check if we have a good dipole 
      numBadDip = 0;
      for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;

      Vector dispVals(numBPMs);
      if ((!mydmsPlan.usePerfectResponse) && (itSteer > 0) ) {
        delete am0Cor; // get rid of the previous one. 
        am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
				     stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, 
				     mydmsPlan.scaleResponseMatrix, mydmsPlan.numPseudoPartRP);
	am0 = am0Cor;			     
      } 
      // Use a real bunch, and the default value in steerUtil
      MLBPMs bpmsTmp; // just to get a local copy
     // Set the BPMs. We take the one downstream.  This make an assumption on 
     // the lattice is layed out. 
      
      theSteerUtil->getBPMPtrs(bpmsTmp, firstBPM, numBPMs);
      double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
    // Testing, playing with resolution.  
//    bpmsTmp.setResolution(50.e-6);
      // At full energy...
//      cout << " Steering::SteerPureDMS Sending real bunch iteration " << itSteer << endl;
//      cerr << "  Cavity Status " << mydmsPlan.firstCavity << " numCav " << numCav << endl;
      if (!theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, false, 
                                        bpmStartThisIt, false)) {
      
         cerr << " Hitting aperture, Must reduce dipole settings.... For the whole LINAC " << endl;
	 for (size_t iDip = mydmsPlan.macroFirstDipole; 
	                 iDip != mydmsPlan.firstDipole+mydmsPlan.numDipoles; iDip++) {
	      double corrSet = theSteerUtil->GetOneDipoleSetting(iDip);
//	      cerr << " Reducing dipole " << iDip << " from " << corrSet 
//		     << " to " << 0.9999999*corrSet << endl; // Don't know how to tune this...
	      theSteerUtil->SetOneDipole(iDip, 0.999999*corrSet);      	 
	 }
         if (mydmsPlan.usePerfectResponse) delete am0;
         delete am0Cor;
         return mydmsPlan.numMaxLocalIteration+1; // don't go any further on this iteration... 		  
      }
    // collect the reading from the BPMs 
      double valBPMsFullE[numBPMs];
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    valBPMsFullE[kBpm] = (*itBpm)->GetMeasurement();
//	 if (itSteer == 0) 
//	    cerr << " BPM " << (*itBpm)->GetId() << " records " << valBPMsFullE[kBpm] << endl;
	  kBpm++;    
      }
      if (mydmsPlan.firstDipole == 99999996) { 	       	       
        cout << " In Steering, at nominal energy " << endl;
	std::string aTmpLabel1("Dip6DebugFullE");
	StudyPerformanceDebug(aTmpLabel1, 3, 12);
//        PrintEnergyAtEachBPM(20);
      }
      am0Cor->setEnergy(false, true);
      if (mydmsPlan.firstDipole == 999999996) { 	       	       
        cout << " In Steering, at reduced energy " << endl;
	std::string aTmpLabel1("Dip6DebugReducedEP9999");
	StudyPerformanceDebug(aTmpLabel1, 3, 12);
	cerr << " And quit !! " << endl;
	exit(2);
//        PrintEnergyAtEachBPM(20);
      }
     // At full partial energy Assume we'll get through, less kick since less 
      // energy.. 
      theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, false, 
                                     bpmStartThisItOffE, false);
      kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	   int iBPMIndex = (*itBpm)->GetId();
	   double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	   double relDeltaE = am0Cor->getEnergyOffset()/nominalE;
	   double disp = (valBPMsFullE[kBpm] - (*itBpm)->GetMeasurement())/relDeltaE;
	   disp -= theSteerUtil->getDispersionAtBPM(iBPMIndex);
	   dispVals(kBpm) = 1.0e-6*disp;  // scale factor back to meter. 
	   // more reasonable values for the determinant.
//	 if (itSteer == 0) 
//	    cerr << " At BPM " << (*itBpm)->GetId() << " value " << 
//	    (*itBpm)->GetMeasurement() << " nominal E " << nominalE 
//	    << " relDeltaE " << relDeltaE << 
//	      " dispersion " << disp << " DipoPrev " << 
//	      theSteerUtil->GetOneDipoleSetting(((*itBpm)->GetId()-1)) << endl;
	kBpm++;    
      }
      // Linac reset 
      am0Cor->setEnergy(false, false);
      // Compute the correction
      // Compute the inverse or the pseudoinverse
     size_t nL =  mydmsPlan.numDipoles-numBadDip; // nL is the number of unknows, 
                                                  // i.e., the number of active dipoles. 
      Vector diPVals(nL);
// Here, we'll put some qualifiers on the BPMs..nBPMs is the number of constraints 
// ( " equations" )
      bool debugSVD = false;
      bool isRegularSquare = (nL == numBPMs); // No idea how regular this is....
      if (isRegularSquare || debugSVD) {
        Matrix m0 = am0->inverse();
	diPVals = m0*dispVals;
      } else {
         bool debugSVD = false;
	   // Pseudo inverse, Using Singular Value Decomposition  
      
         Vector w(nL);  // Will be the W matrix in SVD formalism 
	 Matrix um0(nL, numBPMs);
	 Matrix vm0T(nL, nL);
	 // To be adjusted, possible improvements here
	 for (size_t i = 0; i != nL; i++) w(i) = 1.;
         if (debugSVD) { // testing ... I know I have a square matrix.  
            cerr << " M0Cor Matrix ... " << endl;
	    cerr << (*am0);
	    cerr << " ---------------------------------------- " << endl;
	    size_t nL =  mydmsPlan.numDipoles-numBadDip;
	    Matrix m1 = am0->inverse();
            cerr << " True inverse Matrix ... " << endl;
	    cerr << m1 ;  
	    cerr << " ---------------------------------------- " << endl;
            Vector diPValsT(nL);
	    diPValsT = m1*dispVals;
	    cerr << " True inverse solution " << endl;
	    for (int i=0; i != nL; i++) 
	        cerr << "  " <<  i << "   " << diPValsT(i) << endl;
	      cerr << " ---------------------------------------- " << endl;
	    // Check this inversion.. 
	    Matrix checkUnit = m1*(*am0);
            cerr << " Check Inversion ... " << endl;
	    cerr << checkUnit ;  
	    cerr << " ---------------------------------------- " << endl;
	  } // Testing, debugging .... 
	  am0->SVD(um0, w, vm0T);  // Perhaps bad notation here, should be called vm0
	  // Sign Change, for SVD. 
//	  for (size_t k=0; k != numBPMs; k++) dispVals(k) *= -1.; 
	  Vector utyTmp(um0.transpose()*dispVals);
//	  cerr << " utyTmp " << utyTmp << endl;
//	  cout << " W-Vect";
	  for (size_t i = 0; i != nL; i++) {
//	     cout << " - " << w(i);
//	     if ((i%5 == 0) && (i > 0)) {
//	       cout << endl << " .... ";
//	     } 
	     if (std::abs(w(i)) > mydmsPlan.minWeightSVD) utyTmp(i) /= w(i); // Arbitrary !!! 
             else utyTmp(i) = 0.;
	 }
//	cerr << " iM " << iM <<  " w " << w(iM) << " utyTmp " <<  utyTmp(iM) << endl;   
	  diPVals = vm0T*utyTmp;
	  // Flip sign, different convention to write down the linear system.
//	  for (size_t iL=0; iL !=nL; iL++) diPVals(iL) *= -1.0;
	  // Testing Debugging...
	  if (debugSVD) {
	    Matrix vm0 = vm0T.transpose(); // ?? Expected to have to use this one.  
	    Matrix um0T = um0.transpose();
            cerr << " Check diagonal W Matrix ... " << endl;
	    for (size_t i = 0; i != nL; i++)
	        cerr << "  " <<  i << "   " << w(i) << " Inv " << 1.0/w(i) << endl;
	    cerr << " ---------------------------------------- " << endl;
	    Matrix dm1(nL,nL);
	    for (int i=0; i != nL; i++) {
	      for (int j=0; j != nL; j++) {
	         dm1(i,j) = 0.;
	         if (i == j)  dm1(i,j) = 1.0/w(i); 
	      }
	    }
	    Matrix mInv = vm0T*dm1*um0T;
	    Matrix mCheck1 = (*am0)*mInv;
            cerr << " Check Pseudo-Inversion ... mInv*m " << endl;
	    cerr <<  mCheck1;  
	    cerr << " ---------------------------------------- " << endl;
	    Matrix mCheck2 = mInv*(*am0);
            cerr << " Check Pseudo-Inversion ... m*mInv " << endl;
	    cerr << mCheck2 ;  
	    cerr << " ---------------------------------------- " << endl;
	    
	     
	    cerr << " Pseudo inverse solution, TMatrix in CHEF  " << endl;
	    for (int i=0; i != nL; i++) 
	        cerr << "  " <<  i << "   " << diPVals(i) << endl;
	      cerr << " ---------------------------------------- " << endl;
	    // And we done 

	     // Bug in CHEF???
	     // for this square matrix, attempting to use Numerical recipes 
	     // instead.  
	     // Attempting to use numrec_c
	     /* No longer needed, we did figure out how to use CHEF's SVD 
	    cerr << " Attempting to solve using Numrec recipes. nL = " << nL <<  endl;
	    
	     int nLInt = nL;
	     double *uNr[nL+1]; // Because Stupid Numrec is in Fortran ! 
	     double *vNr[nL+1];
	     for (int i=1; i <= nL; ++i) {
	       uNr[i] = (double*) malloc(sizeof(double) *(nL+1));
	       vNr[i] = (double*) malloc(sizeof(double) *(nL+1)); // and let it leak... 
	      }	     	     
	     double wNr[nL+1];
	     for (int i=1; i <= nL; ++i) {
	       for (int j=1; j <= nL; ++j) {
	          uNr[i][j] = m0Cor(i-1,j-1);
	       }
	     }
	     dsvdcmp(uNr, nLInt, nLInt, wNr, vNr);
	     cerr << " dsvdcmp done " << endl;
	     // Now solve.. 
	     double bNr[nL+1];
	     for (int i=1; i <= nL; i++) bNr[i] = dispVals(i-1);
	     double xNr[nL+1];
	     dsvbksb((double**) uNr, wNr, (double**) vNr, nL, nL, bNr, xNr);
	     cerr << " Solution using numrec recipes " << endl;
	     for (int i=1; i <= nL; i++) 
	        cerr << "  " <<  i << "   " << xNr[i] << endl;
	      cerr << " ---------------------------------------- " << endl;
	      cerr << " Quit for now after SVD test " << endl;
	      exit(2);
	    */  
	  } 
      }
      localIterData newIt(mydmsPlan);
      newIt.fillSuggested(diPVals);
      if (!newIt.appraise(mydmsPlan, itResults)) numConsecutiveBad++;
      // Loop if we made a significant change. if so, we break, we done... 
      if (newIt.areWeDone(mydmsPlan))  {
        if (!mydmsPlan.useCryoTilt) { // if we tilt the cavities, these bunches are not valid..
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...       
           am0Cor->setEnergy(false, true);
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 false, bpmStartThisItOffE, true);
           am0Cor->setEnergy(false, false);
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 false, bpmStartThisIt, true);
         }
         if (mydmsPlan.usePerfectResponse) delete am0;
	 delete am0Cor;
         return (itSteer+1);
      }
      if (numConsecutiveBad > 1) {
        cout << " Steering::SteerPureDMSLocal, Two bad consecutive iteration after " << 
	     (itSteer+1) << " iterations, quit on this one " << endl;
        if (mydmsPlan.usePerfectResponse) delete am0;
	delete am0Cor;
        return mydmsPlan.numMaxLocalIteration;
      }
      newIt.applyChanges(mydmsPlan);
      itResults.push_back(newIt);
      numConsecutiveBad = 0;
      ////Now apply the correction.  Only for the good dipoles.
        
    // Last dipole does something screwy, not sure why... 
     size_t iDipOK = 0;
     // Debugging with more details... 
/*
        {
           std::string labelIter("Demo5_Debug");
           std::ostringstream lIterStr; lIterStr << "_M0NCM_D" 
	                                         << mydmsPlan.firstDipole << "_it" <<itSteer;
           labelIter += lIterStr.str();
           DumpTableCorrector (beamLine, labelIter);
           std::string noPart("");
           StudyPerformance(beamLine, labelIter, noPart);
       }
*/
    } // on Iteration.. 
    if (mydmsPlan.usePerfectResponse) delete am0;
    delete am0Cor;
    return mydmsPlan.numMaxLocalIteration;

}
