#include "beamline.h"
#include "MatrixCalcVisitor.h"

int MatrixCalcVisitor::DONE         =  0;
int MatrixCalcVisitor::UNSTABLE     = -1;
int MatrixCalcVisitor::INTEGER_TUNE = -2;
int MatrixCalcVisitor::PHASE_ERROR  = -3;


MatrixCalcVisitor::MatrixCalcVisitor( const Particle& x )
: _numberOfElements(0), 
  _firstTime(1),
  _calcDone(0),
  _map_h(2,2), 
  _map_v(2,2),
  _counter(0), 
  _myParticle(&x),
  _myBeamlinePtr(0),
  _linearModel_h(0),
  _linearModel_v(0),
  _beta_h(0),
  _beta_v(0),
  _alpha_h(0),
  _alpha_v(0),
  _psi_h(0),
  _psi_v(0)
{
  _map_h(0,0) = 1.0;    _map_h(0,1) = 0.0;
  _map_h(1,0) = 0.0;    _map_h(1,1) = 1.0;
  _map_v(0,0) = 1.0;    _map_v(0,1) = 0.0;
  _map_v(1,0) = 0.0;    _map_v(1,1) = 1.0;
}


MatrixCalcVisitor::MatrixCalcVisitor( const MatrixCalcVisitor& x )
{
  cerr << "ERROR: MatrixCalcVisitor copy constructor should not be called." 
       << endl;
  exit(1);
}

MatrixCalcVisitor::~MatrixCalcVisitor()
{
  if( _myParticle ) delete _myParticle;
  if( _numberOfElements > 0 ) {
    for( int i = 0; i < _numberOfElements; i++ ) {
      delete _linearModel_h[i];
      delete _linearModel_v[i];
    }
    delete [] _linearModel_h;
    delete [] _linearModel_v;
  }
  if( _beta_h )  delete [] _beta_h;
  if( _beta_v )  delete [] _beta_v;
  if( _alpha_h ) delete [] _alpha_h;
  if( _alpha_v ) delete [] _alpha_v;
  if( _psi_h )   delete [] _psi_h;
  if( _psi_v )   delete [] _psi_v;
}



void MatrixCalcVisitor::visitBeamline( beamline* x )
{
  if( _firstTime ) 
  {
    _myBeamlinePtr    = x;
    _numberOfElements = x->countHowManyDeeply();
    _linearModel_h    = new MatrixD* [_numberOfElements];
    _linearModel_v    = new MatrixD* [_numberOfElements];
    _beta_h           = new double   [_numberOfElements];
    _beta_v           = new double   [_numberOfElements];
    _alpha_h          = new double   [_numberOfElements];
    _alpha_v          = new double   [_numberOfElements];
    _psi_h            = new double   [_numberOfElements];
    _psi_v            = new double   [_numberOfElements];
    _arcLength        = new double   [_numberOfElements];

    for( int i = 0; i < _numberOfElements; i++ ) {
      _linearModel_h[i] = new MatrixD(2,2,0.0);
      _linearModel_v[i] = new MatrixD(2,2,0.0);
    }

    _firstTime = 0;
  }


  if( x == _myBeamlinePtr ) 
  {
    _calcDone = 0;

    _map_h(0,0) = 1.0;    _map_h(0,1) = 0.0;
    _map_h(1,0) = 0.0;    _map_h(1,1) = 1.0;
    _map_v(0,0) = 1.0;    _map_v(0,1) = 0.0;
    _map_v(1,0) = 0.0;    _map_v(1,1) = 1.0;

    DeepBeamlineIterator dbi( x );
    bmlnElmnt* q;
    double s = 0.0;
    _counter = -1;
    while((  q = dbi++  )) {
      _counter++;
      if( _counter < _numberOfElements ) {
  	q->accept( *this );
  	s += q->OrbitLength( *_myParticle );
  	_arcLength[_counter] = s;
      }
      else {
  	cerr << "*** ERROR *** MatrixCalcVisitor::visitBeamline \n"
  		"*** ERROR *** A horrible, inexplicable error has occurred. \n"
                "*** ERROR *** The counter has overrun the number of elements: "
  	     << _counter << " > " << _numberOfElements
  	     << endl;
  	exit(137);
      }
    }
  
    if( _counter + 1 != _numberOfElements ) {
      cerr << "*** ERROR *** MatrixCalcVisitor::visitBeamline \n"
  	      "*** ERROR *** A horrible, inexplicable error has occurred. \n"
           << "*** ERROR *** The count is not correct: "
  	   << _counter << " + 1 != " << _numberOfElements
  	   << endl;
      exit(137);
    }
  }
}


void MatrixCalcVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  // The arbitrary elements will act like a drift.
  static double lng;
  lng = x->Length();

  (*(_linearModel_h[_counter]))(0,0) = 1.0;    
  (*(_linearModel_h[_counter]))(0,1) = lng;
  (*(_linearModel_h[_counter]))(1,1) = 1.0;

  (*(_linearModel_v[_counter]))(0,0) = 1.0;
  (*(_linearModel_v[_counter]))(0,1) = lng;
  (*(_linearModel_v[_counter]))(1,1) = 1.0;
}


void MatrixCalcVisitor::visitMarker( marker* x )
{
  // The matrices are unit matrices; 
  // i.e., nothing happens.

  (*(_linearModel_h[_counter]))(0,0) = 1.0;    
  (*(_linearModel_h[_counter]))(0,1) = 0.0;
  (*(_linearModel_h[_counter]))(1,0) = 0.0;
  (*(_linearModel_h[_counter]))(1,1) = 1.0;

  (*(_linearModel_v[_counter]))(0,0) = 1.0;
  (*(_linearModel_v[_counter]))(0,1) = 0.0;
  (*(_linearModel_v[_counter]))(1,0) = 0.0;
  (*(_linearModel_v[_counter]))(1,1) = 1.0;
}


void MatrixCalcVisitor::visitDrift( drift* x )
{
  static double lng;
  lng = x->Length();

  (*(_linearModel_h[_counter]))(0,0) = 1.0;    
  (*(_linearModel_h[_counter]))(0,1) = lng;
  (*(_linearModel_h[_counter]))(1,1) = 1.0;

  (*(_linearModel_v[_counter]))(0,0) = 1.0;
  (*(_linearModel_v[_counter]))(0,1) = lng;
  (*(_linearModel_v[_counter]))(1,1) = 1.0;
}


void MatrixCalcVisitor::visitSbend( sbend* x )
{
  static double theta;
  static double rho;
  static double cs;
  static double sn;
  static double lng;
  
  lng   = x->Length();
  rho   = _myParticle->ReferenceBRho() / x->Strength();
  theta = lng / rho;
  cs    = cos( theta );
  sn    = sin( theta );

  (*(_linearModel_h[_counter]))(0,0) = cs;
  (*(_linearModel_h[_counter]))(0,1) = rho*sn;
  (*(_linearModel_h[_counter]))(1,0) = - sn/rho;
  (*(_linearModel_h[_counter]))(1,1) = cs;

  (*(_linearModel_v[_counter]))(0,0) = 1.0;
  (*(_linearModel_v[_counter]))(0,1) = lng;
  (*(_linearModel_v[_counter]))(1,1) = 1.0;
}


void MatrixCalcVisitor::visitRbend( rbend* x )
{
  cerr << "Sorry: MatrixCalcVisitor::visitRbend is not written."
       << endl;
  exit(1);
}


void MatrixCalcVisitor::visitSector( sector* x )
{
  cerr << "*** ERROR ***                              \n"
          "*** ERROR *** MatrixCalcVisitor::visitSector   \n"
          "*** ERROR *** Sorry. Sectors not allowed.  \n"
          "*** ERROR ***                              \n"
       << endl;
  exit(1);
}


void MatrixCalcVisitor::visitQuadrupole( quadrupole* x )
{
  static double theta;
  static double kappa;
  static double cs;
  static double sn;
  static double csh;
  static double snh;
  
  kappa = sqrt( fabs( x->Strength() / _myParticle->ReferenceBRho() ) );
  theta = x->Length() * kappa;
  cs    = cos ( theta );
  sn    = sin ( theta );
  csh   = cosh( theta );
  snh   = sinh( theta );

  if( x->Strength() > 0.0 ) {
    (*(_linearModel_h[_counter]))(0,0) = cs;
    (*(_linearModel_h[_counter]))(0,1) = sn/kappa;
    (*(_linearModel_h[_counter]))(1,0) = - kappa*sn;
    (*(_linearModel_h[_counter]))(1,1) = cs;

    (*(_linearModel_v[_counter]))(0,0) = csh;
    (*(_linearModel_v[_counter]))(0,1) = snh/kappa;
    (*(_linearModel_v[_counter]))(1,0) = - kappa*snh;
    (*(_linearModel_v[_counter]))(1,1) = csh;
  }
  else {
    (*(_linearModel_v[_counter]))(0,0) = cs;
    (*(_linearModel_v[_counter]))(0,1) = sn/kappa;
    (*(_linearModel_v[_counter]))(1,0) = - kappa*sn;
    (*(_linearModel_v[_counter]))(1,1) = cs;

    (*(_linearModel_h[_counter]))(0,0) = csh;
    (*(_linearModel_h[_counter]))(0,1) = snh/kappa;
    (*(_linearModel_h[_counter]))(1,0) = - kappa*snh;
    (*(_linearModel_h[_counter]))(1,1) = csh;
  }
}


