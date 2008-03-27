#include <cmath>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"
#include "SteerAnalysis.h"
#include <time.h>
#include <basic_toolkit/SVDFit.h>
#include "SVDFitPL.h"


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

// Assuming we have a perfect lattice, and run plan on how to define the sections, 
// number of BPMs, and so forth, we can pre-compute these matrices for 

void printKickAngles(const BmlPtr& bmlPtr, int nMax);

int Steering::SteerWeightedDMS(BmlPtr beamLine, std::string &label, 
                               Steering::DMSPlan &aPlan    ) {

  if (theSteerUtil == 0) {
    cerr << " No Steering utility activated... must be defined to proceed..." << endl;
    exit(2);
  }
  // Perhaps look for broken dipoles, currently not supported in this context.. 
  // (old 1 to 1 response matrices don't support them. ).. 
  // 
  // To be done at a later stage.. 
  //
  int nTotalIter = 0;
  bool printMinimum = false;
//  aPlan.printPlan();
  size_t numTotalDip = theSteerUtil->GetNumCorrectorDipoles();
  size_t numTotalBPMs = theSteerUtil->GetNumBPMs();
  theSteerUtil->ResetPulseCounter();
  std::ostringstream labelG;
  int iGradientFract = (int) ( 100.*aPlan.macroGradientFraction);
       if (!aPlan.skipInitialPerfMeas) {
           char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
           std::string defDataDir("./data");
           if (ilcTestDataChr == 0) ilcTestDataChr = (char *) defDataDir.c_str();    
           std::string topDir(ilcTestDataChr);
           topDir += std::string("/Test4/");
	   std::string prelimLabel("PrelimNoWake");
           std::string noPart("");
	   bool wakeWasOn = theSteerUtil->isWakeOn();
	   theSteerUtil->setWake(false);
           StudyPerformance(beamLine, topDir,  prelimLabel, noPart);
	   theSteerUtil->setWake(true);
	   std::string prelimLabelWW("PrelimWithWake");
           StudyPerformance(beamLine, topDir,  prelimLabelWW, noPart);
	   theSteerUtil->setWake(wakeWasOn);
	   std::cerr << " And quit after prelim studies.. " << endl;
	   exit(2);
       }
  for (size_t itMacro=0; itMacro != aPlan.macroNumMacroIter; itMacro++ ) {
    aPlan.currentMacroIteration = itMacro;
    aPlan.firstDipole = aPlan.macroFirstDipole;
    aPlan.numDipoles = aPlan.macroNumDipoles;  // fixed, part of the global plan. 
    aPlan.numExtraBPMs = aPlan.macroNumExtraBPMs; 
    bool dipoleLeft = (aPlan.firstDipole + aPlan.numDipoles <= 
                         (numTotalDip - aPlan.macroNumExtraBPMs));
    aPlan.firstCavity = 0;
    aPlan.numKlystrons = aPlan.macroNumKlystrons; // Number of klystrons at reduced voltage per local iteration,
    if (aPlan.useReducedFieldInBunchCompressor && (aPlan.macroFirstDipole == 0))
      aPlan.numKlystrons = 0;
    aPlan.gradientFraction = aPlan.macroGradientFraction;
    // We start steering after three klystrons.. 
    if (printMinimum) 
     cout << " Steering::SteerPureDMS At Macro iteration " << itMacro << std::endl;
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
    aPlan.doLastSegment = false;
    while (dipoleLeft) {
       bool keepGoingLocal = true;
       if (printMinimum) cout << endl << " Local Iteration for first dipole at  " 
            << aPlan.firstDipole << " ..... Pulse count= " 
	    << theSteerUtil->GetPulseCount() << endl;
//       std::ostringstream gDipStr; gDipStr << "d" << aPlan.firstDipole << "/";
//       std::string fOutDirPathDip(fOutDirPath); fOutDirPathDip += gDipStr.str();
//       std::string cmd("/bin/mkdir "); cmd += fOutDirPathDip;
//       system(cmd.c_str());
        std::string fOutDirPathDip(fOutDirPath);
       // See if we gain by tilting cryo modules. First make sure we have enough of them 
       // 
       if (aPlan.useCryoTilt) { // More complexity: attempt to optimize by tilting cryo-modules
          int nIterTilt =  Steering::CorrectCryoTilt(beamLine, aPlan, fOutDirPathDip.c_str());
	  cout << " Total number of tilt iterations = " <<  nIterTilt << endl;\
//	  cerr << " And really quit now !!!!! " << std::endl;
//	  exit(2);
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
       int nnIter = 0;
       if (aPlan.useDFSMerlin)	
        nnIter = Steering::SteerWeightedDMSLocalMerlin(beamLine, aPlan);
//          nnIter = Steering::SteerWeightedDMSLocalMDebug(beamLine, label, itMacro, aPlan);
        else   nnIter = Steering::SteerWeightedDMSLocal(beamLine, aPlan);
//	std::cerr << " And Quit for Now, after one call to SteerWeightedDMSLocalMerlin " << endl;
//	exit(2);
	
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
       if (printMinimum) cout << endl << " Local iteration at section " << iterLocal 
            << " done, number of iteration " 
            << nnIter << " Total Pulse Count " << theSteerUtil->GetPulseCount() << endl;
/// Noo! we keep reducing the whole Linac. 
//       aPlan.firstCavity += 1*24; // Assume one dipole per 24 cavities. 
// debugging with cavity moves...		      
//       dipoleLeft = (aPlan.firstDipole  < 6);
/*
        {
           std::string noPart("");
	   std::string okLabel("OK");
	   int lastBPM = aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs + 1;
           StudyPerformance(beamLine, fOutDirPathDip, okLabel, noPart, lastBPM);
       }
*///       cerr << " And quit for Now, only first 20 dipoles only .. " << endl;
//       exit(2); 
       aPlan.firstDipole += aPlan.macroNumDipIncrement;
       if (aPlan.numCavitiesPerKlystrons == 24) 
           aPlan.numKlystrons += aPlan.macroNumDipIncrement; 
       else if (aPlan.numCavitiesPerKlystrons == 8)
           aPlan.numKlystrons += 3 * aPlan.macroNumDipIncrement; 
       else {
         cerr << " Unexpected number of cavities per klystrons !!! Quit here " << endl;
	 exit(2);
       }   
       // !!!!! 
       // Modified Merlin... 
       // If we decide to back to the strictMerlin algorithm, can't use Pre-steering from Jeff.
//       if (aPlan.usePreSteer) {
//         aPlan.numKlystrons = aPlan.firstDipole - 2;
//       }
       dipoleLeft = IsDipoleLeftAndUpatePlanAtEnd(&aPlan); // might change the local plan if at end 
       // Simulate a pause in case of dynamical studies..
       // Need to be controlled from outside... 
       // Wait 5 minute. 
       if (aPlan.doDynamicalPrelim) theSteerUtil->IncrementPulseCounter(aPlan.numPulseBetweenLocalIteration);
       iterLocal++;

       // Debugging... Just do the first section... 
//       if (iterLocal > 0) break;
    }  // moving from section to section. 
    std::string noPart("");
    int lastBPM = aPlan.firstDipole + aPlan.numDipoles + aPlan.numExtraBPMs + 1;
    std::string lastMacroLabel(label); lastMacroLabel += std::string("_final");
    StudyPerformance(beamLine, fOutDirPath, lastMacroLabel, noPart, lastBPM);
    if (printMinimum) 
     cout << " WeightedDMS, Done Study Performance for this macro iteration, with no wake " << endl;
    { 
	bool wakeWasOn = theSteerUtil->isWakeOn();
	theSteerUtil->setWake(true);
       std::string lastMacroLabelWW(lastMacroLabel); lastMacroLabelWW += std::string("WW");
       if (printMinimum) cout << " WeightedDMS, Ready with Wake  " << endl;
       StudyPerformance(beamLine, fOutDirPath, lastMacroLabelWW, noPart, lastBPM);
       if (printMinimum) cout << " WeightedDMS, final study done with Wake   " << endl;
       if (!wakeWasOn) theSteerUtil->setWake(false);
     }
    if (printMinimum) {
     cout << endl << "--------------------- Macro Iteration " << itMacro << "finished  ----------------" << endl;
     cout << " ..... Number of pulses used " << theSteerUtil->GetPulseCount() << endl; 
    }
    theSteerUtil->clearAllStoredBunches();
      // Update on the message file 
      
      std::ostringstream gridUptStrStr;
      gridUptStrStr << " DMSMacro, iteration " << itMacro;
      std::string gridUptStr = gridUptStrStr.str();	     
      theSteerUtil->gridUpdate(gridUptStr, true); 
      if (aPlan.doDynamicalPrelim) theSteerUtil->IncrementPulseCounter(aPlan.numPulseBetweenGlobalIteration);
    
  } // On macro iteration. 
  return nTotalIter;
}


// Straight and strict DFS, using simple quare, linear solving. 
// Extended to include extra BPMs ==> using SVD..
// Oboslete, use the Merlin style global fit now... 

int Steering::SteerWeightedDMSLocal(BmlPtr beamLine, 
                            const DMSPlan& mydmsPlan) {
   
    mydmsPlan.printPlan();
    std::string labelMat("SteerDFSPureLocal_v0");
    const double epsSVDSafe = 0.001;
    unsigned int nnDipolesPreSteering = 3;
    bool doPreSteer = false;    
    if (mydmsPlan.usePreSteer &&  
        ((mydmsPlan.firstDipole - mydmsPlan.numDipolesPreSteering) >= 0)) {
       doPreSteer = true;  
       nnDipolesPreSteering = mydmsPlan.numDipolesPreSteering;
    }
    double dipValPre[nnDipolesPreSteering]; 
    
    Vector stateInBC = theSteerUtil->getStateAtInjection();; 

    if (mydmsPlan.useReducedFieldInBunchCompressor) 
          stateInBC[5] = mydmsPlan.gradientFraction - 1.0;
    
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out.
    size_t numBPMs = mydmsPlan.numDipoles + mydmsPlan.numExtraBPMs;  // NOT square matrix problem. 
    if (mydmsPlan.doLastSegment) numBPMs = mydmsPlan.numDipoles;
    size_t firstBPM = mydmsPlan.firstDipole+1;
    size_t indexBPMMax =  firstBPM + numBPMs;
    size_t numCav = mydmsPlan.numKlystrons*mydmsPlan.numCavitiesPerKlystrons;
    if (indexBPMMax >= theSteerUtil->getNumBPMs()) {
      cout << " Steering::SteerDFSPure: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
         numBPMs = theSteerUtil->getNumBPMs() - firstBPM - 1;
      cout << " .... firstBPM " << firstBPM << " ...new numBPMs = " << numBPMs << endl;
    }
    
    std::vector<localIterData> itResults;
    size_t numConsecutiveBad = 0;
    Matrix* am0 = 0;  Matrix* am0OtO = 0;
    // Define one steering matrix no matter what, as we will use the ancillary 
    // information in it to measure the deviations.
    size_t numBadDip = 0;
    for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;
    if (numBadDip != 0) {
      cerr << 
      " SteerWeightedDMSLocal : Bad dipoles detected... Not yet supported " << endl;
      return mydmsPlan.numMaxLocalIteration;
    }
    Steering::ResponseMatrix<Steering::ECHANGE> *am0Cor = 0; 
    Steering::ResponseMatrix<Steering::CORDIP> *am0CorOtO = 0; 
    if (mydmsPlan.usePerfectResponse) { // indepedent of iterations ! 
      size_t indexRepository = 0;
      if (mydmsPlan.doLastSegment) indexRepository = mydmsPlan.responsesPerfect.size() -1;
      else {
        indexRepository = mydmsPlan.firstDipole - mydmsPlan.macroFirstDipole;
	indexRepository /= mydmsPlan.macroNumDipIncrement;
      }
	am0 = new Matrix(mydmsPlan.responsesPerfect[indexRepository]);
	am0OtO = new Matrix(mydmsPlan.respPerfectOneToOne[indexRepository]);
	am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, -2);
    } else {
       am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC, 
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, 
				     mydmsPlan.numPseudoPartRP);
       am0 = am0Cor;
       am0Cor->setEnergy(false, false); // Reset Linac, just to make sure..  
       
       am0CorOtO = new Steering::ResponseMatrix<Steering::CORDIP>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
                                     firstBPM, numBPMs, 
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles);
     
       am0OtO = am0CorOtO;
    }  
    int bpmStartThisIt = mydmsPlan.firstDipole - 3;
    // If off energy, must start one upstream, as we will store the good beam with 
    // the off energy configuration of this iteration.. 
    // Optimization for speed not very robust.. 
    int bpmStartThisItOffE = mydmsPlan.firstDipole - 3;
    if (doPreSteer) { // We will re-tweak upstream dipoles.. 
      bpmStartThisItOffE -= 3;
      bpmStartThisIt -= 3;
    }    
    if (bpmStartThisIt < 0) bpmStartThisIt = 0;
    if (bpmStartThisItOffE < 0) bpmStartThisItOffE = 0;
    // Pre-Steering to orbit... 
    
    if (doPreSteer) {
      for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
        unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	dipValPre[i] = theSteerUtil->GetOneDipoleSetting(iP);
      }
      Steering::PreSteerToOrbit( theSteerUtil->getTheModelPtr(), mydmsPlan, am0Cor);
      
    } 
    for (size_t itSteer = 0; itSteer != mydmsPlan.numMaxLocalIteration; itSteer++ ) { 
     
      cout << " Steering::SteerPureDMSLocal At iteration.. " << itSteer << endl; 
     
      // count the number of broken dipoles. 
      // Check if we have a good dipole 
      numBadDip = 0;
      for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;

      Vector responderVals(numBPMs);
      if ((!mydmsPlan.usePerfectResponse) && (itSteer > 0) ) {
        delete am0Cor; // get rid of the previous one. 
        am0Cor = new Steering::ResponseMatrix<Steering::ECHANGE>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.rescaleLattice, mydmsPlan.byRFPhase,
				     numBadDip, 
				     mydmsPlan.scaleResponseMatrix, mydmsPlan.numPseudoPartRP);
	am0 = am0Cor;			     
        delete am0CorOtO; // Same for 1- 1, get rid of the previous one. 
        am0Cor->setEnergy(false, false); // Reset Linac, just to make sure..  
        am0CorOtO = new Steering::ResponseMatrix<Steering::CORDIP>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), 
                                     firstBPM, numBPMs, 
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles);
	am0OtO = am0CorOtO;			     
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
      if (!Steering::TestWithBeam(&mydmsPlan, zMax, bpmStartThisIt)) {      
         if (mydmsPlan.usePerfectResponse) {
	    delete am0; delete am0OtO; am0CorOtO = 0;
	  }
         delete am0Cor; delete am0CorOtO;
         return mydmsPlan.numMaxLocalIteration+1; // don't go any further on this iteration... 		  
      }
    // collect the reading from the BPMs 
      double valBPMsFullE[numBPMs];
      size_t kBpm = 0;
      Vector bpmsValsOtO(bpmsTmp.size());
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    valBPMsFullE[kBpm] = (*itBpm)->GetMeasurement();
	    bpmsValsOtO(kBpm) = -1.0* 1.0e-6*valBPMsFullE[kBpm]; // change units, and sign
            (*itBpm)->Reset();
	  kBpm++;    
      }
      // Now compute the expected correction assuming One to One steering. 
      
      size_t nL = mydmsPlan.numDipoles; // No broken dipole assumed, number of unknows..
      // Now do the SVD inversion. Don't bother with the square case
      // Suspicious, use new SVDFit class. 
      Vector wOtO(bpmsTmp.size());  // Will be the W matrix in SVD formalism 
      for (size_t i = 0; i != nL; i++) wOtO(i) = 1.;
      Matrix um0OtO(nL, numBPMs);
      Matrix vm0TOtO(nL, nL);
      am0OtO->SVD(um0OtO, wOtO, vm0TOtO);
      Vector utyTmpOtO(um0OtO.transpose()*bpmsValsOtO);
      for (size_t iM = 0; iM != nL; iM++) {
	 if (std::abs(wOtO(iM)) > mydmsPlan.minWeightSVD) 
	        utyTmpOtO(iM) /= wOtO(iM); // Arbitrary !!! 
	 else utyTmpOtO(iM) = 0.;
      }
      Vector diPValsOtO(nL);
      diPValsOtO = 1.0*vm0TOtO*utyTmpOtO;

      
      am0Cor->setEnergy(false, true);
      if (!Steering::TestWithBeam(&mydmsPlan, zMax, bpmStartThisItOffE)) {
          if (mydmsPlan.usePerfectResponse) {
	    delete am0; delete am0OtO; am0CorOtO = 0;
	  }
         delete am0Cor; delete am0CorOtO;
         return mydmsPlan.numMaxLocalIteration+1; // don't go any further on this iteration... 		  
      }      
      kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	   int iBPMIndex = (*itBpm)->GetId();
	   double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	   double relDeltaE = am0Cor->getEnergyOffset()/nominalE;
	   double disp = (valBPMsFullE[kBpm] - (*itBpm)->GetMeasurement())/relDeltaE;
	   disp -= theSteerUtil->getDispersionAtBPM(iBPMIndex);
	   responderVals(kBpm) = 1.0e-6*disp;  // scale factor back to meter. 
            (*itBpm)->Reset();
	kBpm++;    
      }
      // Linac reset 
      am0Cor->setEnergy(false, false);
      // Compute the correction
      // Compute the inverse or the pseudoinverse
      Vector diPVals(nL);
      bool isRegularSquare = (nL == numBPMs); // No idea how regular this is....
      if (isRegularSquare ) {
        Matrix m0 = am0->inverse();
	diPVals = m0*responderVals;
      } else {
	   // Pseudo inverse, Using Singular Value Decomposition  
      
         Vector w(numBPMs);  // Will be the W matrix in SVD formalism
	 // Use new SVDFit class!   
	 Matrix um0(nL, numBPMs);
	 Matrix vm0T(nL, nL);
	 // To be adjusted, possible improvements here
	 for (size_t i = 0; i != nL; i++) w(i) = 1.;
	 am0->SVD(um0, w, vm0T);  // Perhaps bad notation here, should be called vm0
	 Vector utyTmp(um0.transpose()*responderVals);
	  for (size_t i = 0; i != nL; i++) {
	     if (std::abs(w(i)) > mydmsPlan.minWeightSVD) utyTmp(i) /= w(i); // Arbitrary !!! 
             else utyTmp(i) = 0.;
	 }
	 diPVals = vm0T*utyTmp;
      }
      localIterData newIt(mydmsPlan, ((int) itSteer));
      // Now we do the weigthing... 
      Vector diPValsW(nL);
      for (size_t k=0; k != nL; k++) diPValsW(k) = mydmsPlan.weight1to1DMS * diPValsOtO(k) + 
                                           (1.0-mydmsPlan.weight1to1DMS) * diPVals(k);
      
      newIt.fillSuggested(diPValsW);
      if (!newIt.appraise(mydmsPlan, itResults)) numConsecutiveBad++;
      // Loop if we made a significant change. if so, we break, we done... 
      if (newIt.areWeDone(mydmsPlan))  {
        if (doPreSteer) { // Restore the previous dipoles.. 
          for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
          }
	}
        if ((!mydmsPlan.useCryoTilt) || (!mydmsPlan.doDynamicalPrelim)) { 
	  // if we tilt the cavities, these bunches are not valid..
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...
	   // if Dynamical, skip as well..        
           am0Cor->setEnergy(false, true);
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 false, 0, true);
           am0Cor->setEnergy(false, false);
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 false, 0, true);
         }
         if (mydmsPlan.usePerfectResponse) delete am0;
	 delete am0Cor;
         return (itSteer+1);
      }
      if (numConsecutiveBad > 1) {
        cout << " Steering::SteerPureDMSLocal, Two bad consecutive iteration after " << 
	     (itSteer+1) << " iterations, quit on this one " << endl;
        if (mydmsPlan.usePerfectResponse) {
	   delete am0; delete am0OtO; am0CorOtO = 0;}
	delete am0Cor; delete am0CorOtO;
        if (doPreSteer) { // Restore the previous dipoles.. 
          for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
          }
	}
        return mydmsPlan.numMaxLocalIteration;
      }
      newIt.applyChanges(mydmsPlan);
      itResults.push_back(newIt);
      numConsecutiveBad = 0;
    } // on Iteration.. 
    if (mydmsPlan.usePerfectResponse) {
	   delete am0; delete am0OtO; am0CorOtO = 0; 
     }
    delete am0Cor; delete am0CorOtO;
    if (doPreSteer) { // Restore the previous dipoles.. 
      for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
      }
    }
    return mydmsPlan.numMaxLocalIteration;

}
// Mixed, 1 - 1 and DMS, with Merlin type Matrices  
int Steering::SteerWeightedDMSLocalMerlin(BmlPtr beamLine, 
                  const DMSPlan &mydmsPlan) {
	
    // Set the wake off, as we will send pilot beam, low current. 
    	  
    theSteerUtil->setWake(false);
    	  
    std::string labelMat("SteerDFSPureLocal_v1");
    bool printDebugTestBeam = false;
    bool printMinimum = false;
//    printKickAngles(beamLine, 30);
    Vector stateInInj = theSteerUtil->getStateAtInjection(); 
    
     Vector stateInBC(6); 
    for (size_t k=0; k != 6; k++) stateInBC[k] = 0.;
    if (mydmsPlan.useReducedFieldInBunchCompressor) { 
          stateInBC[5] = mydmsPlan.gradientFraction - 1.0;
 	  stateInBC[1] += theSteerUtil->getInputVerticalDispersion() * stateInBC[5];
          stateInBC[4] += theSteerUtil->getInputVerticalDispersionPrime() * stateInBC[5];
       }
    stateInBC[1] += stateInInj[1]; //Kirti and Pt beam shift. 
    stateInBC[4] += stateInInj[4];
    if (printMinimum) 
     std::cout << " From SteerWeightedDMSLocalMerlin, Dump of Disp at BPM  " << endl;
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t firstBPM = mydmsPlan.firstDipole+1;
    size_t numBPMs = mydmsPlan.numDipoles + mydmsPlan.numExtraBPMs;  // NOT square matrix problem. 
//    for (int kBPM=firstBPM; kBPM != firstBPM+numBPMs; kBPM++) 
//      std::cerr << " BPM number " << kBPM << " Dy = " 
//                <<  theSteerUtil->getDispersionAtBPM(kBPM) << std::endl;

    if (mydmsPlan.doLastSegment) numBPMs = mydmsPlan.numDipoles;
    size_t indexBPMMax =  firstBPM + numBPMs;
    size_t numCav = mydmsPlan.numKlystrons*mydmsPlan.numCavitiesPerKlystrons;
    bool doPreSteer = false;
    unsigned int nnDipolesPreSteering = 3;
    if (mydmsPlan.usePreSteer &&  
        ((mydmsPlan.firstDipole - mydmsPlan.numDipolesPreSteering) >= 0)) {
       doPreSteer = true;  
       nnDipolesPreSteering = mydmsPlan.numDipolesPreSteering;
    }
    double dipValPre[nnDipolesPreSteering]; 
    if (indexBPMMax >= theSteerUtil->getNumBPMs()) {
      cout << " Steering::SteerDMSLocalMerlin: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
         numBPMs = theSteerUtil->getNumBPMs() - firstBPM - 1;
      cout << " .... firstBPM " << firstBPM << " ...new numBPMs = " << numBPMs << endl;
    }
    
    std::vector<localIterData> itResults;
    size_t numConsecutiveBad = 0;
    Matrix* am0 = 0;
    // Define one steering matrix no matter what, as we will use the ancillary 
    // information in it to measure the deviations.
    size_t numBadDip = 0;
    for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;
    if (numBadDip != 0) {
      cerr << 
      " SteerWeightedDMSLocal : Bad dipoles detected... Not yet supported " << endl;
      return mydmsPlan.numMaxLocalIteration;
    }
    Steering::ResponseMatrix<Steering::ECHANGEMERLIN> *am0Cor = 0; 
    if (mydmsPlan.usePerfectResponse) { // indepedent of iterations ! 
      size_t indexRepository = 0;
      if (mydmsPlan.doLastSegment) indexRepository = mydmsPlan.responsesPerfect.size() - 1;
      else {
        indexRepository = mydmsPlan.firstDipole - mydmsPlan.macroFirstDipole;
	indexRepository /= mydmsPlan.macroNumDipIncrement;
      }
      am0 = new Matrix(mydmsPlan.responsesPerfect[indexRepository]);
      am0Cor = new Steering::ResponseMatrix<Steering::ECHANGEMERLIN>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC, 
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.weightAbsolute, mydmsPlan.weightDMS,
				     mydmsPlan.weightCorr, 
				     mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, -2,
				     mydmsPlan.usePerfectDispKnow);
    } else {
       am0Cor = new Steering::ResponseMatrix<Steering::ECHANGEMERLIN>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.weightAbsolute, mydmsPlan.weightDMS,
				     mydmsPlan.weightCorr, 
				     mydmsPlan.byRFPhase,
				     numBadDip, mydmsPlan.scaleResponseMatrix, 
				     mydmsPlan.numPseudoPartRP,
				     mydmsPlan.usePerfectDispKnow);
       am0 = am0Cor;
       am0Cor->setEnergy(false, false); // Reset Linac, just to make sure..  
       
    }  
    int bpmStartThisIt = mydmsPlan.firstDipole - 2;
    // If off energy, must start one upstream, as we will store the good beam with 
    // the off energy configuration of this iteration.. 
    // Optimization for speed not very robust.. 
    int bpmStartThisItOffE = mydmsPlan.firstDipole - 3;
    if (doPreSteer) { // We will re-tweak upstream dipoles.. 
      bpmStartThisItOffE -= 3;
      bpmStartThisIt -= 3;
    }    
    if (bpmStartThisIt < 0) bpmStartThisIt = 0;
    if (bpmStartThisItOffE < 0) bpmStartThisItOffE = 0;
    // Pre-Steering to orbit... 
    if (doPreSteer) {
      for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
        unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	dipValPre[i] = theSteerUtil->GetOneDipoleSetting(iP);
      }
      Steering::PreSteerToOrbit( theSteerUtil->getTheModelPtr(), mydmsPlan, am0Cor);
      
    } 
     
    for (size_t itSteer = 0; itSteer != mydmsPlan.numMaxLocalIteration; itSteer++ ) { 
     
      if (printDebugTestBeam) 
         cerr << " Steering::SteerPureDMSLocal At iteration.. " << itSteer << endl; 
     
      // count the number of broken dipoles. 
      // Check if we have a good dipole 
      numBadDip = 0;
      for (size_t i=0; i != mydmsPlan.numDipoles; i++) 
         if (!theSteerUtil->isDipoleOK(i+mydmsPlan.firstDipole)) numBadDip++;

      Vector responderVals(3*numBPMs);
      if ((!mydmsPlan.usePerfectResponse) && (itSteer > 0) ) {
        delete am0Cor; // get rid of the previous one. 
        am0Cor = new Steering::ResponseMatrix<Steering::ECHANGEMERLIN>(labelMat, 
                                     theSteerUtil->getTheModelPtr(), stateInBC,
                                     firstBPM, numBPMs, mydmsPlan.gradientFraction,
				     mydmsPlan.firstCavity, numCav,
				     mydmsPlan.firstDipole, mydmsPlan.numDipoles,
				     mydmsPlan.weightAbsolute, mydmsPlan.weightDMS,
				     mydmsPlan.weightCorr, 
				     mydmsPlan.byRFPhase,
				     numBadDip, 
				     mydmsPlan.scaleResponseMatrix, 
				     mydmsPlan.numPseudoPartRP, 
				     mydmsPlan.usePerfectDispKnow);
	am0 = am0Cor;			     
        am0Cor->setEnergy(false, false); // Reset Linac, just to make sure..  
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
      if (!Steering::TestWithBeam(&mydmsPlan, zMax, bpmStartThisIt, printDebugTestBeam)) {
         if (printDebugTestBeam) 
           cerr << " Steering::SteerPureDMSLocal Test beam failed..at iter " 
	        << itSteer << endl; 
         if (mydmsPlan.usePerfectResponse) {
	    delete am0;
	  }
         delete am0Cor;
         return mydmsPlan.numMaxLocalIteration+1; // don't go any further on this iteration... \
	 		  
      }
    // collect the reading from the BPMs 
      double valBPMsFullE[numBPMs];
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    valBPMsFullE[kBpm] = (*itBpm)->GetMeasurement();
            (*itBpm)->Reset();
	  kBpm++;    
      }
      // Now compute the expected correction assuming weighted 1-to-1 and DMS..
      
      size_t nL = mydmsPlan.numDipoles; // No broken dipole assumed, number of unknows..
      
      am0Cor->setEnergy(false, true);
      if (!Steering::TestWithBeam(&mydmsPlan, zMax, bpmStartThisItOffE, printDebugTestBeam)) {
           cerr << " Steering::SteerPureDMSLocal Test beam failed..at iter " 
	        << itSteer << " diff E " << endl; 
          if (mydmsPlan.usePerfectResponse) {
	    delete am0; 
	  }
         delete am0Cor; 
         return mydmsPlan.numMaxLocalIteration+1; // don't go any further on this iteration... 		  
      }      
      kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	   int iBPMIndex = (*itBpm)->GetId();
	   double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	   double relDeltaE = am0Cor->getEnergyOffset()/nominalE;
	   double disp = (valBPMsFullE[kBpm] - (*itBpm)->GetMeasurement())/relDeltaE;
	   double dispMeas = disp;
	   // We do not have to subtract the nominal Dispersion if the 
	   // the Response matrices are computed from the perfect lattice. 
	   // This is already taken in... 
	   if (mydmsPlan.usePerfectDispKnow)
	     disp -= theSteerUtil->getDispersionAtBPM(iBPMIndex);
	   if (printDebugTestBeam) {
	     cerr << " Measured Dispersion " << dispMeas << " Residual " 
	         << disp << "  DE/E " << relDeltaE << " Nominal E " << nominalE << endl;
	     cerr << " BPM Full Val  " << valBPMsFullE[kBpm] 
	        << " BPM Off " << (*itBpm)->GetMeasurement() << endl;
           }
	   responderVals(kBpm) = 1.0e-6*valBPMsFullE[kBpm];  // scale factor back to meter. 
            (*itBpm)->Reset();
	   responderVals(kBpm+numBPMs) = 1.0e-6*disp;  // scale factor back to meter. 
           unsigned int iPResp=mydmsPlan.firstDipole + kBpm;
	   responderVals(kBpm+2*numBPMs) = std::abs(theSteerUtil->GetOneDipoleSetting(iPResp))*
	                                         theSteerUtil->getTheBeamEnergyInput() / 
	                                         theSteerUtil->getRefMomentumAtDipole(iPResp);  
	                                // scale factor back to meter. 
	kBpm++;    
      }
      // Linac reset 
      am0Cor->setEnergy(false, false);
      // Compute the correction
      // Compute the inverse or the pseudoinverse
      Vector diPValsW(nL);
      // 
      // Based on SVD from CHEF. Comment this out for now and use TLAS SVD for 
      // direct comparison Merlin/CHEF. 
      //
