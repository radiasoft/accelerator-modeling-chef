// class SteerUtil
#ifndef _h_SteerOneToOne
#define _h_SteerOneToOne 1
#include <vector>
#include <time.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/Distribution.h>
#include <beamline/ParticleBunch.h>
#include <SteeringResponseMatrix.h>
// No need to define class, at least not yet.. 

namespace Steering {

// as part of file SteerOneToOne
//! \brief Steering, 1 to 1
/*! BPM offset are assumed to be zero. Old implementation, Not to be trusted.
   for now.. 
*/
bool SteerOneToOne(BmlPtr beamLine, size_t firstDip, 
                   size_t numDips, size_t numIter=3);
bool SteerOneToOneExtraBPMs(BmlPtr beamLine, size_t firstDip, 
                   size_t numDips, size_t numBPMs, size_t numIter=3);
bool SteerOneToOneWQuad(BmlPtr beamLine, size_t firstQuad, 
                   size_t numQuads, size_t numIter=3);
bool SteerToOrbit(const ResponseMatrix<STEERTOORBIT> *mPreSteer);

// as part of file SteerDispersion

// We might want to create a nicer class, but this will do for a start.. 
//! \brief Dispersion Match Steering Run Plan 
/*! A place to upload parameters for the steering process. 
   Data members are all public.  For default value, see the constructor of this 
   class.A container holding the 
   pre-computed response matrices has been added, to gain CPU performance, 
   an a method to instantiate these response matrices.   
*/
class DMSPlan {
  public:
   DMSPlan();
   /*! 
      The first dipole which will be modified, in a given DFS section
      Typically set in Top level Steering functions, such SteerWeightDispersion
   */
   size_t firstDipole;  
   /*! 
    The number of dipoles per DFS section.i.e. the length of
                       of the section to DFS Steer the beam through.   
   */
   size_t numDipoles; 
   /*! 
      A bit tricky, possibly harmful. 
      We seem to get the wrong solution for  the last few dipoles.  Definitly the last one.
      Perhaps related to the number of extra BPM  needed. So we don't apply the correction 
      for these. Related to the number of dipole increments per sections! 
      November 2006: Obsolete parameter, the current SVD algorithm  now performs correctly!	
   */
   size_t numDipoleTailRej; 
   /*!
     Number of cavities powered by a given klystron. 
     This is a constraint: the Energy policy for changing the gradient operates on klystrons,
      not individual cavities. 
   */
   size_t numCavitiesPerKlystrons;  
   /*!
     There are no a-priori reasons to neglect the BPMs located downstream of the 
     end of the current DFS section. by default, it is 0.  
   */
   size_t numExtraBPMs; 
   
