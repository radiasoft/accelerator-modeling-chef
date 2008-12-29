//! Utility Class for Steering
/*!
  A set of containers to keep track of the beam line elements 
  and manipulate them.  For instance, tweak the current in dipole correcotr
  elements, indexed by Beam Posiition Monitors (BPM). 
  
  Paul Lebrun, 2006-2007, finalized Feb 2008. 
*/
#ifndef _h_SteerUtil
#define _h_SteerUtil 1

#include <vector>
#include <iostream>
#include <fstream>

#include <basic_toolkit/MathConstants.h>
#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/Distribution.h>
#include <beamline/beamline.h>
#include <beamline/kick.h>
#include <beamline/quadrupole.h>
#include <beamline/thinpoles.h>
#include <beamline/LinacCavity.h>
#include <beamline/rfcavity.h>
#include <basic_toolkit/Matrix.h>
#include <MLBPMs.h>
#include <myPositronBunch.h>
#include <time.h>

using std::vector;
using std::ostringstream;

//! \brief Forward Declaration for Misalignament constant   
class ILCMisalignmentSet;

//! \brief A set of utilities For Steering
/*! The goal is to conveniently move relevant beam line elements, 
    tweak corrector element, send bunch and collect BPM data, etc... 
*/ 
  
class SteerUtil  {

public :
//! \brief Explicit Constructor, from a CHEF beamline model Pointer  
/*! 
  \param model is a CHEF/Boost Shared pointer such that the model can be accessed. 
  \param bpms  is a pointer to a container of pointer to the BPMS 
  \param seed Optional. A seed number for setting Monte-Carlo.  Unused, so far, since 
              the CHEF method to control the set of sequences is uncertain 
	      (to my knowledge) 
  \param withPlots Optional.  Currently unused.  Aflag to make standard root plots.
                    Since the root subpackage has been removed, does nothing. Leave it 
		    for possible future use.   	  
*/  
   explicit SteerUtil(BmlPtr model, 
                                 MLBPMs* bpms,
				 long seed = 0,
				 bool withPlots=false );
  // Note: no const in this argument list. We will change the settings 
  // on the BPMs, as part of the steering algorithm.
   					 
   ~SteerUtil();
   
  // Futur development: these moving routines should probably be moved to 
  // an other dedicated class...
  //! \brief Re-adjust the Strength of a dipole corrector, with a signed increment. 
  /*!
   \param corrIndex  Sequential Corrector index into the line .  Start with 0. 
   \param increment  Incremental value ( in T.m.) 
  */ 
   bool AdjustOneDipole(unsigned int corrIndex, double increment); 
  //! \brief Set the Strength of a dipole corrector, with a signed incerement. 
  /*!
   \param corrIndex  Sequential Corrector index into the line .  Start with 0. 
   \param value  The new value ( in T.m.) 
  */ 
   bool SetOneDipole(unsigned int corrIndex, double value); 
  //! \brief Set the Strength of all dipole corrector from an ASCII file
    
  /*!
    File format is tabular, a row refers to one corrector.
    Lines starting with ! are ignored. 
    First column is ignored, can be used to index the list 
    Values are in the second column are the dipole corrector values, 
  
   \param fileName  Unix path name to the file  
   \param fact  Rescale factor.  
  */ 

   bool SetAllDipoles(const char *fileName, double fact = 1.); 
  //! \brief Get the Strength of a given dipole 
  /*!
   \param corrIndex  Sequential Corrector index into the line .  Start with 0. 
  */ 
   double GetOneDipoleSetting(unsigned int corrIndex) const; // Return the value of the corrector.
  //! \brief Move a quadrupole along the vertical axis by a relative delta Y   
  /*!
   \param corrIndex  Sequential Quadrupole index into the line .  Start with 0. 
   \param deltaY  Change in the yOffset of the CHEF alignment Data structure. Units are meters.   
  */ 
   bool MoveOneQuadrupole(unsigned int corrIndex, double deltaY); 
  //! \brief Move a quadrupole along the vertical axis by a relative delta Y   
  /*!
   \param corrIndex  Sequential Quadrupole index into the line.  Start with 0. 
   \param deltaY  Same as previous method, but using the Chef moveRelY method 
                   from the beamline element class.     
  */ 
   bool MoveOneQuadrupoleRel(unsigned int corrIndex, double deltaY); 
  //! \brief Move a cavity along the vertical axis by a relative delta Y.   
  /*!
   \param corrIndex  Sequential Cavity index into the line .  Start with 0. 
   \param deltaY  Relative vertical offset change, with respect to previous position.  
  */ 
   bool MoveOneCavity(unsigned int corrIndex, double deltaY); 
  //! \brief Change the vertical position offset of BPM.   
  /*! 
  
   \param corrIndex  Sequential Radio Frequency Cavity index into the line .  Start with 0. 
   \param newY      New vertical offset.  Units in microns. ( I think.. Try both ways, see what
   happens. )   
  */ 
   bool MoveOneBPM(unsigned int corrIndex, double newY); 
  //! \brief Rotate a cavity.  
  /*!  The axis of rotation is perpendicular to the Y-S plane ( vertical plane) and is 
      at the geometrical center of the cavity. That is, we change the pitch of the cavity. 
      Units are in radians.    

   \param corrIndex  Sequential Cavity index into the line .  Start with 0. 
   \param  deltaYAngle Relative change in the pitch angle, in radians      
  */ 
   bool RotateOneCavity(unsigned int corrIndex, double deltaYAngle); 
   