/*
      cout << endl << " Dump of Measurement " << endl << " BPM resp " << endl;
      for (size_t ik=0; ik != numBPMs; ik++) {
        cout << " " << ik << " " << responderVals(ik) 
	            << " " << responderVals(ik+numBPMs) << endl;
      }
*/
 // Oct 24... Still broken... 
// November first, an other daya, an other try...
// Much, much better..  
//
// Pseudo inversion, SVD from CHEF/Num.Rec. 
// 
     if (printDebugTestBeam) cerr << " ... Start computing correction .. " << endl;
     size_t numData = 3*numBPMs;      
     Vector diPVals(nL);
     if (numData >= nL) {
     
      Matrix w(numData);  // Will be the W matrix in SVD formalism 
         // 
	 // To be adjusted, possible improvements here
	 // This is inconsistent !!! Weights are used differently in both packages!! 
	 //
      if (mydmsPlan.weightAbsolute > 0.) {  
           for (size_t i = 0; i != numBPMs; i++) w(i) = 
                                     1.0/(mydmsPlan.weightAbsolute*mydmsPlan.weightAbsolute);
      } else {
           for (size_t i = 0; i != numBPMs; i++) w(i) = (1+i+firstBPM)*(1+i+firstBPM)/
	                                (mydmsPlan.weightAbsolute*mydmsPlan.weightAbsolute);
      }			     
      for (size_t i = 0; i != numBPMs; i++) w(numBPMs+i) = 
                                            1.0/(mydmsPlan.weightDMS*mydmsPlan.weightDMS);
      for (size_t i = 0; i != numBPMs; i++) w(2*numBPMs+i) = 
                                            1.0/(mydmsPlan.weightCorr*mydmsPlan.weightCorr);
      Matrix data(numData); // unnecessary copy for now.. 
      for (size_t i=0; i != numData; i++) data(i) = responderVals(i);       
      SVDFitPL aFit;
      aFit.setLinearResponse( *am0 );
      aFit.setLimitNullSpace(mydmsPlan.minWeightSVD);

      aFit.setErrorCovariance( w );
      diPVals = aFit.solve(data);
      
      // Still need to change the sign for SVD - CHEF
      for (size_t ik=0; ik != nL; ik++) diPVals(ik) *= -1.0;
   } else {
      cerr << " Pseudo-inversion of under-constrained problem illegal " << endl;
      cerr << " numData = " << numData << " num constraints " << nL << endl;
      cerr << " Quit !" << endl;
      exit(2); 
   }
      localIterData newIt(mydmsPlan, ((int) itSteer));
      // 
      // ??? Which one do we choose for criteria.. To be investigated...
      // I don't trust anymore SVD from CHEF for pseudo inversion where the 
      // the number of constraint is higher than the dimension of the solution 
      // (# of dipole correctors )
      //
      // Load TLAS SVD 
