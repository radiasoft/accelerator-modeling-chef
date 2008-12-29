#include <iostream>
#include <string>
#include <basic_toolkit/PhysicsConstants.h>
#include <SteerUtil.h>
#include <ILCMisalignmentSets.h>
#include <beamline/Alignment.h>

#include <SteeringAlgos.h>
#include <unistd.h>

using std::cout;
using std::cerr;
using std::endl;
// More misliagnment utilites here... Modules vs file organization 
// becomes a bit heavy and messy, but lets keep going.. 
// 
  
 bool SteerUtil::MoveAllElements(const ILCMisalignmentSet& m) {

  size_t nCav = 0;
  size_t nQ = 0;
  size_t nBPM = 0;
  int iErr = 0; 
  BmlPtr aModel = this->getTheModelPtr();
  for(beamline::iterator dbi = aModel->begin(); 
        dbi != aModel->end(); ++dbi) {
    ElmPtr q = *dbi;	
    if (LinacCavityPtr rfcPtr= boost::dynamic_pointer_cast<LinacCavity> (q)) {
      double yOff = m.getCavVerticalOffset(nCav);
      if (std::abs(yOff) > 1.e-9) 
	              theModel->moveRelY(dbi, yOff);
      // theModel->moveRelY(q, yOff, iErr );
      double yTilt = m.getCavVerticalPitch(nCav);
      if (std::abs(yTilt) > 1.e-9) 
	              theModel->pitch(dbi, yTilt, 0.5);
//         theModel->pitch(q, yTilt, 0.5, iErr);
      nCav++;
    }
    
    if (QuadrupolePtr quadPtr= boost::dynamic_pointer_cast<quadrupole> (q)) {
      double yOff = m.getQuadVerticalOffset(nQ);
      double roll = m.getQuadRoll(nQ);
      if (std::abs(yOff) > 1.e-9) {
         // Make sure the lements have drift in between!  
//         theModel->moveRelY(q, yOff, iErr );
	 // Absolute alignments..
	 alignmentData alPrev = q->Alignment();
	 alPrev.yOffset = yOff;
	 alPrev.tilt = roll;
	 q->setAlignment(alPrev);
      }
      // Add tilts here... 
      nQ++;
    }
     if (MLBPMPtr bPtr= boost::dynamic_pointer_cast<MLBPM> (q)) {
      double yOff = m.getVerticalBPMOffset(nBPM);
	           if(std::abs(yOff) > 1.0e-9)  theModel->moveRelY(dbi, yOff);
//         theModel->moveRelY(q, yOff, iErr );
        nBPM++;
    }
  }
}
  
void SteerUtil::printAllMisalignments(const std::string &label) const {

  std::string aFNameCav("CavityMisalign_"); aFNameCav += label + std::string(".txt");
  std::ofstream outCav(aFNameCav.c_str());
  outCav.precision(8);
  std::vector<LinacCavityPtr>::const_iterator cavFr = allCavities.begin();
  std::vector<QuadrupolePtr>::const_iterator quadFr = quadList.begin();
  int iErr = 0;
  size_t n = 0;
  outCav << " CavNum xOff yOff tilt yaw " << endl;
  while (cavFr != allCavities.end()) {
    Frame atmpFrame = ((ElmPtr) (*cavFr))->getUpstreamPinnedFrame();
    double tilt=0., yaw = 0., xOff = 0. , yOff = 0.;
    tilt = std::asin(atmpFrame.getzAxis()(1));
    yaw = std::asin(atmpFrame.getzAxis()(0));
    xOff = (atmpFrame.getOrigin())(0);
    yOff = (atmpFrame.getOrigin())(1);
    outCav << n << " " << xOff << " " << yOff << " " << yaw << " " << tilt << endl;
    cavFr++;
    n++;
  }
  outCav.close();
  
  std::string aFNameQ("QuadMisalign_"); aFNameQ += label + std::string(".txt");
  std::ofstream outQ(aFNameQ.c_str());
  outQ.precision(8);
  n = 0;
  outQ << " QuadNum xOff yOff roll " << endl;
  while (quadFr != quadList.end()) {
    double roll=0., xOff = 0. , yOff = 0.;
    alignmentData alPrev = (*quadFr)->Alignment();   
    xOff = alPrev.xOffset;
    yOff = alPrev.yOffset;
    Frame atmpFrame = (*quadFr)->getUpstreamPinnedFrame();
    roll = std::asin(atmpFrame.getxAxis()(0));
    outQ << n << " " << xOff << " " << yOff << " " << roll << endl;
    quadFr++;
     n++;  
  }
  outQ.close();
  
  std::string aFNameB("BPMMisalign_"); aFNameB += label + std::string(".txt");
  std::ofstream outB(aFNameB.c_str());
  outB << " BPMNum xOff yOff " << endl;
  outB.precision(8);
  for (size_t i=0; i != theBPMs->GetNumBPMS(); i++) 
    outB << i << " 0. " << theBPMs->getOffset(i) << endl; 
  outB.close();   
}

