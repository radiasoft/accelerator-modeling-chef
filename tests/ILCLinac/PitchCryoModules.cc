#include <cmath>
#include <iostream>
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"
#include <time.h>
#include <SVDFit.h>
// 
// 
// Part of a collection of algorithm, using SteerUitl and 
// SteeringRespMatrixTwo to correct cavity tilts by tilting the cryo-modules.  
//

using std::cerr;
using std::endl;
using std::cout;


Steering::CryoTiltPlan::CryoTiltPlan(DMSPlan& aBiggerPlan):
theDMSPlan(&aBiggerPlan),
firstCavityPitched(48),
numKlystronsPitched(1), // ignored in this version... 
firstCavity(0),
numKlystrons(2), 
firstBPM(3),
numBPMs(5),
numCavitiesPerCryo(12),
minimumTilt(8e-6), // 8 microRadiants. See how it works..
numBPMsPerKlystrons(1.),
gradientFraction(aBiggerPlan.gradientFraction), 
usePerfectResponse(false),
responsesPerfect(0) 
{ }

// Obsolete!!! do not use ..... 
void Steering::CryoTiltPlan::fillResponsesPerfect() { // Untested...

  if (theSteerUtil == 0) {
    cerr << " No Steering utility activated... must be defined to proceed..." << endl;
    exit(2);
  } else { 
    cerr << " Steering::CryoTiltPlan::fillResponsesPerfect: Obsolete...Do not use..." << endl;
    exit(2);
  }
  size_t numTotalKlystrons = theSteerUtil->GetNumCavities()/theDMSPlan->numCavitiesPerKlystrons;
  theSteerUtil->ResetPulseCounter();
  size_t fCavPitched = firstCavityPitched;
  size_t nCav = numKlystrons*theDMSPlan->numCavitiesPerKlystrons;
  size_t nCavPitched = numKlystronsPitched*theDMSPlan->numCavitiesPerKlystrons;
  
  bool klystronsLeft = 
   (fCavPitched/theDMSPlan->numCavitiesPerKlystrons   < numTotalKlystrons);
  gradientFraction = theDMSPlan->macroGradientFraction;  
  size_t fCav = firstCavity;
  size_t fBPM = firstBPM;
  while (klystronsLeft) {
       std::string labelMat("tmp");    
       size_t numCavTmp = numKlystrons * theDMSPlan->numCavitiesPerKlystrons;
       // Let us ge the matrix.. 
      Steering::ResponseMatrix<Steering::CRYOTILT> m0tmp(labelMat, 
                                     theSteerUtil->getTheModelPtr(),
				     fBPM, numBPMs, gradientFraction,
                                     fCav, nCav, fCavPitched, nCavPitched, 
				     nCavPitched/numCavitiesPerCryo);
      // Not sure if I have to make a copy... Just do it to play safe. 
       Matrix mct(m0tmp);
       responsesPerfect.push_back(m0tmp);
       // Shift everything... 
       fCavPitched += theDMSPlan->numCavitiesPerKlystrons * numKlystronsPitched;
       fCav += theDMSPlan->numCavitiesPerKlystrons * numKlystronsPitched;
       fBPM += (int) ((float) theDMSPlan->numCavitiesPerKlystrons 
                           * numKlystronsPitched * numBPMsPerKlystrons);
       klystronsLeft = 
        (fCavPitched/theDMSPlan->numCavitiesPerKlystrons   < numTotalKlystrons);
  } // Looping section by section..
}

void Steering::CryoTiltPlan::printPlan() {
  cout << endl;
  cout << " ---- Plan for Tilting Cryo-modules -- " << endl;
  cout << " This Part .. " << endl;
  
  cout << " first Cavity Pitched " <<  firstCavityPitched 
       << " num Kystrons Pitched " << numKlystronsPitched << endl; 
  cout << " first Cavity Upstream section " <<  firstCavity
       << " num Kystrons in Upstream  " << numKlystrons << endl; 
  cout << " first BPM  " <<  firstBPM
       << " number of BPMs   " << numBPMs << endl; 
  cout << " Gradient Fraction  " << gradientFraction 
       << " Ratio of number of Klystrons per BPM    " << numBPMsPerKlystrons << endl; 
  cout << " Minimum tilt factor " << minimumTilt << endl;
  cout << " ------------------------------ " << endl;
  theDMSPlan->printPlan();

}