//      newIt.fillSuggested(diPValsTLas);
      // Load CHEF SVD 
      newIt.fillSuggested(diPVals);
      if (!newIt.appraise(mydmsPlan, itResults)) numConsecutiveBad++;
      // Loop if we made a significant change. if so, we break, we done... 
      if (newIt.areWeDone(mydmsPlan, printMinimum))  {
//         cerr << " We seem to be done !!!... " << endl;
        if (doPreSteer) { // Restore the previous dipoles.. 
          for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
          }
	}
	// Temporary removal of this... 
        if ((!mydmsPlan.useCryoTilt) || (!mydmsPlan.doDynamicalPrelim)) { 
	  // if we tilt the cavities, these bunches are not valid..
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...
	   // if Dynamical, skip as well..        
           am0Cor->setEnergy(false, true);
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 printDebugTestBeam, 0, true);
           am0Cor->setEnergy(false, false);
	   // We send one more bunch, and this time we load this good beam, 
	   // as the beam line is tuned up to some point...
	   theSteerUtil->sendRealBunch(mydmsPlan.numPseudoParticles, zMax, 
	                                 printDebugTestBeam, 0, true);
         }
         if (mydmsPlan.usePerfectResponse) delete am0;
	 delete am0Cor;
//         cerr << " Done iterating, satisfactory convergence, good return at iter " 
//	      << itSteer+1 << endl;
         return (itSteer+1);
      }
      if (numConsecutiveBad > 1) {
        if (doPreSteer) { // Restore the previous dipoles.. 
          for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
          }
	}
        cout << " Steering::SteerPureDMSLocal, Two bad consecutive iteration after " << 
	     (itSteer+1) << " iterations, quit on this one " << endl;
        if (mydmsPlan.usePerfectResponse) delete am0; 
	delete am0Cor;
        return mydmsPlan.numMaxLocalIteration;
      }
      newIt.applyChanges(mydmsPlan);
      itResults.push_back(newIt);
      numConsecutiveBad = 0;
      
      // Update on the message file 
      
      std::ostringstream gridUptStrStr;
      gridUptStrStr << " DMSLocalMerlin, Macro it. = " << mydmsPlan.currentMacroIteration 
		     << " iteration " << itSteer 
                    << " 1st Dip " << mydmsPlan.firstDipole;
      std::string gridUptStr = gridUptStrStr.str();	     
      theSteerUtil->gridUpdate(gridUptStr, true); 
