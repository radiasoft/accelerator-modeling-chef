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



  double ResponseMatrix<CORDIP>::kFact = 2.5e-5;
                                   // arbitrary, to be adjusted. 
				   // done dynamicall in EChange, optional arg. in 
				   // ResponseMatrix<ECHANGE> constructor

  ResponseMatrix<Steering::CORDIP>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstDipole, size_t numDipoles,
			 size_t numBrokenDipoles):
     Matrix(numBPMs, numDipoles-numBrokenDipoles),			 
     label(labelIn),
     theBmlPtr(bmlPtr),			 
     dipoles(0),
     bpms(),
     stateIn(Particle::PSD)			 
  {
     this->buildIt(firstBPM, numBPMs, firstDipole, numDipoles, numBrokenDipoles);
  }

  ResponseMatrix<CORDIP>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstDipole, size_t numDipoles,
			 size_t numBrokenDipoles):
    Matrix(numBPMs, numDipoles-numBrokenDipoles),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    dipoles(0),
    bpms(),
   stateIn(stateInTest)			 
  {
     this->buildIt(firstBPM, numBPMs, firstDipole, numDipoles, numBrokenDipoles);
  }

// Real constructor here. Send the beam.. 

  void ResponseMatrix<CORDIP>::buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firstDipole, size_t numDipoles, size_t numBrokenDipoles) 
  {
    if (theSteerUtil == 0) {
      cerr << 
      "ResponseMatrix<CORDIP>::ResponseMatrix..." << 
        " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    // Pick the dipoles and the BPMS
    dipoles.clear();
    bpms.clear();
    size_t nDips = 0;
    size_t nBpms = 0;
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
      "ResponseMatrix<CORDIP>::ResponseMatrix..." << 
      " Alleged Number of broken dipole inconsistent with SteerUtil content ! " << endl;
      exit(2);
    }
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    // Got the context.. Prepare to fire pilot particles.   
    nDips = firstDipole;
    size_t nDipsOK = firstDipole;
    double valBPMsBlank[numBPMs];
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1;
      // Send beam, record orbit without perturbation. 
    theSteerUtil->sendRealBunch(1000, zMax);
    size_t k=0;
//      cerr << " Blank Beam " << endl;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsBlank[k] = (*itBpm)->GetMeasurement();
//	 cerr << " BPM " << (*itBpm)->GetId() << " records " << valBPMsBlank[k] << endl;
	   k++;
     }	       	       
    
      
    for (vector<ElmPtr >::iterator itDip = dipoles.begin(); 
               itDip != dipoles.end(); itDip++) { 
      if (!theSteerUtil->isDipoleOK(nDips)) {
        nDips++;
        continue;
      }
      // Send beam, record orbit without known perturbation. 
      theSteerUtil->sendRealBunch(1000, zMax);
      size_t k=0;
//      cerr << " Blank Beam " << endl;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsBlank[k] = (*itBpm)->GetMeasurement();
//	 cerr << " BPM " << (*itBpm)->GetId() << " records " << valBPMsBlank[k] << endl;
	   k++;
      }	       	       
      //second, pick the reference momentum, we need this to set a sensible 
      // Best scale probably only 0.2
      double bdlTest = 0.2*kFact * theSteerUtil->getRefMomentumAtDipole(nDips);
      theSteerUtil->AdjustOneDipole(nDips, bdlTest); 
      // Send an other pilot beam.
      theSteerUtil->sendRealBunch(1000, zMax);
      k=0;
      // Load the matrix
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
        (*this)(k, nDipsOK-firstDipole) = 1.0e-6*(valBPMsTweaked - valBPMsBlank[k])/
	       bdlTest;
	 k++;
      }
    // Restore it back      
    theSteerUtil->AdjustOneDipole(nDips, -bdlTest);       
    nDipsOK++;	       
    nDips++;	       
  }	        
  // done..
 }
 void Steering::ResponseMatrix<CORDIP>::setEnergy(bool byPhase, 
                              bool fromNomToReduced, bool other) {
  			      
  // Just make sure the linac is reset.. 
  theSteerUtil->resetLinacGradient();  // Set it back to nominal 
 		      
 }
 
 // Qaudrupole moves..
 // typical move  
  const double Steering::ResponseMatrix<QUADMOVE>::ampl= 0.0001; 
      // 100 microns default.. 
                                   // arbitrary, to be adjusted.
 
   ResponseMatrix<Steering::QUADMOVE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstQuad, size_t numQuads):
     Matrix(numBPMs, numQuads),			 
     label(labelIn),
     theBmlPtr(bmlPtr),			 
     quads(0),
     bpms(),
     stateIn(Particle::PSD)			 
  {
     this->buildIt(firstBPM, numBPMs, firstQuad, numQuads);
  }

  ResponseMatrix<QUADMOVE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstQuad, size_t numQuads):
    Matrix(numBPMs, numQuads),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    quads(0),
    bpms(),
   stateIn(stateInTest)			 
  {
     this->buildIt(firstBPM, numBPMs, firstQuad, numQuads);
  }

