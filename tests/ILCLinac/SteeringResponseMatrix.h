#ifndef _h_SteeringResponseMatrix
#define _h_SteeringResponseMatrix 1
#include <vector>
#include <MathConstants.h>
#include <PhysicsConstants.h>
#include <beamline.h>
#include "MLBPMs.h"
#include <Matrix.h>
#include "SteerUtil.h"

using std::vector;

namespace Steering {

// CORDIP : Using simple corrector settings, like in 1-to-1 steering. 
// QUADMOVE:  Use a quadrupole move instead  
// ECHANGE:  a set of kylstrons now have a different energy ( used in DFS)
// ECHANGEREL: set the machine to a match otpic for this energy.    
// ECHANGEMERLIN: Dispersion Matched Steering     

//  (This file should be called Steering Response Matrices) 

// We will need a Steering utilitiy class, good for all the steering 
// problems.. Low level control utility. 
 
  enum actuator {CORDIP, QUADMOVE, ECHANGE, STEERTOORBIT, CRYOTILT,
                  STEERTOORBITONOFFE, ECHANGEMERLIN, ECHANGEPUREDMS} ;
//! \brief Interface Definition for Response matrix. 
/*!
   In terms of method, must define the On and Off momenta. 
   Can be trivial for algorithms that don't involved 
   changing the energy. 
   \param byPhase if true, change the rf phase instead ofthe gradient. 
   \param fromNomToReduced  going from nominal accelration to 
                            reduced acceleration ( off momenta) 
   \param otherOption	for futur use.. 		    
*/ 
  template<actuator> class ResponseMatrix {
     virtual void setEnergy(bool byPhase, bool fromNomToReduced, 
                            bool otherOption=false);
  };
 
//! \brief One to One simple steering algorithm 
/*! 
   Used only if the BPM offset are know and stable. 
   Obsolete, needt to be checked again. 
*/  
  template<> class ResponseMatrix<CORDIP>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstDipole, size_t numDipoles,
			 size_t numBrokenDipoles = 0);  
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
                                  size_t firstDipole, size_t numDipoles,
				  size_t numBrokenDipoles = 0);
				  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getDipolesPtrs(std::vector<ElmPtr >& actuators);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
    static double kFact; // factor for amplitude of the kick.. 
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other = false);
  
    private:
  
    std::string label;
    BmlPtr theBmlPtr; // not owned
    std::vector<ElmPtr> dipoles;
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
  
    void buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firstDipole, size_t numDipoles, size_t numBroken); 
		 // real constructor. 
  
  }; // ResponseMatrix<CORDIP>

//! \brief One to One simple steering algorithm 
/*! 
   One to One steering, by moving the quadrupole. 
   Obsolete, need to be recomissioned. 
*/  
  template<> class ResponseMatrix<QUADMOVE>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs, 
                         size_t firstQuad, size_t numQuads);  
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
                                  size_t firstQuad, size_t numQuads);
				  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getQuadrupolePtrs(std::vector<QuadrupolePtr >& actuators);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other = false);
  
    private:
  
    static const double ampl; // Amplitude of the physical motion.. 
    std::string label;
    BmlPtr theBmlPtr; // not owned
    vector<QuadrupolePtr > quads; // generic vector of elements, dipole, quads.
                                // Object themself belong to Beamline(s) in CHEF. 
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
  
    void buildIt(size_t firstBPM, size_t numBPMs, 
                 size_t firsQuad, size_t numQuads); // real constructor. 
  
  }; // ResponseMatrix<QUADMOVE>
  
//! \brief One to One simple steering algorithm 
/*! 
   Not sure what that tried to do... 
   Obsolete, need to be recomissioned. 
   Next:
   Simple response function to a given (i.e., the current one ) 
   orbit. 
   Used in 1 to 1 steering, not all that relevant. 
*/
  template<> class ResponseMatrix<STEERTOORBIT>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole);  
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                         size_t firstBPM, size_t firstDipole);  
				  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     inline size_t getFirstBPMIndex() const {return firstBPMIndex;}			    
     inline size_t getDipoleIndex1() const {return firstDipIndex;}
     inline size_t getDipoleIndex2() const {return firstDipIndex+1;}
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other = false);
     
    private:
  
//    static const double kFact; // We'll be using the one from CORDIP
    std::string label;
    BmlPtr theBmlPtr; // not owned
    size_t firstBPMIndex;
    size_t firstDipIndex;// The first dipole we'll tweak.
    VKickPtr dipole1; // Probably not need to keep these pointers in 
                        // memory
    VKickPtr dipole2; // the next one  
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
  
    void buildIt(); // real constructor. 
  
  }; // ResponseMatrix<STEERTOORBIT>
  
  
  