//      std::cerr << " finished first iteration ,and quit .. " << endl; exit(2);      
    } // on Iteration.. 
    if (mydmsPlan.usePerfectResponse) delete am0; 
    delete am0Cor; 
    if (doPreSteer) { // Restore the previous dipoles.. 
       for (unsigned int i=0; i != nnDipolesPreSteering; i++) {
            unsigned int iP=mydmsPlan.firstDipole - nnDipolesPreSteering + i;
	    theSteerUtil->SetOneDipole(iP, dipValPre[i]);
       }
    }
    return mydmsPlan.numMaxLocalIteration;
}

		   
template<Steering::actuator A>
int Steering::PreSteerToOrbit(BmlPtr beamLine, const DMSPlan& aPlan, 
                                         ResponseMatrix<A> *aRespMatrix)
 {
//    size_t numBPMsOnOff = 3; 
    size_t numBPMsOnOff = aPlan.numBPMsPreSteering; 
    size_t numDipsOnOff = aPlan.numDipolesPreSteering;
    double gainFactPreSteer = aPlan.gainChangeDipole;
    if (gainFactPreSteer > 0.5) gainFactPreSteer = 0.5;
    size_t fBPM = aPlan.firstDipole;
    cout << " Entering PreSteerToOrbit... firstDipole DFS " 
         << aPlan.firstDipole << " NumDipsOnOff " << numDipsOnOff 
	 << " NumBPMsOnOff " << numBPMsOnOff <<  endl;
    int firstDip = aPlan.firstDipole - numDipsOnOff; 
    if (firstDip < 0) {
       cout << " Can not do pre-steer, no upstream beam line " << endl;
       return -1; // Can't do it, too early.
    }
    std::string labelMat("");
    double valDip0Start = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip);
    double valDip1Start = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip+1);