bool SteerUtil::MoveAllElementsNew(const std::string &dir, const std::string &label) {

  
  std::string aFNameB(dir);
  aFNameB += std::string("BPMMisalign_"); aFNameB += label + std::string(".txt");
  std::ifstream inB(aFNameB.c_str());
  if (!inB) {
   cerr << " SteerUtil::MoveAllElementsNew: Can not open file " << aFNameB << endl;
   return false; 
  }
  size_t n = 0;
  bool isOK = false;
  int nLines = 0;
  while (n < theBPMs->GetNumBPMS()) {
    try {
      std::string t1;
      getline(inB, t1);
      if (t1.length() < 3) {
       cout << " Shor line, file  " << aFNameB << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
	  inB.close();
          return false;
       }
       std::cerr << " Closing BPM Misalignment file " << endl;
       inB.close();
       break;
      }
      nLines++;
      if ((nLines == 1) && 
          (t1.find("BPMNum xOff yOff") != std::string::npos)) {
	    isOK =  true;
	    continue;
      } // Actually, not really needed for now, since the second 
      if (!isOK) { 
            cerr << " SteerUtil::MoveAllElementsNew: wrong token in " << aFNameB << endl;
	    inB.close();
	    return false;
      }
      std::istringstream t1Str(t1);
      double xOff, yOff;
      t1Str >> n >> xOff >> yOff;
      // XOffset missing. 
      theBPMs->setAnOffset(n, yOff);
//      if (nLines%10 == 0) std::cerr << " At BPM/line " << nLines << " n " << n << endl;
  } catch (std::ios_base::failure) { 
       cout << " Eof of file " << aFNameB << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
	  inB.close();
          return false;
       }
       std::cerr << " Closing BPM Misalignment file " << endl;
       inB.close();
       break;
     }
   } // line by line ..
  std::string aFNameCav(dir);
  aFNameCav += std::string("CavityMisalign_"); aFNameCav += label + std::string(".txt");
  std::cout << "SteerUtil::MoveAllElementsNew , from " << dir << ",  label " << label << endl; 
  std::ifstream inCav(aFNameCav.c_str());
  if (!inCav) {
   cerr << " SteerUtil::MoveAllElementsNew: Can not open file " << aFNameCav << endl;
   return false; 
  }
  std::vector<LinacCavityPtr>::const_iterator cavFr = allCavities.begin();
  int iErr = 0;
  n = 0;
  isOK = false;
  nLines = 0;
  while (cavFr != allCavities.end()) {
    try {
      std::string t1;
      getline(inCav, t1);
      nLines++;
      if ((nLines == 1) && 
          (t1.find("CavNum xOff yOff tilt yaw") != std::string::npos)) {
	    isOK =  true;
	    continue;
      } // Actually, not really needed for now, since the second 
      if (!isOK) { 
            cerr << " SteerUtil::MoveAllElementsNew: wrong token in " << aFNameCav << endl;
	    inCav.close();
	    return false;
      }
      std::istringstream t1Str(t1);
      double xOff, yOff, xAngle, yAngle;
      t1Str >> n >> xOff >> yOff >> xAngle >> yAngle;
      for(beamline::iterator dbi = theModel->begin(); 
                                 dbi != theModel->end(); ++dbi) {
	   ElmPtr q = *dbi;	
	   if (LinacCavityPtr cavPtrTmp= boost::dynamic_pointer_cast<LinacCavity> (q)) {
	   if ((*cavFr) == cavPtrTmp) {
                 if (std::abs(xOff) > 1.e-9) theModel->moveRelX(dbi, xOff);
                 if (std::abs(yOff) > 1.e-9) theModel->moveRelY(dbi, yOff);
                 if (std::abs(xAngle) > 1.e-9) theModel->yaw(dbi, xAngle, 0.5);
                 if (std::abs(yAngle) > 1.e-9) theModel->pitch(dbi, yAngle, 0.5);
		 break;
	       }
	   }  
     }  
      cavFr++;
//      if (nLines%500 == 0) std::cerr << " At cavity/line " << nLines << endl;
  } catch (std::ios_base::failure) { 
       cout << " Eof of file " << aFNameCav << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
          inCav.close();
          return false;
       }
       inCav.close();
       break;
     }
   } // line by line .. 
   std::cerr << " All Cavity misaligned.. " << endl;
  // Quadrupole now
  std::vector<QuadrupolePtr>::const_iterator quadFr = quadList.begin();
  std::string aFNameQ(dir);
  aFNameQ += std::string("QuadMisalign_"); aFNameQ += label + std::string(".txt");
  std::ifstream inQ(aFNameQ.c_str());
  if (!inQ) {
   cerr << " SteerUtil::MoveAllElementsNew: Can not open file " << aFNameQ << endl;
   return false; 
  }
  n = 0;
  isOK = false;
  nLines = 0;
  while (quadFr != quadList.end()) {
    try {
      std::string t1;
      getline(inQ, t1);
      nLines++;
      if ((nLines == 1) && 
          (t1.find("QuadNum xOff yOff rol") != std::string::npos)) {
	    isOK =  true;
	    continue;
      } // Actually, not really needed for now, since the second 
      if (!isOK) { 
            cerr << " SteerUtil::MoveAllElementsNew: wrong token in " << aFNameQ << endl;
	    inQ.close();
	    return false;
      }
      std::istringstream t1Str(t1);
      double xOff, yOff, xRoll;
      t1Str >> n >> xOff >> yOff >> xRoll;
      alignmentData alPrev = (*quadFr)->Alignment();   
      if (std::abs(xOff) > 1.e-9) alPrev.xOffset = xOff;
      if (std::abs(yOff) > 1.e-9) alPrev.yOffset = yOff;
       (*quadFr)->setAlignment(alPrev);
       // Roll here... 
      quadFr++;
//      if (nLines%50 == 0) std::cerr << " At Quadrupole/line " << nLines << endl;
  } catch (std::ios_base::failure) { 
       cout << " Eof of file " << aFNameQ << " at line  " <<  nLines << endl;
       if (nLines < 2) {
          cerr << " Less than 2 lines, anomalously short file  quit .." << endl;
          return false;
       }
       std::cerr << " Closing quadrupole Misalignment file " << endl;
       inQ.close();
       break;
     }
   } // line by line .. 
 
   std::cerr << "SteerUtil::MoveAllElementsNew: Done " << endl;  
   return true;
}

