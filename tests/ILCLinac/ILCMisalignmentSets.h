/*! 
  Proper definition of the Misalignment models are crucial to the study of 
  Emittance preservation in the ILC Main Linac and Ring to Main Linac.
  Such models were discussed throughout the ILC RDR phase. Latest 
  state of the discussion can be found in the talk given by Kyioshi Kubo, 
  at the SLAc meeting, at 
  http://ilcagenda.linearcollider.org/getFile.py/access?contribId=8&amp;sessionId=6&amp;resId=1&amp;materialId=slides&amp;confId=2364 

  Here, we define first an abstract class, then a specified set of misalignement 
       used in the latest round of bench mark misalignments. 
  
  A second specification is the actual translation of the algorithm discussed at 
    that Slac December meeting. 
        
  Paul Lebrun, 2006-2007, finalized Feb 2008. 
*/
#ifndef _h_ILCMisalignmentSets
#define _h_ILCMisalignmentSets 1
#include <vector>
#include <iostream>
#include <fstream>
#include <MathConstants.h>
#include "bmlnElmnt.h"
#include <PhysicsConstants.h>
#include <Distribution.h>
#include <beamline.h>
#include <kick.h>
#include <quadrupole.h>
#include <thinpoles.h>
#include <LinacCavity.h>
#include <Matrix.h>
#include "MLBPMs.h"
#include <time.h>
#include "SteerUtil.h"
//! \brief Abstract interface. 
/*! Basically, a set of formall access routines.
   This interface is in fact specified by what is it's missing 
   accessors,. For instance, the quadupole pitches are not implemented, 
   as, to first order, and to a very good approximation, the don't matter. 
   The Coordinate system in which displacement values are returned is the 
   beamline model coordinated. That is, the coordinate systme of the perfect 
   machine. Realistic algorithm in the realm of specified misalignment consist of 
   translated coordinates from a realistic survey model to the CHEF model, 
   and making adhoc and realistic displacement.  
   
   The indices always refers to beam line elements count, starting from s=0. 
   They refer to the specified beam line element, i.e., a quadrupole index for 
   the getQuadRoll method. (once placed and flattened, Elements don't change their 
    s locations.)
*/
class ILCMisalignmentSet {
   public: 
     virtual ~ILCMisalignmentSet() {}
     /* ! \brief Interface specification 
        Offset are given with respect to the nominal beam line 
	 (perfect and mytical, or the ideal survey line)
        Cav stands for r.f. Cavity 
	Offset is a distance, in meters
	Pitch is the rotation in the Y-S plane.
	(Only Vertical plane considered, needs to be extend to the horizontal..)  
	Roll is the rotation in the X-Y plane. 
	Sign convention is the CHEF one.  
	(Might not be a very helpfull comment!  shame, shame..) 
     */ 
     virtual double getCavVerticalOffset(size_t index) const = 0;    
     virtual double getCavVerticalPitch(size_t index) const = 0;    
     virtual double getQuadVerticalOffset(size_t index) const = 0;    
     virtual double getQuadRoll(size_t index) const = 0;    
     virtual double getVerticalBPMOffset(size_t index) const = 0;    
};
//! \brief Set of offsets used in the Lucretia models.  
/*! A simple data structure to specify the set of "sigma", root mean squares 
   of the anticipated deviations.   
*/
class LucretiaSigmas {
   public:
     //! \brief No argument constructor, this just a public struct. 
     LucretiaSigmas();
     //! vertical offset between nominal line and cryo-module. Units are in meters.   
     double cryoVOffset;
     double cryoHOffset;
     //! \brief Pitch angles, i..e rotation in the vertical plane.    
     double cryoVTilt; 
     double cryoHTilt;
     //! \brief Displacement of quadrupoles with respect to cryo-modules.  
     double quadDispx;
     double quadDispy;
     //! \brief Displacement of BPMs with respect to cryo-modules.  
     double bpmDispx;
     double bpmDispy;
     //! \brief Quadrupole rolls are rotation angle in the plane perp. to the beam.   
     double quadRollxy;
     //! \brief Displacement of Cavities  with respect to cryo-modules.  
     double cavDispx;
     double cavDispy;
     //! \brief rotation in YS plane for the Linac Cavities (with respect to cryo-modules.)  
     double cavTiltx;
     double cavTilty;
    //! \brief Number of cavities  perfectly aligned upstream 
     int nCavMisStart;
    //! \brief Number of quadrupoles  perfectly aligned upstream 
     int nQuadMisStart;
    //! \brief Sigma cut, if thrown at greater than n sigma, throw again. 
     double sigmaCut; 
    //! \brief Number of cavity per cryo-modules. 
    /*! In principle, could/should be gotten from the model itself.
        CHEF had a too simplistic mis-alignment model to support all these nuances. 
    */ 
     int numCavityPerCryoModules;

};
//! \brief Lucretia vintage Summer 2007 misalignement sets. 
/*! Mis alignment set used in November/December 2007 to do benchmark comparison 
    between Lucretia and CHEF. 
    First, a stand-alone implementation was written, based on private and e-mail discussion 
    of what Lucretia does. In a second phase, we simply exchange data files containing 
    misalignment data. This is what's used 
*/

