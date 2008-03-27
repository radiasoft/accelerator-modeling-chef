////////////////////////////////////////////////////////////
// 
// File:          demo_5.cc    
// Author:        Paul Lebrun, based on demo_5c, the Tesla Linac Bench 3 steering code. 
// Revision date: May 16 2006
////////////////////////////////////////////////////////////
// 
// Demonstration program for testing Steering utilities.
// 
// Reads a MAD v.8 .lat file or a .bml file, constructs 
// a beamline, generates a uniformly distributed ensemble 
// of particles, and propagates it down the line.
// 
// Command line: demo_3 [options] filename beamline cell momentum(GeV/c)
// 
// where filename           is the name of a .lat file or a .bml file.
//       beamline           is the name of the beamline within the .lat
//                          file that is to be constructed; ignored if
//                          a .bml file is specified.
//       cell               is the name of a standard cell within the beamline
//       momentum [GeV/c]   If a .lat file is specified, this will
//                          become the reference momentum of the
//                          line. The argument is ignored for a .bml
//                          file.
// 
// In the following, NN represents a number.
// 
// Options: See code below.. 
// 
////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include <PhysicsConstants.h>
#include "bmlnElmnt.h"
#include <bmlfactory.h>
#include <parsers/xsif/XSIFFactory.h>
#include <beamline.h>
#include <drift.h>
#include <BmlUtil.h>
#include <beamline/RefRegVisitor.h>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "myPositronBunch.h"
#include "MLBPMs.h"
#include "SteerUtil.h"
#include "ILCMisalignmentSets.h"
#include "SteeringResponseMatrix.h"
#include "SteeringAlgos.h"
#include "SteerAnalysis.h"

typedef boost::uniform_real<double>
        basUnifGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> >
        varUnifGen;
typedef boost::normal_distribution<double>
        basGaussGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> >
        varGaussGen;

const double mm           = 0.001;
const double percent      = 0.01;
const double GHz          = 1.0e9;
const double microsecond  = 1.0e-6;
const double microradian  = 1.0e-6;
const double nanosecond   = 1.0e-9;
const double picosecond   = 1.0e-12;

using namespace std;

void printKickAngles(const BmlPtr& bmlPtr, int nMax);

struct Parameters
{
                   // All parameters represent initial conditions
  double eps_1;    // [pi mm-mr]  sigma horizontal invariant emittance
  double eps_2;    // [pi mm-mr]  sigma vertical   invariant emittance
  double eps_3;    // [eV-sec]    max longitudinal emittance (NOT USED)
  double sigdPoP;  //  Delta P / P 
  double sigTau;   // Bunch length. 
  double dx;       // [mm]        horizontal beam offset
  double dy;       // [mm]        vertical   beam offset
  double dt;       // [musec]     injection time error: dt > 0 -> late
  int    n;        //             number of Positrons in the bunch
  int misAlignNum; // The misalignment file
                   // if 9999, use random 
  long randomSeed;    // the random number seed for this job. 		   
  // These above are kind of obsolete... 
  // Tuning the Steering... See data structure DFSPlan
  int macroNumMacroIter;
  int macroNumExtraBPMs;
  int macroNumDipoles;
  double macroGradientFraction; 
  std::string labelRun;
  bool moveCavities;
  double kScaleRM; 
  double weightDMS;
  double weightAbs;
  double weightCorr;
  double weightSVD; // NullSpace SVD cut. 
  double gainFact;
  double beamJitterY;
  int withWakeCode;
  bool withPreSteer;
  bool usePerfectDispKnow;
  bool doDynamics;
  // Dynamic stuff
  
  
};