// Additional utility for gauging Steering performance 
// results. 
// Assume the global pointer Steering::theSteerUtil is valid, where other 
// basic utilities are dedfined.
//
void Steering::StudyPerformance(BmlPtr beamLine, std::string& dirPath,
                        std::string& label,   
                        std::string& labelSelectedParticle, int maxBPM  ) {
if (Steering::theSteerUtil == 0) return;
 std::string fName(dirPath);  fName += std::string("StPf_");
 fName += label; fName += std::string(".txt");
// std::cerr << " StudyPerformance, output fName " << fName << std::endl;
 int numPartTmp = 5000;
 std::ofstream fOut(fName.c_str());
 fOut << 
 " BPM z y yp epsilY epsilNDY epsilNDYZcut epsil4D epsil6D "
 << " epsil6DND Disp DispP DispN DispPN " <<
    " Emitt2DD sigz sigdPoP DipVal  " << endl;
 std::ofstream* outOne = 0;
 if (labelSelectedParticle.size() > 1) {
   // debug one particle in the bunch..
   std::string fNameOutOne(dirPath);  fNameOutOne += std::string("DumpOne_");
   fNameOutOne += label; fNameOutOne += std::string("_");
   fNameOutOne += labelSelectedParticle;
   fNameOutOne += std::string(".txt");
   outOne = new std::ofstream(fNameOutOne.c_str());
   (*outOne) << " S x y z xp yp dPoP PRef" << std::endl;
   outOne->precision(8);
 }
 vector<double> yPosFullE;
 vector<double> npyPosFullE;
 vector<double> yPosOffE1;
 vector<double> npyPosOffE1;
// vector<double> yPosOffE2; In case we want second order dispersion.. 
 vector<double> pPosFullE;
 vector<double> pPosOffE1;
// vector<double> pPosOffE2;
// vector<double> pPosOffE2;
 Vector stateIn = theSteerUtil->getStateAtInjection();
 Positron ptrPos1(theSteerUtil->getTheBeamEnergyInput(), stateIn );
 BmlPtr aBmlPtr = theSteerUtil->getTheModelPtr();
 // Propagate...
 int iCav = 0; 
 for(beamline::iterator dbi = aBmlPtr->begin(); 
        dbi != aBmlPtr->end(); ++dbi) {
       ElmPtr q = *dbi;
    q->propagate(ptrPos1);
     if( VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q) )  {
        yPosFullE.push_back(ptrPos1.get_y());
        pPosFullE.push_back(ptrPos1.Momentum());
        npyPosFullE.push_back(ptrPos1.get_npy());
     }
 }