  //! \brief Change the vertical position offset of BPM.   
  /*! 
  
   \param aCav  Shared Pointer to a Linac Cavity in the model.  
   \param deltaY  New vertical incremental offset.  Units are in meters. 
  */ 
   bool MoveOneCavity(LinacCavityPtr aCav, double deltaY); 
  //! \brief Rotate a cavity.  
  /*!  The axis of rotation is perpendicular to the Y-S plane ( vertical plane) and is 
      at the geometrical center of the cavity. That is, we change the pitch of the cavity. 
      Units are in radians.    

   \param aCav  Shared Pointer to a Linac Cavity in the model.  
   \param  deltaYAngle Relative change in the pitch angle, in radians      
  */ 
   bool RotateOneCavity(LinacCavityPtr aCav, double deltaYAngle); 
  //! \brief Get the a Cavity Pitch.  
  /*!     

   \param aCav  Shared Pointer to a Linac Cavity in the model.  
  */ 
   double getCavityPitch (LinacCavityPtr aCav) const ;
  //! \brief Get the average Cavity Pitch over a girder or cryo-module.  
  /*!     

   \param firstCav  Shared Pointer to a Linac Cavity in the model.  
   \param numCav  The number of cavities downstream of the first cavity, including that first
   cavity. 
  */ 
   double getAverageCavityPitch (LinacCavityPtr firstCav, size_t numCav) const ;
  //! \brief Move all the quadrupoles assuming a simple Gaussian model.  
  /*! They are moved with respect ot the nominal CHEF reference frame, that is, with respect to
  the perfect beam line model. Which is typically undetermined, in real life. 
  A more realistic misalignment model is implemented via the virtual class ILCMisalignmentSet  

   \param deltaYSigma  The root mean square of the distribution of deltaY
  */ 
   bool MoveAllQuadrupolesNormal(double deltaYSigma);
  //! \brief Set all quadrupole on the nominal beam lines.   

   bool SetAllQuadrupolesYNominal();
  //! \brief Randomize the quadrupole strength assuming a simple Gaussian model.  
  /*! The change is relative, i.e new value = oldvalue * (1 + delta), where delta is 
      Gaussian distributed  

   \param sigma  The root mean square of the distribution of relative change
  */ 
   bool SetAllQuadrupoleStrengthRandom(double sigma);
   //! \brief Setting all quads to value disctated by an optical model.  
   /*!  Obsolete. Was written to study agreement on how the lattice function are set a-la LIAR 
        or Merlin, as part of BenchMark 2,  when we were extremly confused about 
       the use of a Proton instead of a positron to registering the beamline..! 
       No longer expeted to be relevant. 
       
      \param kStrength An Stl vector with the quadrupole strength.  
   */
   void SetAllQuadStrengthByFixedOpticalK(std::vector<double>& kStrength);
   //! \brief Get a Quadrupole offset from the nominal Chef Beamline.  
   /*!
   \param corrIndex  Sequential Quadrupole index into the line .  Start with 0. 
   */
   double GetOneQuadrupoleYTrans(unsigned int corrIndex) const; 
   //! \brief Set the voltage on a given r.f. cavity.    
   /*!
   \param icav  Sequential Cavity index into the line .  Start with 0. 
   \param value  Accelerator gradient, in MeV/m.  
   */
   bool setVoltageOnCavity(unsigned int icav, double value);
   //! \brief Misalign the lattice, vintage code from 2005-2006 Studies. 
   /*!  The file name is a flat Ascci file, 
        listing the misalignment figure (x, y, tilts.. ) for all elements. 
	Mostly obsolete, See code for details on file format.  
     \param fileName Unix path name to the file.  
     \param moveQuad Leave the quads in place if set to false. 
     \param moveBPM  Leave the BPMs in place if set to false.
     \param moveCav  Leave the r.f. cavities (or Linac Cavities)  in place if set to false.
     \param firstBPMMoved  Leave the beamline element unchanged if they are located before the 
                           BPM refered by this sequential index. 
   */
   bool MoveAllElements(const char *fileName, bool moveQuad=true, 
                   bool moveBPM = true, bool moveCav = true,
		   unsigned int firstBPMMoved = 0);
   //! \brief Misalign the lattice, 2007 Studies. 
   /*!  Actual move of all elements following the directive in the ILCMisalignment Set.
   
    \param misAligned  The misalignement set
   */
   bool MoveAllElements(const ILCMisalignmentSet& misAligned);
   //! \brief Misalign the lattice, 2006 Studies. 
   /*!  Actual move of all elements following the directive in an (obsolete) set of misalignement
   file. Not recommened for use. 
   
    \param dir  The directory where the file reside. 
    \param label A mnemonic label used to build the relevant list of files.   
   */
   bool MoveAllElementsNew(const std::string &dir, const std::string &label);
   //! \brief Print the misalignment, retrieved fro mthe Chef Model.  
   /*!  Used in the context of the 2006 studies.  Might not be valid.  
   
    \param label A mnemonic label used to build the relevant list of files.   
   */
   void printAllMisalignments(const std::string &label) const;
   //! \brief Misalign the lattice, 2006 Studies. 
   /*!  Actual move of all elements following the directive in an (obsolete) algorithm.
        Probably not worth reading this code, ever.. 
   
    \param fileName  A Unix file name 
   */
   bool MoveAllElementsILCMLAlgo(const char *fileName);
   //! \brief Setting the Dipole corrector value from the Quadrupole displacement.  
   /*!  Big Cheat: Imagine we have access to the nominal beam line coordinate system, 
        and the quadrupole offset.  Since the dipole are located very close the quadrupole, 
	the effect of a known quadrupole displacement can be almost exactly compensated.   
   
   */
   void SetAllDipolesFromQuadYTrans();
   
