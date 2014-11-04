/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      normalFormSage.cc
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Authors:   Leo Michelotti     michelotti@fnal.gov
******             Eric Stern         egstern@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Sep 2001        michelotti@fnal.gov
******  - original version of this code was written as a graphical
******    demonstration of normal form coordinates at any order
******    in two-dimensional longitudinal phase space.
******  - It used the already existing procedures in
******    physics_toolkit/src/normalForm.cc to generate the
******    operators, which were then used to create the normal
******    form coordinates.
******  - the original demo was called "macdemo.cc," because it was
******    written for Jim Maclachlan. A modified version still survives
******    as widget_toolkit/tests/lpsDemo.cc.
******
******  Sep 2010        michelotti@fnal.gov
******  - procedures in macdemo are encapsulated into a new class 
******    called EquilibriumSage, because its intent was to
******    generate equilibrium distributions using normal form coordinates.
******  - only prototype version written.
******
******  Jan - Feb 2011  michelotti@fnal.gov
******  - prototype EquilbriumSage is generalized to higher dimensions.
******  - name change to normalFormSage and passed on for testing.
******
******  Feb - Apr 2011  egstern@fnal.gov
******  - changed signature of constructor to accept JetProton
******  - transformations to canonical coordinates (x, p_x, y, p_y, -t, E )
******  - tests added to the code. 
******  - utility functions added, such as checkLinearNormalForm
******  - changed normalFormSage class so it does not create jet environment
******  
******  Jan - Feb 2012  egstern@fnal.gov
******  - added signatures to checkLinearNormalForm
******  - repressed warnings about imaginary coordinates
******
******  Jun 2014        michelotti@fnal.gov
******  - changed signature of constructor to eliminate usage of
******    both a JetProton and a Mapping.
******  - modified code in anticipation of need to extend to
******    usage with offset closed orbits (for Alex Macridin).
******
******
**************************************************************************
*************************************************************************/

#include <physics_toolkit/normalFormSage.h>

#define DIM  6  // ??? BAD FORM ???

#define I_X  0  // ??? BAD FORM ???
#define I_Y  1
#define I_T  2
#define I_PX 3
#define I_PY 4
#define I_DE 5

#define THROTTLE 10
#define NAG      1000

extern void normalForm( const Mapping& theMapping, /* input */
                        int            maxOrder,   /* input */
                        MatrixC*       Bptr,
                        CLieOperator*  N,
                        CLieOperator*  T );



// -----------------------------
// Implementation: class normalFormSage
// -----------------------------

normalFormSage::normalFormSage( JetParticle const& jpr, int theOrder )
    : E_(DIM,DIM)
    , invE_(DIM,DIM)
    , XG_(0)
    , T_(0)
    , f_(0)
    , g_(0)
    , CanonToChef("id")
    , ChefToCanon("id")
    , order_(theOrder)
    , canonMap_("id")
{
  Mapping M( jpr.State() );
  MatrixD jac( M.Jacobian() );
  E_ = jac.eigenVectors();
  invE_ = E_.inverse();      // ??? Not necessary here; see below  ???

  // initialize the mappings that turn chef coordinates into a
  // symplectic and canonical coordinate basis

  double e0    = jpr.ReferenceEnergy();
  double pc0   = jpr.ReferenceMomentum();
  double pmass = jpr.Mass();


  // canonMap_ was set to the identity
  // before entering the constructor's body
  // and unchanged since.
  // Thus, these are coordinates.
  // -------------------------
  Jet  x( canonMap_[0] );
  Jet  y( canonMap_[1] );
  Jet  t( canonMap_[2] );
  Jet px( canonMap_[3] );
  Jet py( canonMap_[4] );
  Jet de( canonMap_[5] );  // ??? de ???  WHAT ???

  Jet realP, deltaE;
  // first map chef variables to canonical variables
  // change transverse momenta
  ChefToCanon.SetComponent(I_PX, px*pc0/PH_MKS_c);
  ChefToCanon.SetComponent(I_PY, py*pc0/PH_MKS_c) ;
  // cdt -> -dt
  ChefToCanon.SetComponent(I_T, -t/PH_MKS_c);
  // convert dp/p to deltaE
  realP = (1.0+de)*pc0 ;
  deltaE = sqrt( realP*realP + pmass*pmass ) - e0;
  ChefToCanon.SetComponent(I_DE, deltaE);

  // map from canonical variables to chef variables
  CanonToChef.SetComponent(I_T, -t*PH_MKS_c);
  CanonToChef.SetComponent(I_PX, px*PH_MKS_c/pc0);
  CanonToChef.SetComponent(I_PY, py*PH_MKS_c/pc0);

  // convert deltaE to dp/p.  For the canonical particle, pz is
  //    really deltaE.
  Jet dp, realE;
  realE = de+e0;     // ??? ???
  dp = sqrt(realE*realE - pmass*pmass) -  pc0;
  CanonToChef.SetComponent(I_DE, dp/pc0);

  // The combined transformation that we will use for the normal form
  // analysis is the one turn map of a canonical particle.  To get this,
  // apply the maps that turns a canonical particle to a chef particle,
  // one turn map of a chef particle, chef particle to canonical particle.

  canonMap_ = ChefToCanon(M(CanonToChef));

  if( XG_ ) { delete [] XG_;}
  XG_ = new CLieOperator [order_];
  if( T_  ) { delete [] T_;}
  T_  = new CLieOperator [order_];

  normalForm( canonMap_, order_, &E_, XG_, T_ );

  invE_ = E_.inverse();

  // Construct the transformations
  MappingC id( "ident" );
  if( f_  ) { delete [] f_; }
  f_ = new MappingC [ order_ - 1 ];
  if( g_  ) { delete [] g_; }
  g_ = new MappingC [ order_ - 1 ];
  for( int i = 0; i < order_ - 1; i++ ) {
    f_[i] = T_[i].expMap(-1.0,id);
    f_[i] = f_[i].filter(0,i+2);
    g_[i] = T_[i].expMap(1.0,id);
    g_[i] = g_[i].filter(0,i+2);
  }
}