// theSteerUtil->setLinacGradient(0.9999);
 stateIn(5) = -.001;
 double dispOrig = 0.96510E-3;
 double dispPOrig= -15.3113E-6 ;
 stateIn(1) += dispOrig*stateIn(5);
 stateIn(4) += dispPOrig*stateIn(5);
 Positron ptrPos2(theSteerUtil->getTheBeamEnergyInput(), stateIn  );
 BmlPtr aModelPtr = theSteerUtil->getTheModelPtr();
 iCav = 0;
 for(beamline::iterator dbi = aModelPtr->begin(); 
        dbi != aModelPtr->end(); ++dbi) {
     ElmPtr q = *dbi;
     q->propagate(ptrPos2);
     if( VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q) )  {
        yPosOffE1.push_back(ptrPos2.get_y());
        npyPosOffE1.push_back(ptrPos2.get_npy());
        pPosOffE1.push_back(ptrPos2.Momentum());
     }
     if (LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q)) {
       ptrPos2.set_ndp(-.001);
     }
 }
 theSteerUtil->resetLinacGradient();
// We assume here that if the reference particle could propagate, 
// the bunch will go in as well, but not all the way...
 
 myPositronBunch aBunch(theSteerUtil->getTheBeamEnergyInput());
 theSteerUtil->prepareRealBunch(numPartTmp, aBunch);
 size_t iBPM = 0;
 double s = 0;
 double rSqMax = 0.;
 bool doneFirst = true;  // Skip the first point, we added a dummy BPM.   
 aBmlPtr = theSteerUtil->getTheModelPtr();
 bool doAnalysis = true;
 for(beamline::iterator dbi = aBmlPtr->begin(); 
        dbi != aBmlPtr->end(); ++dbi) {
     ElmPtr q = *dbi;
    q->propagate(aBunch);
    s += q->Length();
//    std::cerr << " At elem  " << q->Name() << endl;
    if (outOne != 0) aBunch.dumpOneParticleMerlinParam(*outOne, 
                      s, q, labelSelectedParticle);
    // Done internaly in cavityPhysics, since Feb 2007	      
//    if ((theWake != 0) && (( 0 == strcmp( "rfcavity", q->Type() ) ))) 
//         theWake->DoItAll(aBunch, q);
     MonitorPtr monPtr = boost::dynamic_pointer_cast<monitor> (q);
     if (doAnalysis && ((monPtr != 0) || (!doneFirst))) { 
      double disp = pPosFullE[iBPM]*(yPosOffE1[iBPM] - yPosFullE[iBPM])/
                       (pPosOffE1[iBPM] - pPosFullE[iBPM]);
      double dispPrime =  pPosFullE[iBPM]*(npyPosOffE1[iBPM] - npyPosFullE[iBPM])/
                       (pPosOffE1[iBPM] - pPosFullE[iBPM]);
      double dispNom = theSteerUtil->getDispersionAtBPM(iBPM);       
      double dispPrimeNom = theSteerUtil->getDispersionPrimeAtBPM(iBPM);       
      double yC = 1.0e3*aBunch.getYPosition();
      double xC = 1.0e3*aBunch.getXPosition();
      double rSq = xC*xC + yC*yC;
      double dipVal = 0.;
      if (theSteerUtil->isDipoleOK(iBPM))  
       dipVal = theSteerUtil->GetOneDipoleSetting(iBPM);
      if (rSq > rSqMax) rSqMax = rSq;
      if (!doneFirst) {
        disp = 0., dispPrime = 0.; dipVal = 0.;
      }	 
      fOut << " " << iBPM << " " << s << " " << 
	     1.0e3*yC << " " << 
             1.0e6*aBunch.getYPrime() << " " ;
      fOut << 1.0e9*aBunch.getVerticalProjectedEmittance() << " " ;
      fOut << 1.0e9*aBunch.getVerticalNoDispEmittance() << " " ;
      fOut << 1.0e9*aBunch.getVerticalNoDispEmittance(-0.6e-3, 0.6e-3) << " " ;
      fOut << 1.0e15*aBunch.get4DNoDispEmittance(-0.6e-1, 0.6e-1) << " " ;
      fOut << 1.0e17*aBunch.get6DEmittance(-0.6e-1, 0.6e-1, false) << " " ;
      fOut << 1.0e17*aBunch.get6DEmittance(-0.6e-1, 0.6e-1, true) << " " ;
      fOut << 1.0e6*disp << " " << 1.0e6*dispPrime << "  " ;
      fOut << dispNom << " " << 1.0e6*dispNom << "  " ;
      fOut << 1.0e9*aBunch.getVerticalNoDispEmittance(
	             1.0e-6*dispNom, 1.0e-6*dispPrimeNom, -0.6e-1, 0.6e-1) << " " ; 
      fOut << 1.0e3*aBunch.getSigma((size_t) 2) << " "; 
      fOut << 100.0*aBunch.getSigma((size_t) 5) << " " ;
      fOut << 1.0e3*dipVal << " " << std::endl;
      
// To dump the beam, if need be.. 
//      if (iBPM < 40) { 
//        std::ostringstream fNBStrStr; 
//        fNBStrStr << dirPath << "/" << "Bunch_" << label 
//                << "_" << iBPM << ".txt"; 
//        std::string fNBStr = fNBStrStr.str();
//	aBunch.dumpItForR(fNBStr.c_str());
//      }             
      iBPM++;
      if (!doneFirst) iBPM = 0;	    		
      doneFirst = true;      
    } 
    if (iBPM > maxBPM) {
       break;
    }
    if (rSqMax > 1225.) {  // hard type Machine Protection System.. 
                           // Tesla IRIS constant 
   
       break;
    }
  }
