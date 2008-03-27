#include <cmath>
#include <iostream>
#include <beamline/Alignment.h>
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"
#include <time.h>

using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

namespace Steering {
  //
  // New energy change..Inspired in part by Merlin.  
  // 
    ResponseMatrix<Steering::ECHANGEMERLIN>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double wAbs, double wDMS, double wCorr, 
			 bool withRFPhase,
			 size_t numBrokenDipoles,
			 double kMininScale,
			 int numPart, bool useDisp ):
    gradReduxFact(gradientReduction),
    weightAbsolute(wAbs),
    weightDMS(wDMS),
    weightCorrector(wCorr),			 
    Matrix(3*numBPMs, numDipoles-numBrokenDipoles), // We need two of them, in fact... 			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    stateIn(Particle::PSD),
    mPreSteer(0),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    usePerfectDispKnow(useDisp),
    kScale(kMininScale),
    quadStrength(0),
    numPartPerPulse(numPart)			 
  {
     for (size_t k=0; k != 6; k++) stateIn[k] = 0.;
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                            firstDipole, 
			    numDipoles, numBrokenDipoles);
  }

  ResponseMatrix<ECHANGEMERLIN>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			  size_t firstDipole, size_t numDipoles,
			 double wAbs, double wDMS, double wCorr, 
			  bool withRFPhase,
			  size_t numBrokenDipoles,
			   double kMininScale, int numPart,
			   bool useDisp  ):
    gradReduxFact(gradientReduction),			 
    weightAbsolute(wAbs),
    weightDMS(wDMS),
    weightCorrector(wCorr),			 
    Matrix(3*numBPMs, numDipoles-numBrokenDipoles),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    mPreSteer(0),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    usePerfectDispKnow(useDisp),
    kScale(kMininScale),
    quadStrength(0),
    stateIn(stateInTest),
    numPartPerPulse(numPart)			 
  {
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                   firstDipole, numDipoles, numBrokenDipoles);
  }

// Real constructor here. A lot of silly duplicated code for no good 
// reasons... 

  void ResponseMatrix<ECHANGEMERLIN>::buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firstCavity, size_t numCavities,
		 size_t firstDipole, size_t numDipoles, 
		 size_t numBrokenDipoles) 
  {
//    clock_t beginClock = clock();
//    cerr << " Begin ResponseMatrix, clock " << beginClock << endl;
    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    // Pick the dipoles and the BPMS
    cavities.clear();
    bpms.clear();
    size_t nCavs = 0;
    size_t nBpms = 0;
    size_t nDips = 0;
   for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q) ) {
        if (nDips >= firstDipole) dipoles.push_back(q); 
        if (numDipoles == dipoles.size()) break;
        nDips++;
      }
    }
    // Check the number of declared broken dipoles.. 
    size_t numBadDip = 0;
    for (size_t iDip = 0; iDip != dipoles.size(); iDip++) 
      if (!theSteerUtil->isDipoleOK(iDip+firstDipole)) numBadDip++;
    if (numBadDip != numBrokenDipoles) {
      cerr << 
      "ResponseMatrix<ECHANGEMERLIN>::ResponseMatrix..." << 
      " Alleged Number of broken dipole inconsistent with SteerUtil content ! " << endl;
      cerr << " Num Bad " << numBadDip << " Broken " << numBrokenDipoles << endl;
      exit(2);
    }
    //
    std::string firstCavityTag("");
    std::string lastCavityTag("");
    if (numCavities != 0) {
      for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
          if (nCavs == 0) firstCavityTag = q->getTag();
	  if (numCavities == 0) break;
	  lastCavityTag = q->getTag();
          if (nCavs >= firstCavity) cavities.push_back(cavPtr);
          if (numCavities == cavities.size()) break;
          nCavs++;
        }
      }
    }
    
//    cerr << " ECHANGEMerlin v-Jeff, first Dipole " << firstDipole 
//         << " First cavity tag " << firstCavityTag << " last " 
//	 << lastCavityTag << " Num Cav " << cavities.size() <<  endl;
    
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    if (numPartPerPulse != -2) rebuildwPreSteer();
//    clock_t endClock = clock();
//    cerr << " End ResponseMatrix, clock " << endClock << " delta time  " << 
//        (endClock - beginClock)/CLOCKS_PER_SEC << " sec " << endl;

  }  
    
  void ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer() {
               
    double valBPMsFullE[bpms.size()];
    double valBPMsFullEDipChanged[bpms.size()];
    double valBPMsPartialE[bpms.size()];
    // For down-weighting large corrections... 
    // Works only if the number of corrector is equal to the number of BPMs!!!! 
    // Trivial vector for now, in fact...
//    double valCorr[bpms.size()];  
    double dispersion[bpms.size()]; // spurious dispersion, actually. Volatile stuff, usedt to do some debugging
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1; 
    // First compute the Dispersion at each dipole.
//    cerr << " Rebuilding ECHANGEMERLIN response matrixt with " << 
//          numPartPerPulse << " and kScale = " << kScale << endl;
    Vector stateInNominal = theSteerUtil->getStateAtInjection();
    
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateInNominal, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
    size_t k=0;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsFullE[k] = (*itBpm)->GetMeasurement();
//	   if ((*itBpm)->GetId() < 20) cerr << " Nominal E, 0 Corr, At BPM " << (*itBpm)->GetId()  
//	        << " = " <<  valBPMsFullE[k] << endl;
	   k++;
    }
    setEnergy(changeRFPhase, true); // Reducing the voltage. 
    
      // Pree Steering, optionally 
    if (mPreSteer != 0) {
        // Should bother to recreate as thing  changes? 
	// No for now.. 
        const ResponseMatrix<STEERTOORBIT> *mpp = mPreSteer;
        SteerToOrbit(mpp);
    }
    k = 0;
    
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
     
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsPartialE[k] = (*itBpm)->GetMeasurement();
	   // BPM id to figure out the reference momentum.. Assume positive number 
	   size_t iBPMIndex = (size_t) (*itBpm)->GetId();
	   double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	   double relDeltaE = energyOffset/nominalE;
	   dispersion[k] = (valBPMsFullE[k] - valBPMsPartialE[k])/relDeltaE;
	   // Do not subtract out the lattice Dispersion, 
	   // since we will not do it in the optimization.
	   // Well not sure about this... . 
	   // .. Alternate philosophy : We set the effective Dispersion here... 
//	   if (usePerfectDispKnow)
//             dispersion[k] -= theSteerUtil->getDispersionAtBPM(iBPMIndex); 
           theSteerUtil->setDispersionAtBPM(iBPMIndex, dispersion[k]); 
//          and then..
//            dispersion[k] = 0.; 	   


