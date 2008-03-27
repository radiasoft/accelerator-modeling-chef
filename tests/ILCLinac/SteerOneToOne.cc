#include <cmath>
#include <iostream>
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"

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

bool Steering::SteerOneToOne(BmlPtr beamLine, 
                            size_t firstDip, size_t numDips, size_t numIter) {
   
    std::string labelMat("SteerOneTOne_v0");
    
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t numBPMs = numDips;  // square matrix problem. 
    size_t firstBPM = firstDip+1;
    size_t indexBPMMax =  firstBPM + numBPMs;
    if (indexBPMMax >= theSteerUtil-> getNumBPMs()) {
      cerr << " Steering::SteerOneTOne: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
	 return false;
    
    }
    
    for (size_t itSteer = 0; itSteer != numIter; itSteer++ ) { 
     
      Steering::ResponseMatrix<Steering::CORDIP> m0Cor(labelMat, beamLine, 
                                     firstBPM, numDips, firstDip, numDips);
				     

     // Assume here we have NO BROKEN DIPOLES 
      Matrix m0 = m0Cor.inverse();
    // Use a real bunch, and the default value in steerUtil
      MLBPMs bpmsTmp; // just to get a local copy
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
      
      theSteerUtil->getBPMPtrs(bpmsTmp, firstBPM, numBPMs);
      double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
    // Testing, playing with resolution.  
//    bpmsTmp.setResolution(50.e-6);
      theSteerUtil->sendRealBunch(1000, zMax);
    // collect the reading from the BPMs 
      Vector bpmsVals(numDips);
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    bpmsVals(kBpm) = 1.0e-6* ((*itBpm)->GetMeasurement()); // in m
//	 cerr << " BPM " << (*itBpm)->GetId() << " records " << bpmsVals(kBpm) << endl;
	kBpm++;    
      }
      Vector diPVals = m0*bpmsVals;
    // Last dipole does something screwy, not sure why... 
      for (size_t i=0; i != numDips-1; ++i) {
        size_t iDip = firstDip + i;
      cerr << " Adjusting dipole " << iDip << " by " << diPVals(i) << endl;
        theSteerUtil->AdjustOneDipole(iDip, -diPVals(i));
      } 
//    bpmsTmp.setResolution(5e-10);
    } // on Iteration..  
    return true;

}
//
// Same, but using quadrupoles
//
bool Steering::SteerOneToOneWQuad(BmlPtr beamLine, 
                            size_t firstQuad, size_t numQuads, size_t numIter) {
   
    std::string labelMat("SteerOneTOneWQuad_v0");
    
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t numBPMs = numQuads;  // square matrix problem. 
    size_t firstBPM = firstQuad+1;
    size_t indexBPMMax =  firstBPM + numBPMs;
    if (indexBPMMax >= theSteerUtil-> getNumBPMs()) {
      cerr << " Steering::SteerOneTOneWQuad: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
	 return false;
    
    }
    
    for (size_t itSteer = 0; itSteer != numIter; itSteer++ ) { 
     
      Steering::ResponseMatrix<Steering::QUADMOVE> m0Cor(labelMat, beamLine, 
                                     firstBPM, numQuads, firstQuad, numQuads);
				     
      Matrix m0 = m0Cor.inverse();
    // Use a real bunch, and the default value in steerUtil
      MLBPMs bpmsTmp; // just to get a local copy
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
      
      theSteerUtil->getBPMPtrs(bpmsTmp, firstBPM, numBPMs);
      double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;
      theSteerUtil->sendRealBunch(1000, zMax);
    // collect the reading from the BPMs 
      Vector bpmsVals(numQuads);
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	    bpmsVals(kBpm) = 1.0e-6* ((*itBpm)->GetMeasurement());
//	 cerr << " BPM " << (*itBpm)->GetId() << " records " << bpmsVals(kBpm) << endl;
	kBpm++;    
      }
      Vector diPVals = m0*bpmsVals;
    // Last quad probably problem: not a good idea to move it.. 
      for (size_t i=0; i != numQuads-1; ++i) {
        size_t iDip = firstQuad + i;
//      cerr << " Adjusting dipole " << iDip << " to " << diPVals(i) << endl;
        theSteerUtil->MoveOneQuadrupole(iDip, -diPVals(i));
      } 
    } // on Iteration..  
    return true;

}
bool Steering::SteerToOrbit(const ResponseMatrix<STEERTOORBIT> *mPreSteer) {

    if (theSteerUtil == 0) {
      cerr << 
      " No Steering Utility has been defined. quit for now ! " << endl;
      exit(2);
    }
    
    // Simple inversion like in 1-to-1
    Matrix m2 = Matrix((*mPreSteer));
    Matrix m2Inv = m2.inverse();
    MLBPMs bpmsPS;
    size_t bpm0 = mPreSteer->getFirstBPMIndex();
    double zMax = (theSteerUtil->getABPMPtrs(bpm0+1))->GetZPosition();
    // Should we cheat a little bit here ?? 
    // No send a real bunch... .. Compromise, only 500 particles 
    theSteerUtil->sendRealBunch(500, zMax);
    Vector bpmsPSVals(bpmsPS.size());
    size_t kBpm = 0;
    for (vector<MLBPMPtr>::const_iterator itBpm = bpmsPS.begin(); 
          itBpm != bpmsPS.end(); itBpm++ ) {
	    bpmsPSVals(kBpm) = (*itBpm)->GetMeasurement();
	kBpm++;    
    }
    // Up to a scale factor..
    double dipVal1 = m2Inv(0,0) * bpmsPSVals(0) + m2Inv(0,1) * bpmsPSVals(1);
    double dipVal2 = m2Inv(1,0) * bpmsPSVals(0) + m2Inv(1,1) * bpmsPSVals(1);
    theSteerUtil->AdjustOneDipole(mPreSteer->getDipoleIndex1(), -dipVal1); 
    theSteerUtil->AdjustOneDipole(mPreSteer->getDipoleIndex2(), -dipVal2); 
    return true;
 }  