//! \brief Pure DMS steering algorithm 
/*! 
   Obsolete, need to be recomissioned. 
*/
  template<> class ResponseMatrix<ECHANGE>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 bool readjustTheLattice=false, 
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500 );
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 bool readjustTheLattice=false,
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500);
			  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getCavityPtrs(std::vector<LinacCavityPtr >& contextInfo);			    
     size_t getDipolePtrs(std::vector<ElmPtr >& actuators);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
     void buildPreSteer(size_t firstBPM, size_t firstDipole);
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other = false);
     inline double getEnergyOffset() const {return energyOffset;}
     void studyStability(int option);
     
  
    private:
  
    double gradReduxFact; // factor by which the energy will be reduced
    std::string label;
    BmlPtr theBmlPtr; // not owned
    vector<LinacCavityPtr > cavities; // generic vector of elements, dipole, quads.
                                // Object themself belong to Beamline(s) in CHEF. 
    size_t firstDipIndex;
    vector<ElmPtr> dipoles; // which we'll tune 
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    ResponseMatrix<STEERTOORBIT> *mPreSteer;
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
    
    int numPartPerPulse; // Number of particles per pulse to compute 
                         // derivatives. 
    
    bool rescaleLattice; // If true, we will change the quad setting to 
                         // to preserve the same nominal lattice. 
    bool changeRFPhase; // Instead of changing the amplitude, 
                        // shift the r.f. phase. 

    double kScale;
    std::vector<double> quadStrength;
    // Internal methods.. 

    void buildIt(size_t firstBPM, size_t numBPMs,
                 size_t firstCavity, size_t numCavities,
                 size_t firstDipole, size_t numDipoles,
		 size_t numBroken); // real constructor. 
    double energyOffset; // internal, deduced variable.  
    void rebuildwPreSteer(); // Recomputation of the matrix due to a change 
                            // in the preSteering option.
			    
    
  }; // ResponseMatrix<ECHANGE>
 
 // Implemented in SteeringRespMatrixTwo
//! \brief Pure DMS, by tilting Cryo-Modules.  
/*! 
   Could be usefull. Need to be recomissioned.
   Not tested in years..  
*/
  
  template<> class ResponseMatrix<CRYOTILT>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstCavityPitched, size_t numCavityPitched,
			 size_t numCavitiesPerCryo,
			 bool withRFPhase=false,
			 double kMininScale = 1.,
			 int numPart = 1);
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstCavityPitched, size_t numCavityPitched,
			 size_t numCavitiesPerCryo,
			 bool withRFPhase=false,
			 double kMininScale = 1.,
			 int numPart = 1);
			  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getCavityPtrs(std::vector<LinacCavityPtr >& contextInfo);			    
     size_t getCavityPitchedPtrs(std::vector<LinacCavityPtr >& contextInfo);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
     void setEnergy(bool byPhase, bool fromNomToReduced, bool pitchedAtFullEnergy);
     void setDownstreamOff();
     inline double getEnergyOffset() const {return energyOffset;}
     void applyTilt(double value, size_t iModule);
     double getAverageCavTilts(size_t iModule) const;
  
    private:
  
    double gradReduxFact; // factor by which the energy will be reduced
    double gradReduxPrePitched; // factor by which the energy will be reduced
                                // in the upstream sections of the pitched
				// section, when the pitched region is reduced
    size_t numMod;
    size_t numCavPerModule;
    std::string label;
    BmlPtr theBmlPtr; // not owned
    vector<LinacCavityPtr > cavities; // generic vector of elements, dipole, quads.
                                // Object themself belong to Beamline(s) in CHEF. 
    vector<LinacCavityPtr> cavitiesPitched; // We will correct their pitch 
    size_t firstCavIndex;  // Section upstream of the pitched section.
    size_t firstCavPitchedIndex;
    vector<LinacCavityPtr> cavitiesDwnstr; // We will correct their pitch 
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
    
    int numPartPerPulse; // Number of particles per pulse to compute 
                         // derivatives. 
    
    bool rescaleLattice; // If true, we will change the quad setting to 
                         // to preserve the same nominal lattice. 
    bool changeRFPhase; // Instead of changing the amplitude, 
                        // shift the r.f. phase. True for both the 
			// region upstream of the pitched section or 
			// the pitched section itself
			// !!!! not yet supported. 

    double kScale;
    std::vector<double> nominalRFSettingUpstream;
    std::vector<double> nominalRFSettingPitched;
    std::vector<double> reducedRFSettingUpstrFP;  // Pitched at full energy 
    std::vector<double> reducedRFSettingUpstrRP;  // Pitched at reduced energy 
    std::vector<double> reducedRFSettingPitched;
    
    // Internal methods.. 

    void buildIt(size_t firstBPM, size_t numBPMs, size_t numCavU, size_t numCavP); 
                           // real constructor. 
    double energyOffset; // internal, deduced variable. 
                        // Total enegy offset, same if the pitched section is 
			// reduced or not.  
    void rebuild(); // Recomputation of the matrix due to a change 
                            // in the preSteering option.
			    
    
  }; // ResponseMatrix<CRYOTILT>
  //! \brief DMS, related. 
  /* Used in the Pres-steer algorithm   
   Used in ECHANGE: one - optionally-, can set the orbit in such a 
   way that the first two BPM in the section where we Fix Dispersion Steer
    have the same reading as on the nominal energy. 
   Assumes that these two dipoles are working. 
   suggested by Jeff Smith, Spring 2006.
   
   Obsolete, needt to be recommissioned. 
  */ 
  template<> class ResponseMatrix<STEERTOORBITONOFFE>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGE> *aRespMatrix,
			 double kMininScale = 1.,
			 int numPart = 500,
			 double zMaxBeamDisplay = -1.,
			 size_t numDipoles = 2, 
			 size_t numBPMs = 3);
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGE> *aRespMatrix,
			 double kMininScale = 1.,
			 int numPart = 500,
			 double zMaxBeamDisplay = -1.,
			 size_t numDipoles = 2, 
			 size_t numBPMs = 3); // Last optional 
			                         // for display purposes.
						 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGEMERLIN> *aRespMatrix,
			 double kMininScale = 1.,
			 int numPart = 500,
			 double zMaxBeamDisplay = -1.,
			 size_t numDipoles = 2, 
			 size_t numBPMs = 3);
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                         size_t firstBPM, size_t firstDipole,
			 ResponseMatrix<ECHANGEMERLIN> *aRespMatrix,
			 double kMininScale = 1.,
			 int numPart = 500,
			 double zMaxBeamDisplay = -1.,
			 size_t numDipoles = 2, 
			 size_t numBPMs = 3); // Last optional 
			                         // for display purposes.
				  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     inline size_t getFirstBPMIndex() const {return firstBPMIndex;}			    
     inline size_t getDipoleIndex1() const {return firstDipIndex;}
     inline size_t getDipoleIndex2() const {return firstDipIndex+1;}
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other = false);
     
    private:
  