//    double valDip2Start = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip+2);
    int bpmStartThisIt = firstDip-4; // only 3 should be sufficient, yet... 
    if (bpmStartThisIt < 0) bpmStartThisIt = 0;
    MLBPMs bpmsTmp; 
    int maxNumBPM = numBPMsOnOff; 
    if (aPlan.doMovie) maxNumBPM = aPlan.numDipoles + aPlan.numExtraBPMs; 
    theSteerUtil->getBPMPtrs(bpmsTmp, fBPM, maxNumBPM+1);
    vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin();
    double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
    cout << " Entering PreSteerToOrbit... firstDip PreSteer  " << firstDip 
         << " zMax " << zMax << endl;
    ResponseMatrix<STEERTOORBITONOFFE> m0Corr(labelMat, 
                                                theSteerUtil->getTheModelPtr(),
						fBPM, (size_t) firstDip, aRespMatrix,
						aPlan.scaleResponseMatrix, 1, zMax,
						numDipsOnOff, numBPMsOnOff);
           // Linac reset 
    aRespMatrix->setEnergy(false, false);
    theSteerUtil->setLinacOnEnergyFlag();
      // Assume we can go through...
    
    // Pre-loading Trajectory suspicious.. Run without it !!!! 
    bpmStartThisIt = 0;
        
    if (!Steering::TestWithBeam(&aPlan, zMax, bpmStartThisIt)) 
         return aPlan.numMaxLocalIteration+1;

    // Debugging... Do not use a Bunch, just one particle, like in the Response Matrix