//	   if ((*itBpm)->GetId() < 20) 
//	      cerr << " Partial E, 0 Corr, At BPM,  " << (*itBpm)->GetId() << " Pos " <<  valBPMsPartialE[k]
//	        << " Dispersion = " <<  dispersion[k] 
//	        << " Nominal E " <<  nominalE <<  " relDeltaE " <<   relDeltaE << endl; 

/*
           ffOut << " " << iBPMIndex << " " << valBPMsFullE[k] << " " 
	         << valBPMsPartialE[k] << " " <<  nominalE << " " 
		 << relDeltaE << " " << dispersion[k] << endl;
*/
	   k++; 
    }	
//    ffOut.close();
    setEnergy(changeRFPhase, false);
    // Now we start changing the dipole one by one, storing finite difference of dispersion 
    // with respect to a change of dipole setting.  
    size_t iDip = firstDipIndex;
    size_t iDipOK = firstDipIndex;
         		       		
    for (vector<ElmPtr >::iterator itDip = dipoles.begin(); 
               itDip != dipoles.end(); itDip++) { 
     // Check if we have a good dipole 
       if (!theSteerUtil->isDipoleOK(iDip)) {
         iDip++;
	 continue;
       }       
	       
	       // Send beam, record orbit without known perturbation.  
      double bdlTest = kScale*ResponseMatrix<CORDIP>::kFact * 
                       theSteerUtil->getRefMomentumAtDipole(iDip);
//      std::cerr << " Dipole change at " << iDip << " = " << bdlTest << endl;	       
      theSteerUtil->AdjustOneDipole(iDip, bdlTest);
      // 
      // Note: one must use the nominal beam at the onset. 
      //
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateInNominal, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
	    cerr << 
	       " ... Dipole " << iDip << " bdlTest " << bdlTest << endl;
            cerr << " ... Reduce probe scale from " << ResponseMatrix<CORDIP>::kFact;
	    ResponseMatrix<CORDIP>::kFact *= 0.5;
	    cerr << " to " << ResponseMatrix<CORDIP>::kFact << endl;
            theSteerUtil->AdjustOneDipole(iDip, -bdlTest); 
	    bdlTest = kScale*ResponseMatrix<CORDIP>::kFact * 
                       (theSteerUtil->getRefMomentumAtDipole(iDip));
            theSteerUtil->AdjustOneDipole(iDip, bdlTest);
           if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	      cerr << 
	       " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer, Probing at full energy 2nd try  " << endl;
	      cerr << " Fatal error, quit altogether.. " << endl;
	      exit(2);  
	   }  
	}
    }
     k=0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsFullEDipChanged[k] = (*itBpm)->GetMeasurement();
//	   if (((*itBpm)->GetId() == 20) && (iDip == firstDipIndex))
//	      cerr << " Full E, Dip Corr, At BPM,  " << (*itBpm)->GetId() 
//	        << " Pos " << valBPMsFullEDipChanged[k] << endl; 
	   k++;
      }	       	       
      setEnergy(changeRFPhase, true);
      
      // Send an other pilot beam.
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
	      cerr << " Fatal error, quit altogether.. " << endl;
	      exit(2);  
	   }  
      }
    // Load the matrix. 
      k = 0;
      
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
	 size_t iBPMIndex = (size_t) (*itBpm)->GetId();
	 double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	 double relDeltaE = energyOffset/nominalE;     
	 double dispNow =  (valBPMsFullEDipChanged[k] - valBPMsTweaked)/relDeltaE;
	   if (usePerfectDispKnow)
             dispNow -= theSteerUtil->getDispersionAtBPM(iBPMIndex); 	   
	 
         (*this)(k, iDip-firstDipIndex)  = 
	      1.0e-6 * (valBPMsFullEDipChanged[k] - valBPMsFullE[k])/bdlTest;
         (*this)(k+bpms.size(), iDip-firstDipIndex)  = 
	      1.0e-6 * (dispNow) /bdlTest;
	  // Adding the weight correction factors.     
         (*this)(k+2*bpms.size(), iDip-firstDipIndex)  = 0.;
	 if ((iDip-firstDipIndex) == k) (*this)(k+2*bpms.size(), iDip-firstDipIndex)  = 1.;
//	   if ((iBPMIndex == 7) && (iDip == firstDipIndex))
//	   if (iBPMIndex < 20)
//	   cerr << " ....... M build BPM  " << (*itBpm)->GetId() << 
//	    " value = "  << valBPMsTweaked << " disp " << dispNow
//	    << " d disp " << (dispersion[k] - dispNow) << " bldTest " << bdlTest <<
//	    " elem " << (*this)(k, iDip-firstDipIndex) << endl;
	 k++;
      }
      
// Restore to previous ... 
      setEnergy(changeRFPhase, false);
      theSteerUtil->AdjustOneDipole(iDip, -bdlTest); 
//      cerr << " ResponseMatrix<ECHANGEMERLIN>::rebuildwPreSteer , at idip " << iDip << endl;
      iDip++;
      iDipOK++;
    } // loop on dipole to be adjusted. 
    // Assemble the matrix to be inverted
//    delete mInv;
  // done..