   //! \brief Beam Condition matching. First part: setting the nominal trajectory.  
   /*! Injection can not always be done on center.  For instance, the dispersion might
       not set to zero at s = 0..  
    \param stateIn : A MXYZPTLK Vector. 
   */ 
   void setStateAtInjection(const Vector &stateIn);
   
   //! \brief Convience method, getting back a set pointer to BPMs, into a BPM container   
   /*! Simply copying Shared Pointer from this class to a user array.. 
   
    \param bpms  The container. It will be empties first, than filled.
    \param first  Sequential index in the lattice. 
    \param num   The number of BPMs after first, including this first. The size of bpms will be
    num. 
   */ 
   size_t getBPMPtrs(MLBPMs& bpms, size_t first, size_t num) const;
   
   //! \brief Steering utlity per-se: Setting the Wake field on or off for real bunches. 
   /*! 
      \param isOn Obvious.. 
   */
    inline void setWake(bool isOn) { wakeIsOn = isOn;} 
   //! \brief Steering utlity per-se: Finding on is we have wake field or not. 
   /*! 
      \return Obvious.
   */
    inline bool isWakeOn() const { return wakeIsOn;}
   
   
   //! \brief Beam Tracking utility. Send particle through the model.
   /*! Send pilot bunch, i.e., low intensity, such that the wakefield can be turned off. 
       Position information is recorded by the BPM. 
      If pilot beam hits something, we don't care..  
    \param stateIn: the CHEF state of the particle at injection.  Must be dimensioned to 6. 
    \param sMax:  Tracking will stop past this location, to gain time. 
   */ 
   void sendPilotBeam(const Vector& stateIn, double sMax);
   
   //! \brief Beam Tracking utility. Send a bunch of macro particles through the model.
   /*! Send a nominal intensity (2.0 10^10) bunch. The effect of the wakefield are simulated. 
       Position information is recorded by the BPM. 
       Return false if we hit aperture. 
       The initial state used in this case is the one declared via the 
       setInjectionState method. Default state is the zero vector, on axis in 6D.   
    \param numParticles the CHEF state of the particle at injection.  Must be dimensioned to 6. 
    \param sMax:  Tracking will stop past this location, to gain time. 
    \param printDebug  Optional, provide some debugging info on std::cout and/or std::cerr 
    \param bpmStart The starting point.  If the initial state of the lattice matters, this should
    not be used. 
    \param withStorage Keep an copy of the trajectory. For really advanced use... 
    \return false if we hit aperture. (fiducial an ILC Cavity, r max =~ 3.5 cm. 
   */ 
   // Return false if we hit aperture. 
   bool sendRealBunch(int numParticles, double sMax, 
                       bool printDebug=false, int bpmStart = 0, 
		       bool withStorage=false);
   
   //! \brief Beam Tracking utility. Send multiple bunches, consecutive.
   /*!  Used for dynamical studies. If realistic Beam Position resolution, beam jitter and 
        ground motion are allowed, then one needs to send more than a bunch to get a good 
	estimate of the performance of the machine. 
	The BPMs will automatically average the positions, bunch after bunches. 
	
      \param numBunches The number of bunches.
      \param numParticles The number of macro Particle in the bunch. 
      \param sMax Tracking will stop past this location, to gain time.
      \param printDebug  Optional, provide some debugging info on std::cout and/or std::cerr 
      \param bpmStart Optional The starting point.  If the initial state of the lattice matters, this should
    not be used.
   */ 
   bool sendRealBunches(int numBunches, int numParticles, double sMax, 
                       bool printDebug=false, int bpmStart = 0);
   //! \brief Beam Tracking utility. Prepare a bunch for the model. 
   /*!  Could have been made private, but it might be of use to just prepare the beam 
        and thow it away, for study or debugging purposes.  Called by sendReal Bunch.  
	
      \param numParticles The number of macro Particle in the bunch. 
      \param bunch  A ChefSteering Positron Bunch.  It will be emptied first, than filled.
      \param bpmStart Optional. The starting point... Irrelevant in this context, I would think.
    not be used.
   */ 
   void prepareRealBunch(int numParticles, myPositronBunch& bunch,
                         int bpmStart = 0);
			 