//    static const double kFact; // We'll be using the one from CORDIP
    size_t numDipolesTweaked;
    size_t numBPMsOnOff; // shoulbe a member of this
    std::string label;
    BmlPtr theBmlPtr; // not owned
    size_t firstBPMIndex;
    size_t firstDipIndex;// The first dipole we'll tweak.
    double zMaxBeamProp;
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
		  
    ResponseMatrix<ECHANGE> *theEChangeMatrix; // Used to configured the 
                                               // the energy pattern. 
    ResponseMatrix<ECHANGEMERLIN> *theEChangeMatrixMerlin;
                                               // Used to configured the 
                                               // the energy pattern. 
    
    void buildIt(double kScale, int numParticles); // real constructor. 
  
    public:
    
    inline void setEnergy(bool byPhase, bool fromNomToReduced ) 
          {theEChangeMatrix->setEnergy(byPhase, fromNomToReduced);}
    inline size_t getNumBPMsOnOff() {return numBPMsOnOff;}
    inline size_t getNumDipolesTweaked() {return numDipolesTweaked;}	  
	  
 }; // ResponseMatrix<STEERTOORBITONOFFE>

//! \brief Weigthed Dispersion Matched Steering. 
/*!
   This is the response matrix used to support DMS studies. 
   Current.
   Argument list of constructor too long, should have use the DMS Plan 
   classes mre extensively.  Re-design might be called for...  
*/ 
  
  template<> class ResponseMatrix<ECHANGEMERLIN>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double weightAbs, double weightDMS, 
			 double weigthCorr, 
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500, bool useDisp = false);
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double weightAbs, double weightDMS, 
			 double weigthCorr, 
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500, bool useDisp = false );
			  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getCavityPtrs(std::vector<LinacCavityPtr >& contextInfo);			    
     size_t getDipolePtrs(std::vector<ElmPtr >& actuators);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
     void buildPreSteer(size_t firstBPM, size_t firstDipole);
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other=false);
     inline double getEnergyOffset() const {return energyOffset;}