//  std::cerr << " And Quit for now " << std::endl;
//  exit(2);
 } //rebuild with presteer  
 
 void ResponseMatrix<ECHANGEMERLIN>::setEnergy(bool byPhase, 
                                                bool fromNomToReduced, bool other) {
 
    if ((!fromNomToReduced) && (!byPhase)) { // Back to nominal 
                             // We just do a simple reset to nominal 
			     //for the entire Linac. 
        theSteerUtil->resetLinacGradient();
	// To be upgraded if byPhase is used...
        return;
     }
     double newPhase = 0.;
     double fact = 1.;
     
     // Operate also on the Bunch Compressor, 
     // simulated by shifting the in energy the injected bunch.
     
     theSteerUtil->setBunchDPoPOffsetBC(stateIn[5]);
     if (fromNomToReduced) energyOffset = 0.;
     energyOffset += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
     theSteerUtil->setLinacOffEnergyFlag();
     if (cavities.size() == 0) return; // The Bunch compressor does the job.. 
     
     // Assume all cavities are identical
     LinacCavityPtr aCav = (*(cavities.begin()));
     if (byPhase) {
       if (fromNomToReduced) newPhase = std::asin(gradReduxFact*std::sin(aCav->getPhi()));
       else newPhase = std::asin((1.0/gradReduxFact)*std::sin(aCav->getPhi()));
     } else {
       if (fromNomToReduced) fact = gradReduxFact;
       else fact = 1.0/gradReduxFact;
     }
     
     
     size_t numCavTmp = 0;
         
//     double eTotTmp = 0.;
//     cerr << " Number of cavities in setEnergy " << cavities.size() 
//          << " grad fact " << fact << endl;
//     cout << " Dump of energy gradient from ECHANGEMERLIN " << endl;
//     std::string lastCavChanged("");
     for (std::vector<LinacCavityPtr>::iterator itCav = cavities.begin(); 
           itCav != cavities.end(); itCav++) {
	   aCav = (*itCav);
//	   lastCavChanged = aCav->getTag();
	   if (byPhase) aCav->setPhi(newPhase);   
	   else {
	     double oldStrengh = theSteerUtil->getNominalCavityVoltage(numCavTmp);
	     aCav->setStrength(1.0e9*fact*aCav->Strength()); // silly unit conversion.
//	     aCav->setStrength(fact*aCav->Strength()); // Hopefully fixed
	     double newStrength = aCav->Strength();
//	     cout << " Cavity" << aCav->getTag() << " Old " << oldStrengh << " new " << newStrength << endl;
	     if (fromNomToReduced) 
	       energyOffset += (oldStrengh - newStrength)*std::sin(aCav->getPhi());
           } 
	   numCavTmp++;
//	   eTotTmp += aCav->Strength();
    }
    // To support energy jitter... 
    theSteerUtil->loadOffECavitySettings();
    
//    if (fromNomToReduced) cerr << " ResponseMatrix<ECHANGEMERLIN>::setEnergy, Missing Energy " << 
//           energyOffset << endl; 
//     cerr << " .. SetEenergy first cavity pointer " << (void*) aCav 
//          << " numCav " << numCavTmp << " eTotTmp " << eTotTmp << endl;
    // debugging, checking we restore things properly 
/*
     aCav = (*(cavities.begin()));
     if (fromNomToReduced)  cerr << " Reduced Voltage is now ";
     else cerr << " Restored Voltage ";
     cerr  << aCav->Strength() << " Phase " << aCav->getPhi() << endl;
*/
  } // setEnergy 
  
 // 
 //
 // Cryo-tilting... 
 //
  ResponseMatrix<Steering::CRYOTILT>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			 size_t firstCavityPitched, size_t numCavPitched,
			 size_t numModules,
			 bool withRFPhase,
			 double kMininScale,
			 int numPart ):
    Matrix(numBPMs+numModules, numModules),	// add one for the weight of the absolute sum 
                                        // of corrections.		 
    gradReduxFact(gradientReduction),
    gradReduxPrePitched(0.),			 
    numMod(numModules),	
    numCavPerModule(numCavPitched/numModules),		 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    cavitiesPitched(0),
    firstCavIndex(firstCavity),
    firstCavPitchedIndex(firstCavityPitched),
    bpms(),
    stateIn(Particle::PSD),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    kScale(kMininScale),
    numPartPerPulse(numPart),			 
    nominalRFSettingUpstream(0),
    nominalRFSettingPitched(0),
    reducedRFSettingUpstrFP(0),
    reducedRFSettingUpstrRP(0),
    reducedRFSettingPitched(0)			 
  {
     this->buildIt(firstBPM, numBPMs, numCavities, numCavPitched);
  }

  ResponseMatrix<CRYOTILT>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			  size_t firstCavityPitched, size_t numCavPitched,
			 size_t numModules,
			  bool withRFPhase,
			   double kMininScale, int numPart  ):
    Matrix(numBPMs+numModules, numModules),			 
    gradReduxFact(gradientReduction),
    gradReduxPrePitched(0.),
    numMod(numModules),			 
    numCavPerModule(numCavPitched/numModules),		 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    cavitiesPitched(0),
    firstCavIndex(firstCavity),
    firstCavPitchedIndex(firstCavityPitched),
    bpms(),
    stateIn(stateInTest),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    kScale(kMininScale),
    numPartPerPulse(numPart),
    nominalRFSettingUpstream(0),
    nominalRFSettingPitched(0),
    reducedRFSettingUpstrFP(0),
    reducedRFSettingUpstrRP(0),
    reducedRFSettingPitched(0)			 
  {
     this->buildIt(firstBPM, numBPMs, numCavities, numCavPitched);
  }

