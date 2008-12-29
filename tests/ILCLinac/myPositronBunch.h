#ifndef _h_myPositronBunch
#define _h_myPositronBunch 1
#include <vector>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/ParticleBunch.h>
#include <beamline/beamline.h>

typedef boost::uniform_real<double>
        basUnifGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> >
        varUnifGen;
typedef boost::normal_distribution<double>
        basGaussGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> >
        varGaussGen;


using std::vector;
//! \brief Twiss Parameters for the Injected beam 
/*! 
	
  Just a plain struct to hold the Twiss parameters for the injected bunch. 
  A similar data struct will or is implemented in CHEF in the context 
  of the XSIF parser There "MAD-like" key words that refer to the same 
  qunatities. So this might be obsolete at the next re-write of these examples. 
*/
class myTwissParam {
  public:
/*! 
   Constructor.  No valid data loaded!.  Use direct access.. 
*/
  explicit myTwissParam();
/*!
  Aflag to make sure the user has explicitely loaded the data.. 
  or at least considered it... 
  If not defined,  some of the beam generation routine will flag error 
  messages, or croak.. 
  
  Units are in meters, dor dispersion and beta  
*/
  bool  defined; 
/*!
  Twiss beta, Horizontal Plane   
*/   
  double betaX;
/*!
  Twiss alpha, Horizontal Plane   
*/   
  double alphaX;
/*!
  Twiss beta, Vertical Plane   
*/   
  double betaY;
/*!
  Twiss alpha, Vertical Plane   
*/   
  double alphaY;
/*! 
    Linear Dispersion, vertical plane 
*/ 
  double DY;
/*! 
    Linear Dispersion prime, vertical plane
*/ 
  double DYp;
/*! 
    Linear Dispersion, horizontal plane
*/ 
  double DX;
/*! 
    Linear Dispersion prime, horizontal plane
*/ 
  double DXp;
};

// needed by sort algorithm
// Old code. 
/*
    inline bool operator<(const ParticlePtr p1, const ParticlePtr p2)
{
    return p1->get_cdt() < p2->get_cdt();
}
*/

//! \brief Predicate Class to sort the bunch 
/*!
   Sorting along longitudinal coordinate. 
   Used for the old wake field, or bunch studies. 
   See code for details. 
*/   
class myPositronZPredicate {
  public:

  typedef  Particle const& first_argument_type;
  typedef  Particle const& second_argument_type;
  typedef  bool     result_type;

   bool operator()(Particle const & p1, Particle const & p2) const 
   {
   
     return (p1.get_cdt() < p2.get_cdt());
   } 
};
//! \brief Predicate Class to cut the longitudinal tails
/*! 
   The predicate for truncating the bunch along z. 
   Used to study the longitudinal beam halo, for instance. 
    
*/
class myTruncatorBunchPredicate {
  public:
    explicit myTruncatorBunchPredicate(double zMin, double zMax);
    
  private:
    double _zMin;
    double _zMax;
    
  public:
    bool operator()( Particle const& p) const;
    inline double getZMin() const { return _zMin;}
    inline double getZMax() const { return _zMax;}
};

//! \brief A Positron Bunch for Low Emittance Transport Studies. 
/*!
   Inhereting from the CHEF Particle Bunch class, specialized to 
     Positron.
     
    For Positron only ( no multiple inheritance )
    Some of these utilities might be implemented in CHEF, as the 
    ILC LET studies kept progressing...
    
    Almost no private additional data, except for the nominal bunch charge
    which was not there in older version of CHEF.  
     
*/
class myPositronBunch : public ParticleBunch {
  // Constructor and destructor are the CHEF PositronBunch one, 
  // no changes needed.. 
  public:
  /*! 
    Constructor and destructor are the CHEF PositronBunch one,
     i.e., must define a reference energy. This is the same energy 
     used for registering the beam line.
     Bunch is not yet filled, see generate method.        
  */
  myPositronBunch(double ENominal);
  // Add a method to generate the bunch 
  ~myPositronBunch();
  
  inline void setNominalEnergy(double e) { ENominal = e; }
  inline double getNominalEnergy() {return ENominal;}
  
  // Following a Gaussian Model 
  