normalFormSage::~normalFormSage()
{
  if( XG_ ) { delete [] XG_;}
  if( T_  ) { delete [] T_;}
  if( f_  ) { delete [] f_; }
  if( g_  ) { delete [] g_; }
}

// coordinates are assumed to be "sensibly ordered" in the Michelotti sense
// hform are the coordinates in human form input
// nform are the (complex) normal form coordinates (output)
void normalFormSage::cnvDataToNormalForm( Vector const& hform, VectorC &nform )
{
  static MatrixC u(DIM,1);
  std::vector<std::complex<double> > a(DIM);
  static int i, j;

  // first transform to canonical coordinates
  Vector hsymp(DIM);
  hsymp = ChefToCanon(hform);
  Vector hf_test(DIM);
  hf_test = CanonToChef(hsymp);
  for (i=0; i<DIM; ++i) {
    if (abs(hf_test(i) - hform(i)) > 1.0e-6) {
      cout << "cnvDataToNormalForm: CanonToChef error: " << hf_test(i) << " != " << hform(i) << endl;
    }
  }

  for( i = 0; i < DIM; i++ ) {
    u(i) = hsymp(i);
  }

  u = invE_*u;

  for( i = 0; i < order_ - 1; i++ ) {
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    // cout << "    before: " << u << endl;
    for( j = 0; j < DIM; j++ ) {
      u(j) = (f_[i](j))(a);
      // u(j) = a[j];
    }
    // cout << "    after: " << u << endl;
  }

  for (i=0; i<DIM; ++i) {
    nform(i) = u(i);
  }
}