// Real constructor here. 

  void ResponseMatrix<CRYOTILT>::buildIt(size_t firstBPM, size_t numBPMs, 
                                         size_t numCavities, size_t numCavPitched)  
  {
    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    if (changeRFPhase) {
       cerr << 
      "  ResponseMatrix<CRYOTILT>: Changing the rf phase not yet supported. ! " << endl;
      exit(2);
    }
    // Dumping information... 
    std::cout << " CRYOTILT, buildIt: Upstream Cavity Range " << firstCavIndex << " to " 
              << firstCavIndex + numCavities-1 <<  " Tilted Cav Range " 
	      << firstCavPitchedIndex << " to " << firstCavPitchedIndex + numCavPitched-1 
	      <<  " BPM " << firstBPM << " to " << firstBPM + numBPMs -1 << endl;
    
    // Make sure we start with a nominal Linac. 
    theSteerUtil->resetLinacGradient();
    // Pick the upstream, pitched cavities and the BPMS
    cavities.clear();
    cavitiesPitched.clear();
    bpms.clear();
    size_t nCavs = 0;
    size_t nBpms = 0;
    size_t nCavsP = 0;
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
       if (nCavsP >= firstCavPitchedIndex) {
          cavitiesPitched.push_back(cavPtr);
	  nominalRFSettingPitched.push_back(cavPtr->Strength()); 
        }
        if (numCavPitched == nominalRFSettingPitched.size()) break;
        nCavsP++;
      }
    }
    // Upstream cavities.
    nCavs = 0;
    if (numCavities != 0) {
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
          if (nCavs >= firstCavIndex) {
	    cavities.push_back(cavPtr);
 	    nominalRFSettingUpstream.push_back(cavPtr->Strength()); 
           }
          if (numCavities == nominalRFSettingUpstream.size()) break;
          nCavs++;
        }
      }
    }
    cavitiesDwnstr.clear();
    nCavs = 0;
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
         if (nCavs >= (firstCavPitchedIndex + cavitiesPitched.size())) {
          cavitiesDwnstr.push_back(cavPtr); 
        }
        nCavs++;
      }
    }
    // Compute the reduced voltages for upstream cavities and pitched cavities, 
    // in the two mode, one where all of them are reduced, and when only the upstream are reduced. 
    nCavs = 0;
    size_t nCavsUpst = 0;
    size_t nCavsPitched = 0;
    energyOffset = 0.;
    double energyOffsetUpstream = 0.;
    double energyOffsetPitched = 0.;
    energyOffset += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
    energyOffsetUpstream += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
    energyOffsetPitched += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
   cout << " ResponseMatrix<CRYOTILT>::buildIt Number of Upstr Cav. " 
         << cavities.size() << " Pitched " << cavitiesPitched.size() << endl; 
   for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
        if ((nCavs >= firstCavIndex) && (nCavsUpst < cavities.size())) {
          nCavsUpst++;
	  double redFP = (1.0-gradReduxFact)*aCav->Strength();
	  double redFPS = redFP*std::sin(aCav->getPhi());
          energyOffset += redFPS;
	  energyOffsetUpstream += redFPS;
	  reducedRFSettingUpstrRP.push_back(gradReduxFact*aCav->Strength()); 
         }
        if ((nCavs >= firstCavPitchedIndex) && (nCavsPitched < cavitiesPitched.size())) {
          nCavsPitched++;
	  double redFP = (1.0-gradReduxFact)*aCav->Strength();
	  double redFPS = redFP*std::sin(aCav->getPhi());
          energyOffset += redFPS;
	  energyOffsetPitched += redFPS;
	  reducedRFSettingPitched.push_back(gradReduxFact*aCav->Strength()); 
         }
        nCavs++;
	if ((nCavsUpst == cavities.size()) && (nCavsPitched == cavitiesPitched.size())) break;
      }
    }
    cout << " .. Check Number of Upstr Cav. " 
         << nCavsUpst << " Pitched " << nCavsPitched 
	 << " gradReduxFact " << gradReduxFact << endl; 
    // We now re-distribute this energy to the upstream section
    gradReduxPrePitched = 1.0 - (1.0-gradReduxFact) * (energyOffset/energyOffsetUpstream);
    cout << " ... energyOffset = " << energyOffset 
         << " Upstr " << energyOffsetUpstream << " gradReduxPrePitched " << 
	 gradReduxPrePitched << endl;
    // That was crummy, and possibly wrong, since the phase might be different, upstream 
    // 
    // Recompute the total energy loss now, loop and we have it right.. 
    bool goCheckFinalEloss = true;
    int nCrazy = 0;
    while (goCheckFinalEloss) {
      nCavsUpst = 0;
      double eOffsetCurr = 0.;
      for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
          if ((nCavs >= firstCavIndex) && (nCavsUpst < cavities.size())) {
            nCavsUpst++;
	    eOffsetCurr += 
	      (1.0 - gradReduxPrePitched)*aCav->Strength()*std::sin(aCav->getPhi());
           }
	   if (nCavsUpst == cavities.size()) break;
         }
       }
       double deltaERel = (eOffsetCurr - energyOffset)/energyOffset;
       if (std::abs(deltaERel < 1.e-4)) goCheckFinalEloss = false;
       gradReduxPrePitched *= (1.0 + deltaERel/2.);
       nCrazy++;
       if (nCrazy > 10) {
          cerr 
	    << " ResponseMatrix<CRYOTILT>::buildIt, wrong algo to find optimum energy reduction ";
	  cerr << endl << " QUIT !!" << endl;
	  exit(2);       
       }
    }
    cout <<  " Final gradReduxPrePitched " << gradReduxPrePitched << endl;
    
    nCavsUpst = 0;
   for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
     ElmPtr q = *dbi;
     if( LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
        if ((nCavs >= firstCavIndex) && (nCavsUpst < cavities.size())) {
          nCavsUpst++;
	  reducedRFSettingUpstrFP.push_back(gradReduxPrePitched*aCav->Strength()); 
         }
	if (nCavsUpst == cavities.size()) break;
      }
    }

    
    
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    rebuild();
  }  
    
  void ResponseMatrix<CRYOTILT>::rebuild() {
          
    double valBPMsFullEPitched[bpms.size()];
    double valBPMsPartialEPitched[bpms.size()];
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1;
    // Debugging , only true if 12 cavities per cryo modules 
    cout << " Average Cavity Tilt, 1st Cryo begin ResponseMatrix<CRYOTILT>::rebuild() = " 
         << theSteerUtil->getAverageCavityPitch((*cavitiesPitched.begin()), 12) << endl;
    std::vector<LinacCavityPtr>::iterator itCav = cavitiesPitched.begin();
    size_t kCav = 0;
    while ( kCav != cavitiesPitched.size()) { kCav++; itCav++;} 
    cout << "  ... 2nd Cryo begin ResponseMatrix<CRYOTILT>::rebuild() = " 
         << theSteerUtil->getAverageCavityPitch((*itCav), cavitiesPitched.size()) << endl;
      

    bool printDebug = false; 
    // First compute the Dispersion at each dipole. 
    setEnergy(changeRFPhase, true, true); // Redistribution of the accelerating voltage. 
                                          // Reducing further upstream, Pitched Cav at full energy
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax, printDebug)) {
	    cerr << 
	    " ResponseMatrix<CRYOTILT>::rebuild, bunch does not go through mode aa " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
    size_t k=0;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsFullEPitched[k] = (*itBpm)->GetMeasurement();	   
	   if ((*itBpm)->GetId() > 9999999) 
	      cerr << " Straight, Pitched Cav full energy At BPM " << (*itBpm)->GetId()  
	        << " = " <<  valBPMsFullEPitched[k] << endl;
	   k++;
    }

    setEnergy(changeRFPhase, true, false); // Reducing upstream and on pitched cavity. 
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax, printDebug)) {
	    cerr << 
	    " ResponseMatrix<CRYOTILT>::rebuild, bunch does not go through mode ab " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
    k=0;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsPartialEPitched[k] = (*itBpm)->GetMeasurement();
	   if ((*itBpm)->GetId() > 999999) 
	     cerr << " Straight, Pitched Cav reduced energy, At BPM " << (*itBpm)->GetId()  
	        << " = " <<  valBPMsPartialEPitched[k] << endl;
	   k++;
    }
    
    // Now make a test tilt of kScale*100 microRadiants
    for (size_t iM=0; iM != numMod; iM++) { 
      setEnergy(changeRFPhase, true, false);  // Pitched Cav at partial energy
      double tiltTest = 1.0e-4*kScale;
      applyTilt(tiltTest, iM);
//      cout << " Average Cavity Tilt,  applying titlTest  = " 
//         << theSteerUtil->getAverageCavityPitch((*cavitiesPitched.begin()), 12) << endl;
     
      double valBPMsPartialEPitchedP[bpms.size()];
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax, printDebug)) {
	    cerr << 
	    " ResponseMatrix<CRYOTILT>::rebuild, bunch does not go through mode ba " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
      }
      k=0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsPartialEPitchedP[k] = (*itBpm)->GetMeasurement();
	   if ((*itBpm)->GetId() > 999999) 
	      cerr << " Tilted, Pitched at Reduced Voltage, At BPM " << (*itBpm)->GetId()  
	        << " = " <<  valBPMsPartialEPitchedP[k] << endl;
	   k++;
      }
    
      setEnergy(changeRFPhase, true, true);  
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax, printDebug)) {
	    cerr << 
	    " ResponseMatrix<CRYOTILT>::rebuild, bunch does not go through mode bb " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
      }
 
      k = 0;   
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   double delta1 = valBPMsFullEPitched[k] - valBPMsPartialEPitched[k];
	   double delta2 = (*itBpm)->GetMeasurement() - valBPMsPartialEPitchedP[k];
	   if ((*itBpm)->GetId() > 999999) {
	      cerr << " Tilted, Pitched at Full Voltage, At BPM " << (*itBpm)->GetId()  
	        << " = " << (*itBpm)->GetMeasurement()  << endl;
	      cerr << " delta1 " << delta1 << " delta2 " << delta2 << endl;
	   }
           (*this)(k, iM) = (delta2 - delta1)/tiltTest;
	   k++;
      }
      // Add the constant term to weight the correction itself 
       for (size_t iMi=0; iMi != numMod; iMi++) (*this)(bpms.size()+iMi, iM) = 1.;
      
      applyTilt(-tiltTest, iM);
    } // loop on Modules 
    // Restore the Linac to its previous state. 
    setEnergy(changeRFPhase, false, true); // Redistribution of the accelerating voltage. 