void MatrixCalcVisitor::visitJetQuadrupole( JetQuadrupole* x )
{
  cerr << "Sorry: MatrixCalcVisitor::visitJetQuadrupole is not written."
       << endl;
  exit(1);
}


void MatrixCalcVisitor::visitThinQuad( thinQuad* x )
{
  static double kappa;  // = 1/f
  kappa = x->Strength() / _myParticle->ReferenceBRho();
  
  (*(_linearModel_h[_counter]))(0,0) = 1.0;    
  (*(_linearModel_h[_counter]))(1,0) = - kappa;
  (*(_linearModel_h[_counter]))(1,1) = 1.0;

  (*(_linearModel_v[_counter]))(0,0) = 1.0;
  (*(_linearModel_v[_counter]))(1,0) = kappa;
  (*(_linearModel_v[_counter]))(1,1) = 1.0;
}


void MatrixCalcVisitor::visitSlot( Slot* x )
{
  cerr << "Sorry: MatrixCalcVisitor::visitSlot is not written."
       << endl;
  exit(1);
}


void MatrixCalcVisitor::visitCF_rbend( CF_rbend* x )
{
  cerr << "Sorry: MatrixCalcVisitor::visitCF_rbend is not written."
       << endl;
  exit(1);
}


void visitCombinedFunction( combinedFunction* x )
{
  cerr << "Sorry: MatrixCalcVisitor::visitcombinedFunction is not written."
       << endl;
  exit(1);
}



void MatrixCalcVisitor::getState( Vector& x )
{
  _myParticle->getState( x );
}


void MatrixCalcVisitor::getState( Vector* x )
{
  _myParticle->getState( x );
}


void MatrixCalcVisitor::getState( double* x )
{
  _myParticle->getState( x );
}

Vector MatrixCalcVisitor::State()
{
  return _myParticle->State();
}