// For a given DMS section, we need to do multiple passes on various 
// 
int Steering::CorrectCryoTilt(BmlPtr beamLine, const DMSPlan aDMSPlan, const char *dirPath) {

   // make a copy of this plan, since, we need to adapt it.. we are copying unnecessarily 
   // dozens of DMS response matrices, but what the hell. Copy is fast. 
   Steering::DMSPlan aNewPlan(aDMSPlan);
   
   std::string fOutDirPathDipTilt(dirPath); fOutDirPathDipTilt += std::string("Tilt/");
   std::string cmd("/bin/mkdir "); cmd += fOutDirPathDipTilt;
   system(cmd.c_str());
   int nTotalIter = 0;
   if (aNewPlan.doMovie) { // choice of BPM a bit arbitrary.. 
         theSteerUtil->clearBPMsForDisplay();
	  theSteerUtil->addBPMForDisplay(aNewPlan.firstDipole +  aNewPlan.numDipoles - 1);
  }
  int nAllTries = 0;
  const int maxNumAllTries = 2;
   // We go back 3 kylstrons...Hard type for now... 
  const int numCryoModInBetweenBPMs = 3;  
  while ( nAllTries < maxNumAllTries) {
    int iStep = 0;
    bool cryoTiltSection = true;
    	
    while ( (iStep != (aNewPlan.macroNumDipoles-1)) && cryoTiltSection) {
      // Assume we have one klystron per cryo-module here !!! 
        // we must enough klystrons upstream to do the trick.  
        if (aNewPlan.numKlystrons < 2) {
          aNewPlan.numKlystrons++;
	  cout << " Not enough Upstream Klystrons, assumed aligned " << endl; 
          continue;  // The are the total number of Klystrons 
        }                                          // upstream of the DMZ 
        CryoTiltPlan aCryoPlan(aNewPlan);
       aCryoPlan.firstCavity = aNewPlan.firstCavity;
       aCryoPlan.numKlystrons = aNewPlan.numKlystrons - numCryoModInBetweenBPMs; 
       aCryoPlan.numBPMs = 4; // To be tuned.. not sure where the optimum 
       aCryoPlan.firstCavityPitched = aNewPlan.firstCavity + 
                aCryoPlan.numKlystrons*aCryoPlan.theDMSPlan->numCavitiesPerKlystrons;
	 // Set to old Tesla design, on Benchmark2
       aCryoPlan.numCavitiesPerCryo = 8; // See note below.. Only for Tesla design!   				
       aCryoPlan.firstBPM = aNewPlan.firstDipole-1+iStep;
	 // others parameters are default 
       cout << "Tilting at step " << iStep << " ...1st Cav Pitched  " <<
	       aCryoPlan.firstCavityPitched  << endl;
       if ((aCryoPlan.firstBPM + aCryoPlan.numBPMs) >= theSteerUtil->GetNumBPMs()) {
         cout << " Reaching the end of Linac, not enough BPM s to cryo-tilt align " << endl;
	 break;
       } 
       if (aNewPlan.doMovie) { // Create new directories, file limit reached...
 	  std::string ffLabelFinalTiltC(fOutDirPathDipTilt);
	  ffLabelFinalTiltC += std::string("R");
	  std::ostringstream fOutTmpStr; fOutTmpStr << "R" << iStep << "/";
	  ffLabelFinalTiltC += fOutTmpStr.str();
          std::string cmd2("/bin/mkdir "); cmd2 += ffLabelFinalTiltC;
          system(cmd2.c_str());
	  
	  theSteerUtil->setDisplayToken(ffLabelFinalTiltC);	  
	  theSteerUtil->openFileOutMovieScale ();	  
       }
       //
       // Hard typed number!!! there are 3 cryo-module per dipole correctors. 
       // 	
      for (int iStepCryoMod = 0; iStepCryoMod != numCryoModInBetweenBPMs; iStepCryoMod++) { 
   
        int nnCryoIter = Steering::CorrectCryoTiltLocal(beamLine, aCryoPlan, iStepCryoMod);
       
        if (nnCryoIter < 0) {
          cout << " Cryo-tilting failed nAllTries =  " 
	      << nAllTries << " iStep " << iStep << " iSubStep " << iStepCryoMod << endl;
	  nAllTries++; 
	  if (nAllTries < maxNumAllTries) {
	     cout << " Going back to first dipole/klystron in this section.. " << endl;
	     iStep = 0;
	  } else {
	     cout << " Stopping alltogether the Cryo-tilting for this section " << endl;
	     cryoTiltSection = false;
          }
         } else { // we did not bump too much...  
           if (aNewPlan.doMovie) theSteerUtil->closeFileOutMovieScale ();
	 
           // Moving one klystron at a time. 
           aNewPlan.numKlystrons ++; // Only valid for the Tesla design!... 
	   nTotalIter += nnCryoIter;
         }
       } // on steps within the real kylstrons.. 
       iStep ++;
       
      } // On Steps, one kylstron or dipole at a time... 
      if (cryoTiltSection) break; // if we maded without failing, o.k., we are done..
    } // on tries.. 
    std::string fOutDirPathOrig(dirPath);
    theSteerUtil->setDisplayToken(fOutDirPathOrig);	  

    return nTotalIter;
}
int Steering::CorrectCryoTiltLocal(BmlPtr beamLine, 
                            const CryoTiltPlan& myCryoPlan, int iStepCryoMod) {

    cout << " Entering CorrectCryoTiltLocal.. iStepCryoMod = " << iStepCryoMod << endl;
//    myCryoPlan.printPlan();
    std::string labelMat("V2");
    size_t fCavPitched = myCryoPlan.firstCavityPitched + 
                         iStepCryoMod*myCryoPlan.theDMSPlan->numCavitiesPerKlystrons;
    size_t nCavPitched = myCryoPlan.theDMSPlan->numCavitiesPerKlystrons;
    size_t fCav = myCryoPlan.firstCavity;
    size_t nCav = (myCryoPlan.numKlystrons + iStepCryoMod) 
                            *myCryoPlan.theDMSPlan->numCavitiesPerKlystrons; 
    size_t fBPM = myCryoPlan.firstBPM;
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t nBPM = myCryoPlan.numBPMs;
    size_t indexBPMMax =  fBPM + nBPM;
    size_t numCryoModules = 1;  
    if (indexBPMMax >= theSteerUtil->getNumBPMs()) {
      cerr << " Steering::CorrectCryoTile: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
         nBPM = theSteerUtil->getNumBPMs() - fBPM - 1;
    }
    
    size_t numConsecutiveBad = 0;
    Matrix* am0 = 0; 
    // Define one steering matrix no matter what, as we will use the ancillary 
    // information in it to measure the deviations.
    double gf = myCryoPlan.theDMSPlan->gradientFraction;
    Steering::ResponseMatrix<Steering::CRYOTILT> *am0Cor = 
       new Steering::ResponseMatrix<Steering::CRYOTILT>(labelMat, 
                                     theSteerUtil->getTheModelPtr(),
				     fBPM, nBPM, gf,
                                     fCav, nCav, fCavPitched, nCavPitched,
				     numCryoModules,
				     false, 0.5, 1 );
    am0 = am0Cor;
//    cout << " CorrectCryoTiltLocal, current Missing energy = " << am0Cor->getEnergyOffset() << endl;
    
//    cout << " CryoTilt Matrix ================ " << endl << (*am0Cor) 
//         << endl;
    cout << " List of average cavity tilts prior to alignment...  " << endl;
    std::vector<double> incrTiltVals(numCryoModules);
    for (size_t iM = 0; iM != numCryoModules; iM++) {
	     double averCavTilt = am0Cor->getAverageCavTilts(iM);
	     cout <<  " Initial average cavity tilt for CryoModule " << 
	              (iM+ fCavPitched/myCryoPlan.numCavitiesPerCryo) <<
		       " = " << averCavTilt << endl;
		incrTiltVals[iM] = 0.;       
    }
    for (size_t itSteer = 0; 
             itSteer != myCryoPlan.theDMSPlan->numMaxLocalIteration; itSteer++ ) { 
     
//      cerr << " Steering::SteerPureDMSLocal At iteration.. " << itSteer << endl; 
     

      Vector dispVals(nBPM);
      if ((itSteer > 0) ) { // We'll re-implement UsePerfectResponse at a later stage. 
        delete am0Cor; // get rid of the previous one. 
        am0Cor = new Steering::ResponseMatrix<Steering::CRYOTILT>(labelMat, 
                                     theSteerUtil->getTheModelPtr(),
				     fBPM, nBPM, gf,
                                     fCav, nCav, fCavPitched, nCavPitched,
				     numCryoModules,
				     false, 0.25, 1 );
	am0 = am0Cor;			     
      } 
      // Use a real bunch, and the default value in steerUtil
      MLBPMs bpmsTmp; // just to get a local copy
     // Set the BPMs. We take the one downstream.  This make an assumption on 
     // the lattice is layed out. 
      
      theSteerUtil->getBPMPtrs(bpmsTmp, fBPM, nBPM);
      double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
    // Testing, playing with resolution.  
//    bpmsTmp.setResolution(50.e-6);
      // At reduced energy, pitched cavity at full energy 
      am0Cor->setEnergy(false, true, true);
//      cout << "  Pitched cavity at full energy, Sending real bunch iteration " << itSteer << endl;
//      cout << " Pulse count will be " <<  (theSteerUtil->GetPulseCount() + 1) << endl;
      if (!Steering::TestWithBeam(myCryoPlan.theDMSPlan, zMax))
           return myCryoPlan.theDMSPlan->numMaxLocalIteration;
    // collect the reading from the BPMs 
      double valBPMsFullE[nBPM];
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    valBPMsFullE[kBpm] = (*itBpm)->GetMeasurement();
	    (*itBpm)->Reset();
//	 if (itSteer == 0) 
//	    cerr << " BPM " << (*itBpm)->GetId() << " records " << valBPMsFullE[kBpm] << endl;
	  kBpm++;    
      }
      if (fCavPitched == 99999996) { 	       	       
        cout << " Pitched cavities at reduced energy " << endl;
	std::string aTmpLabel1("CorrectCryoTiltLocalDebugFullE");
	StudyPerformanceDebug(aTmpLabel1, 3, 12);
//        PrintEnergyAtEachBPM(20);
      }
      am0Cor->setEnergy(false, true, false);
     // At full partial energy Assume we'll get through, less kick since less 
      // energy.. 
//      cout << "  .... Pitched cavities at Reduced energy " << itSteer << endl;
//      cout << " Pulse count will be " <<  (theSteerUtil->GetPulseCount() + 1) << endl;
      if (!Steering::TestWithBeam(myCryoPlan.theDMSPlan, zMax))
           return myCryoPlan.theDMSPlan->numMaxLocalIteration;
      kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	   dispVals(kBpm) = -1.0 * ((valBPMsFullE[kBpm] - (*itBpm)->GetMeasurement()));
	   // more reasonable values for the determinant.
	 if (itSteer <  -10) 
	    cerr << " At BPM " << (*itBpm)->GetId() << " value " << 
	    (*itBpm)->GetMeasurement() << "  full E Pitched " 
	    << valBPMsFullE[kBpm] << " dispVal " << dispVals(kBpm) << endl; 
	    (*itBpm)->Reset();
	kBpm++;    
      }
      // Compute the correction via SVD, e.g., the pseudoinverse
      Vector tiltVals(numCryoModules);
      
      Matrix sigCov(nBPM+numCryoModules);  // Will be the W matrix in SVD formalism 
      for (size_t i = 0; i != nBPM; i++) sigCov(i) = 1.0;