//    Vector stateInTest(Particle::PSD);
//    theSteerUtil->sendPilotBeam(stateInTest, zMax);
         
    double bpmFullE[numBPMsOnOff];
    itBpm = bpmsTmp.begin();
    size_t kbp = 0;
    while (itBpm != bpmsTmp.end()) { 
        bpmFullE[kbp] = (*itBpm)->GetMeasurement(); (*itBpm)->Reset(); itBpm++; kbp++;}
    cout << " Full E BPM " << (*bpmsTmp.begin())->GetId() << "= " << bpmFullE[0] 
         << " next = " << bpmFullE[1] << " last " << bpmFullE[numBPMsOnOff-1] << endl;
	 
    // Always off energy while we tune.  
    
    aRespMatrix->setEnergy(false, true);
    theSteerUtil->setLinacOffEnergyFlag();
    for (size_t itSteer = 0; 
             itSteer != aPlan.numMaxLocalIteration; itSteer++ ) { 
     
//      Vector stateInTest2(Particle::PSD);
//      theSteerUtil->sendPilotBeam(stateInTest2, zMax);
      if (!Steering::TestWithBeam(&aPlan, zMax, bpmStartThisIt)) 
          return (aPlan.numMaxLocalIteration+1); 
      double bpmOffE[numBPMsOnOff];
      itBpm = bpmsTmp.begin();
      kbp = 0;
      while (itBpm != bpmsTmp.end()) { 
        bpmOffE[kbp] = (*itBpm)->GetMeasurement(); (*itBpm)->Reset(); itBpm++;  kbp++;}
//      cerr << " Off E BPM " << (*bpmsTmp.begin())->GetId() << "= " << bpmOffE[0] 
//           << " next = " << bpmOffE[1] << " last " << bpmOffE[numBPMsOnOff-1] << endl;
      Vector responderVals(numBPMsOnOff);
      Matrix w(numBPMsOnOff);
      for (size_t k=0; k != numBPMsOnOff; k++) { 
         responderVals(k) = (bpmOffE[k] - bpmFullE[k]);
	 w(k) = 1.0;
      } //The last three BPM confuse things.. Don't know why!  
//      w(3) = 100000.; w(4) = 200000.; w(5) = 5000000.;
      Matrix data(numBPMsOnOff); // unnecessary copy for now.. 
      for (size_t i=0; i != numBPMsOnOff; i++) data(i) = responderVals(i); 
      SVDFit aFit;
      aFit.setLinearResponse( m0Corr );
      aFit.setErrorCovariance( w );
      Vector diPVals = aFit.solve(data);
       // Apply... 
       double maxDipChange = 1.0e-12; 
       for (size_t i = 0; i != numDipsOnOff; i++) {
          // Change of units.. 
	  diPVals(i) *= -1.e-6;
       // small gain, not understanding why it divergences in some cases. 
          double vv =  gainFactPreSteer*diPVals(i);
      	  theSteerUtil->AdjustOneDipole(firstDip+i, vv);
	  if (std::abs(diPVals(i)) > maxDipChange ) maxDipChange = std::abs(diPVals(i));			
       }
       maxDipChange /= theSteerUtil->getRefMomentumAtDipole(aPlan.firstDipole);
       cout << " Pre-steer, it " << itSteer << " at 1st dipole " 
            << firstDip << " by " << diPVals(0) <<
               " .. next dip. " <<  diPVals(1) 
//	       << " last " << diPVals(2)  
               << endl;
       cout << " maxDipChange " << maxDipChange  << " minChangeDipole " << 
                aPlan.minChangeDipole << endl;
      if (maxDipChange < aPlan.minChangeDipole) {
         aRespMatrix->setEnergy(false, false);
         double valDip0Last = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip);
         double valDip1Last = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip+1);