//  if (aWake != 0) {
//     if (!wakeWasOn) aWake->setItOff(); 
//  }
  if (outOne != 0) {
     outOne->close();
     delete outOne;
  }
  fOut.close();

}			  

//adding more debugging code for anomalous section....
void Steering::StudyPerformanceDebug(std::string& label, int minBPM, int maxBPM  ) {
			  
 if (Steering::theSteerUtil == 0) return;
 std::ostringstream fNameStr;
 fNameStr << "SteerPerfDebug_" << label << ".txt";
 std::ofstream fOut(fNameStr.str().c_str());
 fOut << " BPM z y P " << endl;
 myPositronBunch aBunch(theSteerUtil->getTheBeamEnergyInput());
 theSteerUtil->prepareRealBunch(500, aBunch);
 BmlPtr aBmlPtr = theSteerUtil->getTheModelPtr();
 size_t iBPM = 0;
 double s = 0;
 bool printNow = (iBPM <= minBPM);
 for(beamline::iterator dbi = aBmlPtr->begin(); 
        dbi != aBmlPtr->end(); ++dbi) {
     ElmPtr q = *dbi;
    q->propagate(aBunch);
    s += q->Length();
    // Done internally, since early Feb 2007
//    if ((theWake != 0) && (( 0 == strcmp( "rfcavity", q->Type() ) ))) 
//         theWake->DoItAll(aBunch, q);
    if(MonitorPtr monPtr = boost::dynamic_pointer_cast<monitor> (q)) iBPM++;
    printNow = (iBPM >= minBPM); 
    if (printNow) 
      fOut << " " << iBPM << " " << s << " " << 
         1.0e6*aBunch.getYPosition() << " " <<
	 aBunch.getAverageRealMomentum() << endl;
    if (iBPM > maxBPM) break;
  }
  fOut.close();
}			  