//     void studyStability(int option);
     
  
    private:
  
    double gradReduxFact; // factor by which the energy will be reduced
    double weightAbsolute; // The weight associate to the absolute 
                           // orbit deviation, works great if BPM offsets 
			   // are zero. 
    double weightDMS; // Associated to the Dispersion term. 
    double weightCorrector; // If the kick is too hard, this can't be good, 
                            // so add a term...We will do this in the 
			    // the measurements... ???  			   
    std::string label;
    BmlPtr theBmlPtr; // not owned
    vector<LinacCavityPtr > cavities; // generic vector of elements, dipole, quads.
                                // Object themself belong to Beamline(s) in CHEF. 
    size_t firstDipIndex;
    vector<ElmPtr> dipoles; // which we'll tune 
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    ResponseMatrix<STEERTOORBIT> *mPreSteer;
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
    
    int numPartPerPulse; // Number of particles per pulse to compute 
                         // derivatives. 
    
    bool changeRFPhase; // Instead of changing the amplitude, 
                        // shift the r.f. phase. 

    bool usePerfectDispKnow; // See variable in DMS Plan. 
    
    double kScale;
    std::vector<double> quadStrength;
    // Internal methods.. 

    void buildIt(size_t firstBPM, size_t numBPMs,
                 size_t firstCavity, size_t numCavities,
                 size_t firstDipole, size_t numDipoles,
		 size_t numBroken); // real constructor. 
    double energyOffset; // internal, deduced variable.  
    void rebuildwPreSteer(); // Recomputation of the matrix due to a change 
                            // in the preSteering option.
			    
    
  }; // ResponseMatrix<ECHANGEMERLIN>
  
//! \brief Pure Dispersion Matched Steering. 
/*!
   This is the response matrix used to support DMS studies. 
   Obsolete. Used to study the above schema.  
*/ 
 
  template<> class ResponseMatrix<ECHANGEPUREDMS>: public Matrix {

// 
    public:
  
    // Used indices of elements into the beam line 
     ResponseMatrix(std::string& label, BmlPtr bmlPtr,
                         size_t firstBPM, size_t numBPMs,
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double weightAbs, double weightDMS, 
			 double weigthCorr, 
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500, bool useDisp = false);
  
     ResponseMatrix(std::string& label, BmlPtr bmlPtr, Vector& stateInTest, 
                                  size_t firstBPM, size_t numBPMs, 
			 double gradientReduction,  
			 size_t firstCavity, size_t numCavities,
			 size_t firstDipole, size_t numDipoles,
			 double weightAbs, double weightDMS, 
			 double weigthCorr, 
			 bool withRFPhase=false,
			 size_t numBrokenDipoles=0,
			 double kMininScale = 1.,
			 int numPart = 500, bool useDisp = false );
			  
     // Accessors... We will need the pointers to the responders and 
     // actuators. 
     size_t getCavityPtrs(std::vector<LinacCavityPtr >& contextInfo);			    
     size_t getDipolePtrs(std::vector<ElmPtr >& actuators);			    
     size_t getBPMPtrs(MLBPMs& bpms);			    
     void buildPreSteer(size_t firstBPM, size_t firstDipole);
     void setEnergy(bool byPhase, bool fromNomToReduced, bool other=false);
     inline double getEnergyOffset() const {return energyOffset;}
//     void studyStability(int option);
     
  
    private:
  
    double gradReduxFact; // factor by which the energy will be reduced
    double weightAbsolute; // The weight associate to the absolute 
                           // orbit deviation, works great if BPM offsets 
			   // are zero. 
    double weightDMS; // Associated to the Dispersion term. 
    double weightCorrector; // If the kick is too hard, this can't be good, 
                            // so add a term...We will do this in the 
			    // the measurements... ???  			   
    std::string label;
    BmlPtr theBmlPtr; // not owned
    vector<LinacCavityPtr > cavities; // generic vector of elements, dipole, quads.
                                // Object themself belong to Beamline(s) in CHEF. 
    size_t firstDipIndex;
    vector<ElmPtr> dipoles; // which we'll tune 
    MLBPMs bpms;  // The set of BPM we'll be using.. 
    ResponseMatrix<STEERTOORBIT> *mPreSteer;
    Vector stateIn; // The probe particle for which we'll estimate the 
                  // response matrix: default is zero vector  
    
    int numPartPerPulse; // Number of particles per pulse to compute 
                         // derivatives. 
    
    bool changeRFPhase; // Instead of changing the amplitude, 
                        // shift the r.f. phase. 

    bool usePerfectDispKnow; // See variable in DMS Plan. 
    
    double kScale;
    std::vector<double> quadStrength;
    // Internal methods.. 

    void buildIt(size_t firstBPM, size_t numBPMs,
                 size_t firstCavity, size_t numCavities,
                 size_t firstDipole, size_t numDipoles,
		 size_t numBroken); // real constructor. 
    double energyOffset; // internal, deduced variable.  
    void rebuildwPreSteer(); // Recomputation of the matrix due to a change 
                            // in the preSteering option.
    std::vector<double> effectiveDispersion;
    
    public:
    double getEffectiveDispersion(size_t kBpm);
    		    
    
  }; // ResponseMatrix<ECHANGEPUREDMS>
} // Steering namespace 

#endif