// Real constructor here. Send the beam.. 

  void ResponseMatrix<QUADMOVE>::buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firstQuad, size_t numQuads) 
  {
    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    // Pick the quads and the BPMS
    quads.clear();
    bpms.clear();
    size_t nQs = 0;
    size_t nBpms = 0;
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( QuadrupolePtr quadPtr = boost::dynamic_pointer_cast<quadrupole> (q) ) {
        if (nQs >= firstQuad) {
          quads.push_back(quadPtr); 
        }
        if (numQuads == quads.size()) break;
        nQs++;
      }
    }
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    // Got the context.. Prepare to fire pilot particles.   
    nQs = firstQuad;
    double valBPMsBlank[numBPMs];
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1;
      
    for (vector<QuadrupolePtr >::iterator itDip = quads.begin(); 
               itDip != quads.end(); itDip++) { 
      // Send beam, record orbit without known perturbation. 
      theSteerUtil->sendPilotBeam(stateIn, zMax);
      size_t k=0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsBlank[k] = (*itBpm)->GetMeasurement();
	   k++;
      }	       	       
      // Next, we move the quads
      theSteerUtil->MoveOneQuadrupole(nQs, ampl); 
      // Send an other pilot beam.
      theSteerUtil->sendPilotBeam(stateIn, zMax);
      k=0;
      // Load the matrix
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
        (*this)(k, nQs-firstQuad) = (valBPMsTweaked - valBPMsBlank[k])/
	       ampl;
	 k++;
      }
    // Restore it back      
    theSteerUtil->MoveOneQuadrupole(nQs, -ampl);        
    nQs++;	       
  }	        
  // done..
 }
 void Steering::ResponseMatrix<QUADMOVE>::setEnergy(bool byPhase, 
                              bool fromNomToReduced, bool other) {
  			      
  // Just make sure the linac is reset.. 
  // May be we want to do some thing more fancy.. 
  theSteerUtil->resetLinacGradient();
 		      
 }
 // This is a kind of specialization.. 
 // duplicate code, but I don't want to make the design overly 
 // complicated. 
 // Moreover, I suspect there might be more algorithm details for this one. 
 //
  ResponseMatrix<Steering::STEERTOORBIT>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole):
     Matrix(2, 2),			 
     label(labelIn),
     theBmlPtr(bmlPtr),
     firstBPMIndex(firstBPM),			 
     firstDipIndex(firstDipole),			 
     dipole1(),
     dipole2(),
     bpms(),
     stateIn(Particle::PSD)			 
  {
     this->buildIt();
  }

  ResponseMatrix<STEERTOORBIT>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t firstDipole):
    Matrix(2, 2),			 
    label(labelIn),
    theBmlPtr(bmlPtr),
    firstBPMIndex(firstBPM),			 
    firstDipIndex(firstDipole),			 
    dipole1(),
    dipole2(),
    bpms(),
   stateIn(stateInTest)			 
  {
     this->buildIt();
  }