//    cout << " Average Cavity Tilt,  Before restore  = " 
//         << theSteerUtil->getAverageCavityPitch((*cavitiesPitched.begin()), 24) << endl;
//    cerr << " done with ResponseMatrix<CRYOTILT>::rebuild " << endl;
//    cout << " Average Cavity Tilt,  end  = " 
//         << theSteerUtil->getAverageCavityPitch((*cavitiesPitched.begin()), 24) << endl;
    
  // done..
 }
 
 void ResponseMatrix<CRYOTILT>::setEnergy(bool byPhase, 
                                          bool fromNomToReduced, bool pitchedAtFullEnergy) {
 
     size_t iCav = 0;
     if (!fromNomToReduced) { 
         theSteerUtil->resetLinacGradient();
	 return;
     }
     for (std::vector<LinacCavityPtr>::iterator itCav = cavities.begin(); 
           itCav != cavities.end(); itCav++) { // upstream 
	     LinacCavityPtr aCav = (*itCav);
	     if (pitchedAtFullEnergy) 
	       aCav->setStrength(1.0e9*reducedRFSettingUpstrFP[iCav]); // silly unit conversion.
	     else 
	      aCav->setStrength(1.0e9*reducedRFSettingUpstrRP[iCav]);
	     iCav++;
           }
	   
     iCav = 0;
     for (std::vector<LinacCavityPtr>::iterator itCav = cavitiesPitched.begin(); 
           itCav != cavitiesPitched.end(); itCav++) { // Presumed Pitched section 
	     LinacCavityPtr aCav = (*itCav);
	     if (!pitchedAtFullEnergy)
	      aCav->setStrength(1.0e9*reducedRFSettingPitched[iCav]);
	     else aCav->setStrength(1.0e9*nominalRFSettingPitched[iCav]);
	     iCav++;
           }
	 theSteerUtil->setLinacOffEnergyFlag();
     // To support energy jitter... 
    theSteerUtil->loadOffECavitySettings();
      
    if (label.find("V2") != std::string::npos) this->setDownstreamOff();
     	    
  } // setEnergy
  // 
   void ResponseMatrix<CRYOTILT>::setDownstreamOff() {
   
      for (std::vector<LinacCavityPtr>::iterator itCav = cavitiesDwnstr.begin(); 
           itCav != cavitiesDwnstr.end(); itCav++) {
	     LinacCavityPtr aCav = (*itCav);
               aCav->setStrength(1.0e7); // This is very small... 
      }
   }
   
  void ResponseMatrix<CRYOTILT>::applyTilt(double value, size_t iModule) {
     int kCav = 0;
//     cerr << " ResponseMatrix<CRYOTILT>::applyTilt, val " << value 
//          << " mod " << iModule << endl;
        for (std::vector<LinacCavityPtr>::iterator itCav = cavitiesPitched.begin(); 
           itCav != cavitiesPitched.end(); itCav++) {
	     size_t iM = kCav/numCavPerModule;
	     if (iM == iModule) {
//	        cerr << " pitching  .. iM " << iM << " kCav " << kCav << endl; 
	        theSteerUtil->RotateOneCavity((*itCav), value);
	     } 
	     kCav++;
	   }
  } 
  double ResponseMatrix<CRYOTILT>::getAverageCavTilts(size_t iModule) const {
     int kCav = 0;
//     cerr << " ResponseMatrix<CRYOTILT>::applyTilt, val " << value 
//          << " mod " << iModule << endl;
        double aT = 0.;
	size_t ncc = 0;
        for (std::vector<LinacCavityPtr>::const_iterator itCav = cavitiesPitched.begin(); 
           itCav != cavitiesPitched.end(); itCav++) {
	     size_t iM = kCav/numCavPerModule;
	     if (iM == iModule) {
//	        cerr << " pitching  .. iM " << iM << " kCav " << kCav << endl; 
	        aT += theSteerUtil->getCavityPitch((*itCav));
		ncc++;
	     } 
	     kCav++;
	   }
	if (ncc == 0) return 0.;
	else aT /= ncc;
	return aT;
  } 
  
  ResponseMatrix<Steering::STEERTOORBITONOFFE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGE> *aRespMatrix,
			 double kMininScale,
			 int numPart, double zM,
			 size_t numDipoles, 
			 size_t numBPMs):
     Matrix(numBPMs, numDipoles),		 
     numDipolesTweaked(numDipoles),
     numBPMsOnOff(numBPMs),
     label(labelIn),
     theBmlPtr(bmlPtr),
     firstBPMIndex(firstBPM),			 
     firstDipIndex(firstDipole),
     zMaxBeamProp(zM),			 
     bpms(),
     stateIn(Particle::PSD),
     theEChangeMatrix(aRespMatrix),
     theEChangeMatrixMerlin(0)
     			 
  {
     this->buildIt(kMininScale, numPart);
  }

  ResponseMatrix<STEERTOORBITONOFFE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGE> *aRespMatrix,
			 double kMininScale,
			 int numPart, double zM,
			 size_t numDipoles, 
			 size_t numBPMs): 
    Matrix(numBPMs, numDipoles),		 
    numDipolesTweaked(numDipoles),
    numBPMsOnOff(numBPMs),		 
    label(labelIn),
    theBmlPtr(bmlPtr),
    firstBPMIndex(firstBPM),			 
    firstDipIndex(firstDipole),			 
    zMaxBeamProp(zM),			 
    bpms(),
    stateIn(stateInTest),
    theEChangeMatrix(aRespMatrix),
    theEChangeMatrixMerlin(0)
    			 
  {
     this->buildIt(kMininScale, numPart);
  }
  ResponseMatrix<Steering::STEERTOORBITONOFFE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGEMERLIN> *aRespMatrix,
			 double kMininScale,
			 int numPart, double zM,
			 size_t numDipoles, 
			 size_t numBPMs):
     Matrix(numBPMs, numDipoles),		 
     numDipolesTweaked(numDipoles),
     numBPMsOnOff(numBPMs),		 
     label(labelIn),
     theBmlPtr(bmlPtr),
     firstBPMIndex(firstBPM),			 
     firstDipIndex(firstDipole),
     zMaxBeamProp(zM),			 
     bpms(),
     stateIn(Particle::PSD),
     theEChangeMatrix(0),
     theEChangeMatrixMerlin(aRespMatrix)
     			 
  {
     this->buildIt(kMininScale, numPart);
  }

  ResponseMatrix<STEERTOORBITONOFFE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGEMERLIN> *aRespMatrix,
			 double kMininScale,
			 int numPart, double zM,
			 size_t numDipoles, 
			 size_t numBPMs):
    Matrix(numBPMs, numDipoles),		 
    numDipolesTweaked(numDipoles),
    numBPMsOnOff(numBPMs),		 
    label(labelIn),
    theBmlPtr(bmlPtr),
    firstBPMIndex(firstBPM),			 
    firstDipIndex(firstDipole),			 
    zMaxBeamProp(zM),			 
    bpms(),
    stateIn(stateInTest),
    theEChangeMatrix(0),
    theEChangeMatrixMerlin(aRespMatrix)
    			 
  {
     this->buildIt(kMininScale, numPart);
  }

