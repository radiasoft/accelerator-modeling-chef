#include <iostream>
#include <string>
#include <basic_toolkit/PhysicsConstants.h>
#include <ILCMisalignmentSets.h>
#include <unistd.h>

using std::cout;
using std::cerr;
using std::endl;

 
LucretiaSigmas::LucretiaSigmas():
 cryoVOffset(3.e-4),  // Units in meters 
 cryoHOffset(1.0e-3),
 cryoVTilt(2.5e-5),
 cryoHTilt(1.0e-4),
 quadDispx(3.e-4),
 quadDispy(1.0e-3),
 bpmDispx(3.0e-4),
 bpmDispy(3.0e-4),
 quadRollxy(5.e-4),
 cavDispx(1.e-3),
 cavDispy(3.e-4),
 cavTiltx(1.0e-3),
 cavTilty(3.0e-4),
 nCavMisStart(24),
 nQuadMisStart(1),
    // Sigma cut, if thrown at greater than n sigma, throw again. 
  sigmaCut(3.), 
  numCavityPerCryoModules(8) // Subject to change, i.e.. 9/8/9
{ ;}


LucretiaMisalignSet::LucretiaMisalignSet (SteerUtil* aSteerUtil, 
	                    const LucretiaSigmas& misAlignCnst, bool v4 ):
mySteerUtilPtr(aSteerUtil),
mSigmas(misAlignCnst),
QuadVOffsets(0), 
BPMVOffsets(0), 
CavVOffsets(0), 
CavVTilts(0), 
QuadVTilts(0), 
YCorr(0), 
cryoMods(0) 
{
//  setCryoModules(); // to generate our seeds for misalignments.. 
//  setElements();
// 
// Use a precscribed misalignment file. 

  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) {  
//     BOOST_FAIL( " Environment varialble where test data resides not defined " );
      ilcTestDataChr = (char *) defDataDir.c_str();
  }
  
  getFromSolyakFiles(ilcTestDataChr, v4);
  
}

// Assumption ( valid for Benchmark Fall 07) : 8 cavities per cryo-modules, 
// exceptions.  

bool LucretiaMisalignSet::setCryoModules() {
  
  int iCav=0;
  BmlPtr aModel = mySteerUtilPtr->getTheModelPtr();
  std::vector<LinacCavityPtr> tmpCavs(mSigmas.numCavityPerCryoModules);
  std::vector<double> tmpSLoc(mSigmas.numCavityPerCryoModules);
  cryoMods.clear();
  double s = 0.;
  bool currentCryoHasQuad = false;
  int iQuad = 0;
  cryoModule aMod;
  for(beamline::iterator dbi = aModel->begin(); 
        dbi != aModel->end(); ++dbi) {
    ElmPtr q = *dbi;	
    if (LinacCavityPtr rfcPtr= boost::dynamic_pointer_cast<LinacCavity> (q)) {
      int iCavLocal = iCav%mSigmas.numCavityPerCryoModules;
      tmpCavs[iCavLocal] = rfcPtr;
      if (iCavLocal == 0) aMod.sBegin = s ; 
      tmpSLoc[iCavLocal] = s + 0.5*q->Length() - aMod.sBegin;
      if ((iCavLocal == mSigmas.numCavityPerCryoModules-1)) {
        // Load info for the previous cryo module
        double d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        aMod.yOff= d * mSigmas.cryoVOffset;
        d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        aMod.yTilt= d * mSigmas.cryoVTilt;
	aMod.hasQuad=currentCryoHasQuad;
	aMod.cavPtrs = tmpCavs;
	aMod.sEnd = s + q->Length();
	aMod.sLocs = tmpSLoc;
	cryoMods.push_back(aMod);
//	if (cryoMods.size() < 4) 
//	   std::cerr << " Added cryo module starting at " << aMod.sBegin << endl;
	currentCryoHasQuad = false;  
     }
     iCav++;
    }
    if (QuadrupolePtr rfcPtr= boost::dynamic_pointer_cast<quadrupole> (q)) {
      currentCryoHasQuad = true;
    }
    s += q->Length();
  }
  // Load info for the last cryo module
  if (iCav%mSigmas.numCavityPerCryoModules != 0) {
    std::ostringstream sOut;
    sOut << " LucretiaMisalignSet: Unexpected number of cavities vs Cryo-modules, iCav " 
              << iCav << " NumCryo " << cryoMods.size();
//    BOOST_FAIL(sOut.str().c_str());
    std::cerr << sOut << endl;	      
    return false;
  }
//  std::cout << " LucretiaMisalignSet, num Cryo modules " << cryoMods.size() << ", Cavities "
//            << iCav << std::endl; 
  return true; 
}