//         double valDip2Last = theSteerUtil->GetOneDipoleSetting((unsigned int) firstDip+2);
	 
	 cout << " PreSteer to Orbit convergence..." << endl;
	 cout << "   .. total adjustment  for dipole " << firstDip 
	      << " = " << (valDip0Last-valDip0Start)
	      <<  " .. next dip. " 
	      <<  (valDip1Last-valDip1Start) 
//	      << " last " << (valDip2Last-valDip2Start) 
	      << endl;
	 return itSteer;      
       }
        
       			
						
   } // on iterations..  
   
   aRespMatrix->setEnergy(false, false);
   theSteerUtil->setLinacOnEnergyFlag();
   cout << " PreSteer to Orbit NO convergence..." << endl;
   cout << " Reverting to previous solution " << endl;	      
   theSteerUtil->SetOneDipole(firstDip, valDip0Start);
   theSteerUtil->SetOneDipole(firstDip+1, valDip1Start);
//   theSteerUtil->SetOneDipole(firstDip+2, valDip2Start);
   return aPlan.numMaxLocalIteration+1;      


}
// Real implementation for ECHANGE and the Merlin version  
template 
int Steering::PreSteerToOrbit(BmlPtr , const DMSPlan& , 
                                         ResponseMatrix<Steering::ECHANGE> *);