//      for (size_t i = 0; i != numCryoModules; i++) sigCov(nBPM+i) =
//                                            myCryoPlan.theDMSPlan->weightAbsolute;
      for (size_t i = 0; i != numCryoModules; i++) sigCov(nBPM+i) = 2.5e-8;
      Matrix data(nBPM+numCryoModules); // unnecessary copy for now.. 
      for (size_t i=0; i != nBPM; i++) data(i) = dispVals(i); 
      for (size_t i = 0; i != numCryoModules; i++) data(nBPM+i) = incrTiltVals[i]; 
      SVDFit aFit;
      aFit.setLinearResponse( *am0 );
      aFit.setErrorCovariance( sigCov );
      
      tiltVals = aFit.solve(data);
//      for (size_t i=0; i != numCryoModules; i++)  tiltVals(i) *= -1.;
      for (size_t i=0; i != numCryoModules; i++) { 
        incrTiltVals[i] += tiltVals(i);
      }
         // in MicroRadiants.  Not sure why I have to change sign..
      cout << " Steering::CorrectCryoTiltLocal, at iter " << itSteer 
            << " proposed tilt correction ";
      for (size_t iM = 0; iM != numCryoModules; iM++)  cout << " " << tiltVals(iM);
      cout << endl;
      bool keepGoing = false;
      for (size_t iM = 0; iM != numCryoModules; iM++) {
         if  (std::abs(tiltVals(iM)) > myCryoPlan.minimumTilt) keepGoing = true;
         if (std::abs(tiltVals(iM)) > 1.5e-3) 
            tiltVals(iM) *= 3.0e-4/std::abs(tiltVals(iM)); // Very dubious...
	 int iMAbs = iM + myCryoPlan.firstCavityPitched/myCryoPlan.numCavitiesPerCryo;   
//	 cerr << " Applying correction at module " << iMAbs << " value " << 0.5*tiltVals(iM) << endl;
           if  (std::abs(tiltVals(iM)) > myCryoPlan.minimumTilt)
	      am0Cor->applyTilt(0.5*tiltVals(iM), iM);  // gain of 1/2 
	}
      if (!keepGoing) {
         cout <<  " and Cryo Tilt steering done at " << itSteer << " iterations " << endl;
         // Linac reset 
         am0Cor->setEnergy(false, false, true);
         cout << " Convergence Success...  " << endl;
         for (size_t iM = 0; iM != numCryoModules; iM++) {
	     double averCavTilt = am0Cor->getAverageCavTilts(iM);
	     cout <<  " Final average cavity tilt for CryoModule " << 
	              (iM+ fCavPitched/myCryoPlan.numCavitiesPerCryo) <<
		       " = " << averCavTilt << endl;
	 }
	 return itSteer+1;
       }
         
    } // on Iteration.. 
    delete am0Cor;
    // Linac reset 
    am0Cor->setEnergy(false, false, true);
    return myCryoPlan.theDMSPlan->numMaxLocalIteration;
//    cerr << " No convergence And quit for now ... " << endl;
//    exit(2);
}
