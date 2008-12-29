#include <iostream>
#include <string>
#include <stdlib.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/bmlnElmnt.h>
#include <parsers/xsif/XSIFFactory.h>
#include <beamline/beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/quadrupole.h>
#include <beamline/kick.h>
#include <beamline/monitor.h>
#include <beamline/drift.h>
#include <beamline/Particle.h>
#include <beamline/RefRegVisitor.h>
#include <SteerUtil.h>
#include <beamline/Alignment.h>
#include <ILCMisalignmentSets.h>

/* ILC Linac Test 2: 

   Support free function which is used in test 2,3, and 4. 
   Parse and XSIF file, replace Monitors by BPMs, split the drifts into 3 parts
   and register the beam line. 
         
   Paul L. G. Lebrun, January/February 2008 
   
*/

using std::endl;

BmlPtr PrepareILCLinac(const char* latticeFileName, MLBPMs* myBPMs) {
  
  // Get the lattice file...
   
  
 // This is to ensure the default directory won't change under us.. 
 // This is probably not necessary for current version of CHEF, 
 // albeit might be platform dependent 
 
  char myCDir[1024];
  getcwd(myCDir, 1023);
  XSIFFactory bfFull(latticeFileName);
  chdir(myCDir);
  
   
  BmlPtr bmlPtrFull = bfFull.create_beamline("LINAC");
   
  if (!bmlPtrFull) {  
     std::cerr << " Beamline Pointer returned null, XSIF parser failure " << endl;
     return BmlPtr();
  }
   
   // flatten it, speed improvements. (perhaps); 
   
   bmlPtrFull = BmlPtr( new beamline(bmlPtrFull->flatten() ));
 //
 // Check the nominal injection energy.  Must be 15 GeV for this lattice. 
 
 double energy =  bmlPtrFull->Energy();
  
  // 
  // We now instrument the machine, i..e, we make the specify the monitors by replacing them with 
  // Linac Beam Position Monitors  
  //
  int iBPM = 0;
  double s = 0.;
  BmlPtr newBeamline ( new beamline( bmlPtrFull->Name().c_str()));
  newBeamline->setEnergy( bmlPtrFull->Energy());
  for(beamline::deep_iterator dbi = bmlPtrFull->deep_begin(); 
        dbi != bmlPtrFull->deep_end(); ++dbi) {
    ElmPtr q = *dbi;
    if( MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) ) {
	// Replace it with my BPM
        q = MonitorPtr (new MLBPM(iBPM, s));
	newBeamline->append(q);
        myBPMs->addOne(boost::dynamic_pointer_cast<MLBPM> (q));        
//        std::cerr << " Replacing monitor by my BPMs at s = " << sTot << endl;
        iBPM++; 
      } else {
        newBeamline->append(q);
      }
      s += q->Length();
  }
  //
  // Replace... Note: we do not explicitely delete the previous beam line, this is done via 
  // the use of smart pointers. 
  //
  bmlPtrFull = newBeamline;
  if (bmlPtrFull == 0) {
           std::cerr << " Failure to replace Monitors with BPMs " << endl;
	   return BmlPtr();
  }
  
  // We also split the drift spaces, such that we can move arbitrarily all beam elements 
  
  BmlPtr newBeamline2 ( new beamline( bmlPtrFull->Name().c_str()));
  newBeamline2->setEnergy( bmlPtrFull->Energy());
  
  for(beamline::deep_iterator dbi = bmlPtrFull->deep_begin(); 
        dbi != bmlPtrFull->deep_end(); ++dbi) {
      ElmPtr q = *dbi;
      if( 0 == strcmp( "drift", q->Type() ) ) {
        double l = q->Length();
        newBeamline2->append(DriftPtr(new drift( "SPACER_1", 0.2*l ) ) ); 
        newBeamline2->append(DriftPtr(new drift( "SPACER_2", 0.6*l ) ) );
        newBeamline2->append(DriftPtr(new drift( "SPACER_3", 0.2*l ) ) );
      }
      else {
        newBeamline2->append(q);
      }
   }
   
  // Use this lattice...   
  bmlPtrFull = newBeamline2;
  if (bmlPtrFull == 0) {
           std::cerr << 
	   " Failure to replace split drift space to allow the cavity to move " << endl;
	   return BmlPtr();
  }
   // Define a positron 
    
  Vector stateIn(6);
  for (size_t k=0; k != Particle::PSD; ++k) stateIn[k] = 0.;
  Positron pZero(energy, stateIn);
  
  stateIn[1] =  -18.146E-6; // non-trivial injection coordinate on the vertical 
                            // plane.  These are the nominal values,
			    //  recommended for optimum performance of this lattice. 
  stateIn[4] = 2.4347E-6;
  Positron p1(energy, stateIn);
   
  // Establish timing, commission the beam line.. 
  
  RefRegVisitor  rfg( p1);
  rfg.visit( *bmlPtrFull );
  if (rfg.getErrorCode() != RefRegVisitor::OKAY) {   
       std::cerr << " Registration process failed " << endl;
       return BmlPtr();
  }
  return bmlPtrFull;
}