template 
int Steering::PreSteerToOrbit(BmlPtr , const DMSPlan& , 
                                         ResponseMatrix<Steering::ECHANGEMERLIN> *);

// Utility routine to start testing dynamical effect, simple version (in terms of software infra.)

bool Steering::TestWithBeam(const DMSPlan* aPlan, double zMax, int bpmStart, bool pDebug) {

   if (theSteerUtil == 0) {
     cerr << " Steering::testWithBeam : Error, Steering utility not available, quit " << endl;
     return false;
   }
   bool okBeam = false;
   
   
   if (aPlan->doDynamicalPrelim) {
   
// Test first if we are not taking too long.. for practical reasons, we have finit number of 
// pulses available.

    int numSoFar = theSteerUtil->GetPulseCount();
    if (numSoFar > aPlan->maxNumPulseTotal) {
       std::cerr << " Reached Maximum number of ILC pulses in this job. Quit Here, Abbruptly. " << endl;
       std::cout << " Reached Maximum number of ILC pulses in this job.  Quit Here, Abbruptly. " << endl;
       exit(2);
    }

    time_t tNow = time(0);
    int dt = (int) difftime(tNow, theSteerUtil->getStartingSteeringTime()); 
    if (dt > aPlan->maxNumSecondsTotal) {
       std::cerr << " Reached Maximum time while Steering in this job. Quit Here, Abbruptly. " << endl;
       std::cout << " Reached Maximum time while Steering in this job.  Quit Here, Abbruptly. " << endl;
       exit(2);
    }
// Need an algorithm to determine how many pulse we need..

// Note the conversion from bunches to pulse: do we only use the pilot bunches. 

     int numBunches = theSteerUtil->getPlannedPulses();
     // It may not come only from SteerUtil, that class is already too big..
     // We always start from the beginning of the line, for now.. 
     okBeam = theSteerUtil->sendRealBunches(numBunches, aPlan->numPseudoParticles, zMax);     
   } else 
     okBeam = theSteerUtil->sendRealBunch(aPlan->numPseudoParticles, zMax, pDebug, bpmStart);
   
//	 cerr << " Quadrupole 10 position " << 
//	         1.0e6*theSteerUtil->GetOneQuadrupoleYTrans(10) << endl;     
   if (!okBeam) {
         cerr << " Hitting aperture, ... " << endl;
	 cerr << "  .... Surprising this happens.. Stop this phase .. " << endl;
         // Linac reset 
         theSteerUtil->resetLinacGradient();
	 theSteerUtil->setLinacOnEnergyFlag();
	 return false;
   }
   return true;


}