// Real constructor here. Send the beam.. 

  void ResponseMatrix<STEERTOORBIT>::buildIt() 
  {
    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    // Pick the dipoles and the BPMS
    size_t nDips = 0;
    size_t nBpms = 0;
    for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if( VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q) ) {
        if (nDips >= firstDipIndex) {
          if (nDips == firstDipIndex) dipole1 = dipPtr;
	  else if (1 == nDips-firstDipIndex) dipole2 = dipPtr; 
	  else break;
        }
        nDips++;
      }
    }
    theSteerUtil->getBPMPtrs(bpms, firstBPMIndex, 2);
    // Got the context.. Prepare to fire pilot particles.   
    double valBPMsBlank[2];
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1;  
    for (size_t iDip = firstDipIndex; iDip != firstDipIndex+2; iDip++) { 
      // Send beam, record orbit without known perturbation. 
      theSteerUtil->sendPilotBeam(stateIn, zMax);
      size_t k=0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	   valBPMsBlank[k] = (*itBpm)->GetMeasurement();
	   k++;
      }	       	       
      //second, pick the reference momentum, we need this to set a sensible 
      // B.L for this pilot beam
      double bdlTest = Steering::ResponseMatrix<CORDIP>::kFact * 
                       theSteerUtil->getRefMomentumAtDipole(iDip);
      theSteerUtil->AdjustOneDipole(iDip, bdlTest); 
      // Send an other pilot beam.
      theSteerUtil->sendPilotBeam(stateIn, zMax);
      k=0;
      // Load the matrix
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
        (*this)(k, iDip-firstDipIndex) = (valBPMsTweaked - valBPMsBlank[k])/
	       bdlTest;
	 k++;
      }
    // Restore it back      
    theSteerUtil->AdjustOneDipole(iDip, -bdlTest);       
  }	        
  // done..
 }
 void Steering::ResponseMatrix<STEERTOORBIT>::setEnergy(bool byPhase, 
                              bool fromNomToReduced, bool other) {
  			      
  // Just make sure the linac is reset.. 
  // May be we want to do some thing more fancy.. 
  theSteerUtil->resetLinacGradient();
 		      
 }
 //
 // Energy change, for Fixed Dispersion Steering. 
 //
  ResponseMatrix<Steering::ECHANGE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 bool readjustTheLattice, bool withRFPhase,
			 size_t numBrokenDipoles,
			 double kMininScale,
			 int numPart ):
    gradReduxFact(gradientReduction),			 
    Matrix(numBPMs, numDipoles-numBrokenDipoles),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    stateIn(Particle::PSD),
    mPreSteer(0),
    rescaleLattice(readjustTheLattice),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    kScale(kMininScale),
    quadStrength(0),
    numPartPerPulse(numPart)			 
  {
     for (size_t k=0; k != 6; k++) stateIn[k] = 0.;
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                            firstDipole, 
			    numDipoles, numBrokenDipoles);
  }

  ResponseMatrix<ECHANGE>::ResponseMatrix(std::string& labelIn, 
                         BmlPtr bmlPtr,Vector& stateInTest,
                         size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
                         size_t firstCavity, size_t numCavities,
			  size_t firstDipole, size_t numDipoles,
			  bool readjustTheLattice, bool withRFPhase,
			  size_t numBrokenDipoles,
			   double kMininScale, int numPart  ):
    gradReduxFact(gradientReduction),			 
    Matrix(numBPMs, numDipoles-numBrokenDipoles),			 
    label(labelIn),
    theBmlPtr(bmlPtr),			 
    cavities(0),
    firstDipIndex(firstDipole),
    dipoles(0),
    bpms(),
    mPreSteer(0),
    rescaleLattice(readjustTheLattice),
    changeRFPhase(withRFPhase),
    energyOffset(0.),
    kScale(kMininScale),
    quadStrength(0),
    stateIn(stateInTest),
    numPartPerPulse(numPart)			 
  {
     this->buildIt(firstBPM, numBPMs, firstCavity, numCavities,
                   firstDipole, numDipoles, numBrokenDipoles);
  }