  /*! 
    Obsolete interface. 
    
  */
  void generate (MatrixC& EMatched, int nPart, 
                                    double epsX, double epsY, 
                                    double sigCTau, double sigdPoP);
  /*! 
    Current recommended way too generate the bunch.
     Fill a bunch, with Gaussian distribution, 
      no truncation at large excursion. 
    \param aTwiss  A set of Twiss paramters. 
    \param nPart  The number of Pseudo Particle in the bunch. 
    \param epsX  The normalized, invariant horizontal emittance. 
    \param epsY  The normalized, invariant vertical emittance. 
    \param sigCTau  The r.m.s. bunch length 
    \param sigdPoP The r.m.s. of the dP/P distribution.  
  */ 
  void generate (const myTwissParam& aTwiss, int nPart,
                                     double epsX, double epsY,
                                     double sigCTau, double sigdPoP);

  /*!
    Used in the context of Merlin vs CHEF benchmark.  
    Generate by Merlin.  
    Reads a Merlin file .
    \param fName    A Unix file name. 
    \param setLabel : a mnemonic, as, in CHEF, one can name pseudo particles. 
  */ 
  void getFromMerlin(const char *fName, const std::string& selLabel=std::string(""));
  /*!
    Get a bunch from a file generated by dumpItForR method. 
  \param fName    A Unix file name. 
  \param eNominal  The reference energy. 
 */
  void getFromFile(const char *fName, double eNominal);
  /*!
     Set the 6D center of the bunch exactly where it belongs. 
     Upon generating the bunch via random throw following Gaussian distribution,
     the average center of the bunch might not exactly on center.
     We correct for this.  The 2nd order moment of the disribution are not affected, 
     i.e., emittance don't change. 
     \param stateCenter The 6D MYXZPTLK Vector of the phase space center. 
  */
  void shiftCenter(Vector& stateCenter);

  /*!
   Setting of the original Total charge, de-facto defining 
   the charge of one macro particle. Used for Wake fields..
   \param the true number of positrons in the bunch ( ILC default is 2.0e10)
  */
  inline void setOriginalBunchCharge( double numPositrons) 
      {originalTotalCharge = numPositrons;}

  // Sorting by c-tau (third coordinate in phase space. 
  
  /*!
   Sorting the bunch longitudinally. Particle at low cdt (head of the bunch) 
   comes first.
   */  
  void SortByCT();

  /*!
    Truncate away the longitudinal halo..
    Only particle with zMin < cdt < zMax are kept 
    
    \param zmin (in meters) 
    \param zmax (in meters) 
   */  
  
  size_t TruncateZ(double zmin, double zmax);

  /*!
   Fills he state vector.  Used at injection. 
   */  
  void getStateCenter(Vector& stateCenter) const;
  /*!
     Get the reference momentum from the reference energy. 
   */  
  double getReferenceMomentum() const; 
  /*!
  void getStateCenter(Vector& stateCenter) const; 
  // an optimize version to get X and Y centroid
   */  
  /*!
    Statistical mean of the horizontal position of the bunch. 
   */  
  double getXPosition() const;
  /*!
    Statistical mean of the vertical position of the bunch. 
   */  
  double getYPosition() const ;
  /*!
    Tuncated Statistical mean of the longitudinal position of the bunch. 
    \param zL Minimum Z, (maximum leading particle in the bunch)
    \param zM Minimum Z 
   */  
  double getZAverage(double zL, double zM)  const;
  /*!
    Tuncated Statistical mean of the vertical position of the bunch. 
    \param zL Minimum Z, (maximum leading particle in the bunch)
    \param zM Minimum Z 
   */  
  double getYAverage(double zL, double zM)  const;
  /*!
    Tuncated Statistical mean of the normalized vertical momentum of the bunch. 
    \param zL Minimum Z, (maximum leading particle in the bunch)
    \param zM Maximum Z 
   */  
  double getYpAverage(double zL, double zM) const ;
  /*!
    Statistical mean of the normalized horizontal momentum of the bunch. 
   */  
  double getXPrime() const;
  /*!
    Statistical mean of the normalized vertical momentum of the bunch. 
   */  
  double getYPrime() const ;
  /*!
    Statistical mean of the Z distribution.
   */  
  double getZPosition() const ;
  /*!
    Statistical mean of the actual momentum of the pseudo particle
   */  
  double getAverageRealMomentum() const ;
  /*!
    Computes the linear vertical dispersion within the bunch, 
     given by the average linear correlation factor between y and dP/P 
    \param zL Minimum Z, (maximum leading particle in the bunch)
    \param zM Maximum Z 
    \param yYprimOpt  Take (or not) into account the correlation between y and yPrime
       Obsolete.. leave it to false.  
   */  
  double getDispersionWithin(double zL, double zM, bool yYprimOpt= false) const;
  