bool getParameters( Parameters& w, int argc, char** argv )
{
  w.eps_1 = 1.0e-6 * M_PI*0.4;
  w.eps_2 = 1.0e-6 * M_PI*0.02;
  w.eps_3 = 1.0e-6; // ignored I think.. considered fixed 
                    // in the constructor of steer util for now. 
  w.sigdPoP = 0.01;
  w.sigTau = 3.0e-4;
  w.dx    = 0;
  w.dy    = 0;
  w.dt    = 0;
  w.n     = 1000;
  w.macroNumMacroIter = 1;
  w.macroNumExtraBPMs = 0;
  w.macroNumDipoles = 20;
  w.macroGradientFraction = 0.97;
  w.labelRun = std::string("D");
  w.moveCavities = true;
  w.kScaleRM = 0.1; 
  w.weightDMS = 0.07;
  w.weightAbs = 0.00277;
  w.weightCorr = 1.0e-10;
  w.weightSVD = 0.05;
  w.gainFact = 0.5;
  w.beamJitterY = 0.25;
  w.misAlignNum = 999999;
  w.randomSeed = 134513463;
  w.withWakeCode=0; // 0 = Not at all, 1 wake applied in Final, 
                    // 2 = during steering 
  w.withPreSteer=false;
  w.usePerfectDispKnow=true;
  w.doDynamics = false;
  std::cout << " Input arguments are: " << std::endl;
  for (int ii=0; ii != argc; ii++) 
    std::cout << " " << argv[ii];
  std::cout << std::endl << " ------------------------------- " 
            << std::endl << std::endl;
  
  int i = 1;
  int limit = argc - 4;
  string s;
  while( i < limit ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      if( 'x' == s[1] ) {
        if( i < limit ) { w.dx = 0.001 * atof( argv[i++] ); }
      }
      else if( 'y' == s[1] ) {
        if( i < limit ) { w.dy = 0.001 * atof( argv[i++] ); }
      }
      else if( 't' == s[1] ) {
        if( i < limit ) { w.dt = 1.0e-6 * atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "numP" ) ) {
        if( i < limit ) { w.n = atoi( argv[i++] ); }
      }
      else if( string::npos != s.find( "e1" ) ) {
        if( i < limit ) { w.eps_1 = 1.0e-6 * M_PI * atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "e2" ) ) {
        if( i < limit ) { w.eps_2 = 1.0e-6 * M_PI * atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "e3" ) ) {
        if( i < limit ) { w.eps_3 = 1.0e-9 * atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "sigdP" ) ) {
        if( i < limit ) { w.sigdPoP = atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "sigTau" ) ) {
        if( i < limit ) { w.sigTau = atof( argv[i++] ); }
      }
      else if( string::npos != s.find( "label" ) ) {
        if( i < limit ) { w.labelRun = std::string(argv[i++]); }
      }
      else if( string::npos != s.find( "Ma" ) ) {
        if( i < limit ) { w.macroNumMacroIter = atoi(argv[i++]); }
      }
      else if( string::npos != s.find( "Nd" ) ) {
        if( i < limit ) { w.macroNumDipoles = atoi(argv[i++]); }
      }
      else if( string::npos != s.find( "kSc" ) ) {
        if( i < limit ) { w.kScaleRM = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "WDMS" ) ) {
        if( i < limit ) { w.weightDMS = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "WAbs" ) ) {
        if( i < limit ) { w.weightAbs = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "WSVD" ) ) {
        if( i < limit ) { w.weightSVD = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "WCor" ) ) {
        if( i < limit ) { w.weightCorr = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "NEB" ) ) {
        if( i < limit ) { w.macroNumExtraBPMs = atoi(argv[i++]); }
      }
      else if( string::npos != s.find( "Gf" ) ) {
        if( i < limit ) { w.macroGradientFraction = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "gF" ) ) {
        if( i < limit ) { w.gainFact = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "bjY" ) ) {
        if( i < limit ) { w.beamJitterY = atof(argv[i++]); }
      }
      else if( string::npos != s.find( "rr" ) ) {
        if( i < limit ) { w.randomSeed = atol(argv[i++]); }
      }
      else if( string::npos != s.find( "nMis" ) ) {
        if( i < limit ) { w.misAlignNum = atoi(argv[i++]); }
      }
      else if( string::npos != s.find( "wWake" ) ) {
        if( i < limit ) { w.withWakeCode = atoi(argv[i++]); }
      }
      else if( string::npos != s.find( "wPre" ) ) {
        if( i < limit ) { 
	  std::string boolChar(argv[i++]);
	  if (boolChar == std::string("T") || 
	      boolChar == std::string("t"))
	         w.withPreSteer = true; 
	  if (boolChar == std::string("F") || 
	      boolChar == std::string("f"))
	         w.withPreSteer = false;
	} 
      }
      else if( string::npos != s.find( "wPDisp" ) ) {
        if( i < limit ) { 
	  std::string boolChar(argv[i++]);
	  if (boolChar == std::string("T") || 
	      boolChar == std::string("t"))
	         w.usePerfectDispKnow = true; 
	  if (boolChar == std::string("F") || 
	      boolChar == std::string("f"))
	         w.usePerfectDispKnow = false;
	} 
      }
       else if( string::npos != s.find( "MvCav" ) ) {
        if( i < limit ) {
	  std::string boolChar(argv[i++]);
	  if (boolChar == std::string("T") || 
	      boolChar == std::string("t"))
	         w.moveCavities = true; 
	  if (boolChar == std::string("F") || 
	      boolChar == std::string("f"))
	         w.moveCavities = false; 
	}
      }
       else if( string::npos != s.find( "ddy" ) ) {
        if( i < limit ) {
	  std::string boolChar(argv[i++]);
	  if (boolChar == std::string("T") || 
	      boolChar == std::string("t"))
	         w.doDynamics = true; 
	  if (boolChar == std::string("F") || 
	      boolChar == std::string("f"))
	         w.doDynamics = false; 
	}
      }
     else {
        cout << "*** ERROR *** Cannot process command line arguments." << endl;
        return false;
      }
    }
    else {
      cout << "*** ERROR *** Cannot process command line arguments." << endl;
      return false;
    }
  }

  return true;
}