// vec are normal form coordinates.  nform(i+3) is the conjugate of nform(i)
void normalFormSage::cnvDataFromNormalForm( VectorC const& nform, Vector& hform )
{
  static int  nwarnings=0;
  static int  nagthreshold=NAG;
  static int  warningsdecade=0;
  static bool throttled=false;
  
  static MatrixC u(DIM,1);
  for (int i=0; i<DIM; ++i) {
    u(i) = nform(i);
  }

  std::vector<std::complex<double> > a(DIM);
  static int i, j;

  for( i = order_ - 2; i >= 0; --i ) {
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    //cout << "    before: " << u << endl;
    for( j = 0; j < DIM; j++ ) {
      u(j) = (g_[i](j))(a);
    }
    // cout << "    after: " << u << endl;
  }

  u = E_*u;

  Vector hsymp(DIM);
  const double small_thresh = 5.0e-11;

  #if 1
  // This is the test as modified by Eric Stern.
  // ------------------------------------------
  for (i = 0; i < DIM; ++i )
  {
    // imaginary part of u(i) should be small.
    if (std::abs(real(u(i))) > small_thresh) 
    {
      // if the real part is non-zero, the imaginary part should
      // be a small fraction of it.
      if (std::abs(imag(u(i))/real(u(i))) > small_thresh) {
        ++nwarnings;
        if (!throttled) {
          cout << "error, imaginary part of human form coordinate relatively large" << endl;
          cout << u << endl;
        } else if (nwarnings%nagthreshold == 0) { // throttled, but maybe we'll nag
          cout << "error, imaginary part of human form coordinate, " << nwarnings << " times." << endl;
          ++warningsdecade;
          if (warningsdecade == 9) {
            nagthreshold *= 10;
            warningsdecade = 0;
          }
        }
        if (nwarnings >= THROTTLE) {
          throttled = true;
        }
      }
    } else {
      // the absolute value of the real part is small, the imaginary part
      // should be similarly small
      if (std::abs(imag(u(i))) > small_thresh) {
        ++nwarnings;
        if (!throttled) {
          cout << "error, real and imaginary parts of human form coordinate both real and similarly small" << endl;
          cout << u << endl;
        } else if (nwarnings%nagthreshold == 0) { // throttled, but maybe we'll nag
          cout << "error, imaginary part of human form coordinate, " << nwarnings << " times." << endl;
          ++warningsdecade;
          if (warningsdecade == 9) {
            nagthreshold *= 10;
            warningsdecade = 0;
          }
        }
        if (nwarnings >= THROTTLE) {
          throttled = true;
        }
      }
    }
      
    hsymp(i) = real(u(i));
  }
  // -------------------------------------
  #endif
  #if 0
  // Here is the original form of the test
  // -------------------------------------
  for (i=0; i<DIM; ++i) {
    if (std::abs(imag(u(i))) > 1.0e-8) {
      cout << "error, imaginary part of human form coordinate not 0" << endl;
      cout << u << endl;
    }
    hsymp(i) = real(u(i));
  }
  // -------------------------------------
  #endif


  // transform canonical to chef coordinates
  hform = CanonToChef(hsymp);
}


bool normalFormSage::checkLinearNormalForm()
{
  const double default_toler = 1.0e-8;
  return normalFormSage::checkLinearNormalForm(default_toler);
}


bool normalFormSage::checkLinearNormalForm( double toler )
{
  // E_ is the linear normal form matrix.  According to the Theory and
  // Praxis of Linear Normal Forms, when E_ is properly constructed,
  // E_' * J * E_ = i J, or i * E_' * J * E_ * J is the identity
  // matrix.  This routine checks this condition.  Returns true
  // if check works.

  std::complex<double> complex_i(0.0,1.0); // i
  MatrixD J("J", DIM);
  MatrixC NE = (E_.transpose() * J * E_ * J) * complex_i;
  bool checkOK = true;

  cout << "DGN: " << __FILE__ << "  " << __LINE__
       << ": printing NE" << '\n'
       << NE
       << '\n' << endl;

  for (int i=0; i<DIM; ++i) {
    for (int j = 0; j<DIM; ++j) {
      if (i==j) {
        if (std::abs(NE(i,j)-1.0) > toler) {
          checkOK = false;
        }
      } else {
        if (std::abs(NE(i,j)) > toler) {
          checkOK = false;
        }
      }
    }
  }

  return checkOK;
}


// return the mapping that was used by the sage to contruct the normal form
Mapping  normalFormSage::getCanonMap()
{
  return (canonMap_);
}


const MatrixC& normalFormSage::getNormalEigenvectors()
{
  return E_;
}


// use the linear part of the mapping to find the mean actions that result in
//   a distribution of given second moments.  
std::vector<double> normalFormSage::stationaryActions(const double stdx, const double stdy, const double stdz)
{
  MatrixD bmom(3,3);
  int i,j;
  // matrix bmom(i,j) = 2*|B(i,j)|**2
  for (i=0; i<3; ++i) {
    for (j=0; j<3; ++j) {
      bmom(i,j) = 2.0 * real(E_(i,j) * conj(E_(i,j)));
    }
  }

  MatrixD inv_bmom(3,3);
  inv_bmom = bmom.inverse();

  VectorD moments(3);
  moments(0) = stdx*stdx;
  moments(1) = stdy*stdy;
  moments(2) = stdz*stdz;
  
  VectorD mact(3);
  mact = inv_bmom * moments;

  std::vector<double> meanActions(3);
  for (i=0; i<3; ++i) {
    meanActions[i] = mact(i);
  }

  return meanActions;
}