// Real constructor here. 

  void ResponseMatrix<ECHANGE>::buildIt(size_t firstBPM, size_t numBPMs, 
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
         if (nDips >= firstDipole) dipoles.push_back(dipPtr);
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
      "ResponseMatrix<ECHANGE>::ResponseMatrix..." << 
      " Alleged Number of broken dipole inconsistent with SteerUtil content ! " << endl;
      exit(2);
    }
    //
    if (numCavities != 0) { 
      for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if(LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
          if (nCavs >= firstCavity) cavities.push_back(cavPtr);
          if (numCavities == cavities.size()) break;
          nCavs++;
        }
      }
    }
    theSteerUtil->getBPMPtrs(bpms, firstBPM, numBPMs);
    if (numPartPerPulse != -2) rebuildwPreSteer();
//    clock_t endClock = clock();
//    cerr << " End ResponseMatrix, clock " << endClock << " delta time  " << 
//        (endClock - beginClock)/CLOCKS_PER_SEC << " sec " << endl;

  }  
    
  void ResponseMatrix<ECHANGE>::rebuildwPreSteer() {
          
//     std::ofstream ffOut("DispSteer.txt");
//     ffOut << " BPM YNom YOff E  dE Disp " << endl;
     
    double valBPMsFullE[bpms.size()];
    double valBPMsFullEDipChanged[bpms.size()];
    double valBPMsPartialE[bpms.size()];
    double dispersion[bpms.size()]; // spurious dispersion, actually
    double zMax = bpms[bpms.size()-1]->GetZPosition() + 0.1; 
    // First compute the Dispersion at each dipole. 
    if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
    else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGE>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
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
	   k++;
    }
//    if (firstDipIndex == 6) { 	       	       
//      cout << " In ResponseMatrix build, at nominal energy " << endl;
//       PrintEnergyAtEachBPM(20);
//    }
    setEnergy(changeRFPhase, true); // Reducing the voltage. 
//    if (firstDipIndex == 6) { 	       	       
//      cout << " In ResponseMatrix build, at reduced energy " << endl;
//       PrintEnergyAtEachBPM(20);
//    }
    
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
	    " ResponseMatrix<ECHANGE>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
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
	   // Subtract out the 
           dispersion[k] -= theSteerUtil->getDispersionAtBPM(iBPMIndex); 	   

/*
	   if ((*itBpm)->GetId() == 7) 
	      cerr << " Partial E, 0 Corr, At BPM,  " << (*itBpm)->GetId() << " Pos " <<  valBPMsPartialE[k]
	        << " Dispersion = " <<  dispersion[k] 
	        << " Nominal E " <<  nominalE <<  " relDeltaE " <<   relDeltaE << endl; 

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
      
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGE>::rebuildwPreSteer, bunch does not go through at full energy " << endl;
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
	       " ResponseMatrix<ECHANGE>::rebuildwPreSteer, Probing at full energy 2nd try  " << endl;
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
	   k++;
      }	       	       
      setEnergy(changeRFPhase, true);
      
      // Send an other pilot beam.
      if (numPartPerPulse < 2) theSteerUtil->sendPilotBeam(stateIn, zMax);
      else {
        if (!theSteerUtil->sendRealBunch(numPartPerPulse, zMax)) {
	    cerr << 
	    " ResponseMatrix<ECHANGE>::rebuildwPreSteer, bunch does not go through at reduced energy " << endl;
	      cerr << " Fatal error, quit altogether.. " << endl;
	      exit(2);  
	   }  
      }
    // Load the matrix
      k = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpms.begin(); 
          itBpm != bpms.end(); itBpm++ ) {
	 double valBPMsTweaked = (*itBpm)->GetMeasurement();
	 size_t iBPMIndex = (size_t) (*itBpm)->GetId();
	 double nominalE =  theSteerUtil->getRefMomentumAtBPM(iBPMIndex);
	 double relDeltaE = energyOffset/nominalE;     
	 double dispNow =  (valBPMsFullEDipChanged[k] - valBPMsTweaked)/relDeltaE;
        (*this)(k, iDipOK-firstDipIndex) =  1.0e-6*(dispersion[k] - dispNow) /bdlTest;
/*
	   if ((iBPMIndex == 7) && (iDip == firstDipIndex))
	   cerr << " ....... M build BPM  " << (*itBpm)->GetId() << 
	    " value = "  << valBPMsTweaked << " disp " << dispNow
	    << " d disp " << (dispersion[k] - dispNow) << " bldTest " << bdlTest <<
	    " elem " << (*this)(k, iDip-firstDipIndex) << endl;
*/
	 k++;
      }