// Real constructor here. Send the beam.. 

  void ResponseMatrix<STEERTOORBITONOFFE>::buildIt(double kLocalScale, int numP) 
  {
    if (theSteerUtil == 0) {
      cerr << 
      " STEERTOORBITONOFFE: No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    if ((theEChangeMatrix == 0) && (theEChangeMatrixMerlin == 0)) {
    
       cerr << 
      " STEERTOORBITONOFFE: Need a ECHANGE response matrix ! " << endl;
      exit(2);
   }
   // Pick the BPMs
    theSteerUtil->getBPMPtrs(bpms, firstBPMIndex, numBPMsOnOff);
    // Got the context.. Prepare to fire pilot particles. 
    // Nominal energy, to make sure we can go through  
    double valBPMsBlankOnE[numBPMsOnOff];
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1;
    if (zMaxBeamProp > 0.) zMax = zMaxBeamProp; // For display movie purposes. 
    if (theEChangeMatrix != 0) theEChangeMatrix->setEnergy(false, false); // reset.
    else theEChangeMatrixMerlin->setEnergy(false, false); 
    if (numP < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numP, zMax, false)) {
	    cerr << 
   " ResponseMatrix<STEERTOORBITONOFFE>::rebuild, bunch does not go through full E "  << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
    vector<MLBPMPtr>::const_iterator itBpm = bpms.begin();
    size_t kbp = 0;
    while (itBpm !=  bpms.end()) { 
        valBPMsBlankOnE[kbp] = (*itBpm)->GetMeasurement(); itBpm++; kbp++; }
    itBpm = bpms.begin();
    cout << " STEERTOORBITONOFFE res M, first BPM id " <<  (*itBpm)->GetId() 
         << ", val = " << valBPMsBlankOnE[0] << " next " << valBPMsBlankOnE[1] 
	 << " last " << valBPMsBlankOnE[2] << endl;
  // Set the beam off energy 
    if (theEChangeMatrix != 0) theEChangeMatrix->setEnergy(false, true); 
    else theEChangeMatrixMerlin->setEnergy(false, true);
    
     
    for (size_t iDip = firstDipIndex; iDip != firstDipIndex+2; iDip++) { 
      double bdlTest = Steering::ResponseMatrix<CORDIP>::kFact * kLocalScale *
                       theSteerUtil->getRefMomentumAtDipole(iDip);
      theSteerUtil->AdjustOneDipole(iDip, bdlTest); 
      cout << " STEERTOORBITONOFFE, iDip = " << iDip << " bldtest " << bdlTest << endl;
      // Send beam, record orbit with this perturbation, on Energy  
      if (numP < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numP, zMax, false)) {
	    cerr << 
     " ResponseMatrix<STEERTOORBITONOFFE>::bunch does not go through On E, tweaking "<< endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
      }
      itBpm = bpms.begin();
      double valBPMsOffE[numBPMsOnOff];
      kbp = 0;
       while (itBpm !=  bpms.end()) { 
         valBPMsOffE[kbp] = (*itBpm)->GetMeasurement(); itBpm++; kbp++; }
      cout << " STEERTOORBITONOFFE val BPM Off E  "   
         << valBPMsOffE[0] << " next " << valBPMsOffE[1] 
	 << " last " << valBPMsOffE[2] << endl;
      
      // Load the matrix
      for (size_t k=0; k != numBPMsOnOff; k++ ) {
 	   double delta1 = valBPMsOffE[k] - valBPMsBlankOnE[k];
       (*this)(k, iDip-firstDipIndex) = 1.0e-6 * delta1/ bdlTest;
      }
   // Restore it back      
     theSteerUtil->AdjustOneDipole(iDip, -bdlTest);       
  } // On dipoles	        
  cout << " STEERTOORBITONOFFE Matrix   " << (*this) << endl;
      
       
  if (theEChangeMatrix != 0) theEChangeMatrix->setEnergy(false, false); // reset. 
  else theEChangeMatrixMerlin->setEnergy(false, false);
  // done.. 
 }
  //
  // 
  // Clone from ECHANGEMerlin, pure DMS.. Avoid the confusing weighting scheme. 
  //   
  // 
    ResponseMatrix<Steering::ECHANGEPUREDMS>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double wAbs, double wDMS, double wCorr, 
			 bool withRFPhase,
			 size_t numBrokenDipoles,
			 double kMininScale,
			 int numPart, bool useDisp ):
    gradReduxFact(gradientReduction),
    weightAbsolute(wAbs),
    weightDMS(wDMS),
    weightCorrector(wCorr),			 
    Matrix(numBPMs, numDipoles-numBrokenDipoles), // We need two of them, in fact... 			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    stateIn(Particle::PSD),
    mPreSteer(0),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    usePerfectDispKnow(useDisp),
    kScale(kMininScale),
    quadStrength(0),
    numPartPerPulse(numPart),
    effectiveDispersion(numBPMs)			 
  {
     for (size_t k=0; k != 6; k++) stateIn[k] = 0.;
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                            firstDipole, 
			    numDipoles, numBrokenDipoles);
  }

  ResponseMatrix<ECHANGEPUREDMS>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			  size_t firstDipole, size_t numDipoles,
			 double wAbs, double wDMS, double wCorr, 
			  bool withRFPhase,
			  size_t numBrokenDipoles,
			   double kMininScale, int numPart,
			   bool useDisp  ):
    gradReduxFact(gradientReduction),			 
    weightAbsolute(wAbs),
    weightDMS(wDMS),
    weightCorrector(wCorr),			 
    Matrix(numBPMs, numDipoles-numBrokenDipoles),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    mPreSteer(0),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    usePerfectDispKnow(useDisp),
    kScale(kMininScale),
    quadStrength(0),
    stateIn(stateInTest),
    numPartPerPulse(numPart),			 
    effectiveDispersion(numBPMs)			 
  {
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                   firstDipole, numDipoles, numBrokenDipoles);
  }

