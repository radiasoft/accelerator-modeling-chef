#include <physics_toolkit/normalFormSage.h>


extern void normalForm( const Mapping& theMapping, /* input */
                 int            maxOrder,   /* input */
                 MatrixC*       Bptr,
                 CLieOperator*  N,
			CLieOperator*  T ) ;



// -----------------------------
// Implementation: class normalFormSage
// -----------------------------

normalFormSage::normalFormSage( Mapping & Map, const JetParticle &jpr, int theOrder)
    :   _E(DIM,DIM)
    , _invE(DIM,DIM)
    , _XG(0)
    , _T(0)
    , _f(0)
    , _g(0)
    , _state(DIM)
    , _p_je(0)
    , _order(theOrder)
    , _p_rf( &Map )
    , _canonMap("id")
    , _initialized(false)
    , CanonToChef("id")
    , ChefToCanon("id")
{

  _order = theOrder;

  // initialize _state to 0
  for (int ii=0; ii<DIM; ++ii) {
    _state(ii) = 0.0;
  }
  Jet x, y, t, px, py, de;

  x = jpr.get_x();
  y = jpr.get_y();
  t = jpr.get_cdt();
  px = jpr.get_npx();
  py = jpr.get_npy();
  de = jpr.get_ndp();

  Mapping M;
  
  M.SetComponent( I_X, x );
  M.SetComponent( I_Y, y);
  M.SetComponent( I_T, t);
  M.SetComponent( I_PX, px);
  M.SetComponent( I_PY, py);
  M.SetComponent( I_DE, de);
  
  MatrixD jac(DIM,DIM);
  M = *_p_rf;
  
  jac = M.Jacobian();

  _E = jac.eigenVectors();
  _invE = _E.inverse();

  // initialize the mappings that turn chef coordinates into a
  // symplectic and canonical coordinate basis

  double e0 = jpr.ReferenceEnergy();
  double pc0 = jpr.ReferenceMomentum();
  double pmass = jpr.Mass();
  
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
  realE = de+e0;
  dp = sqrt(realE*realE - pmass*pmass) -  pc0;
  CanonToChef.SetComponent(I_DE, dp/pc0);

  // The combined transformation that we will use for the normal form
  // analysis is the one turn map of a canonical particle.  To get this,
  // apply the maps that turns a canonical particle to a chef particle,
  // one turn map of a chef particle, chef particle to canonical particle.

  _canonMap = ChefToCanon(M(CanonToChef));

  if( _XG ) { delete [] _XG;}
  _XG = new CLieOperator [_order];
  if( _T  ) { delete [] _T;}
  _T  = new CLieOperator [_order];

  normalForm( _canonMap, _order, &_E, _XG, _T );

  _invE = _E.inverse();

  // Construct the transformations
  MappingC id( "ident" );
  if( _f  ) { delete [] _f; }
  _f = new MappingC [ _order - 1 ];
  if( _g  ) { delete [] _g; }
  _g = new MappingC [ _order - 1 ];
  for( int i = 0; i < _order - 1; i++ ) {
    _f[i] = _T[i].expMap(-1.0,id);
    _f[i] = _f[i].filter(0,i+2);
    _g[i] = _T[i].expMap(1.0,id);
    _g[i] = _g[i].filter(0,i+2);
  }

  _initialized = true;
}


normalFormSage::~normalFormSage()
{
  if( _XG ) { delete [] _XG;}
  if( _T  ) { delete [] _T;}
  if( _f  ) { delete [] _f; }
  if( _g  ) { delete [] _g; }
}

// coordinates are assumed to be "sensibly ordered" in the Michelotti sense
// hform are the coordinates in human form input
// nform are the (complex) normal form coordinates (output)
void normalFormSage::cnvDataToNormalForm( const Vector &hform, VectorC &nform )
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

  u = _invE*u;

  for( i = 0; i < _order - 1; i++ ) {
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    // cout << "    before: " << u << endl;
    for( j = 0; j < DIM; j++ ) {
      u(j) = (_f[i](j))(a);
      // u(j) = a[j];
    }
    // cout << "    after: " << u << endl;
  }

  for (i=0; i<DIM; ++i) {
    nform(i) = u(i);
  }
}

// vec are normal form coordinates.  nform(i+3) is the conjugate of nform(i)
void normalFormSage::cnvDataFromNormalForm( const VectorC &nform, Vector& hform )
{
  static MatrixC u(DIM,1);

  for (int i=0; i<DIM; ++i) {
    u(i) = nform(i);
  }

  std::vector<std::complex<double> > a(DIM);
  static int i, j;

  for( i = _order - 2; i >= 0; --i ) {
    // cout << "_cnvDataFromNormalForm: applying _g order " << i << endl;
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    //cout << "    before: " << u << endl;
    for( j = 0; j < DIM; j++ ) {
      u(j) = (_g[i](j))(a);
    }
    // cout << "    after: " << u << endl;
  }

  u = _E*u;

  Vector hsymp(DIM);
  for (i=0; i<DIM; ++i) {
    if (std::abs(imag(u(i))) > 1.0e-8) {
      cout << "error, imaginary part of human form coordinate not 0" << endl;
      cout << u << endl;
    }
    hsymp(i) = real(u(i));
  }

  // transform canonical to chef coordinates
  hform = CanonToChef(hsymp);
}

bool normalFormSage::checkLinearNormalForm()
{
  const double default_toler = 1.0e-8;
  return normalFormSage::checkLinearNormalForm(default_toler);
}

bool normalFormSage::checkLinearNormalForm(double toler)
{
  // _E is the linear normal form matrix.  According to the Theory and
  // Praxis of Linear Normal Forms, when _E is properly constructed,
  // _E' * J * _E = i J, or i * _E' * J * _E * J is the identity
  // matrix.  This routine checks this condition.  Returns true
  // if check works.
  std::complex<double> complex_i(0.0,1.0); // i
  MatrixD J("J", DIM);
  MatrixC NE = (_E.transpose() * J * _E * J) * complex_i;
  bool checkOK = true;

  for (int i=0; i<DIM; ++i) {
    for (int j = 0; j<DIM; ++j) {
      if (i==j) {
	if (std::abs(NE(i,j)-1.0) > toler) {
	  checkOK = false;
	  // std::cout << "Error, element "<< i << "," << j << " of iB'*J*B is not unity: " << NE(i,j) << std::endl;
	}
      } else {
	if (std::abs(NE(i,j)) > toler) {
	  checkOK = false;
	  // std::cout << "Error, element " << i << "," << j << " iB'*J*B is not 0: " << NE(i,j) << std::endl;
	}
      }
    }
  }
  return checkOK;
}

// return the mapping that was used by the sage to contruct the normal form
Mapping  normalFormSage::getCanonMap()
{
  return (_canonMap);
}

const MatrixC& normalFormSage::getNormalEigenvectors()
{
  return _E;
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
      bmom(i,j) = 2.0 * real(_E(i,j) * conj(_E(i,j)));
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

		    