   //! \brief Beam Steering utility. Computes and study the dispersion.  
   /*!  In a Linac, the dispersion is a tricky concept, since one does not has a stable orbit. 
        The dispersion is computed by changing the voltage on the cavity, or by changing 
	the energy in the Bunch Compressor located upstream of the Main Linac. 
	The dispersion and dispersion prime are computed at each BPMs. 
	Two pilot bunches are send thorugh the linac, one at the nominal energy and cavity 
	settings, one off momentum.
	
      \param gradFraction The ratio of the "off" momentum cavity gradient over the 
                            "On" momentum gradient. Typically 0.95 to 0.99.   
      \param BCReducedE  If true the above gradient reduction is applied to the inject beam. 
                         That is, the gradient is reduced upstream of this machine.  
      \param fName  Optional.  A Unix file name where the dispersion value will be listed.
      \param DispInit Optional. One can overwrite the dispersion at injection. 
      \param dispPrimeInit  Optional.  Tweak the dispersion prime at injection. 
      \param bpmMax Optional. Sequential BPM index in the model. Calculation will stop 
          at that BPM location. 
   */ 

   void fillReferenceDispersion(double gradFraction, bool BCReducedE, 
                                std::string const& fName=std::string(""),
				double DispInit = 0.,
				double dispPrimeInit = 0., 
				unsigned int bpmMax = 100000);
				
   
    //! \brief Beam Study utility. Get thenumber of dipoles in the model. 
   /*!
   */ 
   inline size_t GetNumCorrectorDipoles() const {return correctors.size();}
    //! \brief Beam Study utility. Get thenumber of BPMs in the model. 
   /*!
   */ 
   inline size_t GetNumBPMs() const {return theBPMs->size();}
    //! \brief Beam Study utility. Get thenumber of cavities in the model. 
   /*!
   */ 
   inline size_t GetNumCavities() const {return allCavities.size();}
   
   inline BmlPtr getTheModelPtr() { return theModel;}
    //! \brief Beam Study utility. Get the reference momentum a dipole/BPM location.
   /*! 
       Throughout the Steering study, it is implictly assumed  that each quadrupole package has
       a BPM and dipole corrector.   
   \param iBPM Sequential index of the dipole (or BPM) into the beamline  
   \return The momentum, in GeV/c 
   */ 
   inline double getRefMomentumAtDipole(size_t iBPM) const {
       return refPDip[iBPM]; // No checks !!
   }  

   //! \brief Beam Study utility. Get the reference momentum a dipole/BPM location.
   /*! 
       Throughout the Steering study, it is implictly assumed  that each quadrupole package has
       a BPM and dipole corrector.   
   \param iBPM Sequential index of the BPM (or dipole) into the beamline  
   \return The momentum, in GeV/c 
   */ 
   inline double getRefMomentumAtBPM(size_t iBPM) const {
       return refPBPM[iBPM]; // No checks !!
   }  
   //! \brief Beam Study utility. Get the nominal dispersion at dipole/BPM location.
   /*! 
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \return The dispersion, in meters 
   */ 
   inline double getDispersionAtBPM(size_t iDip) const {
       return refDispersionAtBPM[iDip];
   }
   //! \brief Beam Study utility. Get the nominal d'at dipole/BPM location.
   /*! 
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \return The dispersion prime, a pure number 
   */ 
   inline double getDispersionPrimeAtBPM(size_t iDip) const {
       return refDispersionPrimeAtBPM[iDip];
   }
   //! \brief Beam Study utility. Set the nominal d (dispersion) at a dipole/BPM location.
   /*! 
      The dispersion is a feature of the model.  The array below is a private member of this
      class, not expected to be set by the user. It is filled at creation time and 
      not expected to change. But may be an advanced user knows better. 
      
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \param val The dispersion value, in meters.  
   */ 
   
   inline void setDispersionAtBPM(size_t iDip, double val) {
       refDispersionAtBPM[iDip] = val;
   }
   //! \brief Beam Study utility. Set the nominal d' at a dipole/BPM location.
   /*! 
      The d, d' are a feature of the model.  The array below is a private member of this
      class, not expected to be set by the user. It is filled at creation time and  
      not expected to change. 
      
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \param val The dispersion prime value, pure number.  
   */ 
   inline void setDispersionPrimeAtBPM(size_t iDip, double val) {
       refDispersionPrimeAtBPM[iDip] = val;
   }
   