// Real constructor here. A lot of silly duplicated code for no good 
// reasons... 

  void ResponseMatrix<ECHANGEPUREDMS>::buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firstCavity, size_t numCavities,
		 size_t firstDipole, size_t numDipoles, 
		 size_t numBrokenDipoles) 
  {
//    clock_t beginClock = clock();
//    cerr << " Begin ResponseMatrix, clock " << beginClock << endl;
    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    // Pick the dipoles and the BPMS
    cavities.clear();
    bpms.clear();
    size_t nCavs = 0;
    size_t nBpms = 0;
    size_t nDips = 0;
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q) ) {
        if (nDips >= firstDipole) dipoles.push_back(q); 
        if (numDipoles == dipoles.size()) break;
        nDips++;
      }
    }
    // Check the number of declared broken dipoles.. 
    size_t numBadDip = 0;
    for (size_t iDip = 0; iDip != dipoles.size(); iDip++) 
      if (!theSteerUtil->isDipoleOK(iDip+firstDipole)) numBadDip++;
    if (numBadDip != numBrokenDipoles) {
      cerr << 
      "ResponseMatrix<ECHANGEPUREDMS>::ResponseMatrix..." << 
      " Alleged Number of broken dipole inconsistent with SteerUtil content ! " << endl;
      cerr << " Num Bad " << numBadDip << " Broken " << numBrokenDipoles << endl;
      exit(2);
    }
    //
    std::string firstCavityTag("");
    std::string lastCavityTag("");
    if (numCavities != 0) {
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
          if (nCavs == 0) firstCavityTag = cavPtr->getTag();
	  if (numCavities == 0) break;
	  lastCavityTag = cavPtr->getTag();
          if (nCavs >= firstCavity) cavities.push_back(cavPtr); 
          if (numCavities == cavities.size()) break;
          nCavs++;
        }
      }
    }
    
    cout << " ECHANGEPUREDMS v-Jeff, first Dipole " << firstDipole 
         << " First cavity tag " << firstCavityTag << " last " << lastCavityTag << endl;
    
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    if (numPartPerPulse != -2) rebuildwPreSteer();
//    clock_t endClock = clock();
//    cerr << " End ResponseMatrix, clock " << endClock << " delta time  " << 
//        (endClock - beginClock)/CLOCKS_PER_SEC << " sec " << endl;

  }  
    
  void ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer() {
               
    double valBPMsFullE[bpms.size()];
    double valBPMsFullEDipChanged[bpms.size()];
    double valBPMsPartialE[bpms.size()];
    // For down-weighting large corrections... 
    // Works only if the number of corrector is equal to the number of BPMs!!!! 
    // Trivial vector for now, in fact...
//    double valCorr[bpms.size()];  
    double dispersion[bpms.size()]; // spurious dispersion, actually
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1; 
    // First compute the Dispersion at each dipole.
    cout << " Rebuilding ECHANGEPUREDMS response matrixt with " << 
          numPartPerPulse << " and kScale = " << kScale << endl;
    Vector stateInNominal(6);
    for (size_t k=0; k != 6; k++) stateInNominal[k] = 0.;
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateInNominal, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
    size_t k=0;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsFullE[k] = (*itBpm)->GetMeasurement();
//	   if ((*itBpm)->GetId() == 7) cerr << " Nominal E, 0 Corr, At BPM " << (*itBpm)->GetId()  
//	        << " = " <<  valBPMsFullE[k] << endl;
            (*itBpm)->Reset();
	   k++;
    }
    setEnergy(changeRFPhase, true); // Reducing the voltage. 
    
      // Pree Steering, optionally 
    if (mPreSteer != 0) {
        // Should bother to recreate as thing  changes? 
	// No for now.. 
        const ResponseMatrix<STEERTOORBIT> *mpp = mPreSteer;
        SteerToOrbit(mpp);
    }
    k = 0;
    
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
	    cerr << " Fatal error, quit altogether.. " << endl;
	    exit(2);  
	}
    }
     
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsPartialE[k] = (*itBpm)->GetMeasurement();
	   // BPM id to figure out the reference momentum.. Assume positive number 
	   size_t iBPMIndex = (size_t) (*itBpm)->GetId();
	   double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	   double relDeltaE = energyOffset/nominalE;
	   dispersion[k] = (valBPMsFullE[k] - valBPMsPartialE[k])/relDeltaE;
	   // Do not subtract out the lattice Dispersion, 
	   // since we will not do it in the optimization.
	   // Well not sure about this... . 
//	      
// Keep the effective dispersion here as well, for robustness studies. 	     	   
//          and then..
            effectiveDispersion[k] = dispersion[k];
	   if (usePerfectDispKnow)
             dispersion[k] -= theSteerUtil->getDispersionAtBPM(iBPMIndex);
	     	   

	   if ( ((*itBpm)->GetId() < 3))  
	      cerr << " Partial E, 0 Corr, At BPM,  " << (*itBpm)->GetId() << " Pos " <<  valBPMsPartialE[k]
	        << " Dispersion = " <<  effectiveDispersion[k] 
	        << " Nominal E " <<  nominalE <<  " relDeltaE " <<   relDeltaE << endl; 

            (*itBpm)->Reset();
/*
           ffOut << " " << iBPMIndex << " " << valBPMsFullE[k] << " " 
	         << valBPMsPartialE[k] << " " <<  nominalE << " " 
		 << relDeltaE << " " << dispersion[k] << endl;
*/
	   k++; 
    }	
//    ffOut.close();
    setEnergy(changeRFPhase, false);
    // Now we start changing the dipole one by one, storing finite difference of dispersion 
    // with respect to a change of dipole setting.  
    size_t iDip = firstDipIndex;
    size_t iDipOK = firstDipIndex;
    
     		       		
    for (vector<ElmPtr >::iterator itDip = dipoles.begin(); 
               itDip != dipoles.end(); itDip++) { 
     // Check if we have a good dipole 
       if (!theSteerUtil->isDipoleOK(iDip)) {
         iDip++;
	 continue;
       }       
	       
	       // Send beam, record orbit without known perturbation.  
      double bdlTest = kScale*ResponseMatrix<CORDIP>::kFact * 
                       theSteerUtil->getRefMomentumAtDipole(iDip);
      theSteerUtil->AdjustOneDipole(iDip, bdlTest); 
      
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateInNominal, zMax);
      // This was a lethal bug!... We are very sensitive to the initial conditions... 