//      cerr << " .... pilot bunch, partial energy, last BPM  " << valBPMsFullE[k-1] << endl;
      setEnergy(changeRFPhase, false);
      theSteerUtil->AdjustOneDipole(iDip, -bdlTest); 
//      cerr << " ResponseMatrix<ECHANGE>::rebuildwPreSteer , at idip " << iDip << endl;
      iDip++;
      iDipOK++;
    } // loop on dipole to be adjusted. 	        
  // done..
 }
 
 void ResponseMatrix<ECHANGE>::setEnergy(bool byPhase, 
                              bool fromNomToReduced, bool other) {
 
    if (rescaleLattice) {
      cerr << " NNNOOOO don't do this yet ... !" << endl;
      exit(2);
      double eOff = 0.;
      double eFull = theSteerUtil->getTheBeamEnergyInput();
      size_t nCavOff = 0;
      size_t iQuad = 0;
      size_t iCav = 0;
      bool sectorOn = false;
      if (fromNomToReduced) quadStrength.clear();
      for(beamline::iterator dbi = theBmlPtr->begin(); 
        dbi != theBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
       if(LinacCavityPtr aCav = boost::dynamic_pointer_cast<LinacCavity> (q) ) {
	   eFull += aCav->Strength()*std::sin(aCav->getPhi());
	   if (!sectorOn && (aCav == cavities[0])) {
	      sectorOn = true;
//	      cerr << " Turning sector On, first cavity reached at iCav = " << iCav << endl;
	   }
	   if (sectorOn) {
              eOff += (1.0-gradReduxFact)*aCav->Strength()*std::sin(aCav->getPhi());
	      nCavOff ++;
	   }
	   if (sectorOn && (nCavOff ==  cavities.size())) {
//	      cerr << " Turning sector Off, last cavity reached at iCav = " << iCav << endl;
	      sectorOn = false;
	   }
	   iCav++;      
        } // a cavity
        else if (QuadrupolePtr aQuad = boost::dynamic_pointer_cast<quadrupole> (q) )  {
          if (fromNomToReduced) { 
            double ampl = aQuad->Strength();
	    quadStrength.push_back(ampl); //old value, saved. 
            if (eOff/eFull > 1.0e-4) {
	      double factQ  = (eFull - eOff)/eFull;
	      ampl *= factQ;
//	      if (iQuad < 10) cerr << " Rescaling quad " << iQuad << " by " 
//	                           << factQ << " new value " << ampl << endl;;
             aQuad->setStrength(ampl);
	    }
	  } else { // Restore back... 
	     aQuad->setStrength(quadStrength[iQuad]);
//	      if (iQuad < 10) cerr << " Restoring quad  " << iQuad << " to " 
//	                           << quadStrength[iQuad] << endl;;
	  }
	  iQuad++;
        } 
     } // Beamline iterator to rescale the lattice 
    } // Rescaling the lattice 
    
    if ((!fromNomToReduced) && (!byPhase)) { // Back to nominal 
                             // We just do a simple reset to nominal 
			     //for the entire Linac. 
        theSteerUtil->resetLinacGradient();
	// To be upgraded if byPhase is used...
        return;
     }
     double newPhase = 0.;
     double fact = 1.;
     // Assume all cavities are identical
     LinacCavityPtr aCav = (*(cavities.begin()));
     if (byPhase) {
       if (fromNomToReduced) newPhase = std::asin(gradReduxFact*std::sin(aCav->getPhi()));
       else newPhase = std::asin((1.0/gradReduxFact)*std::sin(aCav->getPhi()));
     } else {
       if (fromNomToReduced) fact = gradReduxFact;
       else fact = 1.0/gradReduxFact;
     }
     
     // Operate also on the Bunch Compressor, 
     // simulated by shifting the in energy the injected bunch.
     
     theSteerUtil->setBunchDPoPOffsetBC(stateIn[5]);
     
     size_t numCavTmp = 0;
     if (fromNomToReduced) energyOffset = 0.;     
     energyOffset += -1.0*stateIn[5]*theSteerUtil-> getTheBeamEnergyInput();
//     double eTotTmp = 0.;
//     cerr << " Number of cavities in setEnergy " << cavities.size() 
//          << " grad fact " << fact << endl;
     for (std::vector<LinacCavityPtr>::iterator itCav = cavities.begin(); 
           itCav != cavities.end(); itCav++) {
	   aCav = (*itCav);	   
	   if (byPhase) aCav->setPhi(newPhase);   
	   else {
	     double oldStrengh = theSteerUtil->getNominalCavityVoltage(numCavTmp);
	     aCav->setStrength(1.0e9*fact*aCav->Strength()); // silly unit conversion.
//	     aCav->setStrength(fact*aCav->Strength()); // silly unit conversion.
	     double newStrength = aCav->Strength();
	     if (fromNomToReduced) 
	       energyOffset += (oldStrengh - newStrength)*std::sin(aCav->getPhi());
           } 
	   numCavTmp++;
//	   eTotTmp += aCav->Strength();
    }
    if (fromNomToReduced) theSteerUtil->setLinacOffEnergyFlag();
    else theSteerUtil->setLinacOnEnergyFlag();
//    if (fromNomToReduced) cerr << " ResponseMatrix<ECHANGE>::setEnergy, Missing Energy " << 
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
  
  // Look here at the stability of the calculation vs various input parameter. 
  
  void ResponseMatrix<ECHANGE>::studyStability(int option) {
    if (option == 0) { // vs the kscale.. 
       double kScaleNominal = kScale;
//       numPartPerPulse = 1000;
       for (int ik=-1; ik != 10; ik++) {
         double kFact = 1.0;
         if (ik != -1) kFact = std::exp((ik-4)*1.);
         kScale = kScaleNominal*kFact;
	 cout << " kScale = " << kScale << endl;
	 cout << " Rebuilding.... " << endl;
	 this->rebuildwPreSteer();
	 cout << endl;
	 cout << " Matrix for number of particle in test bunch = " << numPartPerPulse << endl;
	 cout << " ========================================= " << endl;
	 cout << (*this)  << endl;
	 if (this->rows() == this->cols()) 
           cout << " Determinant " << this->determinant() << endl;
	 cout << " ========================================= " << endl;
       }
       kScale = kScaleNominal;
       this->rebuildwPreSteer();
       cerr << " Debugging.. here we quit.. " << endl;
       exit(2);
   } else if (option == 1) { // vs the numer of particles in the bunch ..
  
       int numPNominal = numPartPerPulse;
       for (int ik=0; ik != 5; ik++) {
         double kFact = std::exp((ik-2)*1.);
         numPartPerPulse = (int) (numPNominal*kFact);
	 this->rebuildwPreSteer();
	 cout << endl;
	 cout << " Matrix for number of particle in test bunch = " << numPartPerPulse << endl;
	 cout << " ========================================= " << endl;
	 cout << (*this)  << endl;
	 if (this->rows() == this->cols()) 
           cout << " Determinant " << this->determinant() << endl;
	 cout << " ========================================= " << endl;
       }
       numPartPerPulse = numPNominal;
       this->rebuildwPreSteer();
   } else if (option == -2) {
         cout << " Dump Response Matrix ";
	 cout << " kScale = " << kScale << " # of particle  = " << numPartPerPulse << endl;
	 cout << " ========================================= " << endl;
	 cout << (*this)  << endl;
	 if (this->rows() == this->cols()) 
           cout << " Determinant " << this->determinant() << endl;
	 cout << " ========================================= " << endl;
   }
 } 
} // namespace Steering