   //! \brief Beam Study utility. Get the lognitudinal position.. 
   /*! 
      No idea why I wrote that one, but here it is... 
      
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \return The s poistion, in meter.   
   */ 
   inline double getRefPositionDispersionAtBPM(size_t iDip) const {
       return refPosDispersionAtBPM[iDip];
   }
   //! \brief Beam Study utility. Get the nominal Corrector strength 
   /*! 
      The strength value of each dipole corrector is kept in memory, such that one can easily
      revert back to the perfect lattice configuration, should the steering process go south.   
      
   \param iDip Sequential index of the BPM (or dipole) into the beamline  
   \return The strength, in T.m.   
   */ 
   inline double getNominalSCorStrength(size_t iDip) const {
       if (iDip >= nominalsCorrStrength.size()) return 0.;
       return nominalsCorrStrength[iDip];
   }
   //! \brief Beam Study utility. Get the nominal accelerating gradient for a given cavity.  
   /*! 
      As for the dipole correctors, settings for each cavity are kept in memory, 
      such that one can easily revert back to the perfect lattice configuration, 
      should the steering process go south.   
      
   \param iCav Sequential index of the cavity into the beamline  
   \return The accelerating voltage, in e.v/m.  (Voltage gradient units confusing in this
   package, sorry.    
   */ 
   inline double getNominalCavityVoltage(size_t iCav) const {
       if (iCav >= nominalCavityVoltage.size()) return 0.;
       return nominalCavityVoltage[iCav];
   }
   //! \brief Beam Study utility. Get the off energy gradient for a given cavity.  
   /*! 
      
   \param iCav Sequential index of the cavity into the beamline  
   \return The accelerating voltage, in e.v/m.  
   */ 
   inline double getOffECavityVoltage(size_t iCav) const {
       if (iCav >= OffECavityVoltage.size()) return 0.;
       return OffECavityVoltage[iCav];
   }
   //! \brief Beam Sterring utility. Find out if a Dipole corrector can be used.   
   /*!  Advanced use: one had, at some point, the capability of declaring a dipole broken, 
        For some lattice (straight, for instance), acceptable emittance preservation 
	was obtained even if some correctors can't be used. 
   \param iDip Sequential index of the cavity into the beamline  
   \return true is dipole corrector can be used for steering ( default).   
   */ 
   inline bool isDipoleOK(size_t iDip) const { return dipoleOK[iDip];}
   
   //! \brief Beam Sterring utility. set a Dipole corrector "broken".   
   /*!  Advanced use.. Not sure this Steering example will work if invoked.  
   \param iDip Sequential index of the cavity into the beamline  
   */ 
   inline void markDipoleBroken(size_t iDip) {dipoleOK[iDip] = false;}
   //! \brief Beam Sterring utility. set a Dipole corrector "repaired ".   
   /*!  Advanced use.. Not sure this Steering example will work if invoked.  
   \param iDip Sequential index of the cavity into the beamline  
   */ 
   inline void markDipoleRepaired(size_t iDip) {dipoleOK[iDip] = true;}
   
   //! \brief Beam Sterring utility. Setting Cavity from a file...   
   /*!  Advanced use.. Not sure this Steering example will work if invoked.  
   */ 
   void loadOffECavitySettings(); // Pick all cavity voltages and Phases 
                                  // put them in memory
   //! \brief Beam Sterring utility. Randomly flutuating vaity gradient.  
   /*!  Advanced use.. A simple Gaussian model for now. Makes only sense for dynamical studies.  
        
   */ 
   void jitterKlystrons();
			
   inline void setKlystronJitter(double j) {klystronJitter = j;}
   inline double getKlystronJitter() { return 	klystronJitter;}
   		
   //! \brief Beam Sterring utility. Part of the Klystron jitter modeling.  
   /*!  
   */ 
   inline void setNumCavPerKlystrons(int j) { numCavPerKlystrons = j;}
   inline int getNumCavPerKlystrons() { return 	numCavPerKlystrons;}
    Gaussian myGauss; // for generating random mis-alignment..
  		
// We make everything protected, because this will be use as a Base class for 
// more sophisticated models... 
   
   protected:
   
   static const double kFact;
   const double beamEnergyZero; // The injection energy. Intrinsic propert of the model.
   
   
   BmlPtr theModel;
   MLBPMs* theBPMs;
   double nominalVoltage;
   bool hasDoubleQuads; // Impose by the benchmarks
//   LatticeFunctionTable latticeFunctions;  // But it not 5 GeV everywhere!... 