int main( int argc, char** argv )
{
  // Process command line arguments
  // ------------------------------
  if( argc < 5 ) {
    cout << "Usage: " << argv[0]
         << " [options]  filename  beamlinename  cellname  momentum(GeV/c)"
         << endl;
    return 1;
  }
  else {
    cout << "Command line:";
    for( int i = 0; i < argc; i++ ) {
      cout << " " << argv[i];
    }
    cout << '\n' << endl;
  }
  const char*  filename     =       argv[argc-4];
  const char*  beamlinename =       argv[argc-3];
  const char*  cellname     =       argv[argc-2];
        double momentum     = atof( argv[argc-1] );

  cerr << " Momentum is " << momentum << endl;
  // Read the parameters from the command line
  // -----------------------------------------
  Parameters param;
  if( !getParameters( param, argc, argv ) ) {
    return 4;
  }


  // Useful numbers
  // --------------
  double brho = momentum / PH_CNV_brho_to_p;
  double energy = sqrt( PH_NORM_me*PH_NORM_me + momentum*momentum );
// found in the original  ilc_linac.xsif

// COMMENT
//  BEAM_LINAC_IN : BEAM, E=15, NPART = 2E10, SIGT = 300E-6, SIGE = 0.0107, &
//       EX = 8E-6 * EMASS / 15, EY = 20E-9 * EMASS / 15
//
//  TWISS_LINAC_IN : BETA0, BETX = 35.3226, ALFX = -0.7486,    &
//                          BETY =  97.7362, ALFY =  1.65062,    &
//                          DY = 0.000496979, DPY = -5.93072e-06,  &
//                          Y = 0.0, PY = 0.0
//
  myTwissParam aP;
  std::string fileNamesLatStr(filename);
  std::string modelName("None");   
  if ((fileNamesLatStr.find("ML_RDR_v0") != std::string::npos) ||
       (fileNamesLatStr.find("ilc_linac") != std::string::npos) || 
       (fileNamesLatStr.find("ilcSh") != std::string::npos))  {
    modelName = std::string("ML_RDR_v0");
     aP.betaX = 117.1429; // in meters 
     aP.alphaX = -1.6792; // in meters 
     aP.betaY = 55.8316; // in meters 
     aP.alphaY = 0.8194; // in meters 
     aP.DY = 0.96510E-3;
//  aP.DY = 0.0006;  // Best performance for me...
     aP.DYp = -15.3113E-6 ;
     aP.DX = 0.;
     aP.DXp = 0.; 
    aP.defined = true;
   } else if  (fileNamesLatStr.find("elin") != std::string::npos) {
     modelName = std::string("ML_RDR_Final");
     aP.betaX = 100.942; // in meters 
     aP.alphaX = -0.31372; // in meters 
     aP.betaY = 51.4707; // in meters 
     aP.alphaY = 0.6450; // in meters 
     aP.DY = 0.000;
     aP.DYp = 0.;
     aP.DX = 0.;
     aP.DXp = 0.; 
     std::cerr << " Model Name is " << modelName << std::endl;
     aP.defined = true;
   } else {
     aP.defined = false;
     std::cerr << " Unknow Twiss paramter for this lattice. Give up " << std::endl;
     exit(2);
   } 
  
//............ The real problem start here.. 
    
 {
// Define a label for debugging output. 

  std::string labelSteerG(param.labelRun);
  bool chefWakes = true;
  bool withWakes = false;
  if (param.withWakeCode > 0) withWakes = true;
  
 // We start by defining short range wakes, long and transverse 
 // Now we use the Merlin Transfer map for quadrupole 
      
   char myCDir[1024];
   getcwd(myCDir, 1023);
   XSIFFactory bfFull( filename);
   chdir(myCDir);
   cerr << " Used bmlfactory on file " << filename << endl;
//   BmlPtr bmlPtrFull = bfFull.create_beamline("LELINAC", brho );
   BmlPtr bmlPtrFull = bfFull.create_beamline("LINAC", brho );
   // flatten it, speed improvements. 
   bmlPtrFull = BmlPtr( new beamline(bmlPtrFull->flatten() ));
    
    MLBPMs* myBPMs = new MLBPMs();
    // Set the RF cavities' frequency, which is not
    // contained in the MAD file.
    // --------------------------------------------
    cout << "Setting frequency of RF cavities to 1.3 GHz." << endl;
    double freq = 1.3021875*GHz; // from the lattice file!!! 
    int nCav = 0;
    double ssCav= 0.;
    
    for(beamline::iterator dbi = bmlPtrFull->begin(); 
        dbi != bmlPtrFull->end(); ++dbi) {
     ElmPtr q = *dbi;
     if(RFCavityPtr rfcPtr =  boost::dynamic_pointer_cast<rfcavity>(q) ) {
        std::cerr << " Affecting Cavity " << q->Type() << std::endl;
        rfcPtr->setFrequency( freq );

        // ... in addition, we change the synchronous phase
        // ... to correspond to linac convention:
        // ... cosine rather than sine
//        rfcPtr->setPhi( M_PI_2 - rfcPtr->getPhi() );
// Per suggestion of Leo/Francois, Friday April 13 
        rfcPtr->setPhi( M_PI_2 + rfcPtr->getPhi() );
	// tagging the cavities. 
	ostringstream fNameCCPtr;  fNameCCPtr << "C_" << nCav;
	std::string fNameCC = fNameCCPtr.str();
	rfcPtr->setTag(fNameCC);
//	if (nCav < 3000) cout << " cavity " << q->Name() << " num " << nCav 
//	    << " has phase " << rfcPtr->getPhi() << " at s = " << ssCav << endl; 
	    nCav++;
      }
      ssCav += q->Length();
    }
    cout << "Finished setting frequency." << endl;
    
    cout << " Adding the BPMs, inheriting from monitors " << endl;
    int iBPM = 0;
    double sTot = 0;
    BmlPtr newBeamline ( new beamline( bmlPtrFull->Name().c_str()));
    newBeamline->setEnergy( bmlPtrFull->Energy());
    for(beamline::deep_iterator dbi = bmlPtrFull->deep_begin(); 
        dbi != bmlPtrFull->deep_end(); ++dbi) {
      ElmPtr q = *dbi;
      if( MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) ) {
	// Replace it with my BPM
        q = MonitorPtr (new MLBPM(iBPM, sTot));
	newBeamline->append(q);
        myBPMs->addOne(boost::dynamic_pointer_cast<MLBPM> (q));        
//        std::cerr << " Replacing monitor by my BPMs at s = " << sTot << endl;
        iBPM++; 
      } else {
        newBeamline->append(q);
      }
      sTot += q->Length();
    }
    cout << " Number of BPM  " << iBPM << endl; 
    bmlPtrFull = newBeamline;

     cout << "Finished Replacing BPM.  Add spacer now " << endl;
  
  BmlPtr newBeamline2 ( new beamline( bmlPtrFull->Name().c_str()));
  newBeamline2->setEnergy( bmlPtrFull->Energy());
  double l;
  for(beamline::deep_iterator dbi = bmlPtrFull->deep_begin(); 
        dbi != bmlPtrFull->deep_end(); ++dbi) {
      ElmPtr q = *dbi;
      if( 0 == strcmp( "drift", q->Type() ) ) {
        l = q->Length();
        newBeamline2->append(DriftPtr(new drift( "SPACER_1", 0.2*l ) ) ); 
        newBeamline2->append(DriftPtr(new drift( "SPACER_2", 0.6*l ) ) );
        newBeamline2->append(DriftPtr(new drift( "SPACER_3", 0.2*l ) ) );
      }
      else {
        newBeamline2->append(q);
      }
   }
   bmlPtrFull = newBeamline2;
     cout << "Finished creating the line." << endl;
  std::cerr << " Print kick angles, after finish creating Beamline " << endl;
//  printKickAngles (bmlPtrFull, 10); 
  // commission the beamline for action.
  // -----------------------------------
  cerr << " Before Registrar " << endl;
  Positron pt( energy );
  pt.SetReferenceEnergy(bmlPtrFull->Energy()); 

  RefRegVisitor( pt).visit( *bmlPtrFull );
  cerr << " Regitration done  " << endl;

  cerr << " Ready to generate the bunch " << endl;
    
  // Create a reference Positron and 
  // We will use a steering utility 
  
  myBPMs->setResolution(5.e-10);
  
  // Must do this for the initalization.. only one utility is needed...

  MatrixC EDummy(6,6);

  SteerUtil myUtility(bmlPtrFull, myBPMs, EDummy, param.randomSeed);
  myUtility.setWake(withWakes);
  // Redfine the injection parameters.  From old fashioned comments 
  // from the MAD files. 
  // Valid only for the '07 Linac Benchmark !!! 
  
  Vector stateIn(6);
  for (size_t k=0; k != Particle::PSD; ++k) stateIn[k] = 0.;
  stateIn[1] =  -18.146E-6;
  stateIn[4] = 2.4347E-6;
  
  myUtility.setStateAtInjection(stateIn);
  
//  std::cerr << " Print kick angles, after myUtility " << endl;
//  printKickAngles (bmlPtrFull, 10); 
  
  myUtility.setInputTwissParam(aP);
  myUtility.setBunchDPoP(param.sigdPoP);
  myUtility.setBunchSigmaZ(param.sigTau);
  myUtility.setBunchRMDPoP(param.sigdPoP);
  myUtility.setBunchRMSigmaZ(param.sigTau);

// Testing Bunch allocation/deallocation... 
  
//  myUtility.testBunchMemUtil();
  
  
  // setting defacto a global..Protected in namespace.  
  
  Steering::theSteerUtil = &myUtility;

// Just do analysis 

//   Steering::StudyDispersionDMSAA2(&myUtility);
//   std::cerr << " After Analysis, stop " << std::endl;
//   exit(2);

  std::string gridUptStr1(" Starting, Utility Defined ");	     
  myUtility.gridUpdate(gridUptStr1, false); 

  
  // Prepare the Steering Plan. 
  
  Steering::DMSPlan aPlan;
  aPlan.firstCavity = 0;
  aPlan.numKlystrons = 1;
  aPlan.macroNumKlystrons = 3;
  aPlan.firstDipole = 9;
  aPlan.numDipoles = 5;
  aPlan.numExtraBPMs = 5;
  aPlan.gradientFraction = 0.80;
  aPlan.rescaleLattice = false;
  aPlan.byRFPhase = false;
  aPlan.minChangeDipole = 2.50e-7; //  sensible default value 
//  aPlan.minChangeDipole = 2.50e-6; // Not so sensible default value 
                                   // Need to se if we will converge. 
//  aPlan.minChangeDipole = 2.50e-9; // Not so sensible for non-convergence for robustness study. 
   
  aPlan.numMaxLocalIteration = 100;
  aPlan.numPseudoParticles = param.n;
//  if (withWakes) aPlan.numPseudoParticles = 2000;
  aPlan.macroNumMacroIter = param.macroNumMacroIter; 
  aPlan.macroFirstDipole = 3;
  aPlan.macroNumDipIncrement = 10;
  aPlan.macroNumDipoles = param.macroNumDipoles;
  aPlan.macroNumExtraBPMs = param.macroNumExtraBPMs;
//  aPlan.macroNumKlystrons = param.macroNumKlystrons; 
  aPlan.macroGradientFraction = param.macroGradientFraction;
  aPlan.scaleResponseMatrix = param.kScaleRM;
  aPlan.doMovie = false;
// Attempting to speed things up by pre-loading the response matrices 
// for the perfect lattice. 
// For now, if we don't move  the cavities, let us attempt to adjust angles of cryo-modules.  
  aPlan.useCryoTilt = false;
  aPlan.usePreSteer = param.withPreSteer;
  if (aPlan.usePreSteer) {
     aPlan.numDipolesPreSteering = 2;
     aPlan.numBPMsPreSteering = 3;
  }
  aPlan.doDynamicalPrelim = false;
  aPlan.minWeightSVD = param.weightSVD;
  aPlan.weight1to1DMS = param.weightDMS;
  aPlan.skipInitialPerfMeas = true; // to gain time initially. 
  aPlan.usePerfectResponse = true; // Good default 
//  aPlan.usePerfectResponse = false; // Just to test Random misalignment 
                                    // to go faster. 
  // 
  // DFS Merlin style 
  //
  { 
    aPlan.useDFSMerlin = true;
    aPlan.numCavitiesPerKlystrons = 24; // default for both Tesla and ILC 
    aPlan.weightAbsolute = param.weightAbs;
    aPlan.weightCorr = param.weightCorr;
    aPlan.weightDMS = param.weightDMS;
    aPlan.macroFirstDipole = 0;
    aPlan.macroNumDipoles = param.macroNumDipoles;
    aPlan.macroNumDipIncrement = param.macroNumDipoles/2;
//    aPlan.macroNumDipIncrement = 1;
// No !!! May this was the Merlin algorythm, but Jeff studies 
// change only the energy upstream of the section to be steered. 
//    aPlan.macroNumKlystrons = param.macroNumDipoles;
    aPlan.useReducedFieldInBunchCompressor = true;
    aPlan.usePerfectDispKnow = param.usePerfectDispKnow;
    if (aPlan.useCryoTilt) {
      aPlan.macroFirstDipole = 3;
      aPlan.numCavitiesPerKlystrons = 8; // No longer 24 cavities 
                                         // per kylstrons.. 
      aPlan.macroNumKlystrons = 2*3; // so we have 3 times as many klystrons. 
    }
    aPlan.doMovie = false;
    
//    aPlan.minChangeDipole = 2.50e-1; // One step behavior 
    aPlan.gainChangeDipole = param.gainFact;
    aPlan.numMaxLocalIteration = 25;
//    aPlan.numMaxLocalIteration = 12;
    if (aPlan.gainChangeDipole > 0.99999) aPlan.numMaxLocalIteration = 1;
  }
  
  std::string afNameRespMatrix(labelSteerG);
  afNameRespMatrix += std::string("_RespMatrices.txt");
//  std::string afNameRespMatrix("DM10D20NegWAbs2p5em5Rot_RespMatrices.txt");
//    std::string afNameRespMatrix("ML_DispStabil2Sb_RespMatrices.txt");
  // Let us create this matrices... 
  aPlan.printPlan();
  
  
  
  std::string fNameDispOut("./DispersionNominal_");
  fNameDispOut += param.labelRun; fNameDispOut += ".txt";
//  myUtility.fillReferenceDispersion(aPlan.macroGradientFraction,
//                                     aPlan.useReducedFieldInBunchCompressor,
//				     fNameDispOut);
  double refGFDispersion = aPlan.macroGradientFraction;
  std::cerr << " Reference Dispersion " << refGFDispersion << std::endl;
  myUtility.fillReferenceDispersion(refGFDispersion,
                                     true,
				     fNameDispOut, aP.DY, aP.DYp);

  std::cerr << " Print kick angles, after fillReferenceDispersion " << endl;
  // printKickAngles (bmlPtrFull, 10); 

  if (aPlan.usePerfectResponse) { 
// Use previous run, debugging....     
    std::cout << " Skipping Steering, no Response Perfect.... " << std::endl;
//   aPlan.fillResponsesPerfect(afNameRespMatrix, labelSteerG, false); //set it to false 
    
  }						
// Now we modify the line... 
//    myUtility.MoveOneQuadrupole(5, 1.5e-4);
//  myUtility.MoveOneQuadrupole(5, 5.0e-7);
 
 
  const double factDip = 1.000;
  
  std::ostringstream misAlignFNameStr;
  if (param.misAlignNum < 100) 
     misAlignFNameStr << "nick23p4_misxy_" << param.misAlignNum << ".txt";
  bool isMisAligned = true;
  if (param.misAlignNum == 314159) isMisAligned = false;
  if (isMisAligned) {
     if (param.misAlignNum == 9998) myUtility.setCavRotation144DMSDebug();
     else if (param.misAlignNum == 991606) {
       std::string directory("../../GridJobAnalysis/GLE1606/");
       std::string label1606("MLStN1_GFE_rr1175347648");
       myUtility.MoveAllElementsNew(directory, label1606);
//       std::string fNamDip(directory); 
//       fNamDip += std::string("StPf_final_g1.txt");
//       Steering::DFSRun aRun(fNamDip, true);
//       aRun.loadCorrectors(&myUtility);
     }
      else if (param.misAlignNum < 100) {
         std::string misAlignFName = misAlignFNameStr.str(); 
         myUtility.MoveAllElements(misAlignFName.c_str(), 
          true, // Moving the quads, obviously.... Studying Cav tilts if false..  
          true, // No BPM moves. 
          param.moveCavities); // Cavity are moving..
     } else  {
         LucretiaSigmas sigA;
	 // Cheating, first 3 perfectly aligned 
	 sigA.nCavMisStart = 0;
	 sigA.nQuadMisStart = 0;
//	 sigA.slopeY = 3e-7; 
//	 sigA.slopeX = 3e-7*1.e-8;  // No Steering in X ..
//	 sigA.bpmDispy = 20.0e-6; // Assume now the BBA has been done with 
	                      // this accuracy
	 LucretiaMisalignSet misA(&myUtility, sigA); 		       
//         myUtility.MoveOneQuadrupoleRel(99, 1.0e-4);
	 myUtility.MoveAllElements(misA);
	 std::ostringstream misAlignOutStr;
       misAlignOutStr << param.labelRun << "_rr" << param.randomSeed;
       const std::string labelMisAlign = misAlignOutStr.str();
       myUtility.printAllMisalignments(labelMisAlign);
       std::cout << " Loading Lucretia Corrector values .. " << endl;
       misA.setSolyakYCor(&myUtility, 1.0);
//       printKickAngles(bmlPtrFull, 10);

      }  
      double sigQuadStrength = 0.0000000005;
      
//      myUtility.SetAllQuadrupoleStrengthRandom(sigQuadStrength);
//      cout << " Setting Random quadrupole strength defect, sigma = " 
//           << sigQuadStrength << endl;
      
//     myUtility.SetOneDipole(5, 0.000015);
//    myUtility.MoveOneQuadrupole(5, 300.0e-6);

//    myUtility.SetAllDipoles("nick23p4_misxy_ycor_1.txt", factDip);
//    myUtility.SetAllDipoles("nick23p4_ycor_1_js.txt", factDip);
//      myUtility.SetAllDipoles("SteerCorQuadTable_D6Nd8MvCav_Ma5_Gf80_NEB0_0_5.txt", factDip);

  }  else {
  
//    std::string labelMat("test0");
//    Steering::ResponseMatrix<Steering::CORDIP> m0Cor(labelMat, bmlPtrFull, 
//                                     0, 20, 1, 20);
//    cout << " Response Matrix for perfectly aligned lattice " 
//         << m0Cor << endl; 			     
    cout << " Setting specific offset... Read code.... " << endl;				     
//    myUtility.MoveOneQuadrupole(11, 300.0e-6);
//    myBPMs->setAnOffset(11, -350e-6);
//    myBPMs->setAnOffset(12, 50e-6);
//    myBPMs->setAnOffset(13, 150e-6);
//    myUtility.markDipoleBroken(13);
//    myUtility.MoveOneQuadrupole(12, 250.0e-6);
    cerr << " Realistic beam line constructed. " << endl;
  }
  
// Do some prelim dynamical studies...
// Call for ground motion setting here..   
//
   aPlan.doDispersionBump = true; 
      // not yet upgraded to support multiPulse Ops
//   aPlan.doDynamicalPrelim = true ;
   aPlan.doDynamicalPrelim = false ;
   // Removed the section to enable ground motion. 
   // And set klystron jitter to a negligible value. 
   
     myUtility.setKlystronJitter(1.0e-10);
    
 
    
//  aPlan.macroFirstDipole = 6;
//  aPlan.macroNumKlystrons = 4;
//  int nnIter = Steering::SteerPureDMS(bmlPtrFull, labelSteerG, aPlan);

//    std::cout << " Skipping Steering, testing Grid.... " << std::endl;
    
   std::string gridUptStr2(" Ready to Steer ");	     
   myUtility.gridUpdate(gridUptStr2, true); 
   std::cerr << " Print kick angles, before Steering.. " << endl;
//   printKickAngles (bmlPtrFull, 10);
   
   // Temporary analysis ..skipped
    
   aPlan.skipInitialPerfMeas = false;
   int nnIter = Steering::SteerWeightedDMS(bmlPtrFull, labelSteerG, aPlan);
        
  std::cout << " Skipping final Check Quit here.... " << std::endl;
    
  exit(2);
     
//  cerr << " Ready to Steer One to One with extra BPMS " << endl;
//  Steering::SteerOneToOneExtraBPMs(bmlPtrFull, 4, 8, 12, 100);
    
//  cout << endl << " -----------------------------------"
//          << endl << " ---  Number of iteration for PureDMS " << nnIter << endl;
// Do the re-optimization without wakes first 

//  myTwissParam aPopt = Steering::OptimizeDyDypInit(aP, &myUtility);    


//  cerr << " Check after test-steering code " << endl;
    cout << " Check after test-steering code " << endl;
  
 // Now generate my Bunch 
 myPositronBunch bunch(bmlPtrFull->Energy());
 bunch.setNominalEnergy(myUtility.getTheBeamEnergyInput());
 myTwissParam aPNow = myUtility.getInputTwissParam();

 bunch.generate(aPNow, param.n, param.eps_1, 
                      param.eps_2, param.sigTau, param.sigdPoP);
   cerr << " Bunch generated with " << bunch.size() << " particles " << endl;
   Vector displacement(6);
   for (int k=0; k !=6; k++) displacement(k) = 0.;
// displacement(1) = 500.0e-6;
   bunch.shiftCenter(displacement);
  
// Final emittance measurement after steering..

   
  cout << " Emittance, Y " << bunch.getVerticalProjectedEmittance() << " X "
       << bunch.getHorizontalProjectedEmittance() << endl;
  
//  cout << "Can not yet propagate the bunch." << endl;
//  exit(2);

  // Propagate the bunch down the line
  // ---------------------------------
//  bmlPtrFull->propagate( bunch );

  cout << "Beginning to propagate the bunch." << endl;
  
  double s = 0.;
  std::string fNameProp("mlSt1_");
  // Tagging the job... 
    
  fNameProp += labelSteerG; fNameProp += "_Final";
//  if (!isMisAligned) fNameProp+= "Not";
//  fNameProp+= "Misalignedv4";  
//  if (withWakes) fNameProp += "_WWake";
  fNameProp += ".txt";
  ofstream ofst2(fNameProp.c_str());
  ofst2 << " BPM z y epsilY epsilNDY epsilYM epsilNDYM " << endl;
  iBPM = 0;
  bool keepGoing = true;
  std::ofstream* outOne = 0;
  std::string dumpTag ="";
/*  
  {  
    // debug one particle in the bunch..
    // could select here.. for now, random choice.
//    size_t iSel  = bunch.size()/3;
    size_t iSel  = 21;
    std::ostringstream oTag;
    oTag << "L_" << iSel;
    dumpTag = oTag.str();
    std::string fOutOne("DumpOne");
    fOutOne += dumpTag;
    fOutOne += "_";
    fOutOne += fNameProp;
    outOne = new std::ofstream(fOutOne.c_str());
    (*outOne) << " S x y z xp yp dPoP PRef" << std::endl;
    outOne->precision(8);
  }
*/
  if (param.withWakeCode > 0) {
       std::cout << " Turning Wake field On for final propagation " << std::endl;
       withWakes = true;
       gridUptStr1 = " Final check in mlSt1, with Wake  ";	     
    } else {
       gridUptStr1 = " Final check in mlSt1, without wake ";	     
    }
  myUtility.gridUpdate(gridUptStr1, true); 
  for(beamline::iterator dbi = bmlPtrFull->begin(); 
        dbi != bmlPtrFull->end(); ++dbi) {
      if (!keepGoing) break;
      ElmPtr q = *dbi;
      
      q->propagate((bunch));
      s += q->Length();
//      cerr << " At s = " << s << " type " << q->Type() << 
//            " y " <<  bunch.getYPosition() << " AverP " << 
//	    bunch.getAverageRealMomentum() << endl;
      if (std::abs(bunch.getYPosition()) > 0.01) {
         cerr << " At s = " << s << " type " << q->Type() << " Beam is at one mm. off.."  << endl;
         break; // To avoid wasting time on screwed
      }                                                   // beam
//      cerr << " Type " << q->Type() << " at s = " << s << endl;
      if (outOne != 0) bunch.dumpOneParticleMerlinParam(*outOne, s, q, dumpTag);
//        cerr << " At s = " << s << " type " << q->Type() << endl;
//         int sInCM = (int)  (s*100.);
//         if (s > 150.800) {
//	     std::ostringstream fNameBPMPtr;
//	     fNameBPMPtr << "./BeamData/Debug_" << sInCM << ".txt";
//             bunch.dumpItForR(fNameBPMPtr.str().c_str());
//         }
     if( 0 == strcmp( "monitor", q->Type() ) ) { 
          ofst2 << " " << iBPM << " " << s << " " << 
	     1.0e6*bunch.getYPosition() << " " << 
             1.0e9*bunch.getVerticalProjectedEmittance() << " " <<
             1.0e9*bunch.getVerticalNoDispEmittance() << " " <<
             1.0e9*bunch.getVerticalProjectedEmittance(true) << " " <<
             1.0e9*bunch.getVerticalNoDispEmittance(true) << 
	     std::endl;
	     std::ostringstream fNameBPMPtr;
	     fNameBPMPtr << "./BeamData/bd_" << labelSteerG;
	     if (!isMisAligned) fNameBPMPtr <<"Not";
	     fNameBPMPtr << "MisAligned_AtBPM_" << iBPM;
	     if (withWakes) fNameBPMPtr << "_WWake";
	     fNameBPMPtr << ".txt";
             bunch.dumpItForR(fNameBPMPtr.str().c_str());
         iBPM++;
      }
      if (withWakes && (( 0 == strcmp( "LinacCavity", q->Type() ) ))) {
//        if (s > 5. && s < 6.) {
//           cout << " Dumping Wake fields at the last cavity, s = " << s << endl;
//           wkProc->SetDebugOn(1); 
//	 }
//         cerr << " withWakes: Number of particles in the bunch " << bunch.size() << endl;
// Now done in rf cavity.. 
//         wkProc->DoItAll(bunch, q);
	  
       } // Wake field 
       // Brutal end...
       if (s > 200000.) {
        // bail out, testing 
	   cerr << " Bail out !  This takes too long... " << endl;
	   keepGoing = false;
       } 
  } // propagation
  ofst2.close();
  if (outOne != 0) outOne->close();
  cerr << " Finished Propagation... " << endl;  
//  cerr << " Exit for now... " << endl;  
//  exit(2);

  {

//  bunch.dumpItForR((const char*) "demo_3_AfterMisalignedv2.txt");
  
//    cerr << " Exit for now.. " << endl;
//    exit(2);

    cerr << " Finished dumping Beam After.. " << endl;
    std::string gridUptStr2 = " Out Now, done with mlSt1 Application   ";	     
    myUtility.gridUpdate(gridUptStr2, true); 
  }
    
 } // Second problem, second factory... 
// outNow : cerr << " Out now... " << endl;

  return 0;
}
void printKickAngles(const BmlPtr& bmlPtr, int nMax) {

  int iDip = 0;
  for(beamline::iterator dbi = bmlPtr->begin(); 
        dbi != bmlPtr->end(); ++dbi) {
	ElmPtr q = *dbi;
     if(VKickPtr dipPtr = boost::dynamic_pointer_cast<vkick> (q)) {
       cerr << " Dipole corrector " << iDip << " set to " << 
         dipPtr->Strength() << endl;
	 iDip++;
     } 
     if (iDip == nMax) break;
  }
}