void Steering::DumpTableCorrector(BmlPtr beamLine, std::string& label ) {

 if (Steering::theSteerUtil != 0) return;
// if (Steering::theSteerUtil == 0) return;
 // Don't do this, just clogg my disk space .. 
 std::ostringstream fNameStr;
 fNameStr << "SteerCorQuadTable_" << label << ".txt";
 std::ofstream fOut(fNameStr.str().c_str());
 fOut << "QuadY CDipVal " << endl;
 // Assumes we have the same number of corrector dipoles as quadrupoles. 
 for (size_t iQ = 0; iQ != theSteerUtil->GetNumCorrectorDipoles(); iQ++) {
   double dqy = theSteerUtil->GetOneQuadrupoleYTrans(iQ);
   double dip = 0.;
   if (theSteerUtil->isDipoleOK(iQ)) 
       dip = theSteerUtil->GetOneDipoleSetting(iQ);
   fOut << "  " << (1.0e6*dqy) << " " << dip << endl;
 }
 fOut.close();
}

void Steering::PrintEnergyAtEachBPM(int maxBPM) {

 if (Steering::theSteerUtil == 0) return;
  size_t iBPM = 0;
  double s = 0;
  Positron   tmpPart( theSteerUtil->getTheModelPtr()->Energy() );
  cout << " Dump of BPM Num vs Energy " << endl; 
  cout << " BPM z y Momentum " << endl;
  BmlPtr aModel = theSteerUtil->getTheModelPtr(); 
  for(beamline::iterator dbi = aModel->begin(); 
        dbi != aModel->end(); ++dbi) {
     ElmPtr q = *dbi;
     q->propagate(tmpPart);
     s += q->Length();
     if(MonitorPtr monPtr = boost::dynamic_pointer_cast<monitor> (q)) { 
       cout  << " " << iBPM << " " << s << " " << tmpPart.get_y() 
             << " " << tmpPart.Momentum() << endl;
       iBPM++;      
     }
     if (iBPM > maxBPM) break;
 }
}

void SteerUtil::fillReferenceDispersion(double gradFraction, bool BCReducedE,     
                                         std::string const& fName,
					 double dispInit, 
					 double dispPrimeInit, 
					 unsigned int ibpmMax ) {
   // Reset Linac, On Energy 
   resetLinacGradient();
   
  // generate a Positron, nominal energy 
  Positron   tmpPart( theModel->Energy(), stateAtInjection );
  
  vector<double> eNominalBPM(theBPMs->size());
  vector<double> eNominaldPoP(theBPMs->size());
  vector<double> yPosENomBPM(theBPMs->size());
  vector<double> yPosPrimeENomBPM(theBPMs->size());
  size_t iBPM = 0;
  size_t iDip = 0;
  for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
     ElmPtr q = *dbi;
     q->propagate(tmpPart);
     if(MonitorPtr monPtr = boost::dynamic_pointer_cast<monitor> (q)) { 
       eNominalBPM[iBPM] = tmpPart.Momentum();
       eNominaldPoP[iBPM] = tmpPart.get_ndp();
       yPosENomBPM[iBPM] = tmpPart.get_y();
       yPosPrimeENomBPM[iBPM] = tmpPart.get_npy();
       iBPM++;
       if (iBPM > ibpmMax) break; // to save time
     }
//     if(VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q)) {
//       cerr << " Dipole corrector " << iDip << " set to " << 
//         dipPtr->Strength() << endl;
//	 iDip++;
//     } 
  }
  // Skipp this for now, suspecting corruption.. 
//  setLinacGradient(gradFraction);
  Vector stateIn(6);
  for (size_t k=0; k != 6; k++) stateIn[k] = 0.;
  stateIn[5] =(gradFraction-1.0);
  stateIn[1] += dispInit*stateIn[5];
  stateIn[4] += dispPrimeInit*stateIn[5];
  stateIn[1] += stateAtInjection[1];
  stateIn[4] += stateAtInjection[4];
  
  Positron   tmpPartOff( theModel->Energy(), stateIn);
  iBPM = 0;
  std::ofstream *fOut = 0;
  double s = 0.;
  if (fName.length() > 2) {
    fOut = new std::ofstream(fName.c_str());
    (*fOut) << " BPM  s Disp EOn Eoff yOn yOff yyp xx xxp dpOpCur dpOpNom " << endl; 
  }
  for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
	ElmPtr q = *dbi;
     q->propagate(tmpPartOff);