   size_t numDim; // the number of BPM's    
   std::vector<VKickPtr> correctors; // My correctors magnets..
   std::vector<QuadrupolePtr> quadList; // and my quadrupole list. 
   std::vector<LinacCavityPtr> allCavities; // my list of cavities.  
   std::vector<double> nominalCavityVoltage; // Keep a copy of nominal voltage 
   std::vector<double> nominalsCorrStrength; // Keep a copy of sBend 
                                             // lattice values.  
   std::vector<double> nominalCavityPhase; // and phase on each cavity.   
   std::vector<double> OffECavityVoltage; // Keep a copy of Off-Energy voltage 

   std::vector<double> OffECavityPhase; // Keep a copy of Off-Energy Phase 

   std::vector<double> refPDip; // Reference momentum at each dipole, indexed sequentially 
   std::vector<double> refPQuad; // Reference momentum at each quadrupole.
   std::vector<double> refBRhoQuad; // Reference Quad Strenth..
   std::vector<double> refPBPM;
   // The reference dispersion at the dipoles. 
   std::vector<double> refDispersionAtBPM;
   std::vector<double> refDispersionPrimeAtBPM;
   std::vector<double> refPosDispersionAtBPM;
   // When we'll do the curved Linac, we must have a way to set this vector. 
   // Machine imperfection section: broken dipoles. 
   std::vector<bool> dipoleOK;
   // A simple counter to tally the total number of pulses we need to send 
   // to steer the beam. 
   size_t totalNumberPulses;
   // For debugging purpose.. 
   double theCurrentBunchMomentum;

// More protected method to make calculation easier for derived class:
// Load the reference momentum, and also the component frames vector..    
   void loadRefMomentum(); 
   
   private:
      // For studies. 
// 
// Data for the real pilot bunch, which have a finite size 
// and potentially submitted to jitter 
//

// state at injection, for single particle or bunches 
   Vector stateAtInjection;
   MatrixC bunchMatch; // At the beginning.. 
   double bunchEpsilY;
   double bunchEpsilX;
   double bunchDPoP;
   double bunchSigmaZ;
   double bunchYPrimeOffset; // These will be 0... 
   double bunchYOffset;
   double bunchXPrimeOffset;
   double bunchXOffset;
   double bunchDPoPOffset;  // From beam jitter, unknow causes 
   double bunchDPoPOffsetBC;
   double klystronJitter;
       // Fixed offset, reduced energy in the bunch compressor.
//  Ancillary Physics processes
   bool wakeIsOn; // Turning wake field on or Off..  

// We clone the above data member describing a second pilot bunch, 
// with arbitrary smaller emittance, to study the dependence of the 
// repsonse matrices on pilot bunches. 

   double bunchRMEpsilY;
   double bunchRMEpsilX;
   double bunchRMDPoP;
   double bunchRMSigmaZ;
   double bunchRMYPrimeOffset; // These will be 0... 
   double bunchRMYOffset;
   double bunchRMXPrimeOffset;
   double bunchRMXOffset;
   double bunchRMDPoPOffset;
   double bunchRMDPoPOffsetBC;
       // Fixed offset, reduced energy in the bunch compressor.
//
// Now, a repository a bunch that made it through the lattice, to avoid 
// having to recretate them. 
// 
   bool atOffEnergy; // Boolean flag to indicate if we run off energy 
                     // while dipersion steering.
   int numRequiredPulses; // Per steering setting		     
   std::vector<myPositronBunch *> pilotBunches; // indexed on BPM.    
   std::vector<myPositronBunch *> pilotBunchesOffE; // For Off energies..    
   std::vector<myPositronBunch *> pilotBunchesRM; // same, for Response Matrices    
   std::vector<myPositronBunch *> pilotBunchesRMOffE; 
              // same, for Response Matrices, Off energy.    
// a token string to download the moview files. 
   std::string displayToken;
   std::vector<int> bpmsDisplayed;
   int numCavPerKlystrons; // for Jitter Studies 
   std::ofstream *fOutMovieScale;
   // No Ground Motionfor Chef testing.. 
//   GroundMotionInterf *myGMInterface; // Owned !!!! 
// Inpute Twiss parameters for input beam 
   myTwissParam twissIn; // defined in myPositron Bunch, where it will be used. 
// 
    std::string gridFileNameUpdate;
    time_t startingSteeringTime;
// More steering utilities dealing with the bunch.. 
// Accessors..
   public:
    //! \brief Dynamical Steering Utilities. 
    /*!  Advanced use. For dynamical steering, we keep trakc of both the number of pulses and the 
        real time. Convenience accessor here.. 
     \return a Number of seconds, I think.. 
    */
    inline time_t getStartingSteeringTime() {return startingSteeringTime;}
    //! \brief Steering Utilities. 
    /*!  Advanced use. Access to theGaussian generator, should one want to keep track of 
        random numbers consistently.  
     \return A pointer to a Chef Gaussian Distribution. 
    */
    inline Gaussian* getGaussianDist() {return &myGauss;}
    //! \brief Steering Utilities. Access to the bunch property at injection. 
    /*!    
     \return The normalized, invariant, dispersion corrected vertical emittance. 
    */
    inline double getBunchEpsilY() const { return bunchEpsilY;} 
    //! \brief Steering Utilities. Access to the Bunch property at injection. 
    /*!    
     \return The normalized, invariant, dispersion corrected horizontal emittance. 
    */
    inline double getBunchEpsilX()  const { return bunchEpsilY;}
    inline double getBunchDPoP()  const { return bunchDPoP;}
    inline double getBunchSigmaZ()  const { return bunchSigmaZ;}
    inline double getBunchYPrimeOffset() const  {return bunchYPrimeOffset;}
    inline double getBunchYOffset()  const {return bunchYOffset;}
    inline double getBunchXPrimeOffset() const  {return bunchXPrimeOffset;}
    inline double getBunchXOffset() const  {return bunchXOffset;}
    inline double getBunchDPoPOffset() const  {return bunchDPoPOffset;}
    inline double getBunchDPoPOffsetBC() const  {return bunchDPoPOffsetBC;}
//
     inline Vector getStateAtInjection() const {return stateAtInjection;} 
    inline double getBunchRMEpsilY() const { return bunchRMEpsilY;} 
    inline double getBunchRMEpsilX()  const { return bunchRMEpsilY;}
    inline double getBunchRMDPoP()  const { return bunchRMDPoP;}
    inline double getBunchRMSigmaZ()  const { return bunchRMSigmaZ;}
    inline double getBunchRMYPrimeOffset() const  {return bunchRMYPrimeOffset;}
    inline double getBunchRMYOffset()  const {return bunchRMYOffset;}
    inline double getBunchRMXPrimeOffset() const  {return bunchRMXPrimeOffset;}
    inline double getBunchRMXOffset() const  {return bunchRMXOffset;}
    inline double getBunchRMDPoPOffset() const  {return bunchRMDPoPOffset;}
    inline double getBunchRMDPoPOffsetBC() const  {return bunchRMDPoPOffsetBC;}

// And back to beamlines.. 