int MatrixCalcVisitor::_doCalc()
{
  int i;
  for( i = 0; i < _numberOfElements; i++ ) {
    _map_h = (*(_linearModel_h[i]))*_map_h;
    _map_v = (*(_linearModel_v[i]))*_map_v;
  }

  // ==========================================================
  // This is more or less cloned from LattFuncSage::Fast_CS_Calc
  // 

  short int i_x  = 0,
            i_px = 1,
            i_y  = 0,
            i_py = 1;

  // Calculate initial lattice functions ...
  // ... first horizontal
  double alpha_x, beta_x;

  double sn;
  double cs = ( _map_h( i_x, i_x ) + _map_h( i_px, i_px ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( _map_h( i_x, i_px ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                             sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    cerr << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** cos( psi_H ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::UNSTABLE;
  }

  if( sn == 0.0 ) {
    cerr << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::INTEGER_TUNE;
  }

  beta_x  = _map_h( i_x, i_px ) / sn;
  alpha_x = ( _map_h( i_x, i_x ) - _map_h( i_px, i_px ) ) / ( 2.0*sn );


  // ... then vertical.
  double alpha_y, beta_y;
  cs = ( _map_v( i_y, i_y ) + _map_v( i_py, i_py ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( _map_v( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    cerr << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** cos( psi_V ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::UNSTABLE;
  }

  if( sn == 0.0 ) {
    cerr << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::INTEGER_TUNE;
  }

  beta_y  = _map_v( i_y, i_py ) / sn;
  alpha_y = ( _map_v( i_y, i_y ) - _map_v( i_py, i_py ) ) / ( 2.0*sn );


  // Set up the "proton" states ...
  MatrixC w_x(2,1);
  MatrixC w_y(2,1);
  double  dum;
  Complex ci( 0., 1.0 );

  w_x( i_x,  0 ) = dum = sqrt( beta_x );
  w_x( i_px, 0 ) = ( ci - alpha_x ) / dum;
  w_y( i_y,  0 ) = dum = sqrt( beta_y );
  w_y( i_py, 0 ) = ( ci - alpha_y ) / dum;

  MatrixC outState_x(2,1), outState_y(2,1);
  Complex phase;
 
  double psi_x = 0.0;
  double psi_y = 0.0;
  double dpsi_x, dpsi_y;

  // cout << "DGN> number of elements = " << _numberOfElements << endl;
  // cout << "DGN> w_x = \n" << w_x << endl;
  for( int jc = 0; jc < _numberOfElements; jc++ )
  {
    MatrixC w_z;
    w_z = w_x;
    w_x = (*(_linearModel_h[jc])) * w_z;
    w_y = (*(_linearModel_v[jc])) * w_y;
    outState_x = w_x;
    outState_y = w_y;

    // cout << "DGN> M = \n" << (*(_linearModel_h[jc])) << endl;
    // cout << "DGN> w_x = \n" << w_x << endl;
    phase = w_x(i_x,0) / abs( w_x(i_x,0) );
    outState_x = outState_x/phase;
    if(   fabs( imag( outState_x(i_px,0) )*real( outState_x(i_x,0) ) - 1.0 )
        > 0.01 )
    {
  	cerr << "*** ERROR *** Phase error in horizontal plane at index " 
             << jc
             << endl;
  	cerr << "*** ERROR *** imag( outState_x(i_px,0) ) = " 
             << imag( outState_x(i_px,0) ) 
             << endl;
  	cerr << "*** ERROR *** outState_x(i_px,0) = " 
             << outState_x(i_px,0)
             << endl;

  	return MatrixCalcVisitor::PHASE_ERROR;
    }
    dpsi_x = atan2( imag(phase), real(phase) );
    while( dpsi_x < psi_x ) dpsi_x += M_TWOPI;
  
    phase = outState_y(i_y,0) / abs( outState_y(i_y,0) );
    outState_y = outState_y/phase;
    if(   fabs( imag( outState_y(i_py,0) )*real( outState_y(i_y,0) ) - 1.0 )
        > 0.01 )
    {
  	cerr << "*** ERROR *** Phase error in vertical plane at index " 
             << jc
             << endl;
  	cerr << "*** ERROR *** imag( outState_y(i_py,0) ) = " 
             << imag( outState_y(i_py,0) ) 
             << endl;
  	cerr << "*** ERROR *** outState_y(i_py,0) = " 
             << outState_y(i_py,0)
             << endl;

 	return MatrixCalcVisitor::PHASE_ERROR;
    }
    dpsi_y = atan2( imag(phase), real(phase) );
    while( dpsi_y < psi_y ) dpsi_y += M_TWOPI;
  

    // Calculate lattice functions ...
    beta_x  =   real( outState_x(i_x,0)  );
    alpha_x = - real( outState_x(i_px,0) );
    alpha_x *= beta_x;
    beta_x  *= beta_x;
  
    beta_y  =   real( outState_y(i_y,0) );
    alpha_y = - real( outState_y(i_py,0) );
    alpha_y *= beta_y;
    beta_y  *= beta_y;

    _beta_h[jc] = beta_x;
    _beta_v[jc] = beta_y;
    _alpha_h[jc] = alpha_x;
    _alpha_v[jc] = alpha_y;
    _psi_h[jc]   = psi_x;
    _psi_v[jc]   = psi_y;

  }  // End loop on jc ...


  // 
  // ==========================================================  

  _calcDone = 1;
  return MatrixCalcVisitor::DONE;
}



const double* MatrixCalcVisitor::Beta_h() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _beta_h;
}

const double* MatrixCalcVisitor::Beta_v() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _beta_v;
}

const double* MatrixCalcVisitor::Alpha_h() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _alpha_h;
}

const double* MatrixCalcVisitor::Alpha_v() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _alpha_v;
}

const double* MatrixCalcVisitor::Psi_h() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _psi_h;
}

const double* MatrixCalcVisitor::Psi_v() {
  static int ret;
  if( !_calcDone ) ret = this->_doCalc();
  if( ret < 0 )  return (const double*) ret;
  else           return _psi_v;
}

