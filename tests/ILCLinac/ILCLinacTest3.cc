#include <iostream>
#include <string>
#include <stdlib.h>

#include <boost/test/minimal.hpp>

#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/quadrupole.h>
#include <beamline/kick.h>
#include <beamline/monitor.h>
#include <beamline/drift.h>
#include <beamline/Particle.h>
#include <beamline/RefRegVisitor.h>
#include <beamline/Alignment.h>
#include <parsers/xsif/XSIFFactory.h>

#include <SteerUtil.h>
#include <ILCMisalignmentSets.h>
#include <SteeringAlgos.h>

/* ILC Linac Test 3: 

   Very similar to ILC Linac Test2, except instead of running a single positron, we 
   run a bunch, with wake fields in the cavities,
    and (i) measure the emittance dilution in the perfectly aligned lattice 
   (should be less than 1 nm), and in the misaligned lattice, for a bunch with the nominal 
   ILC charge (2e10 positrons) and for a weak charge.    
      Paul L. G. Lebrun, January/February 2008 
   
*/
 using std::endl;
// Global pointer initialization

SteerUtil* Steering::theSteerUtil=0;

// A module will be common to this test and test 3 and 4. 

BmlPtr PrepareILCLinac(const char* latticeFileName, MLBPMs* myBPMs);