   /*! 
    The first cavity for which the gradient will be reduced when running the Linac 
                      off-momentum, for the section where steering occurs (steered section) 
   */
   size_t firstCavity; 
   /*!
    One might decide to do it all over again, so we keep track of the macro-iteration
    (a single macro iteration runs over the entire Linac.  A local iteration occurs in a given 
       steered section).  
   */
    size_t currentMacroIteration;
   /*!
      The number of klystrons running at reduced gradient when off momentum, typically 
      upstream of the steered section (could include the steered section as well, see
      param useReducedFieldInSteeredSection 
   */
   size_t numKlystrons; 
   /*!
     Fraction of the reduced voltage over nominal voltage for the klystron running 
       off-momentum.   
   */
   double gradientFraction;   
   /*! 
     Number of particles being tracked in the pilot bunches, while steering.  
   */
   int numPseudoParticles;  
   /*! 
    Number of particles being tracked while computing  response matrix.  Obsolete, just use 
    one particle at the center of phase space ( nominal particle).  
   */
   int numPseudoPartRP;   
   /*! 
       True is going through the last steered section of the LINAC
   */
   bool doLastSegment; 
   /*! 
      If true, prior to Dispersion Matching Steering, optimize the trajectory in such a way that the 
      y and y' off momentum and on momentum are equal entering the steered section.  This optimization 
      is done using the 3 dipoles upstream of the steered section.  J. Smith (Cornell) thinks this is 
      a good idea, I have not been able to see any real advantage of doing this.. Obsolete. 
   */
   bool preSteer;   
   /*! 
     Rescale the quadrupole strength to match the optical values given in the lattice 
         file when running off-momentum.  Bad idea, but I haven't check after the SVD 
         upgrade. Most likely obsolete.. 
   */
   bool rescaleLattice;  
   /*! 
     Instead of reducing the voltage, change the synchronous phase to reduce the 
                   net acceleration traversing the cavity.. Never tested 
   */
   bool byRFPhase;   
   /*!
      In a given iteration of a steered section, correction to the dipole setting are compared to the previous
      iteration.  The maximum change among those is considered and compared to this parameter. 
      if found smaller, the iteration is over, convergence is declared established. 
   */
   double minChangeDipole;   
   /*!
      The gain factor while iterating in a given steered section. For each each actuators 
      (i.e., corrector settings),  the set value is equal to  gainChangeDipole * SVDSuggested value. 
   */
   double gainChangeDipole;   
   /*!
      Parameter to control the SVD null space suppression.   
   */
   double minWeightSVD;   
   /*! 
      The Dispersion matched algorithm is in fact a bit more than just minimizing the spurious dispersion. 
       One can always simply attempt to minimize the kick angle across the steered section, or the 
       measured beam position at each Quadrupole package. What is actualy minimized is a weighted 
       of these contribution. A first attempt at this was done via this parameter, which is now 
       obsolete.  See the next three parameters   
   */
   double weight1to1DMS;  
   /*! 
    Refers to the weight associated to the contribution of the absolute BPM reading. 
    Could be set to the inverse of the squared of the expected BPM offsets or displacement   
   */
   double weightAbsolute;
   /*! 
    The weight associated to the contribution of the spurious dispersion. (defined as 
    the difference between the nominal dispersion and the measured one.   
   */
   double weightDMS;
   /*! 
    The weight associated to the absolute value of the corrector setting ( "kick minimazation") 
    Not very well studied in this package, found unnecessary..    
   */
   double weightCorr;
   /*! 
     Now, a few parameters controlling the steering of the entire Linac. 
     Here, the maximum number of iterations within a Steered section.  If convergence 
     does not occurs, after this numMaxLocalIteration, we stop and move to the next section.     
   */
   			 