//  
// With extra BPM, using SVD for inversion.. 
// Simply testing the more complete algorithm developed along with Dispersion Matched 
// Steering. 
//     
bool Steering::SteerOneToOneExtraBPMs(BmlPtr beamLine, 
                            size_t firstDip, size_t numDips, size_t numBPMs, 
			    size_t numIter) {
   
    std::string labelMat("SteerOneTOne_v0");
    
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
    size_t firstBPM = firstDip+1;
    size_t indexBPMMax =  firstBPM + numBPMs;
    if (indexBPMMax >= theSteerUtil-> getNumBPMs()) {
      cerr << " Steering::SteerOneTOne: Illegal demand for BPMs, maximum # " << 
         theSteerUtil-> getNumBPMs() << " requested maxim index " << 
	 indexBPMMax << endl;
	 return false;
    
    }
    
    for (size_t itSteer = 0; itSteer != numIter; itSteer++ ) { 
     
      Steering::ResponseMatrix<Steering::CORDIP> m0Cor(labelMat, beamLine, 
                                     firstBPM, numBPMs, firstDip, numDips);
				     
    // Use a real bunch, and the default value in steerUtil
      MLBPMs bpmsTmp; // just to get a local copy
    // Set the BPMs. We take the one downstream.  This make an assumption on 
    // the lattice is layed out. 
      
      theSteerUtil->getBPMPtrs(bpmsTmp, firstBPM, numBPMs);
      double zMax = bpmsTmp[bpmsTmp.size()-1]->GetZPosition() + 0.1;

      theSteerUtil->sendRealBunch(1000, zMax);

    // collect the reading from the BPMs 
      Vector bpmsVals(numBPMs);
      size_t kBpm = 0;
      for (vector<MLBPMPtr>::const_iterator itBpm = bpmsTmp.begin(); 
          itBpm != bpmsTmp.end(); itBpm++ ) {
	   // Note the change of sign... 
	    bpmsVals(kBpm) = -1.0 * 1.0e-6 * ((*itBpm)->GetMeasurement());
//	 cerr << " BPM " << (*itBpm)->GetId() << " records " << bpmsVals(kBpm) << endl;
	kBpm++;    
      }
      size_t nL = numDips; // No broken dipole assumed, number of unknows..
      // Now do the SVD inversion. 
      Vector w(nL);  // Will be the W matrix in SVD formalism 
      for (size_t i = 0; i != nL; i++) w(i) = 1.;
      Matrix um0(nL, numBPMs);
      Matrix vm0T(nL, nL);
      m0Cor.SVD(um0, w, vm0T);  // Perhaps bad notation here, should be called vm0
      Vector utyTmp(um0.transpose()*bpmsVals);
//      cerr << " SVD Weights ";
      for (size_t iM = 0; iM != nL; iM++) {
//         cerr << " " << w(iM,0);
	 if (std::abs(w(iM)) > 1.0e-6) 
	        utyTmp(iM) /= w(iM); // Arbitrary !!! 
	 else utyTmp(iM) = 0.;
      }
//      cerr << endl;
      Vector diPVals(nL);
      diPVals = 1.0*vm0T*utyTmp;

      double maxDipChange = -1.0;
      int iMax = 0;
    // Last dipole does something screwy, not sure why... 
      for (size_t i=0; i != numDips; ++i) {
        size_t iDip = firstDip + i;
	if (std::abs(diPVals(i)) > maxDipChange) {
	   iMax = iDip;
	   maxDipChange = std::abs(diPVals(i));
	}
//        cerr << " Adjusting dipole " << iDip << " by " << diPVals(i) << endl;
        theSteerUtil->AdjustOneDipole(iDip, 0.5*diPVals(i));
      }
      maxDipChange /= theSteerUtil->getRefMomentumAtDipole(iMax);
      cout << " Iter " << itSteer 
           << " Relevant Maximum change/P = " << maxDipChange << " At dipole " << 
          iMax << " At P = " << 
	  theSteerUtil->getRefMomentumAtDipole(iMax) << " GeV/c " << endl;
      if (maxDipChange < 5.0e-7) {
        return true;
      }
    } // on Iteration..  
    return false;

}