int test_main( int, char *[] )             // note the name!  the real main comes from Boost Test. 
{
  
  // Get the lattice file...
   
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0)  { 
//     BOOST_FAIL( " Environment varialble where test data resides not defined " );
      ilcTestDataChr = (char *) defDataDir.c_str();
  }
  
  std::string ilcTestDataStr(ilcTestDataChr);
  bool shortLattice = false;
  if (shortLattice) 
     ilcTestDataStr += std::string("/ilc_linacKirti3S.xsif");
   else 
     ilcTestDataStr += std::string("/ilc_linacKirti3.xsif");
     
  
  MLBPMs* myBPMs = new MLBPMs(); // We keep track of our instrumentation.. 
  myBPMs->setResolution(5.e-10); // extremly good resolution, size of Niobium atom, or so! 
  

  BmlPtr bmlPtrFull = PrepareILCLinac(ilcTestDataStr.c_str(), myBPMs );
  
  if (bmlPtrFull == 0) {
           BOOST_FAIL(
	   " Failure to prepare the lattice. See stderr log for details  ");
  }
  
  double energy =  bmlPtrFull->Energy();
  if (std::abs(energy - 15.0) > 1.e-6) {
    ostringstream sOut;
    sOut <<  " Unexpected Nominal energy of " << energy  << " for this lattice " << endl;
    BOOST_FAIL(sOut.str().c_str());
  }

  // Define the nominal Positron
  
  Vector stateIn(6);
  for (size_t k=0; k != Particle::PSD; ++k) stateIn[k] = 0.;
  Positron pZero(energy, stateIn);
  
  stateIn[1] =  -18.146E-6; // non-trivial injection coordinate on the vertical 
                            // plane.  These are the nominal values,
			    //  recommended for optimum performance of this lattice. 
  stateIn[4] = 2.4347E-6;
  
   // Instantiate a set of utilities to allow us to run a beam. 
  SteerUtil myUtility(bmlPtrFull, myBPMs);
  Steering::theSteerUtil=&myUtility; // set the only global pointer in this application.
  myUtility.setStateAtInjection(stateIn);
  // Define a set of Twiss Parameter
  myTwissParam aTWParam;
  aTWParam.defined = true;
  aTWParam.betaX = 117.1429; // in meters 
  aTWParam.alphaX = -1.6792; // in meters 
  aTWParam.betaY = 55.8316; // in meters 
  aTWParam.alphaY = 0.8194; // in meters 
  aTWParam.DY = 0.96510E-3;  // Vertical Dispersion at injection
  aTWParam.DYp = -15.3113E-6 ; // Dispersion prime 
  aTWParam.DX = 0.;
  aTWParam.DXp = 0.; 
  
  myUtility.setInputTwissParam(aTWParam);
  myUtility.setBunchDPoP(0.01);   // 1 percent momentum spread coming out of the Bunch Compressor. 
  myUtility.setBunchSigmaZ(3.0e-4); // 300 micron bunch length 
  std::string fDirTop(ilcTestDataChr); // Directory path to write a file with results 
  fDirTop += std::string ("/");
  std::string noPart(""); // no extra file with detailed particle trajectory
  double emittCut[3]; 
  emittCut[0] = 0.25; emittCut[1] = 0.5; emittCut[2] = 0.75;
  for (int iCase=0; iCase !=3; iCase++) {
    myUtility.setWake(true);  // Nominal bunch charge for first and last Case  
    std::string testLabel(""); std::string nomLabel(""); 
    // First case, perfectly aligned 
    if (iCase == 0) {
      testLabel += std::string("Bunch-PerfectAlignmentTest"); // Token appearing in file name in data directory  
      nomLabel += std::string("Bunch-PerfectAlignment");   
    } else if (iCase == 1) {
      myUtility.setWake(false);
      testLabel += std::string("Bunch-MisAlignNoWakeTest");  
      nomLabel += std::string("Bunch-MisAlignNoWake"); 
    } else { 
      testLabel += std::string("Bunch-MisAlignWithWakeTest");  
      nomLabel += std::string("Bunch-MisAlignWithWake");  
    }
    Steering::StudyPerformance(bmlPtrFull, fDirTop,  testLabel, noPart); // go
     // 
    // Now compare with nominal answers. 
    //
    bool allOK = false;
    // Now check the result.. 
    std::string fNameTest(fDirTop);  fNameTest += std::string("StPf_");
    fNameTest += testLabel; fNameTest += std::string(".txt");
    std::string fNameNom(fDirTop);  fNameNom += std::string("StPf_");
    fNameNom += nomLabel; fNameNom += std::string(".txt");
    std::ifstream fInTest(fNameTest.c_str());
    std::ifstream fInNom(fNameNom.c_str());
    int iBPMTest = 0;
    int iBPMNom = 0;
    size_t il = 0;
    double epsily0T = 0.;
    double epsily0N = 0.;
    while (fInNom.good()) {
      if (!fInTest.good()) { 
           std::ostringstream sOut1; 
	   sOut1 << " Case " << nomLabel << ", incomplete bunch propagation. "  << endl;
	   sOut1 << " Reached BPM " << iBPMTest  << endl;
           BOOST_FAIL(sOut1.str().c_str());
      } 
      char lineCT[1024];
      char lineCN[1024];
      fInTest.getline(lineCT, 1024);
      fInNom.getline(lineCN, 1024);
      il++;
      if (il == 1) continue; // skip the header 
      double yT=0.; double yN=0.; double ypT=0.; double ypN=0.; double sT=0.; double sN=0.; 
      double epsyN = 0.; double epsyT = 0.;
      double epsyNN = 0.; double epsyNT = 0.;
      std::string lineT(lineCT);
      std::string lineN(lineCN);
      std::istringstream lldStrT(lineT);
      std::istringstream lldStrN(lineN);
      lldStrT >> iBPMTest >> sT >> yT >> ypT >> epsyT >> epsyNT;
      lldStrN >> iBPMNom >> sN >> yN >> ypN >> epsyN >> epsyNN;
      // Check the normalized emittance...
      if (iBPMTest == 0) {
         epsily0T = epsyNT;
	 epsily0N = epsyNN;
      } else { 
        double grN = epsyNN - epsily0N; 
        double grT = epsyNT - epsily0T; 
        if (std::abs(grN-grT) > emittCut[iCase]) {
            std::ostringstream sOut2; 
	   sOut2 << " Case " << nomLabel << ", incomplete bunch propagation. "  << endl;
	   sOut2 << " Reached BPM " << iBPMTest << " espil Nominal " << epsyNN 
	         << " this test  " << epsyNT << endl;
           BOOST_FAIL(sOut2.str().c_str());
	}
      }
      if (iBPMTest > 238) allOK = true; // forgiveness for the last BPM... Not sure why,  
      if (shortLattice && (iBPMTest == 38)) {
          allOK = true;
	  break;  
      }
    } // Check line by line.  
    if (!allOK) {
            std::ostringstream sOut3; 
	   sOut3 << " Case " << nomLabel << ", incomplete bunch propagation. "  << endl;
	   sOut3 << " Failure to propagate a test bunch to last BPM " << iBPMTest << endl;
           BOOST_FAIL(sOut3.str().c_str());
  
    }
    // We decide to re-register.. it should work.   
/*  A test for great experts.. 
    Vector stateIn(6);
    stateIn[1] =  -18.146E-6; // non-trivial injection coordinate on the vertical 
                            // plane.  These are the nominal values,
			    //  recommended for optimum performance of this lattice. 
    stateIn[4] = 2.4347E-6;
    Positron p1(energy, stateIn);
    RefRegVisitor  rfgNew( p1);
    rfgNew.visit( *bmlPtrFull );
    if (rfgNew.getErrorCode() != RefRegVisitor::OKAY) {   
       std::cerr << " Registration process failed " << endl;
    }
*/
    if (iCase == 0) { 
      // Now, we misaligned the lattice and run this test again.. 
      // To do this, we invoke the big Steering Utility class.  
      // Many method won't be used here, yet, 
      // this is what is most convenient. 
      //
      LucretiaSigmas sigA; // a default data structure to interface to the beam definition ala Lucretia. 
      LucretiaMisalignSet misA(&myUtility, sigA); 
      misA.realignBPMs();
      if (!misA.areQuadMisaligned()) {
           BOOST_FAIL(
	   " Failure to find Misalignment files ");
      }		       
      myUtility.MoveAllElements(misA);
      // Adjujusting the current in vertical dipole correctors
      misA.setSolyakYCor(&myUtility, 1.0);
      // Make a 5 % mistake! To test the test... 
//      double v6 = myUtility.GetOneDipoleSetting(6); 
//      myUtility.SetOneDipole(6, v6*1.05);
    }
    
  } // on Cases. 
  return 0;
}