   size_t numMaxLocalIteration;  
   /*!
     We can do more than pass through the entire Linac.  when 
     convergence was not established in a few steered sections, we do it again, and it sometimes help.  
   */
   size_t macroNumMacroIter; 
   /*!
      The first dipole which will be modified in the LINAC.
   */    
   size_t macroFirstDipole; 
   /*!
       The number of dipoles per steered section. 
   */
   size_t macroNumDipoles; 
   /*!
      Steered Sections typically overlap. Here is the number of dipole by which the steered section first dipole 
      will be incremented, moving from one steered section to the next. 
   */
   size_t macroNumDipIncrement; 
   /*!
     In principle, BPM reding downstream of the steered section could be used.  This additional set of responders was
     found not all that useful, but the beneift of this have not  been well documented in the latest 
     lattice version or after optimizing the above parameters. 
   */
   size_t macroNumExtraBPMs;  
   /*!
     Number of kylstrons being changed while running off-momentum, for the first steered section. 
   */
   size_t macroNumKlystrons;  
   /*!
     Same as gradient fraction, for the entire Linac. Unless this parameter changes from steered section to 
     steered section, no useful.. 
   */
   double macroGradientFraction;  // Fraction of the nominal gradient  
   /*!
     A rescaling parameters for the response matrix.. Used to debug SVD. No use, really.. 
   */
   double scaleResponseMatrix;     
   /*!
     If true, the response matrices will be pre-computed, prior to lattice misalignment. 
     Such matrices can not be measured for a real machine. They have to be computed from 
     the model. Yet, this option was found to be better then computing the response matric from 
     the real machine.  
   */
   bool usePerfectResponse;   
   /*!
     The Low Emittance Transport performance can be studied prior to Steering. Often, this is not 
     possible, the beam can not propagate through the misaligned lattice. 
   */
   bool skipInitialPerfMeas;  
   /*!
     Number of dipole used in the pre-sterring only relevant if usePreSteer is true. Obsolete, probably. 
   */
   int numDipolesPreSteering; 
   /*!
     Number of BPMs used in the pre-sterring 
                              // only relevant if usePreSteer is true. 
   */
   int numBPMsPreSteering; 
   /*!
     For each DFS section, prior to DFS steer, attempts to correct the cavity tilts 
                      by tilting the cryo-modules. Currently not supported in hardware. Yet, found to 
		      be useful in simulation. 
   */
   bool useCryoTilt;   
   /*!
     Same as preSteer, but for the entire Linac. 
   */
   bool usePreSteer;  // Runs the 
   /*!
      Preliminary version of the weight Diserpsion Free Steering has been written, and later adapted 
      to follow what Merlin does. ( or close to it..) This is now the recommended and default 
      algorithm. 
    
   */
   bool useDFSMerlin;  
   /*!
     Obsolete. Don't touch.  
   */
   bool useReducedFieldInSteeredSection;
   /*!
     Important.  Assume that the Bunch Compressor used upstream of the Linac can be use to generate the 
     off momentum beam. 
   */
   bool useReducedFieldInBunchCompressor; //
   /*!
     Dipspersion is computed using a very small difference in off vs nominal mometum. This is the designed 
     (MAD-like) dispersion, which, in practice can not bem easured. Yet, this is the recommended option. 
   */
   bool usePerfectDispKnow;  
   /*! 
       Every two macro iterations, the dispersion and idpsersion prime is retuned. Found to be very effective, 
       but this is a non-local optimization....  
   */
   bool doDispersionBump; 
    /*!
     The following set of parameters are for dynamical studies only.  A dedicated class should 
     designed for this purpose.. Yet, preliminary dynamical studies were done. 
   */
  bool doDynamicalPrelim; 
   /*!
     Maximum number of pulses dedicated to this Steering study.  Avoid globalized infinite loop! 
   */
   int maxNumPulseTotal;
   /*!
     Same, but in simulated real time ( the machine is not always pulsing) 
   */
   int maxNumSecondsTotal; 
   /*!
     Wait period between steered section, for performance analysis or whatever.. 
   */
   int numPulseBetweenLocalIteration ; 
   /*!
     Between complete Linac iteration. 
   */
   int numPulseBetweenGlobalIteration ; // Wait, simulated tuning process 
   /*!
      Keep a snap shots of each beam profile, make vertical phase space plots.
      Used to make a computer simulation of this steering process. 
      Shown at Super-Computing 07.  
   */
   bool doMovie; 
   /*!
     Container for the relevant Response Matrices.
   */   

   std::vector<Matrix> responsesPerfect; 
   /*!
     Mostly obsolete, merged with the above. 
   */
   std::vector<Matrix> respPerfectOneToOne; 
    /*!
      Method to instantiate all relevant responce matrices for the considered Linac,
       following the above DMS plan. 
   */
   void fillResponsesPerfect(std::string& fName, std::string& label, 
                                bool fromFile=true);
   /*!
      Print all these parameters, rambling format. 
   */
   void printPlan() const;
   		     		      
};
//! \brief Optimization by tilting the cryo-modules.  
/*! 
    ... when the responders are the BPM and the actuators are the movers 
     on cryo-modules.  The idea is to correct the spurious dispersion 
     by slightly changing the direction of the accelrating field.
   No detailed documentation, not used at all in the ILC Linac test. 
   Some documention in the source code.   
*/
class CryoTiltPlan {
  public:
   CryoTiltPlan(DMSPlan& aBiggerPlan);
  
