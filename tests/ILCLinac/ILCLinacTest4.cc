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

/*! ILC Linac Test 4: 

   Steering of the ILC Main Linac, limited edition. 
   As the other tests, we start by parsing the XSIF file, and adpting it 
   to the steering problem.  Actuator are dipole correctors ( VKick) 
   Responders are the BPMs.  Algorithm is pure Dispersion Matching.

      Paul L. G. Lebrun, January/February 2008 
   
*/

// Global pointer initialization

SteerUtil* Steering::theSteerUtil=0;

// A module will be common to this test and test 3 and 4. 

BmlPtr PrepareILCLinac(const char* latticeFileName, MLBPMs* myBPMs);

int test_main( int, char *[] )             // note the name!  the real main comes from Boost Test. 
{
  
  // Get the lattice file...
   
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) {    
//     BOOST_FAIL( " Environment varialble where test data resides not defined " );
    ilcTestDataChr = (char *) defDataDir.c_str();
  }
  
  std::string ilcTestDataStr(ilcTestDataChr);
  ilcTestDataStr += std::string("/Test4/ilc_linacKirti4S.xsif");
  
  MLBPMs* myBPMs = new MLBPMs(); // We keep track of our instrumentation.. 
  myBPMs->setResolution(5.e-10); // extremly good resolution, size of Niobium atom, or so! 
  

  BmlPtr bmlPtrFull = PrepareILCLinac(ilcTestDataStr.c_str(), myBPMs );
  
  if (bmlPtrFull == 0) {
           BOOST_FAIL(
	   " Failure to prepare the lattice. See stderr log for details  ");
  }
  
//  std::cerr << " ...Got a Linac defined " << std::endl;
  double energy =  bmlPtrFull->Energy();
  if (std::abs(energy - 15.0) > 1.e-6) {
    ostringstream sOut;
    sOut <<  " Unexpected Nominal energy of " << energy  << " for this lattice " << std::endl;
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
//  std::cerr << " ...Got a Utility, with a Positron State " << std::endl;
  
  myUtility.setInputTwissParam(aTWParam);
  myUtility.setBunchDPoP(0.01);   // 1 percent momentum spread coming out of the Bunch Compressor. 
  myUtility.setBunchSigmaZ(3.0e-4); // 300 micron bunch length 
  std::string fDirTop(ilcTestDataChr); // Directory path to write a file with results 
  fDirTop += std::string ("/");
  std::string noPart(""); // no extra file with detailed particle trajectory
  // Prepare the Steering Plan. 
  Steering::DMSPlan aPlan; // use default values.
  // Chenge default values for the Steering plan here... 
//  std::cerr << " ...Got a Utility, with a DMS " << std::endl;
   
  std::string afNameRespMatrix(ilcTestDataChr);
  afNameRespMatrix += std::string("/Test4/Default_RespMatrices.txt");

  // Set the threshold for when to quit on a local iteration
  // And the weight of the 1 to 1 component to the response matrix..  
  
  aPlan.minChangeDipole = 5.0e-5;
  aPlan.weightAbsolute = 0.333e-12;
  aPlan.numPseudoParticles = 50;
  aPlan.macroGradientFraction=0.975;
  aPlan.scaleResponseMatrix=1.0;
  aPlan.numMaxLocalIteration=100;
  std::string labelSteerGTest("SteerDMSk1Test");
  
//  std::cerr << " ... Before fillResponsesPerfect, fName   " 
//            << afNameRespMatrix << " label " << labelSteerGTest << std::endl;
  
  aPlan.fillResponsesPerfect(afNameRespMatrix, labelSteerGTest, false); //set it to false 

//  std::cerr << " ... fillResponsesPerfect done " << std::endl;

  // Now, we misaligned the lattice and run this test again.. 
  // To do this, we invoke the big Steering Utility class.  
  // Many method won't be used here, yet, 
  // this is what is most convenient. 
  //
  LucretiaSigmas sigA; // a default data structure to interface to the beam definition ala Lucretia. 
  LucretiaMisalignSet misA(&myUtility, sigA, true); 
  // this last argument instructs the shift downstream the offsets by 1/2 a
  // cell, since a dummy quadrupole and BPM have been added in the beginning 
  // of the lattice.
  if (!misA.areQuadMisaligned()) {
           BOOST_FAIL(
	   " Failure to find Misalignment files ");
  }		       
  myUtility.MoveAllElements(misA);

  // Now we steer ....
  
  int nnIter = Steering::SteerWeightedDMS(bmlPtrFull, labelSteerGTest, aPlan);
  // 
 // Now compare with nominal answers. 
 //
  std::string labelSteerGNom("SteerDMSk1");
  bool allOK = false;
    // Now check the result.. 
    std::string fNameTest(fDirTop);  fNameTest += std::string("/Test4/StPf_");
    fNameTest += labelSteerGTest; fNameTest += std::string("_finalWW.txt");
    std::string fNameNom(fDirTop);  fNameNom += std::string("/Test4/StPf_");
    fNameNom += labelSteerGNom; fNameNom += std::string("_finalWW.txt");
    std::ifstream fInTest(fNameTest.c_str());
    std::ifstream fInNom(fNameNom.c_str());
    if (!fInNom.good()) {
           std::ostringstream sOut0; 
	   sOut0 << " Can not open nominal result file "  << fNameNom <<  std::endl;
           BOOST_FAIL(sOut0.str().c_str());
    }
    int iBPMTest = 0;
    int iBPMNom = 0;
    size_t il = 0;
    double epsily0T = 0.;
    double epsily0N = 0.;
    while (fInNom.good()) {
      if (!fInTest.good()) { 
           std::ostringstream sOut1; 
	   sOut1 << " Case " << labelSteerGNom << ", incomplete bunch propagation. "  << std::endl;
	   sOut1 << " Reached BPM " << iBPMTest  << std::endl;
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
        if (std::abs(grN-grT) > 0.5) {
            std::ostringstream sOut2; 
	   sOut2 << " Case " << labelSteerGNom << ", incomplete bunch propagation. "  << std::endl;
	   sOut2 << " Reached BPM " << iBPMTest << " espil Nominal " << epsyNN 
	         << " this test  " << epsyNT << std::endl;
           BOOST_FAIL(sOut2.str().c_str());
	}
      }
      if (iBPMTest > 90) allOK = true; // forgiveness for the last BPM... Not sure why,  
    } // Check line by line.  
    if (!allOK) {
            std::ostringstream sOut3; 
	   sOut3 << " Case " << labelSteerGNom << ", incomplete bunch propagation. "  << std::endl;
	   sOut3 << " Failure to propagate a test bunch to last BPM " << iBPMTest << std::endl;
           BOOST_FAIL(sOut3.str().c_str());
  
    }
  return 0;
}