    inline size_t getNumBPMs() const {return theBPMs->size();} 
    inline MLBPMPtr getABPMPtrs(size_t index) const {
       if (index >= theBPMs->size()) return MLBPMPtr ();
       return (*theBPMs)[index];}
    inline double getTheBeamEnergyInput() const { return beamEnergyZero;} 
     
    
   // Utilities to manipulate steering bunches.
   
    inline void setMatchingMatrix(MatrixC& m) { 
      for (size_t i=0; i != Particle::PSD; ++i) {
        for (size_t j=0; j != Particle::PSD; ++j) bunchMatch(i,j) = m(i,j); 
      }
    }
     //! \brief Steering Utilities. Setting the matching condition for the injected bunch 
    /*    
     \param  myTwissParam  A Twiss parameter data structure, 
                           described in class myPositron bunch.
    */
     inline void setInputTwissParam(myTwissParam &p) { twissIn = p;}
     //! \brief Steering Utilities. Getting the back the Twiss parameter for the injected bunch.
    /*    
     \return The Twiss Parameters.  
    */
     inline myTwissParam getInputTwissParam() const {return twissIn;}
     //! \brief Steering Utilities. Setting the bunch property at injection. 
    /*    
     \return The normalized, invariant, dispersion corrected horizontal emittance. 
    */
    inline void setBunchEpsilY(double e) {bunchEpsilY = e;}
    inline void setBunchEpsilX(double e) {bunchEpsilX = e;}
    inline void setBunchDPoP(double dp) {bunchDPoP = dp;}
    inline void setBunchSigmaZ(double dz) {bunchSigmaZ = dz;}
    inline void setBunchYPrimeOffset(double dyp) {bunchYPrimeOffset = dyp;}
    inline void setBunchXPrimeOffset(double dxp) {bunchXPrimeOffset = dxp;}
    inline void setBunchYOffset(double dy) {bunchYOffset = dy;}
    inline void setBunchXOffset(double dx) {bunchXOffset = dx;}
    inline void setBunchDPoPOffset(double dp) {bunchDPoPOffset = dp;}
    inline void setBunchDPoPOffsetBC(double dp) {bunchDPoPOffsetBC = dp;}
    
    inline void setBunchRMEpsilY(double e) {bunchRMEpsilY = e;}
    inline void setBunchRMEpsilX(double e) {bunchRMEpsilX = e;}
    inline void setBunchRMDPoP(double dp) {bunchRMDPoP = dp;}
    inline void setBunchRMSigmaZ(double dz) {bunchRMSigmaZ = dz;}
    inline void setBunchRMYPrimeOffset(double dyp) {bunchRMYPrimeOffset = dyp;}
    inline void setBunchRMXPrimeOffset(double dxp) {bunchRMXPrimeOffset = dxp;}
    inline void setBunchRMYOffset(double dy) {bunchRMYOffset = dy;}
    inline void setBunchRMXOffset(double dx) {bunchRMXOffset = dx;}
    inline void setBunchRMDPoPOffset(double dp) {bunchRMDPoPOffset = dp;}
    inline void setBunchRMDPoPOffsetBC(double dp) {bunchRMDPoPOffsetBC = dp;}