   DMSPlan *theDMSPlan; // Not own !!!! Pointer to a struct... 
   size_t firstCavityPitched; // The first cavity which will be tilt-corrected.
   size_t numKlystronsPitched; // The number of dipole ( or with above, the length of
                      // of the section to DFS Steer
		      // Ignored in the Novermber 17 2006 version.!!! 
		      // Always set to 1/3, i.e., pitching only one cryo module 
		      // at a time.. 
   size_t firstCavity;
   size_t numKlystrons; 
   size_t firstBPM;
   size_t numBPMs;
   size_t numCavitiesPerCryo; 
   float minimumTilt; // don't bother tweaking if reach this correction size. 
   float numBPMsPerKlystrons;
   double gradientFraction;  // Fraction of the nominal gradient  
   bool usePerfectResponse;
   // Store also the stash of response matrices for a perfect lattice.
   // Potentially a large amount of memory, so we do not all the acillary stuff 
   // that goes along a SteeringResponseMatrix
   std::vector<Matrix> responsesPerfect; 
   // Method to fill that one. 
   void fillResponsesPerfect();
   void printPlan();
   		     		      
}; // CryoTiltPlan

//! \brief A container for the data from one local iteration. 
/*!
    ... The state vector, the suggested changes to the actuator vector,
      and related variables. 
      With methods advicing us on what to do next.
      
      We don't change the plan, just a utlitiy class to faciliate
      the handling of the data for one iteration. 
      All data is  public, not a big class, meant to be volatile.
      Destroyed and recreated at each iteration.    
*/
class localIterData {
  public:
/*! 
   Constructor 
   \param aPlan  .. The steering plan. 
   \param iterationNum .. The sequential iteration number 
*/
  localIterData(const DMSPlan& aPlan, int iterationNum=0 );
/*!
   The sequential iteration number  
*/
  int iterNum;
/*!
   Current dipoles values... 
*/
  std::vector<double> dipValues;
/*!
   Suggested changed, result of the SVD solver, 
   Computed in SteerWeightedDMSLocalMerlin  
*/
  std::vector<double> suggestedChanges;
/*!
   At the dipole corrector working o.k. for us? 
*/
  std::vector<bool> isOK; // changes seem converging.. 
/*!
   Actual change. If all O.K., iteration count O.K., and so forth, 
   than set to suggested changes.  
*/
  std::vector<double> actualChanges;
/*!
   Declaring.. Could have been part ofthe constructor.   
*/
  inline void fillSuggested(const Vector &dVals) {
    for (size_t i=0; i != suggestedChanges.size(); i++) 
     suggestedChanges[i] = dVals(i);
  }
/*!
   Test for convergence, look at suggested changes.. 
   See code for detailed algorithm!  
*/
  bool areWeDone(const DMSPlan& aPlan, bool wPrint=true) const;
/*!
   Study this iteration with respect to the previous one. 
   If we have run-away correction, print an obscur and yet perhaps 
   effective message.   
*/
  bool appraise(const DMSPlan& aPlan, 
                const std::vector<localIterData> &itPrevious);
/*!
   Tweak the correctors.  
   Set the dipole corrector power supplies to a different value.   
*/
  void applyChanges(const DMSPlan& aPlan) ;
};

//! \brief Pure Dispersion Matched Steering
/*! 
   Obsolete.  Merged with SteerWeightedDMSLocalMerlin
*/ 

int SteerPureDMSLocal(BmlPtr beamLine, 
                  const DMSPlan &aPlan); 

//! \brief Dispersion Matched Steering. 
/*! 
   Obsolete.  Merged with SteerWeightedDMSLocalMerlin
*/ 
int SteerWeightedDMSLocal(BmlPtr beamLine, 
                  const DMSPlan &aPlan); 
		  
//! \brief Dispersion Matched Steering. 
/*! 
    The steering algorithm for one sttered section. 
    Optionally re-compute the response matrices, and iterate with 
    a nominal energy beam, off momentum beam, perform an SVD solve 
    for each iteration, study the suggested change, apply them 
    if o.k. 
*/
int SteerWeightedDMSLocalMerlin(BmlPtr beamLine, 
                  const DMSPlan &aPlan); 
//! \brief Dispersion Matched Steering, Debug  
/*! 
    Most likely Obsolete. Toy version to try new 
    ideas.  
*/
int SteerWeightedDMSLocalMDebug(BmlPtr beamLine, std::string label, 
                  int itMacro,  
                  const DMSPlan &aPlan); 

#define STEERREALFAILURE 999999

//! \brief Driver for the Pure DMS  
/*! 
    Most likely obsolete.   
*/
int SteerPureDMS(BmlPtr beamLine, std::string &label,  DMSPlan &aPlan);

//! \brief Driver for the DMS over the entire lattice.  
/*! 
    Expected to be called from main program. 
    Execute the plan!. Loops over steered section, and 
    and study the Low Emittance Transport (LET) performance at the end. 
    
    \param  beamLine  The Chef beamline where the beam will be steered. 
    \param label Mnemonic label to set file names. 
    \param aPlan The Dispersion Matched Steering run plan.   
*/
int SteerWeightedDMS(BmlPtr beamLine, std::string &label,  DMSPlan &aPlan);

//! Utlity to update the DMSPlan at the end of channel 
/*! 
    Ugly design.. Not a good idea to change that struct... 
    Oh well. 
    \param aPlan The Dispersion Matched Steering run plan.   
*/

bool IsDipoleLeftAndUpatePlanAtEnd(DMSPlan* thePlan); // The plan will change ! 

//! Study the LET performance after being steering.
/*!

   Produce diagnostic file(s) At least on flat ascii file showing 
   beam position ( absolute, with respect to nominal beam line) 
   and emittances. Could create more than one file, for instance,
   with and without Wake fields.  
  
 \param beamLine  The Chef beamline where the beam will be steered. 
 \param dirPath  Diagnostic File(s)  will appear in that directory. 
 \param label  a mnemonic label, to name files. 
 \param labelSelectedParticle  if no-zero length, we will generate 6D trajectory data for
         a specified particle. 
 \param maxBPM number. Propagation stops at that BPM. 
*/
void StudyPerformance(BmlPtr beamLine, std::string& dirPath,
                        std::string& label,
                        std::string& labelSelectedParticle,
			int maxBPM = 99999999);
			
//! Study the LET performance after being steering.
/*!

  
 \param labelRun  A mnemonic label, to name files. 
 \param minBPM number. Propagation starts here... Not tested if not equal to 0! 
 \param maxBPM number. Propagation stops at that BPM. 
*/
void StudyPerformanceDebug(std::string& labelRun,
                        int minBPM = 0, 
			int maxBPM = 99999999);

//! Debugging... after being steering.
/*!
     Obsolete.	
*/		
void DumpTableCorrector(BmlPtr beamLine, std::string& labelRun);

//! Debugging... after being steering.
/*!
     	
 \param maxBPM number. Propagation stops at that BPM. 
*/		
void PrintEnergyAtEachBPM(int maxBPM=999999);

//! DIspersion matching by tilting Cryo-Modules. 
/*!

   Througout a given Steered Section.
 \param beamLine  The Chef beamline where the beam will be steered. 
 \param aPlan The corresponding Cryo-tilt run plan.    
 \param iStepCryoMod Advanced use, see code.. 
*/   
int CorrectCryoTiltLocal(BmlPtr beamLine, 
                            const CryoTiltPlan& myCryoPlan, int iStepCryoMod = 0);

//! Driver for Steering by tweaking Cryo-module movers. 
/*!
 \param beamLine  The Chef beamline where the beam will be steered. 
 \param aPlan The Dispersion Matched Steering run plan.   
 \param dirPath. A place to dump diagnostic files.  
*/		
int CorrectCryoTilt(BmlPtr beamLine, const DMSPlan aPlan,
                    const char *dirPath);

//! Driver for Pre-Steering action.  
/*!
 \param beamLine  The Chef beamline where the beam will be steered. 
 \param aPlan The Dispersion Matched Steering run plan.   
 \param aRespMatrix The corresponding steering matrix. 
       (could have been stashed in the Plan. This function 
        was written prio to DMSPlan. )
*/		

template<actuator A>
int PreSteerToOrbit(BmlPtr beamLine, const DMSPlan& aPlan, 
			 ResponseMatrix<A> *aRespMatrix);
			 
//! Test a given configuration, or state, with beam.  
/*!
   Used by the Steering algorythm function. Generate and send beam, 
   collect  data. 
 \param beamLine  The Chef beamline where the beam will be steered. 
 \param aPlan The Dispersion Matched Steering run plan.   
 \param aRespMatrix The corresponding steering matrix. 
       (could have been stashed in the Plan. This function 
        was written prio to DMSPlan. )
*/		
bool TestWithBeam(const DMSPlan* aPlan, double zMax, 
                  int bpmStart = 0, bool printDebug = false);
}; // name space steering
#endif