//     cerr << " At.. " << q->Type() << " s = " << s << endl;
//     if ( 0 == strcmp("sbend", q->Type())) 
//         cerr << " sBend Strength is " << q->Strength() << endl; 
     if(MonitorPtr monPtr = boost::dynamic_pointer_cast<monitor> (q)) { 
       double eOff = tmpPartOff.Momentum();
       double yy = tmpPartOff.get_y();
       double xx = tmpPartOff.get_x();
       double yyp = tmpPartOff.get_npy();
       double xxp = tmpPartOff.get_npx();
       double ndp = tmpPartOff.get_ndp();
       refDispersionAtBPM[iBPM] = 1.0e6*(yPosENomBPM[iBPM] - yy)*
                                  eNominalBPM[iBPM]/(eNominalBPM[iBPM] - eOff);
//       if (iBPM < 5) cerr << " Ref Disp at BPM " 
//              << iBPM << " = " <<refDispersionAtBPM[iBPM] << endl; 			  
       if (fOut != 0) 
        (*fOut) << " " << iBPM << "  " << s << " " 
	     << refDispersionAtBPM[iBPM] << " " 
	     << eNominalBPM[iBPM] << " " << eOff << " "  
	     << yPosENomBPM[iBPM] << " " << yy 
	     << " " << yyp << " " << xx << " " << xxp << " " 
	     << ndp << " " << eNominaldPoP[iBPM]  << endl; 			  
       iBPM++;
     }
    if(LinacCavityPtr monPtr = boost::dynamic_pointer_cast<LinacCavity> (q)) 
       tmpPartOff.set_ndp(gradFraction-1.0);
       
    if (iBPM > ibpmMax) break; // to save time
     s += q->Length();
  }
  if (fOut != 0) fOut->close();
  delete fOut;
  // Reset Linac, On Energy 
  resetLinacGradient();

}
void SteerUtil::setCavRotation144DMSDebug() {


  size_t iCav = 0;
  size_t iCavBad = 0;
  const size_t nCavBad = 16;
  double cavPitch[] = {-300., 600., -300., 600.,-300., 600.,-300., 600., 
                        600., 600., 500.,  200., 600., 600., 300., 386.};
  size_t nCavBadStart = 6*3*8 - 1;
  LinacCavityPtr aCav159;
  LinacCavityPtr aCavLast;
  for(beamline::iterator dbi = theModel->begin(); 
        dbi != theModel->end(); ++dbi) {
    ElmPtr q = *dbi;
    if(LinacCavityPtr cavPtr = boost::dynamic_pointer_cast<LinacCavity> (q)) {
	if (iCav > nCavBadStart) {
	   this->RotateOneCavity(iCav, 1.0e-6*cavPitch[iCavBad]);
	   iCavBad++;
	   if (iCav == 159) aCav159 = cavPtr;
	}
	aCavLast = cavPtr;
	if (iCavBad == nCavBad) break; 
	iCav++;	
    }
  }     		
  cerr << "DispersionAA2_1 .... Cavity Rotated... " 
       <<  " Pitch cavity 159 " << getCavityPitch(aCav159)   
       <<  " Last touched " << getCavityPitch(aCavLast) << std::endl;

}

void SteerUtil::loadOffECavitySettings() { // Pick all cavity voltages and Phases 
                                  // put them in memory
  size_t iCav=0;
     for (std::vector<LinacCavityPtr>::iterator itCav=allCavities.begin(); 
            itCav != allCavities.end(); itCav++) {
	OffECavityVoltage[iCav] = (*itCav)->Strength();
	OffECavityPhase[iCav] = (*itCav)->getPhi();    
	iCav++;   
  }

				  
}
// Jitter power amplitude... 
void SteerUtil::jitterKlystrons() {
   
    int iCav=0;
    double delta = 0;
     for (std::vector<LinacCavityPtr>::iterator itCav=allCavities.begin(); 
            itCav != allCavities.end(); itCav++) {
	 if (iCav%numCavPerKlystrons == 0) {
           delta = 1.e10;  
           while (std::abs(delta) > 3.0) delta = this->myGauss.getValue();
	   delta *= klystronJitter;
	 }
	double v = 0.;
	if (atOffEnergy) v = OffECavityVoltage[iCav];
	else v = nominalCavityVoltage[iCav];
	v *= (1.0 + delta);
	(*itCav)->setStrength(1.0e9*v);
//	(*itCav)->setStrength(v);
	iCav++;   
  }
   
}