class LucretiaMisalignSet : public ILCMisalignmentSet {
     /*! Constructor
         \param aSteerUtil  A pointer (a dum one!) to the class Steer Util supporting the Steering
	 process
	 \param misAlignCnst A set of "sigma constant", related to tolerance. See above. 
	 \param v4 Obsolete, most likely. 
     */
     public:
        LucretiaMisalignSet(SteerUtil* aSteerUtil, 
	                    const LucretiaSigmas& misAlignCnst, bool v4=false );
     
     /*! \brief Interface specialization, 
         See interface definition above. 
	 ....
     */
     double getCavVerticalOffset(size_t index) const;    
     double getCavVerticalPitch(size_t index) const;    
     double getQuadVerticalOffset(size_t index) const;    
     double getQuadVerticalPitch(size_t index) const;    
     double getQuadRoll(size_t index) const;    
     double getVerticalBPMOffset(size_t index) const;
     /* \brief A simple ASCII dump of on instance of a simulated beam line 
     */
     void printIt() const;
     /* !\brief Taking the offset, pitches and rolls from a specific Lucretia simulation. 
         
	 The implementation of the Lucretia algorithm might have been buggy, 
	 so a direct way of uploading the misalignemnt data for a specific seed, 
	 taken from a Lucretia simulation, is given here. 
	 \param dirPath : the Unix path name where the files reside. Each set of beamline 
	   elements have their own file. File names are hardcoded. 
	 \param v4 Obsolete.. ( I think.. who knows..)  
     */
     
     bool getFromSolyakFiles(const char* dirPath, bool v4=false);
     /*! \brief Put back the BPM on the nominal line 
     */
     void realignBPMs(); 
     /*! \brief Upload the set of corrector found in the lucretia steering process. 
         Conceptually, this method does not belong here, since it has little to 
	 do with the misalignment model. Yet, since the directory path and 
	 file name convention for magnet setting and dispalcement, place it here.
	 \param  aSt A pointer to the relevant Steering utility. Probably redundant. 
	 \param fact : A rescale factor.  
      */
      bool setSolyakYCor(SteerUtil* aSt, double fact=1.0);
       
     bool setCryoModules();
// Not formally part of the misalignment, additional info 
     double getVerticalCorrVal(size_t index) const;
      
     /*! \brief a Sub class to define the motion within the cryo-module.
     */     
     class cryoModule {
       public:
         bool hasQuad;
         double yOff;
         double yTilt;
	 double sBegin;
	 double sEnd;
         std::vector<LinacCavityPtr> cavPtrs; //for futur info... 
         std::vector<double> sLocs; //for futur info... 
     };
     
     private:
       SteerUtil* mySteerUtilPtr; // copy of a pointer, never owned! 
       LucretiaSigmas mSigmas; 
       std::vector<double> QuadVOffsets; 
       std::vector<double> BPMVOffsets; 
       std::vector<double> CavVOffsets; 
       std::vector<double> CavVTilts; 
       std::vector<double> QuadVTilts; 
       std::vector<double> YCorr; 
       std::vector<cryoModule> cryoMods; 
      // internal stuff 
      void setElements(); 
     public:
     // For ILC2006e problem, use the same algorithm as previously
     inline std::vector<cryoModule>::const_iterator cryoModsBegin() const { 
       return cryoMods.begin(); } 
     inline std::vector<cryoModule>::const_iterator cryoModsEnd() const { 
       return cryoMods.end(); } 
     inline bool areQuadMisaligned() const { return (QuadVOffsets.size() > 0);} 
         
};
/*!  \brief Alignment model used in 2006 studies, RDR version 
 *  
 *  Introduction of additional reference frames: (i) Local monuments 
 * (ii) Cryo-modules 
 * Linear rectification of local monuments (every ~ 50 m.) 
 * Utility Class To define ILC Misalignments. 
*/

class ILC2006eSigmas {
   public:
     ILC2006eSigmas(); // Defaults 
     
     double shaftMonumentVOffset;
     double localMonumentVOffset;
     double distLocalMonuments;
     double distShaftMonuments;
     LucretiaSigmas lucSigmas;

};

class ILC2006eMisalignSet : public ILCMisalignmentSet {
     
     public:
        ILC2006eMisalignSet(SteerUtil* aSteerUtil, 
	                    const ILC2006eSigmas& misAlignCnst );
	
     // With respect to the nominal beam line. 
     // Implementation of Abstract class above.  
     public: 
     double getCavVerticalOffset(size_t index) const;    
     double getCavVerticalPitch(size_t index) const;    
     double getQuadVerticalOffset(size_t index) const;    
     double getQuadRoll(size_t index) const;    
     double getVerticalBPMOffset(size_t index) const;

     class monument {
      public:
       double s;
       double yOff;
     };
     

     class shaft {
      public:
       double s;
       double yOff;
       std::vector<monument> localMonuments;
     };
     
     private:
       
      SteerUtil* mySteerUtilPtr; // copy of a pointer, never owned! 
      ILC2006eSigmas  mSigmas; 
      LucretiaMisalignSet  lucMisSet;   
      std::vector<shaft> shaftMonuments;
       
      // internal stuff 
      void setShaftMonument(); // include local monuments. 
      // For now, we add to the Lucretia the Cryo modules the shaft variation 
      // and monument location uncetainties.  
//      void setCryoModules();
      void setElements(); 
         
};
#endif
     