    public:
    // More utilities related to real time studies.  
    inline void ResetPulseCounter() {totalNumberPulses = 0;}
    inline void IncrementPulseCounter() {totalNumberPulses++;}
    inline void IncrementPulseCounter(int n) {totalNumberPulses+=n;}
    inline size_t GetPulseCount() const {return totalNumberPulses;}
    inline double getCurrentRealMomentum() 
       const {return theCurrentBunchMomentum;}
    // For changing the whole Linac energy.. 
    // (Utilities for changing specific sets of cavitiy pending)
    void setLinacGradient(double factor);  // A multiplication factor. 
    void resetLinacGradient();  // Set it back to nominal 
    void clearAllStoredBunches(); // To be done if we go back and study the 
                                   // the beginning.. 
    void uploadRMBunchAtBPM (int aBPM, myPositronBunch& abunch); // aBunch should be const
    void uploadBunchAtBPM (int aBPM, myPositronBunch& abunch);
    void downloadRMBunchAtBPM (int aBPM,  myPositronBunch& abunch) const;
    void downloadBunchAtBPM (int aBPM,  myPositronBunch& abunch) const;
    inline bool isBunchEmptyAtBPM (int aBPM) const {
      if (!this->atOffEnergy) {
       if ((aBPM < 0) || (aBPM >= pilotBunches.size())) return true; 
       return pilotBunches[aBPM]->empty();
      } else {
       if ((aBPM < 0) || (aBPM >= pilotBunchesOffE.size())) return true; 
       return pilotBunchesOffE[aBPM]->empty();
      }
    }  
    inline bool isRMBunchEmptyAtBPM (int aBPM) const {
      if (!this->atOffEnergy) {
        if ((aBPM < 0) || (aBPM >= pilotBunchesRM.size())) return true; 
        return pilotBunchesRM[aBPM]->empty();
      } else {
        if ((aBPM < 0) || (aBPM >= pilotBunchesRMOffE.size())) return true; 
        return pilotBunchesRMOffE[aBPM]->empty();
      
      }
    }
    // Details done in SteeringResponseMatrix 
    inline void setLinacOffEnergyFlag() {atOffEnergy = true;} 
    inline void setLinacOnEnergyFlag() {atOffEnergy = false;} 
    
    
    inline void setDisplayToken(const std::string &base) {
	          displayToken = base;
	}
    inline void addToDisplayToken(std::string &aName) 
                                      { displayToken += aName;}		    
    inline void addBPMForDisplay(int aBPM) { bpmsDisplayed.push_back(aBPM);}
    inline void clearBPMsForDisplay() 	{ bpmsDisplayed.clear(); }
    inline void openFileOutMovieScale () {
      std::string fN(displayToken);
      fN += std::string("/Scales.txt");
      fOutMovieScale = new std::ofstream(fN.c_str());
      (*fOutMovieScale) << " Pulse BPM y yp yScale " << std::endl;
    }
    inline void updateFileOutMovieScale ( int aBPM, 
                           double y, double yp, double yScale) { 
			   // units in microns
       if (fOutMovieScale == 0) return;
	(*fOutMovieScale) << totalNumberPulses << " " << aBPM 
	               << " " << y << " " << yp << " " << yScale << std::endl;
					  
    }
    inline void closeFileOutMovieScale () {
       if (fOutMovieScale == 0) return;
       fOutMovieScale->close();
       delete fOutMovieScale;
       fOutMovieScale = 0;
    }				  
    inline int getPlannedPulses() const { return numRequiredPulses;}
    // need to think about algorithms here. 
    inline void setRequiredNumPulses (int n) { numRequiredPulses = n;}
    // In case we left the machine as is for a coffe break.. 
    inline void incrementPulseCount (int n) { totalNumberPulses += n;}
    
    // Investigate difference between Merlin and CHEF in itegrating 
    // Maxwell though the tilted cavity. 
    
    void correctVerticalOffsetInCav(const ElmPtr q, Particle *ptr);				  
    void correctVerticalOffsetInCav(const ElmPtr q, 
                                              myPositronBunch &aBunch);	
    
   inline double getInputVerticalDispersion() const {return twissIn.DY;}				      			  
    inline double getInputVerticalDispersionPrime() const {return twissIn.DYp;}				      			  
    // even more utility code to set specific misalignment.
    // Implemented in SteerAddUtil
    void setCavRotation144DMSDebug();
    
    // Update utility on the farm.  Has the posibility of exiting !!!  
    
    void gridUpdate (const std::string message, bool withOptExit);
    
    void testBunchMemUtil();				      			  
    		   
};
// We allow ourself one global pointer of this class. 
// Dangerous, poorly implemented: SteerUtil util is not a singleton. 
//
namespace Steering {

  extern SteerUtil *theSteerUtil;
} 

#endif