  /*!
    Compute the normalized vertical emittance. 
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    \param yYprimOpt  Take (or not) into account the correlation between y and yPrime
       Obsolete.. leave it to false.  
      
   */  
  double getVerticalProjectedEmittance(bool yYprimOpt = false) const ; // in m.Rad
  /*!
    Compute the normalized Horizontal emittance,   
   */  
  double getHorizontalProjectedEmittance() const;
  /*!
    Compute the normalized vertical emittance. 
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane).
    \param yYprimOpt  Take (or not) into account the correlation between y and yPrime
       Obsolete.. leave it to false.  
    
   */  
  double getVerticalNoDispEmittance(bool yYprimOpt = false)  const;  // remove dispersion  
  /*!
    Compute the normalized vertical emittance,
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane)    
   */  
  double getVerticalNoDispEmittance(double zMin, 
                                    double zMax, bool yYprimeOpt = false) const ;    
  /*!
    Compute the normalized vertical emittance,
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane). Unlike the previous method, the dispersion is not extracted 
    from the bunch, but set by argument.
    \param The assumed dispersion. ( in meters) 
    \param yYprimOpt  Take (or not) into account the correlation between y and yPrime
       Obsolete.. leave it to false.  
   */  
  double getVerticalNoDispEmittance(double dispersion, 
                                     bool yYprimeOpt = false) const;  // remove dispersion  
  /*!
    Compute the normalized horizontal emittance,
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane).  No longitudinal cuts ( hysterical reasons)     
   */  
  double getHorizontalNoDispEmittance() const;  // remove dispersion  
  /*!
    Compute the normalized 4D (x,y,x',y') emittance,
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane).  No longitudinal cuts ( hysterical reasons)     
   */  
  double getVerticalNoDispEmittance(double disp, double dispP, double zMin, 
                                    double zMax) const ;    
  /*!
    Compute the normalized 4D (x,y,x',y') emittance,
    Given by the determinant of the 2nd order moments of the y and y' distribution. 
    These distribution are corrected for dispersion (i.e., rotated in the y dP/P plane, 
    and y' dP/P plane).  No longitudinal cuts ( hysterical reasons)     
   */  
  double get4DNoDispEmittance(double zMin, 
                                    double zMax) const ;
  /*!
  /*!
    Compute the normalized 6D  emittance, with zcut 
    \param zL Minimum Z, (maximum leading particle in the bunch)
    \param zM Maximum Z 
    \param noDisp  Ignore.. irrelevant. 
   */  
  double get6DEmittance(double zMin, 
                                    double zMax, bool noDisp=true) const ;
				        
  /*!
    The r.m.s. of the bunhc, along longitudinal direction. 
   */  
  double getSigma(size_t iCoord) const ;
  
  /*!
    Dump the bunch onto an ASCII file for analysis in R 
    \param fileName A unix file name 
   */  
  void dumpItForR(const char *fileName) const;
  /*!
    Dump the bunch onto two 2D GIF images, y, y',  for visualization. 
    \param tokenName  o make Unix file names. 
    \param yMax  Maximum scale, representation between +- yMax 
    \param ypMax  Maximum scale, and in y' +- yMax 
    \param numPixelsY  Number of Pixels along Y 
    \param numPixelsYp  Number of Pixels along Y'
   */  
  double dumpItForMovie(const std::string& tokenName,
                                   double yMax, double ypMax,
				   short numPixelsY = 320, 
				   short numPixelsYp = 200) const ;
				    
  /*!
    Dump one particle 
    Obsolete and confusing ... Don't use. 
   */  
  void dumpOneParticle(std::ostream& out, double s, ElmPtr q ,
                       const std::string& tag ) ; 
		       // should be const, next release
  /*!
    Dump one particle, Merlin Benchmark... 
    Obsolete and confusing ... Don't use. 
   */  
  void dumpOneParticleMerlinParam(std::ostream& out, double s, ElmPtr q ,
                       const std::string& tag ) ;

  /*!
     Used to compute the Wake fields correctly... 
     Obsolete 
   */  
  inline void setNumMacroParticleAtCreation(size_t n) 
      { numMacroParticleAtCreation = n; }
  /*!
    Return the total charge in the bunch. ( in number of positrons.) 
   */  
  double GetTotalCharge() const;
  
 
  

  private:
  double ENominal; // in GeV 
  double originalTotalCharge; // In number of electrons 
  size_t numMacroParticleAtCreation;

};
#endif // _h_myPositronBunch