void LucretiaMisalignSet::setElements() {

  QuadVOffsets.clear();
  CavVTilts.clear();
  CavVOffsets.clear();
  BPMVOffsets.clear();
  size_t iCav = 0;
  size_t iBPM = 0;
  for(std::vector<cryoModule>::const_iterator itMod = cryoMods.begin(); 
            itMod != cryoMods.end(); ++itMod) {
     // Cavities 
     double sLength = itMod->sEnd - itMod->sBegin; // Should not need it, for debugging.    
//     if (iCav < 40) std::cerr << " Module, sBegin " << itMod->sBegin << " end at "  << itMod->sEnd
//               << " yOff " << itMod->yOff << " cavPtrs.size " << itMod->cavPtrs.size()  << endl;
	        
     for(size_t iCavLoc = 0; iCavLoc != itMod->cavPtrs.size(); iCavLoc++) {
        double sLocal =  itMod->sLocs[iCavLoc];
        double d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        double yy = d * mSigmas.cavDispy;
	yy += sLocal*itMod->yTilt + itMod->yOff;
	CavVOffsets.push_back(yy);
        d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        double yt = d * mSigmas.cavTilty;
	yt += itMod->yTilt;
	CavVTilts.push_back(yt);
	iCav++; 
     } 
//     if (iCav < 40) std::cerr << " Loaded this module... " << endl; 
        
     if (itMod->hasQuad) {
//        std::cerr << " Quad at s = " << 
//	  ( itMod->sBegin + (itMod->sEnd - itMod->sBegin)/2.) << endl; 
        double d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        double yy = d * mSigmas.quadDispy;
	yy += itMod->yOff;
	QuadVOffsets.push_back(yy);
        // Lock the BPM to the Quad, but add a shift 
        d = 1.e10;  
        while (std::abs(d) > mSigmas.sigmaCut) d = mySteerUtilPtr->myGauss.getValue();
        double yyBPM = yy +  d * mSigmas.bpmDispy;
	BPMVOffsets.push_back(yy);
	iBPM++;
     }    
  }
  std::cout << " LucretiaMisalignSet, Final ICav Index " << iCav << "BPM/Quad  "
            << iBPM << std::endl; 
  
   
}

double LucretiaMisalignSet::getQuadVerticalOffset(size_t index) const {
  if (index >= QuadVOffsets.size()) return 0.;
  return QuadVOffsets[index];
} 
   
double LucretiaMisalignSet::getCavVerticalOffset(size_t index) const {
  if (index >= CavVOffsets.size()) return 0.;
  return CavVOffsets[index];
} 
   
double LucretiaMisalignSet::getCavVerticalPitch(size_t index) const {
  if (index >= CavVTilts.size()) return 0.;
  return CavVTilts[index];
}    
double LucretiaMisalignSet::getQuadVerticalPitch(size_t index) const {
// Not implemented 
  return 0.;
}    
double LucretiaMisalignSet::getQuadRoll(size_t index) const {
  if (index >= QuadVTilts.size()) return 0.;
  return CavVTilts[index];  

}	
double LucretiaMisalignSet::getVerticalBPMOffset(size_t index) const {
   if (index >= BPMVOffsets.size()) return 0.;
   return BPMVOffsets[index];
}
void LucretiaMisalignSet::printIt() const{

  std::cout << " Lucretia Style Misalingment Set paramaterers " << std::endl;
  std::cout << " Quadrupole, Offsets, X " << mSigmas.quadDispx 
            << " Y = " << mSigmas.quadDispy << " Rolls " <<  mSigmas.quadRollxy << std::endl;
  std::cout << " BPM, Offsets, X " << mSigmas.bpmDispx 
            << " Y = " << mSigmas.bpmDispy <<  std::endl;
  std::cout << " Cavities, Offsets, X " << mSigmas.cavDispx 
            << " Y = " << mSigmas.cavDispy <<  std::endl;
  std::cout << " Cavities, Tilts, X " << mSigmas.cavTiltx  << " Y = " << mSigmas.cavTilty <<  std::endl;
  std::cout << " Starting at Quad numb. " << mSigmas.nQuadMisStart  
            << " Cav  " << mSigmas.nCavMisStart
            << " BPM = " << mSigmas.nQuadMisStart << std::endl;

}  
bool LucretiaMisalignSet::getFromSolyakFiles(const char* dirPath, bool v4) {

  QuadVOffsets.clear(); 
  BPMVOffsets.clear(); 
  CavVOffsets.clear(); 
  CavVTilts.clear(); 
  QuadVTilts.clear();
  
  std::string dirPathStr(dirPath);       
  if (dirPathStr.rfind("/") != (dirPathStr.length() -1))
    dirPathStr += std::string("/");

  std::vector<std::string> fTypes;
  fTypes.push_back("cavoff"); fTypes.push_back("cavpitch"); 
  fTypes.push_back("qoff"); fTypes.push_back("qtilt");  // Actually, a Roll.
  fTypes.push_back("bpmoff");    
  fTypes.push_back("ycor");
  
  // Add first dummy BPM and first quadrupole. 
  
  if (v4) { 
   QuadVOffsets.push_back(0.);	 
   QuadVTilts.push_back(0.);
   BPMVOffsets.push_back(0.);
  }
  
   for (int iCase = 0; iCase != fTypes.size(); iCase++) {
    int n = 0;	
    std::string fName(dirPathStr);
    fName += fTypes[iCase] + std::string("_6.txt");
    std::ifstream fIstr(fName.c_str());
    fIstr.exceptions(std::ios_base::eofbit);
    if (!fIstr.good()) {
      std::cerr << " Bad file name ,  " << fName << endl;
      return false;
   }
    while (true) {
     double aVal;
     try {
       fIstr >> aVal;
     } catch (std::ios_base::failure) { 
//       std::cout << " Reading " << fTypes[iCase] << " Eof after " 
//                 << n << " elements " << endl; 
       break;
     }
       if (iCase == 0) CavVOffsets.push_back(aVal);
       else if (iCase == 1) CavVTilts.push_back(-1.0*aVal); // Change sign, per directove from Francois.  
       else if (iCase == 2) QuadVOffsets.push_back(aVal);
       else if (iCase == 3) QuadVTilts.push_back(aVal);
       else if (iCase == 4) BPMVOffsets.push_back(aVal);
       else if (iCase == 5) YCorr.push_back(aVal);
       n++;
    } 
  } // Over files 
  return true;    
}