//      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
	    cerr << 
	       " ... Dipole " << iDip << " bdlTest " << bdlTest << endl;
            cerr << " ... Reduce probe scale from " << ResponseMatrix<CORDIP>::kFact;
	    ResponseMatrix<CORDIP>::kFact *= 0.5;
	    cerr << " to " << ResponseMatrix<CORDIP>::kFact << endl;
            theSteerUtil->AdjustOneDipole(iDip, -bdlTest); 
	    bdlTest = kScale*ResponseMatrix<CORDIP>::kFact * 
                       (theSteerUtil->getRefMomentumAtDipole(iDip));
            theSteerUtil->AdjustOneDipole(iDip, bdlTest);
           if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	      cerr << 
	       " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer, Probing at full energy 2nd try  " << endl;
	      cerr << " Fatal error, quit altogether.. " << endl;
	      exit(2);  
	   }  
	}
    }
     k=0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsFullEDipChanged[k] = (*itBpm)->GetMeasurement();
//	   if (((*itBpm)->GetId() == 7) && (iDip == firstDipIndex))
//	      cerr << " Full E, Dip Corr, At BPM,  " << (*itBpm)->GetId() 
//	        << " Pos " << valBPMsFullEDipChanged[k] << endl; 
            (*itBpm)->Reset();
	   k++;
      }	       	       
      setEnergy(changeRFPhase, true);
      
      // Send an other pilot beam.
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
	      cerr << " Fatal error, quit altogether.. " << endl;
	      exit(2);  
	   }  
      }
    // Load the matrix. 
      k = 0;
      
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
	 size_t iBPMIndex = (size_t) (*itBpm)->GetId();
	 double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	 double relDeltaE = energyOffset/nominalE;     
	 double dispNow =  (valBPMsFullEDipChanged[k] - valBPMsTweaked)/relDeltaE;
	 if (usePerfectDispKnow)
             dispNow -= theSteerUtil->getDispersionAtBPM(iBPMIndex); 	   
	 
         (*this)(k, iDip-firstDipIndex)  = 
	      1.0e-6 * dispNow/bdlTest;

//	   if ((iBPMIndex < 3 ) && (iDip == firstDipIndex))
//	   cerr << " ....... M build BPM  " << (*itBpm)->GetId() << 
//	    " value Full E  = "  << valBPMsFullEDipChanged[k] <<  " value off E " << 
//	    valBPMsTweaked << " disp " << dispNow
//	    << " d disp " << theSteerUtil->getDispersionAtBPM(iBPMIndex) << " bldTest " << bdlTest <<
//	    " elem " << (*this)(k, iDip-firstDipIndex) << endl;
            (*itBpm)->Reset();

	 k++;
      }
      
// Restore to previous ... 
      setEnergy(changeRFPhase, false);
      theSteerUtil->AdjustOneDipole(iDip, -bdlTest); 
//      cerr << " ResponseMatrix<ECHANGEPUREDMS>::rebuildwPreSteer , at idip " << iDip << endl;
      iDip++;
      iDipOK++;
    } // loop on dipole to be adjusted. 
    // Assemble the matrix to be inverted
    
    
//    delete mInv;
  // done..
 } //rebuild with presteer 
  
 double ResponseMatrix<ECHANGEPUREDMS>::getEffectiveDispersion(size_t k) {return  effectiveDispersion[k];}
 
 void ResponseMatrix<ECHANGEPUREDMS>::setEnergy(bool byPhase, 
                                                bool fromNomToReduced, bool other) {
 
    if ((!fromNomToReduced) && (!byPhase)) { // Back to nominal 
                             // We just do a simple reset to nominal 
			     //for the entire Linac. 
        theSteerUtil->resetLinacGradient();
	// To be upgraded if byPhase is used...
        return;
     }
     double newPhase = 0.;
     double fact = 1.;
     
     // Operate also on the Bunch Compressor, 
     // simulated by shifting the in energy the injected bunch.
     
     theSteerUtil->setBunchDPoPOffsetBC(stateIn[5]);
     if (fromNomToReduced) energyOffset = 0.;
     energyOffset += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
     theSteerUtil->setLinacOffEnergyFlag();
     if (cavities.size() == 0) return; // The Bunch compressor does the job.. 
     
     // Assume all cavities are identical
     LinacCavityPtr aCav = (*(cavities.begin()));
     if (byPhase) {
       if (fromNomToReduced) newPhase = std::asin(gradReduxFact*std::sin(aCav->getPhi()));
       else newPhase = std::asin((1.0/gradReduxFact)*std::sin(aCav->getPhi()));
     } else {
       if (fromNomToReduced) fact = gradReduxFact;
       else fact = 1.0/gradReduxFact;
     }
     
     
     size_t numCavTmp = 0;
         
//     double eTotTmp = 0.;
//     cerr << " Number of cavities in setEnergy " << cavities.size() 
//          << " grad fact " << fact << endl;
//     cout << " Dump of energy gradient from ECHANGEPUREDMS " << endl;
//     std::string lastCavChanged("");
     for (std::vector<LinacCavityPtr>::iterator itCav = cavities.begin(); 
           itCav != cavities.end(); itCav++) {
	   aCav = (*itCav);
//	   lastCavChanged = aCav->getTag();
	   if (byPhase) aCav->setPhi(newPhase);   
	   else {
	     double oldStrengh = theSteerUtil->getNominalCavityVoltage(numCavTmp);
	     aCav->setStrength(1.0e9*fact*aCav->Strength()); // silly unit conversion.
//	     aCav->setStrength(fact*aCav->Strength()); // silly unit conversion.
	     double newStrength = aCav->Strength();
//	     cout << " Cavity" << aCav->getTag() << " Old " << oldStrengh << " new " << newStrength << endl;
	     if (fromNomToReduced) 
	       energyOffset += (oldStrengh - newStrength)*std::sin(aCav->getPhi());
           } 
	   numCavTmp++;
//	   eTotTmp += aCav->Strength();
    }
    
    // To support energy jitter... 
    theSteerUtil->loadOffECavitySettings();
    
//    if (fromNomToReduced) cerr << " ResponseMatrix<ECHANGEPUREDMS>::setEnergy, Missing Energy " << 
//           energyOffset << endl; 
//     cerr << " .. SetEenergy first cavity pointer " << (void*) aCav 
//          << " numCav " << numCavTmp << " eTotTmp " << eTotTmp << endl;
    // debugging, checking we restore things properly 
/*
     aCav = (*(cavities.begin()));
     if (fromNomToReduced)  cerr << " Reduced Voltage is now ";
     else cerr << " Restored Voltage ";
     cerr  << aCav->Strength() << " Phase " << aCav->getPhi() << endl;
*/
  } // setEnergy 
} // namespace steering 
