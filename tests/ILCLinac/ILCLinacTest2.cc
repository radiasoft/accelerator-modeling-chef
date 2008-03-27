#include <boost/test/minimal.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <PhysicsConstants.h>
#include "bmlnElmnt.h"
#include <parsers/xsif/XSIFFactory.h>
#include <beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/quadrupole.h>
#include <beamline/kick.h>
#include <beamline/monitor.h>
#include <beamline/drift.h>
#include <Particle.h>
#include <RefRegVisitor.h>
#include "SteerUtil.h"
#include <beamline/Alignment.h>
#include "ILCMisalignmentSets.h"

/* ILC Linac Test 2: 

   Read a lattice file, Define a Positron, register the beam line, propagate an other positron 
   Check the trajectory and the momentum at the last BPM. 
   Replace the generic CHEF Monitors with our BPMs
   Split the drift space into segments such that the quadrupople and r.f. cavity can ba aritrarily 
   displaced. 
   Then, misalign the machine, progate an other positron, and check the trajectory again. 
       
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
  if (ilcTestDataChr == 0) {  
//     BOOST_FAIL( " Environment varialble where test data resides not defined " );
      ilcTestDataChr = (char *) defDataDir.c_str();
  }
  
  std::string ilcTestDataStr(ilcTestDataChr);
  ilcTestDataStr += std::string("/ilc_linacKirti3S.xsif");
  
  MLBPMs* myBPMs = new MLBPMs(); // We keep track of our instrumentation.. 

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

  
   // Define a positron 
    
  Vector stateIn(6);
  for (size_t k=0; k != Particle::PSD; ++k) stateIn[k] = 0.;
  Positron pZero(energy, stateIn);
  
  stateIn[1] =  -18.146E-6; // non-trivial injection coordinate on the vertical 
                            // plane.  These are the nominal values,
			    //  recommended for optimum performance of this lattice. 
  stateIn[4] = 2.4347E-6;
 
  Positron p2(energy, stateIn);

  int numBPMs = 0;
  bool allOK = false;
// To by pas this test. 
//  bool allOK = true;
  double s = 0.;
   for(beamline::iterator dbi = bmlPtrFull->begin(); 
        dbi != bmlPtrFull->end(); ++dbi) {
     ElmPtr q = *dbi;
     q->propagate(p2);
     s += q->Length();	
     if(MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) ) {
       if ((numBPMs < 39) && 
            (std::abs(p2.get_npy() - 3.35e-6 ) > 0.5e-6))  { // 1/2 micro-radians precision.
          ostringstream sOut; 
         sOut <<  " Wrong vertical kick angle " << p2.get_npy()  
	      << " at BPM " << numBPMs << " s = " << s << endl;
         sOut <<  " The radius of curvature should be around 3.3 micro-radiants. " << endl; 
         BOOST_FAIL(sOut.str().c_str());
       }
       if (numBPMs == 38) {
         if (std::abs(p2.ReferenceMomentum() - 54.656)/p2.ReferenceMomentum() > 1.0e-4)  { // Tolerance of .01 %
           ostringstream sOut; 
           sOut <<  " Unexpected Final energy of " << p2.ReferenceMomentum()  
	      << " for this lattice, " << endl;
           sOut <<  " Correct answer is  253.455 " << endl;
           BOOST_FAIL(sOut.str().c_str());
         }
         if (std::abs(p2.get_y()) > 0.5e-6)  { // 1/2 micron precision.
           ostringstream sOut; 
           sOut <<  " Wrong vertical position " << p2.get_y()  << " at BPM " << numBPMs <<  endl;
           sOut <<  " Deviation should be less than one half micron " << endl;
           BOOST_FAIL(sOut.str().c_str());
         }
         allOK=true;
       } // last BPM for this lattice. 
       numBPMs++;
     } // encountered a BPM 
  } // on beamline elements 
  if (!allOK) {
           BOOST_FAIL(
	   " Failure to propagate a test positron to last BPM through the perfectly aligned lattice");
  
  } 
  
  // Now, we misaligned the lattice and run this test again.. 
  // To do this, we invoke the big Steering Utility class.  Many method won't be used here.
  //  yet, this is what is most convenient. 
  //
  SteerUtil myUtility(bmlPtrFull, myBPMs);
  myUtility.setStateAtInjection(stateIn);
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
  

  // Back to Initial conditions. 
  Positron p3(energy, stateIn);
  numBPMs = 0;
  allOK = false;
  s = 0.;
  
  // 
  // Load the expected trajectory, given this set of misalignments. 
  //
  std::vector<double> sLoc(0);
  std::vector<double> yNominal(0);
  std::vector<double> yPNominal(0);
  
  std::string fName(ilcTestDataChr);
  fName += std::string ("/ILCLinacTest2_NominalTrajectory.txt");
  std::ifstream fIstr(fName.c_str());
  fIstr.exceptions(std::ios_base::eofbit);
  if (!fIstr.good()) {
      ostringstream sOut; 
      sOut <<  " Could not find the Nominal trajectory file for ILCLinacTest2 " << fName  << endl;
      BOOST_FAIL(sOut.str().c_str());
   }
  int iBPM = 0;
  while (iBPM != 39) {
     double s; double y; double yp; 
     try {
       fIstr >> iBPM >> s >> y >> yp;
     } catch (std::ios_base::failure) { 
        ostringstream sOut; 
         sOut <<  " Incomplete or corrupted Nominal trajectory file for ILCLinacTest2 " 
	     << fName  << " last seen BPM " << iBPM << endl;
        BOOST_FAIL(sOut.str().c_str());
     }
     sLoc.push_back(s); yNominal.push_back(y); yPNominal.push_back(yp);
  }
  fIstr.close();
  
//  std::ofstream fOutD("ILCTest2NomTrajCheck.txt");
  
  iBPM = 0;
  for(beamline::iterator dbi = bmlPtrFull->begin(); 
        dbi != bmlPtrFull->end(); ++dbi) {
     ElmPtr q = *dbi;
     q->propagate(p3);
     s += q->Length();

     double rSq = p3.get_y()*p3.get_y() + p3.get_x()*p3.get_x();
     if (rSq > 1.0e-3) {
           ostringstream sOut; 
           sOut <<  " Tracking Excessive deviation of " << std::sqrt(rSq)  << " m. at s = " << s << endl;
           BOOST_FAIL(sOut.str().c_str());
	   break;
     }
     MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q);
     if(aBPM != 0) {
//       fOutD << " " << iBPM << " " << s << " " 
//             << 1.0e6*p3.get_y() << " " << 1.0e6*p3.get_npy() << std::endl;
       double deltaY = std::abs(1.0e6*p3.get_y() - yNominal[iBPM]); // in microns    
       if (deltaY > 1.0) { // one micron tolerance.
         
         ostringstream sOut; 
         sOut <<  " Trajectory deviation for ILCLinacTest2 at BPM " 
	     << iBPM  << " delta y " << deltaY << " microns " << endl;
         BOOST_FAIL(sOut.str().c_str());
       }    
       if (iBPM > 38) allOK = true;
       iBPM++;
     } // encountered a BPM 
  } // on beamline elements 
//  fOutD.close();
  if (allOK)return 0;
  else return 1;
}