void LucretiaMisalignSet::realignBPMs() {
  for (size_t iBPM=0; iBPM !=BPMVOffsets.size(); ++iBPM) BPMVOffsets[iBPM] = 0.; 
}

bool LucretiaMisalignSet::setSolyakYCor(SteerUtil* aSt, double fact) {
  
 for (int iDip=0; iDip != YCorr.size(); iDip++) {
    // Assuming Tesla Meter from Lucreation. 
    double newVal = fact*YCorr[iDip];
    aSt->SetOneDipole(iDip, newVal);
//    if (iDip < 10) std::cout << " New Value  " << aSt->GetOneDipoleSetting(iDip)
//                             << " Old " << oldVal << endl;
 }
 return true;
}   
// ILC2006e lattice support, adding modeling shaft, local monuments. 
// Still 8 cavities per cryo modules. 

ILC2006eSigmas::ILC2006eSigmas() :
shaftMonumentVOffset(3.0e-4),
localMonumentVOffset(3.0e-4),
distLocalMonuments(50.),
distShaftMonuments(2500.) {
  // Make here adjustments .. 

}

ILC2006eMisalignSet::ILC2006eMisalignSet(SteerUtil* aSteerUtil, 
	                    const ILC2006eSigmas& misAlignCnst) :
mySteerUtilPtr(aSteerUtil),
mSigmas(misAlignCnst),
lucMisSet(aSteerUtil,misAlignCnst.lucSigmas)  {
  
  
  setShaftMonument();
  lucMisSet.setCryoModules();
  setElements();
}
			    
void ILC2006eMisalignSet::setShaftMonument() {
   
   // Compute the total length. 
   std::cerr << " Not implemented yet ... " << std::endl;
   exit(2);
   double sTot = 0.;
   BmlPtr aModel = mySteerUtilPtr->getTheModelPtr();
   
   for(beamline::iterator dbi = aModel->begin(); 
        dbi != aModel->end(); ++dbi) {
        ElmPtr q = *dbi;	
         sTot += q->Length();
   }
}			    

void ILC2006eMisalignSet::setElements() {
   
   std::cerr << " Not implemented yet ... " << std::endl;
   exit(2);

}

double ILC2006eMisalignSet::getQuadVerticalOffset(size_t index) const {
   std::cerr << " Not implemented yet ... " << std::endl;
   return 0.;
} 
   
double ILC2006eMisalignSet::getCavVerticalOffset(size_t index) const {
   std::cerr << " Not implemented yet ... " << std::endl;
   return 0.;
} 
   
double ILC2006eMisalignSet::getCavVerticalPitch(size_t index) const {
   std::cerr << " Not implemented yet ... " << std::endl;
   return 0.;
} 
   
double ILC2006eMisalignSet::getQuadRoll(size_t index) const {
   std::cerr << " Not implemented yet ... " << std::endl;
    return 0.;
}
	
double ILC2006eMisalignSet::getVerticalBPMOffset(size_t index) const {
   std::cerr << " Not implemented yet ... " << std::endl;
   return 0.;
}