void SteerUtil::gridUpdate (const std::string message, bool withOptExit) {
				      			  
   // Update the Grid farm... 
     
     time_t tNow = time(0);
     int dt = (int) difftime(tNow, startingSteeringTime);
     std::ostringstream infoStrStr; 
     infoStrStr << message << " # Pulses " << totalNumberPulses << " time (sec) " << dt << endl;
     std::string infoStr = infoStrStr.str();
     if (!withOptExit) { // first time around 
       std::cout << " SteerUtil::gridUpdate, first time around " << std::endl;
       if (!getenv("GRIDILCMESSAGEDIR")) return;
       std::cout << " SteerUtil::gridUpdate, On the grid " << std::endl;
       std::cerr << " SteerUtil::gridUpdate, On the grid " << std::endl;
       std::string aDir(getenv("GRIDILCMESSAGEDIR"));
       std::string aFName(getenv("GRIDPROCNUM"));
       gridFileNameUpdate = aDir + std::string("Status_") + aFName + std::string(".txt");
       std::ofstream fOut(gridFileNameUpdate.c_str());
       fOut << infoStr << std::endl;
       fOut.close();
       std::string cmd("chmod a+w,g+w ");
       cmd += gridFileNameUpdate;
       system(cmd.c_str());   
       
       std::cout << " Grid Update, file  " << gridFileNameUpdate << " created " << endl;
       std::cerr << " Grid Update, file  " << gridFileNameUpdate << " created " << endl;
       
    } else {
      if (gridFileNameUpdate.length() < 3) return;
      FILE *f = fopen(gridFileNameUpdate.c_str(),"r");
      if (!f) {
        std::cout << " Grid Update not there. Signal to quit.. Try Again, after sleep 5 sec " << endl;
        std::cerr << " Grid Update not there. Signal to quit.. Try Again, after sleep 5 sec  " << endl;
	std::string cmdSleep("sleep 5");
        system(cmdSleep.c_str());
        FILE *f2 = fopen(gridFileNameUpdate.c_str(),"r");
	if (!f2) { 
          std::cout << " Grid Update not there, 2nd try   Try Again, after sleep 20 sec " << endl;
          std::cerr << " Grid Update not there. 2nd try   Try Again, after sleep 20 sec  " << endl;
	  std::string cmdSleep("sleep 20");
          system(cmdSleep.c_str());
          FILE *f3 = fopen(gridFileNameUpdate.c_str(),"r");
	   if (!f3) { 
            std::cout << " Grid Update not there, 3rd try   That is it, Quit here  " << endl;
            std::cerr << " Grid Update not there. 3rd try   That is it, Quit here  " << endl;
            exit(2);
	  } else  {
            std::cout << " Grid Update is here, after all, Keep going.. " << endl;
            std::cerr << " Grid Update is here, after all, keep going  " << endl;
	    fclose(f3);
 	  }
	 } else {
           std::cout << " Grid Update is here, after all, Keep going.. " << endl;
           std::cerr << " Grid Update is here, after all, keep going  " << endl;
	   fclose(f2);
	 }
      } else {
        fclose(f);
      }
      std::string cmd("rm -f ");
      cmd += gridFileNameUpdate;
      system(cmd.c_str());   
      std::ofstream fOut(gridFileNameUpdate.c_str());
      fOut << infoStr;
      fOut.close();
      cmd=std::string("chmod a+w,g+w ");
      cmd += gridFileNameUpdate;
      system(cmd.c_str());   
    }

}

void SteerUtil::testBunchMemUtil()  {


  cerr << " SteerUtil::testBunchMemUtil on  "
       << theBPMs->GetNumBPMS() << " bpms " << endl;



  for (int iPhase=0; iPhase != 1000; iPhase++) {

     cerr << " At Phase " << iPhase << endl;

     for (int ib=0; ib != theBPMs->GetNumBPMS(); ib++) {

         myPositronBunch bunch(this->getTheBeamEnergyInput());
         this->prepareRealBunch(1000, bunch, 0);
         bunch.TruncateZ(-2.5e-4, 2.5e-4);
//         std::cerr << " At bpm, " << ib << " Bunch size  " << bunch.size() << endl;
	 
         this->uploadBunchAtBPM (ib, bunch);
	 bunch.clear();
//       std::cerr << " ......... Bunch loaded " << endl;
     }
     
//     cerr << " Sleeping 1 seconds .... " << endl;
//     sleep(1);
//     cerr << " Clearing  .... " << endl;
//     this->clearAllStoredBunches();
     cerr << " Sleeping 1 seconds after clear...  " << endl;
     sleep(1);
   }
   cerr << " Quit from testBunchMemUtil " << endl;
   exit(2); 
}				      			  
