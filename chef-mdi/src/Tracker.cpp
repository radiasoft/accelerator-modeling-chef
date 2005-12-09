/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      Tracker.cc
******  Version:   4.0
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


// ************************************
// 
// File: Tracker.cc
// 
// Author: Leo Michelotti
// Date: May 24, 2001
// 
// ************************************


// Problem: things get drawn over and over unnecessarily 
// in the DrawSpace::draw functions.

#include <fstream>
// #include <ieeefp.h>   // for isnan

#include <qnamespace.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpainter.h>

#include "PointEdit.h"
#include "Tracker.h"
#include "TrbWidget.h"
#include "beamline.h"
#include "BeamlineContext.h"
// #include "EdwardsTeng.h"


// This undef is needed because of the compiler.
// #undef connect

using namespace std;

// ---------------------------
// Implementation: class Orbit
// ---------------------------

Orbit::Orbit( const Vector& x ) 
: _red(1), _green(1), _blue(1)
{
  _history.append( new Vector( x ) );
}

Orbit::Orbit( const Vector* x ) 
: _red(1), _green(1), _blue(1)
{
  _history.append( new Vector( *x ) );
}

Orbit::Orbit( const Orbit& x ) 
: _red(x._red), _green(x._green), _blue(x._blue)
{
  const Vector* v = 0; 
  Orbit::Iterator oit( x );
  while((  v = oit++  )) {
    _history.append( new Vector(*v) );
  }
}

Orbit::~Orbit()
{
  dlist_iterator getNext( _history );
  Vector* q;
  while((  q = (Vector*) getNext()  )) {
    delete q;
  }
}

void Orbit::add( const Vector& x ) 
{
  _history.append( new Vector( x ) );
}

void Orbit::add( const Vector* x ) 
{
  _history.append( new Vector( *x ) );
}

const Vector* Orbit::lastPoint()
{
  return (const Vector*) _history.lastInfoPtr();
}

const Vector* Orbit::firstPoint()
{
  return (const Vector*) _history.firstInfoPtr();
}


void Orbit::setColor( GLdouble r, GLdouble g, GLdouble b )
{
  if( r < 0. )      _red = 0.;
  else if( r > 1. ) _red = 1.;
  else              _red = r;

  if( g < 0. )      _green = 0.;
  else if( g > 1. ) _green = 1.;
  else              _green = g;

  if( b < 0. )      _blue = 0.;
  else if( b > 1. ) _blue = 1.;
  else              _blue = b;
}


// -------------------------------------
// Implementation: class Orbit::Iterator
// -------------------------------------

Orbit::Iterator::Iterator( const Orbit& x )
{
  _getNext = new dlist_iterator( x._history );
}


Orbit::Iterator::Iterator( const Orbit* x )
{
  _getNext = new dlist_iterator( x->_history );
}


Orbit::Iterator::Iterator( const Iterator& x )
{
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    cerr << "*** WARNING ***                                       \n"
            "*** WARNING *** Orbit::Iterator::Iterator( const Iterator& ) \n"
            "*** WARNING *** Copy constructor has been called.     \n"
            "*** WARNING ***                                       \n"
            "*** WARNING *** This message appears once only.       \n"
            "*** WARNING ***                                       \n"
         << endl;
  }
  
  _getNext = new dlist_iterator( *(x._getNext) );
}


Orbit::Iterator::~Iterator()
{
  delete _getNext;
}



const Vector* Orbit::Iterator::operator++( int )
{
  // ??? Should use PhaseSpacePoint rather than Vector.
  return (const Vector*) _getNext->operator()();
}


void Orbit::Iterator::reset()
{
  _getNext->Reset();
}


void Orbit::Iterator::goBack( int n )
{
  _getNext->GoBack(n);
}



// ---------------------------
// Implementation: class OrbitTransformer
// ---------------------------


OrbitTransformer::OrbitTransformer()
: _x_o(0.0),_xp_o(0.0),_y_o(0.0),_yp_o(0.0),_cdt_o(0.0),_dpp_o(0.0)
{
}


void RectH::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state(Particle::_x());
  *yPtr = state(Particle::_xp());
  *zPtr = 0.0;
}


void RectH::toState( double a, double b, Proton* ptrPtr ) const
{
  ptrPtr->set_x(a);
  ptrPtr->set_npx(b);
}

void RectV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state(Particle::_y());
  *yPtr = state(Particle::_yp());
  *zPtr = 0.0;
}


void RectV::toState( double a, double b, Proton* ptrPtr ) const
{
  ptrPtr->set_y(a);
  ptrPtr->set_npy(b);
}


void NormH::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state(Particle::_x());
  *yPtr = _alpha*state(Particle::_x()) + _beta*state(Particle::_xp());
  *zPtr = state(Particle::_y());  // Somewhat arbitrary. Fix this.
}


void NormH::toState( double a, double b, Proton* ptrPtr ) const
{
  ptrPtr->set_x(a);
  ptrPtr->set_npx(( b - a*_alpha )/_beta);
}


void NormV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state(Particle::_y());
  *yPtr = _alpha*state(Particle::_y()) + _beta*state(Particle::_yp());
  *zPtr = state(Particle::_x());  // Somewhat arbitrary. Fix this.
}


void NormV::toState( double a, double b, Proton* ptrPtr ) const
{
  ptrPtr->set_y(a);
  ptrPtr->set_npy(( b - a*_alpha )/_beta);
}


IHIV::IHIV( double a1, double b1, double a2, double b2 )
: _alphaH(a1), _betaH(b1), _alphaV(a2), _betaV(b2)
{
}


IHIV::IHIV( double a1, double b1, double a2, double b2, const Particle& u )
: _alphaH(a1), _betaH(b1), _alphaV(a2), _betaV(b2)
{
  this->set_center( u );
}


IHIV::IHIV()
: _alphaH(0.0), _betaH(1.0), _alphaV(0.0), _betaV(1.0)
{
}


PhiHPhiV::PhiHPhiV( double a1, double b1, double a2, double b2 )
: _alphaH(a1), _betaH(b1), _alphaV(a2), _betaV(b2)
{
}


PhiHPhiV::PhiHPhiV( double a1, double b1, double a2, double b2, const Particle& u )
: _alphaH(a1), _betaH(b1), _alphaV(a2), _betaV(b2)
{
  this->set_center( u );
}


PhiHPhiV::PhiHPhiV()
: _alphaH(0.0), _betaH(1.0), _alphaV(0.0), _betaV(1.0)
{
}


void IHIV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  static double u, v;

  // Horizontal
  u = state(Particle::_x()) - _x_o;
  v = _alphaH*u + _betaH*( state(Particle::_xp()) - _xp_o );
  *xPtr = ( u*u + v*v )/( 2.0*_betaH );
  
  // Vertical
  u = state(Particle::_y()) - _y_o;
  v = _alphaV*u + _betaV*( state(Particle::_yp()) - _yp_o );
  *yPtr = ( u*u + v*v )/( 2.0*_betaV );

  *zPtr = 0.0;
}


void IHIV::toState( double IH, double IV, Proton* protonPtr ) const
{
  static double  u, v, amp, phi;
  static double  alpha, beta;

  // If orbit has diverged, bring it back.
  if( isnan(protonPtr->get_x()) || isnan(protonPtr->get_npx()) || 
      isnan(protonPtr->get_y()) || isnan(protonPtr->get_npy())    ) 
  {
    protonPtr->set_x  (0.);
    protonPtr->set_y  (0.);
    protonPtr->set_cdt(0.);
    protonPtr->set_npx(0.);
    protonPtr->set_npy(0.);
    protonPtr->set_ndp(0.);
    return;
  }

  // Horizontal
  beta  = _betaH;
  alpha = _alphaH;

  u = protonPtr->get_x() - _x_o;
  v = alpha*u + beta*( protonPtr->get_npx() - _xp_o );
  phi = atan2(u,v);
  if( phi > M_PI ) phi -= M_TWOPI;

  if( IH >= 0.0 ) {
    amp = sqrt(2.0*beta*IH);
    u = amp*sin(phi);
    v = amp*cos(phi);
    protonPtr->set_x( _x_o + u );
    protonPtr->set_npx( _xp_o + (( v - alpha*u )/beta) );
  }
  else {
    protonPtr->set_x( 0. );
    protonPtr->set_npx( 0. );
  }

  // Vertical
  beta  = _betaV;
  alpha = _alphaV;

  u = protonPtr->get_y() - _y_o;
  v = alpha*u + beta*( protonPtr->get_npy() - _yp_o );
  phi = atan2(u,v);
  if( phi > M_PI ) phi -= M_TWOPI;

  if( IV >= 0.0 ) {
    amp = sqrt(2.0*beta*IV);
    u = amp*sin(phi);
    v = amp*cos(phi);
    protonPtr->set_y( _y_o + u );
    protonPtr->set_npy( _yp_o + (( v - alpha*u )/beta) );
  }
  else {
    protonPtr->set_y( 0. );
    protonPtr->set_npy( 0. );
  }
}


void PhiHPhiV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  static double u, v, phiH, phiV;

  // Horizontal
  u = state(Particle::_x()) - _x_o;
  v = _alphaH*u + _betaH*( state(Particle::_xp()) - _xp_o );
  phiH = atan2(u,v);
  if( phiH > M_PI ) phiH -= M_TWOPI;
  
  // Vertical
  u = state(Particle::_y()) - _y_o;
  v = _alphaV*u + _betaV*( state(Particle::_yp()) - _yp_o );
  phiV = atan2(u,v);
  if( phiV > M_PI ) phiV -= M_TWOPI;
  
  *xPtr = phiH;
  *yPtr = phiV;
  *zPtr = 0.0;
}


void PhiHPhiV::toState( double phiH, double phiV, Proton* protonPtr ) const
{
  static double u, v, amp;
  static double alpha, beta;

  // If orbit has diverged, bring it back.
  if( isnan(protonPtr->get_x()) || isnan(protonPtr->get_npx()) || 
      isnan(protonPtr->get_y()) || isnan(protonPtr->get_npy())    ) 
  {
    protonPtr->set_x  (0.);
    protonPtr->set_y  (0.);
    protonPtr->set_cdt(0.);
    protonPtr->set_npx(0.);
    protonPtr->set_npy(0.);
    protonPtr->set_ndp(0.);
    return;
  }

  // Check angle domain
  while( phiH >   M_PI ) phiH -= M_TWOPI;
  while( phiH < - M_PI ) phiH += M_TWOPI;
  while( phiV >   M_PI ) phiV -= M_TWOPI;
  while( phiV < - M_PI ) phiV += M_TWOPI;

  // Horizontal
  beta  = _betaH;
  alpha = _alphaH;

  u = protonPtr->get_x() - _x_o;
  v = alpha*u + beta*( protonPtr->get_npx() - _xp_o );

  amp = sqrt( u*u + v*v );
  u = amp*sin(phiH);
  v = amp*cos(phiH);
  protonPtr->set_x( _x_o + u );
  protonPtr->set_npx( _xp_o + (( v - alpha*u )/beta) );

  // Vertical
  beta  = _betaV;
  alpha = _alphaV;

  u = protonPtr->get_y() - _y_o;
  v = alpha*u + beta*( protonPtr->get_npy() - _yp_o );

  amp = sqrt( u*u + v*v );
  u = amp*sin(phiV);
  v = amp*cos(phiV);
  protonPtr->set_y( _y_o + u );
  protonPtr->set_npy( _yp_o + (( v - alpha*u )/beta) );
}


void OrbitTransformer::copyCenterFrom( const OrbitTransformer* u )
{
  _x_o   = u->_x_o;
  _xp_o  = u->_xp_o;
  _y_o   = u->_y_o;
  _yp_o  = u->_yp_o;
  _cdt_o = u->_cdt_o;
  _dpp_o = u->_dpp_o;
}


void OrbitTransformer::copyCenterFrom( const OrbitTransformer& u )
{
  _x_o   = u._x_o;
  _xp_o  = u._xp_o;
  _y_o   = u._y_o;
  _yp_o  = u._yp_o;
  _cdt_o = u._cdt_o;
  _dpp_o = u._dpp_o;
}


void OrbitTransformer::set_center( const Vector& u )
{
  if( 6 == u.Dim() ) {
    _x_o   = u( Particle::_x());
    _xp_o  = u( Particle::_xp());
    _y_o   = u( Particle::_y());
    _yp_o  = u( Particle::_yp());
    _cdt_o = u( Particle::_cdt());
    _dpp_o = u( Particle::_dpop());
  }
  else {
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__
        << "\nvoid OrbitTransformer::set_center( const Vector& u )"
        << "\nArgument u has dimension = "
        << (u.Dim())
        << " != 6."
        << "\nNo action taken";
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
  }
}


void OrbitTransformer::set_center( const Particle& u )
{
  _x_o   = u.get_x();
  _xp_o  = u.get_npx();
  _y_o   = u.get_y();
  _yp_o  = u.get_npy();
  _cdt_o = u.get_cdt();
  _dpp_o = u.get_ndp();
}


Vector OrbitTransformer::get_center() const
{
  Vector u(6);
  u( Particle::_x())    = _x_o  ;
  u( Particle::_xp())   = _xp_o ;
  u( Particle::_y())    = _y_o  ;
  u( Particle::_yp())   = _yp_o ;
  u( Particle::_cdt())  = _cdt_o;
  u( Particle::_dpop()) = _dpp_o;
  return u;
}


void OrbitTransformer::load_center_into( Particle* u ) const
{
  this->load_center_into( *u );
}


void OrbitTransformer::load_center_into( Particle& u ) const
{
  u.set_x(_x_o);
  u.set_npx(_xp_o);
  u.set_y(_y_o);
  u.set_npy(_yp_o);
  u.set_cdt(_cdt_o);
  u.set_ndp(_dpp_o);
}


// -------------------------------
// Implementation: class DrawSpace
// -------------------------------

const double DrawSpace::DEF_RANGE    = 0.002;
const double DrawSpace::DEF_X_CENTER = 0.000;
const double DrawSpace::DEF_Y_CENTER = 0.000;


DrawSpace::DrawSpace( Tracker* p, QHBox* parent, const char* m )
: QGLWidget(parent), _topTracker(p), 
  _transformPtr(0), 
  _pointSize(3),
  _r(1.0), _g(1.0), _b(1.0),
  _isZooming( false ), 
  _zoomActive( false ), 
  _zoomed( false ),
  _xLo( - DEF_RANGE ), 
  _yLo( - DEF_RANGE ), 
  _xHi( DEF_RANGE ), 
  _yHi( DEF_RANGE )

{
  _transformPtr = 0;         // This must be changed in the 
  _currentContextPtr = 0;    // Tracker constructor
  strcpy( _myName, m );
}


DrawSpace::~DrawSpace()
{
  if( 0 != _rectContext._transformPtr )   
  { delete _rectContext._transformPtr;
    _rectContext._transformPtr = 0;      
  }
  if( 0 != _normContext._transformPtr )   
  { delete _normContext._transformPtr;
    _normContext._transformPtr = 0;      
  }
  if( 0 != _actangContext._transformPtr )   
  { delete _actangContext._transformPtr;
    _actangContext._transformPtr = 0;      
  }
}


void DrawSpace::multScaleBy( double x )
{
  double u = fabs(x);
  if( typeid(*_transformPtr) == typeid(IHIV) ) {
    _xLo /= u;
    _xHi /= u;
    _yLo /= u;
    _yHi /= u;
  }
  else if( typeid(*_transformPtr) != typeid(PhiHPhiV) ) {
    double xc = ( _xHi + _xLo )/2.0;
    double yc = ( _yHi + _yLo )/2.0;
    double dx = ( _xHi - _xLo )/2.0;
    double dy = ( _yHi - _yLo )/2.0;
    _xLo = xc - (dx/u);
    _xHi = xc + (dx/u);
    _yLo = yc - (dy/u);
    _yHi = yc + (dy/u);
  }
}


void DrawSpace::setScaleTo( double x )
{
  double u = fabs(x);
  if( typeid(*_transformPtr) != typeid(IHIV) ) {
    double xc = ( _xHi + _xLo )/2.0;
    double yc = ( _yHi + _yLo )/2.0;
    _xLo = xc - u;
    _xHi = xc + u;
    _yLo = yc - u;
    _yHi = yc + u;
  }
  else {
    _xLo = -u;
    _xHi =  u;
    _yLo = -u;
    _yHi =  u;
  }
}


void DrawSpace::setRange( double xl, double xh, double yl, double yh )
{
  _xLo = xl;
  _xHi = xh;
  _yLo = yl;
  _yHi = yh;
}


void DrawSpace::uploadBuffer()
{
  int myW = this->width();
  int myH = this->height();
  _pic.resize( myW, myH );
  bitBlt( &_pic, 0, 0, this, 0, 0, myW, myH, Qt::CopyROP );
}


void DrawSpace::downloadBuffer()
{
  bitBlt( this, 0, 0, &_pic, 0, 0, this->width(), this->height(), Qt::CopyROP );
}


void DrawSpace::setCenterTo( double x, double y )
{
  double xrange = (_xHi - _xLo)/2.0;
  double yrange = (_yHi - _yLo)/2.0;
  _xLo = x - xrange;
  _xHi = x + xrange;
  _yLo = y - yrange;
  _yHi = y + yrange;
}


void DrawSpace::setCenterOn( const Particle& x )
{
  if( (typeid(*_transformPtr) != typeid(IHIV)) && 
      (typeid(*_transformPtr) != typeid(PhiHPhiV))    ) 
  {
    double xc, yc, dummy;
    _transformPtr->toDynamics( const_cast<Particle&>(x).State(), &xc, &yc, &dummy );
    this->setCenterTo( xc, yc );
  }
}


void DrawSpace::setTransformer( OrbitTransformer* x )
{
  // Responsibility for x is handed over to the DrawSpace.
  if( 0 != x ) {
    if( (typeid(*x) == typeid(RectH)) || 
        (typeid(*x) == typeid(RectV))    ) 
    {
      if( _rectContext._transformPtr == 0 ) {
        _rectContext._transformPtr = x;
      }
      else {
        delete _rectContext._transformPtr;
        _rectContext._transformPtr = x;
      }
      _transformPtr = _rectContext._transformPtr;
    }

    else if( (typeid(*x) == typeid(NormH)) ||
             (typeid(*x) == typeid(NormV))    ) 
    {
      if( _normContext._transformPtr == 0 ) {
        _normContext._transformPtr = x;
      }
      else {
        delete _normContext._transformPtr;
        _normContext._transformPtr = x;
      }
      _transformPtr = _normContext._transformPtr;
    }

    else if( (typeid(*x) == typeid(IHIV)) ||
             (typeid(*x) == typeid(PhiHPhiV))    ) 
    {
      if( _actangContext._transformPtr == 0 ) {
        _actangContext._transformPtr = x;
      }
      else {
        delete _actangContext._transformPtr;
        _actangContext._transformPtr = x;
      }
      _transformPtr = _actangContext._transformPtr;
    }
  }
}


void DrawSpace::setColors( GLdouble r, GLdouble g, GLdouble b )
{
  if( 0.0 <= r && r <= 1.0 ) _r = r;
  if( 0.0 <= g && g <= 1.0 ) _g = g;
  if( 0.0 <= b && b <= 1.0 ) _b = b;
}



void DrawSpace::setClearColor( GLclampf r, GLclampf g, GLclampf b, GLclampf a )
{
  _rClr = r;
  _gClr = g;
  _bClr = b;
  _aClr = a;
  updateGL();
}


void DrawSpace::paintGL()
{
  Orbit* q;

  slist_iterator getNext( _topTracker->_orbits );

  glClearColor( _rClr, _gClr, _bClr, _aClr );
  glClear( GL_COLOR_BUFFER_BIT );
  glLoadIdentity();
  glOrtho( _xLo, _xHi, _yLo, _yHi, -1., 1. );
  glPointSize(_pointSize);
  glBegin( GL_POINTS );

  const Vector* vec;
  double a, b, c;
  while( q = (Orbit*) getNext() ) 
  {
    glColor3f( q->Red(), q->Green(), q->Blue() );
    Orbit::Iterator oi( q );
    while((  vec = oi++  )) {
      _transformPtr->toDynamics( *vec, &a, &b, &c );
      glVertex3f( a, b, c );
    }
  }

  // Paint one white point
  q = (Orbit*) ( _topTracker->_orbits ).lastInfoPtr();
  if(q) {
    vec = q->lastPoint();
    glColor3f( 1., 1., 1. );
    _transformPtr->toDynamics( *vec, &a, &b, &c );
    glVertex3f( a, b, c );
  }

  glEnd();
  glFlush();
}


void DrawSpace::mousePressEvent( QMouseEvent* qme )
{
  if( Qt::LeftButton == qme->button() ) { // Left button pressed
    double ix, iy;

    _ixbf = qme->pos().x();
    _iybf = qme->pos().y();

    ix = ((double) _ixbf) / ((double) this->width() );
    iy = ((double) _iybf) / ((double) this->height());

    _xbf = ( _xHi*ix + _xLo*( 1.0 - ix ) );
    _ybf = ( _yLo*iy + _yHi*( 1.0 - iy ) );

    _isZooming = false;

    if( _zoomActive ) {
      emit _startedZoom();
    }
    else {
      if( !(_topTracker->isIterating()) ) {
      	QPen myPen;
      	myPen.setStyle ( Qt::SolidLine );
      	myPen.setColor ( Qt::gray );
      	myPen.setWidth ( 1 );

      	QPainter paint ( this );
      	paint.setPen ( myPen );

      	this->downloadBuffer();
      	paint.drawEllipse (  _ixbf - 3, _iybf - 3, 6, 6 );
      }

      emit _new_point( _xbf, _ybf, _transformPtr );
    }
  }

  else { // Right button pressed
    if( !(_topTracker->isIterating()) ) {
      if( typeid(*_transformPtr) == typeid(PhiHPhiV) ) 
      {
        // These transformations should be done in
        //   a separate routine!!

  	double alphaH, betaH, alphaV, betaV;
  	double u, v;

  	alphaH = ( _topTracker->_p_info->alpha ).hor;
  	betaH  = ( _topTracker->_p_info->beta  ).hor;
  	alphaV = ( _topTracker->_p_info->alpha ).ver;
  	betaV  = ( _topTracker->_p_info->beta  ).ver;

        slist orbits3D;
        slist_iterator getNext( _topTracker->_orbits );
        Vector z(3);
        const Vector* vec = 0;
        Orbit* orbitPtr = 0;
        Orbit* newOrbitPtr = 0;

        if((  0 == (orbitPtr = (Orbit*) getNext())   ))
        { return; }

        while((  0 != orbitPtr )) {
	  Orbit::Iterator oit( orbitPtr );
          vec = oit++;

      	  // Horizontal
      	  u = (*vec)(0);
      	  v = alphaH*u + betaH*(*vec)(3);
      	  z(0) = atan2(u,v);
      	  if( z(0) > M_PI ) z(0) -= M_TWOPI;
      	  // Vertical
      	  u = (*vec)(1);
      	  v = alphaV*u + betaV*(*vec)(4);
      	  z(1) = atan2(u,v);
      	  if( z(1) > M_PI ) z(1) -= M_TWOPI;
          
          z(2) = 1000.0*sqrt( u*u + v*v );  // ??? Temporary kludge ???
  
          newOrbitPtr = new Orbit( z );
          newOrbitPtr->setColor( orbitPtr->Red(), 
                                 orbitPtr->Green(), 
                                 orbitPtr->Blue()   );

          vec = oit++;
          while( 0 != vec ) {
      	    // Horizontal
      	    u = (*vec)(0);
      	    v = alphaH*u + betaH*(*vec)(3);
      	    z(0) = atan2(u,v);
      	    if( z(0) > M_PI ) z(0) -= M_TWOPI;
      	    // Vertical
      	    u = (*vec)(1);
      	    v = alphaV*u + betaV*(*vec)(4);
      	    z(1) = atan2(u,v);
      	    if( z(1) > M_PI ) z(1) -= M_TWOPI;

            z(2) = 1000.0*sqrt( u*u + v*v );  // ??? Temporary kludge ???
  
            newOrbitPtr->add( z );
            vec = oit++;
	  }

          orbits3D.append( newOrbitPtr );
          orbitPtr = (Orbit*) getNext();
	}

        TrbWidget* trbPtr = new TrbWidget( orbits3D );
        // The TrbWidget constructor assumes ownership
        //   of all Orbits in orbits3D. In the process
        //   it empties the list.
        trbPtr->show();
      }

      else if( (typeid(*_transformPtr) == typeid(NormH)) ||
               (typeid(*_transformPtr) == typeid(NormV))    )
      {
        slist orbits3D;
        slist_iterator getNext( _topTracker->_orbits );
        Vector z(3);
        double a, b, c;
        double xc = ( _xLo + _xHi )/2.0;
        double yc = ( _yLo + _yHi )/2.0;
        const Vector* vec = 0;
        Orbit* orbitPtr = 0;
        Orbit* newOrbitPtr = 0;

        if((  0 == (orbitPtr = (Orbit*) getNext())   ))
        { return; }

        while((  0 != orbitPtr )) {
	  Orbit::Iterator oit( orbitPtr );
          vec = oit++;
          while( 0 != vec ) {
            _transformPtr->toDynamics( *vec, &a, &b, &c );

            if( _xLo < a && a < _xHi && 
                _yLo < b && b < _yHi    ) 
            {
              z(0) = a - xc;
              z(1) = b - yc;
              z(2) = c;

              if( !newOrbitPtr ) 
              {
                newOrbitPtr = new Orbit( z );
                newOrbitPtr->setColor( orbitPtr->Red(), 
                                       orbitPtr->Green(), 
                                       orbitPtr->Blue()   );
	      }
              else 
              {
                newOrbitPtr->add( z );
	      }
            }

            vec = oit++;
	  }

          orbits3D.append( newOrbitPtr );
          newOrbitPtr = 0;
          orbitPtr = (Orbit*) getNext();
	}

        TrbWidget* trbPtr = new TrbWidget( orbits3D );
        // The TrbWidget constructor assumes ownership
        //   of all Orbits in orbits3D. In the process
        //   it empties the list.
        trbPtr->show();

        // REMOVE: getNext.Reset(orbits3D);
        // REMOVE: while((  0 != (orbitPtr = (Orbit*) orbits3D.get())   )) {
        // REMOVE:   delete orbitPtr;
	// REMOVE: }
      }
    }
  }
}


void DrawSpace::mouseMoveEvent( QMouseEvent* qme )
{
  static int w, h;
  static int myW, myH;
  static QPixmap pic;

  if( _zoomActive ) {
    myW = this->width();
    myH = this->height();
  
    if( _isZooming ) {
      bitBlt( this, 0, 0, &pic, 0, 0, myW, myH, Qt::CopyROP );
    }
    else {
      pic.resize( myW, myH );
      bitBlt( &pic, 0, 0, this, 0, 0, myW, myH, Qt::CopyROP );
      _isZooming = true;
    }
  
    w = qme->pos().x() - _ixbf;
    h = qme->pos().y() - _iybf;

    QPainter paint( this );
    paint.setPen( Qt::blue );
    paint.drawRect( _ixbf, _iybf, w, h );
  }
}


void DrawSpace::mouseReleaseEvent( QMouseEvent* qme )
{
  static double ix, iy;
  static double x, y;
  static double dx, dy, w;

  if( _isZooming ) 
  {
    ix = ((double)(qme->pos().x())) / ((double) this->width() );
    iy = ((double)(qme->pos().y())) / ((double) this->height());
  
    x = ( _xHi*ix + _xLo*( 1.0 - ix ) );
    y = ( _yLo*iy + _yHi*( 1.0 - iy ) );
  
    // Square up the edges
    dx = fabs( x - _xbf );
    dy = fabs( y - _ybf );
  
    if( dx < dy ) { w = dx; }
    else          { w = dy; }
  
    if( _xbf < x ) { _xLo = _xbf; }
    else           { _xLo = x; }
    _xHi = _xLo + w;
  
    if( _ybf < y ) { _yLo = _ybf; }
    else           { _yLo = y; }
    _yHi = _yLo + w;

    // Finished
    _isZooming = false;
    _zoomActive = false;
    _zoomed = true;

    this->updateGL();
  }
}


void DrawSpace::setPointSize( int x )
{
  _pointSize = x;
}


void DrawSpace::activateZoom()
{
  _zoomActive = true;
}


void DrawSpace::deactivateZoom()
{
  _zoomed     = false;
  _zoomActive = false;
  _isZooming  = false;
}


void DrawSpace::resetZoom()
{
  this->deactivateZoom();
  this->setScaleTo( DEF_RANGE );
  this->setCenterTo( DEF_X_CENTER, DEF_Y_CENTER );
  this->updateGL();
}


void DrawSpace::storeCurrentContext()
{
  _currentContextPtr->_xLo = _xLo;
  _currentContextPtr->_xHi = _xHi;
  _currentContextPtr->_yLo = _yLo;
  _currentContextPtr->_yHi = _yHi;
  _currentContextPtr->_transformPtr = _transformPtr;
}


void DrawSpace::setRectContext()
{
  _currentContextPtr = &_rectContext;
  _xLo = _currentContextPtr->_xLo;
  _xHi = _currentContextPtr->_xHi;
  _yLo = _currentContextPtr->_yLo;
  _yHi = _currentContextPtr->_yHi;
  _transformPtr = _currentContextPtr->_transformPtr;
}


void DrawSpace::setNormContext()
{
  _currentContextPtr = &_normContext;
  _xLo = _currentContextPtr->_xLo;
  _xHi = _currentContextPtr->_xHi;
  _yLo = _currentContextPtr->_yLo;
  _yHi = _currentContextPtr->_yHi;
  _transformPtr = _currentContextPtr->_transformPtr;
}


void DrawSpace::setActAngContext()
{
  _currentContextPtr = &_actangContext;
  _xLo = _currentContextPtr->_xLo;
  _xHi = _currentContextPtr->_xHi;
  _yLo = _currentContextPtr->_yLo;
  _yHi = _currentContextPtr->_yHi;
  _transformPtr = _currentContextPtr->_transformPtr;
}


// -----------------------------
// Implementation: class Tracker
// -----------------------------

Tracker::Tracker( BeamlineContext* bmlCP, QWidget* parent, const char* name, WFlags f)
: QVBox(parent, name, f),
  _p_info(0),
  _number(1),
  _myWheel(0.0),
  _bmlConPtr( bmlCP ), 
  _centralParticlePtr(0),
  _deleteContext( false ), 
  _isIterating(false), 
  _p_currOrb(0)
  {
  // *** _myWheel.setIncrement( 252.0 );  // = 7*36, will provide ten colors
  _myWheel.setIncrement( 195.0 ); 

  this->_finishConstructor();
}


Tracker::Tracker( /* const */ beamline* x,  QWidget* parent, const char* name, WFlags f)
: QVBox(parent, name, f),
  _p_info(0), 
  _number(1),
  _myWheel(0.0),
  _bmlConPtr( 0 ), 
  _centralParticlePtr(0),
  _deleteContext( true ), 
  _isIterating(false), 
  _p_currOrb(0)
{
  if( 0 == x ) {
    QMessageBox::information( 0, "CHEF::Tracker",
                              "Must specify a beamline first." );
  }
  else {
    // *** _myWheel.setIncrement( 252.0 );  // = 7*36, will provide ten colors
    _myWheel.setIncrement( 195.0 ); 
    _bmlConPtr = new BeamlineContext( false, x );
    this->_finishConstructor();
  }
}


void Tracker::_finishConstructor()
{
  // Separate child widgets
  this->setSpacing(5);

  // Construct the timer
  _p_timer = new QTimer( this );

  // Construct the main menu
  QMenuBar*    myMenuPtr = new QMenuBar( this );

    QPopupMenu*  fileMenu  = new QPopupMenu;
    fileMenu->insertItem( "Close", this, SLOT(_fileClose()) );
  myMenuPtr->insertItem( "File", fileMenu );

    QPopupMenu*  editMenu  = new QPopupMenu;
    editMenu->insertItem( "Clear", this, SLOT(_edit_clear()));
    editMenu->insertItem( "Select orbit", this, SLOT(_edit_select()));
  myMenuPtr->insertItem( "Edit", editMenu );
 
    QPopupMenu*  viewMenu  = new QPopupMenu;
    viewMenu->insertItem( "Rectangular", this, SLOT(_view_rect()) );
    viewMenu->insertItem( "Normal", this, SLOT(_view_norm()) );
    viewMenu->insertItem( "Action-angle", this, SLOT(_view_actang()) );
    viewMenu->insertSeparator();
      QPopupMenu*  zoomMenu  = new QPopupMenu;
      zoomMenu->insertItem( "Out (x 2)",  this, SLOT(_view_zoom_out()) );
      zoomMenu->insertItem( "In  (/ 2)",  this, SLOT(_view_zoom_in()) );
      zoomMenu->insertItem( "Specify..." , this, SLOT(_view_zoom_s()) );
      zoomMenu->insertItem( "Mouse", this, SLOT(_view_zoom()) );
      zoomMenu->insertItem( "Reset", this, SLOT(_view_zoom_reset()) );
    viewMenu->insertItem( "Zoom...", zoomMenu );
    viewMenu->insertItem( "Center", this, SLOT(_view_center()) );
  myMenuPtr->insertItem( "View", viewMenu );

    QPopupMenu*  optionMenu = new QPopupMenu;
      QPopupMenu* pointsizeMenu = new QPopupMenu;
      pointsizeMenu->insertItem( "Large", this, SLOT(_opt_largePoints()) );
      pointsizeMenu->insertItem( "Small", this, SLOT(_opt_smallPoints()) );
    optionMenu->insertItem( "Point Size", pointsizeMenu );
    optionMenu->insertItem( "Strobe ...", this, SLOT(_opt_setIter()) );
      QPopupMenu* bgColorMenu = new QPopupMenu;
      bgColorMenu->insertItem( "Black",  this, SLOT(_opt_bg_black()) );
      bgColorMenu->insertItem( "White",  this, SLOT(_opt_bg_white()) );
      bgColorMenu->insertItem( "Yellow", this, SLOT(_opt_bg_yellow()) );
    optionMenu->insertItem( "Background Color", bgColorMenu );
  myMenuPtr->insertItem( "Options", optionMenu );

    QPopupMenu*  toolMenu  = new QPopupMenu;
    toolMenu->insertItem( "Periodic Orbit", this, SLOT(_tool_pdicOrb()));
    toolMenu->insertItem( "Change dp/p", this, SLOT(_tool_dppMod()));
  myMenuPtr->insertItem( "Tools", toolMenu );

  // Construct tracking toggle button; align it to left.
  _p_trackBox = new QHBox( this );
  _p_startBtn = new QPushButton( "Track", _p_trackBox );
  _p_startBtn->setToggleButton( true );
  _p_startBtn->setMaximumWidth( 100 );
  _p_blankLabel = new QLabel( _p_trackBox );
  this->setStretchFactor( _p_blankLabel, 10 );

  // Construct drawing windows ...
  _p_phaseSpaceViews = new QHBox( this );
  _p_phaseSpaceViews->setSpacing(5);

  _p_leftWindow = new DrawSpace( this, _p_phaseSpaceViews, "First" );
    _p_leftWindow->show();
    _p_leftWindow->setRectContext();
    _p_leftWindow->setTransformer( new RectH );
    _p_leftWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                             - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
    _p_leftWindow->setColors( 1., 0., 1. );
    // _p_leftWindow->qglColor( QColor(255,0,255) );
    int drawArea = (25*QApplication::desktop()->height()*
                       QApplication::desktop()->width())/100;
    int fixedWidth = (int) sqrt( (double) drawArea );
    _p_leftWindow->setFixedSize( fixedWidth, fixedWidth );
    // _p_leftWindow->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );

    _p_leftWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
    _p_leftWindow->update();
    _p_leftWindow->uploadBuffer();

  _p_rightWindow = new DrawSpace( this, _p_phaseSpaceViews, "Second" );
    _p_rightWindow->show();
    _p_rightWindow->setRectContext();
    _p_rightWindow->setTransformer( new RectV );
    _p_rightWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                              - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
    _p_rightWindow->setColors( 0., 1., 0. );
    // _p_rightWindow->qglColor( QColor(0,255,0) );
    _p_rightWindow->setFixedSize( fixedWidth, fixedWidth );
    // _p_rightWindow->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );

    _p_rightWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
    _p_rightWindow->update();
    _p_rightWindow->uploadBuffer();


  // Construct text displays
  _p_numberDisplay = new QHBox( this );

  _p_x_label  = new QLabel   ( "  x  ", _p_numberDisplay );
  _p_x_input  = new PointEdit( "0.0"  , _p_numberDisplay );
  _p_xp_label = new QLabel   ( "  x' ", _p_numberDisplay );
  _p_xp_input = new PointEdit( "0.0"  , _p_numberDisplay );
  _p_y_label  = new QLabel   ( "  y  ", _p_numberDisplay );
  _p_y_input  = new PointEdit( "0.0"  , _p_numberDisplay );
  _p_yp_label = new QLabel   ( "  y' ", _p_numberDisplay );
  _p_yp_input = new PointEdit( "0.0"  , _p_numberDisplay );

  _p_x_input ->setMaxLength( 14 );
  _p_xp_input->setMaxLength( 14 );
  _p_y_input ->setMaxLength( 14 );
  _p_yp_input->setMaxLength( 14 );


  // Make connections...
  connect( _p_x_input,  SIGNAL(textChanged( const QString& )),
           _p_x_input,  SLOT(_txtchg( const QString& )) );
  connect( _p_x_input,  SIGNAL(returnPressed()),
           _p_x_input,  SLOT(_retprs()) );
  connect( _p_x_input,  SIGNAL(_new_value( double )),
                 this,  SLOT(_new_x( double )) );

  connect( _p_xp_input, SIGNAL(textChanged( const QString& )),
           _p_xp_input, SLOT(_txtchg( const QString& )) );
  connect( _p_xp_input, SIGNAL(returnPressed()),
           _p_xp_input, SLOT(_retprs()) );
  connect( _p_xp_input, SIGNAL(_new_value( double )),
                  this, SLOT(_new_xp( double )) );

  connect( _p_y_input,  SIGNAL(textChanged( const QString& )),
           _p_y_input,  SLOT(_txtchg( const QString& )) );
  connect( _p_y_input,  SIGNAL(returnPressed()),
           _p_y_input,  SLOT(_retprs()) );
  connect( _p_y_input,  SIGNAL(_new_value( double )),
                 this,  SLOT(_new_y( double )) );

  connect( _p_yp_input, SIGNAL(textChanged( const QString& )),
           _p_yp_input, SLOT(_txtchg( const QString& )) );
  connect( _p_yp_input, SIGNAL(returnPressed()),
           _p_yp_input, SLOT(_retprs()) );
  connect( _p_yp_input, SIGNAL(_new_value( double )),
                  this, SLOT(_new_yp( double )) );

  connect( _p_leftWindow, SIGNAL(_new_point (double,double,const OrbitTransformer*)),
           _p_x_input,    SLOT  (_set_first (double,double)) );
  connect( _p_leftWindow, SIGNAL(_new_point (double,double,const OrbitTransformer*)),
           _p_xp_input,   SLOT  (_set_second(double,double)) );
  connect( _p_rightWindow,SIGNAL(_new_point (double,double,const OrbitTransformer*)),
           _p_y_input,    SLOT  (_set_first (double,double)) );
  connect( _p_rightWindow,SIGNAL(_new_point (double,double,const OrbitTransformer*)),
           _p_yp_input,   SLOT  (_set_second(double,double)) );

  connect( _p_leftWindow, SIGNAL(_new_point  (double,double,const OrbitTransformer*)),
           this,          SLOT  (_cnvFromView(double,double,const OrbitTransformer*)) );
  connect( _p_rightWindow,SIGNAL(_new_point  (double,double,const OrbitTransformer*)),
           this,          SLOT  (_cnvFromView(double,double,const OrbitTransformer*)) );

  connect( _p_leftWindow, SIGNAL(_startedZoom()),
           _p_rightWindow,SLOT  (deactivateZoom()) );
  connect( _p_rightWindow,SIGNAL(_startedZoom()),
           _p_leftWindow, SLOT  (deactivateZoom()) );

  connect( _p_startBtn,   SIGNAL(clicked()),
           this,          SLOT(_start_callback()));

  connect( _p_timer, SIGNAL(timeout()),
           this,     SLOT(_iterate()));
}


Tracker::~Tracker()
{
  if( _p_info ) 
  { delete _p_info; _p_info = 0; }
  if( _centralParticlePtr ) 
  { delete _centralParticlePtr; _centralParticlePtr = 0; }

  delete _p_yp_input;
  delete _p_yp_label;
  delete _p_y_input;
  delete _p_y_label;
  delete _p_xp_input;
  delete _p_xp_label;
  delete _p_x_input;
  delete _p_x_label;
  delete _p_numberDisplay;

  delete _p_rightWindow;
  delete _p_leftWindow;
  delete _p_phaseSpaceViews;
  delete _p_blankLabel;
  delete _p_startBtn;
  delete _p_trackBox;

  Orbit* q;
  while( q = (Orbit*) _orbits.get() ) {
    delete q;
  }

  if(_deleteContext) { delete _bmlConPtr; }
}


void Tracker::_file_exit()
{
}


void Tracker::_fileClose()
{
  close();
}


void Tracker::_do_nothing()
{
  QMessageBox::information( this, "Tracker",
                            "Sorry. This function is not implemented." );
}


void Tracker::_edit_clear()
{
  _isIterating = false;
  _p_currOrb = 0;

  Orbit* q;
  while(( q = (Orbit*) _orbits.get() )) {
    delete q;
  }

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
}


void Tracker::_edit_select()
{
  // Stop iterations if they are occurring
  if( _isIterating ) {
    _p_startBtn->setOn(false); 
    _start_callback();
  }
}


void Tracker::_view_rect()
{
  if( typeid(*(_p_leftWindow->getTransformer())) == typeid(RectH) ) {
    return;
  }

  if( _p_info ) { delete _p_info; _p_info = 0; }

  _p_leftWindow->storeCurrentContext();
  _p_leftWindow->setRectContext();
  if( 0 == _p_leftWindow->getTransformer() ) {
    _p_leftWindow->setTransformer( new RectH );
    _p_leftWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                             - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }
  
  _p_rightWindow->storeCurrentContext();
  _p_rightWindow->setRectContext();
  if( 0 == _p_rightWindow->getTransformer() ) {
    _p_rightWindow->setTransformer( new RectV );
    _p_rightWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                              - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }

  if( _centralParticlePtr ) {
    _p_leftWindow->setCenterOn( *_centralParticlePtr );
    _p_rightWindow->setCenterOn( *_centralParticlePtr );
  }

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_view_norm()
{
  if( typeid(*(_p_leftWindow->getTransformer())) == typeid(NormH) ) {
    return;
  }

  if( _p_info ) { delete _p_info; _p_info = 0; }
  int n = _bmlConPtr->countHowManyDeeply();

  try {
    _p_info = new LattFuncSage::lattFunc( *(_bmlConPtr->getLattFuncPtr(n-1)) );
  }
  catch( const std::exception& ge ) {
    if( _p_info ) { delete _p_info; _p_info = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }

  _p_leftWindow->storeCurrentContext();
  _p_leftWindow->setNormContext();
  if( 0 == _p_leftWindow->getTransformer() ) {
    _p_leftWindow->setTransformer( new NormH( _p_info->alpha.hor, _p_info->beta.hor ) );
    _p_leftWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                             - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }
  
  _p_rightWindow->storeCurrentContext();
  _p_rightWindow->setNormContext();
  if( 0 == _p_rightWindow->getTransformer() ) {
    _p_rightWindow->setTransformer( new NormV( _p_info->alpha.ver, _p_info->beta.ver ) );
    _p_rightWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                              - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }

  if( _centralParticlePtr ) {
    _p_leftWindow->setCenterOn( *_centralParticlePtr );
    _p_rightWindow->setCenterOn( *_centralParticlePtr );
  }

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_view_actang()
{
  if( typeid(*(_p_leftWindow->getTransformer())) == typeid(IHIV) ) {
    return;
  }

  if( _p_info ) { delete _p_info; _p_info = 0; }
  int n = _bmlConPtr->countHowManyDeeply();

  try {
    _p_info = new LattFuncSage::lattFunc( *(_bmlConPtr->getLattFuncPtr(n-1)) );
  }
  catch( const std::exception& ge ) {
    if( _p_info ) { delete _p_info; _p_info = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }

  double a1 = _p_info->alpha.hor;
  double b1 = _p_info->beta.hor;
  double a2 = _p_info->alpha.ver;
  double b2 = _p_info->beta.ver;

  _p_leftWindow->storeCurrentContext();
  _p_leftWindow->setActAngContext();
  if( 0 == _p_leftWindow->getTransformer() ) {
    if( _centralParticlePtr ) {
      _p_leftWindow->setTransformer( new IHIV( a1, b1, a2, b2, 
                                               *_centralParticlePtr ) );
    }
    else {
      _p_leftWindow->setTransformer( new IHIV( a1, b1, a2, b2 ) );
    }
    _p_leftWindow->setRange( 0.0, 1.e-6, 0.0, 1.e-6 );
  }
  
  _p_rightWindow->storeCurrentContext();
  _p_rightWindow->setActAngContext();
  if( 0 == _p_rightWindow->getTransformer() ) {
    if( _centralParticlePtr ) {
      _p_rightWindow->setTransformer( new PhiHPhiV( a1, b1, a2, b2,
                                               *_centralParticlePtr ) );
    }
    else {
      _p_rightWindow->setTransformer( new PhiHPhiV( a1, b1, a2, b2 ) );
    }
    _p_rightWindow->setRange( -M_PI, M_PI, -M_PI, M_PI );
  }

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}



void Tracker::_view_zoom()
{
  _p_leftWindow->activateZoom();
  _p_rightWindow->activateZoom();
}


void Tracker::_view_zoom_out()
{
  _p_leftWindow->multScaleBy( 0.5 );
  _p_rightWindow->multScaleBy( 0.5 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_view_zoom_in()
{
  _p_leftWindow->multScaleBy( 2.0 );
  _p_rightWindow->multScaleBy( 2.0 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}



void Tracker::_view_zoom_s()
{
  double multiplier = 1.0;

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
      //QLabel* qlb = new QLabel( "Multiplier", qhb1 );
        new QLabel( "Multiplier", qhb1 );
        QString stl;
        stl.setNum( 1.0 );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();
    qvb->adjustSize();
  wpu->setCaption( "Setting strobe rate" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stl != qle->text() ) {
      bool ok;
      double m = (qle->text()).toDouble( &ok );
      if( ok ) {
        multiplier = fabs(m);
      }
    }
  }

  delete wpu;

  _p_leftWindow->multScaleBy ( multiplier );
  _p_rightWindow->multScaleBy( multiplier );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}



void Tracker::_view_zoom_reset()
{
  if( _centralParticlePtr ) 
  { delete _centralParticlePtr; _centralParticlePtr = 0; }

  _p_leftWindow->resetZoom();
  _p_rightWindow->resetZoom();
}


void Tracker::_view_center()
{
  if( _centralParticlePtr ) { delete _centralParticlePtr; }
  _centralParticlePtr = _bmlConPtr->_proton.Clone();

  _p_leftWindow->setCenterOn( _bmlConPtr->_proton );
  _p_rightWindow->setCenterOn( _bmlConPtr->_proton );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_opt_largePoints()
{
  _p_leftWindow ->setPointSize( 3 );
  _p_rightWindow->setPointSize( 3 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_opt_smallPoints()
{
  _p_leftWindow ->setPointSize( 1 );
  _p_rightWindow->setPointSize( 1 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void Tracker::_opt_setIter()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
     QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Strobe period", qhb1 );
        QString stl;
        stl.setNum( _number );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();
    qvb->adjustSize();
  wpu->setCaption( "Setting strobe rate" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stl != qle->text() ) {
      bool ok;
      int j = (qle->text()).toInt( &ok );
      if( ok ) {
        _number = j;
      }
    }
  }

  delete wpu;
}


void Tracker::_opt_bg_black()
{
  _p_leftWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
  _p_rightWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
}

void Tracker::_opt_bg_white()
{
  _p_leftWindow->setClearColor( 1.0, 1.0, 1.0, 1.0 );
  _p_rightWindow->setClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void Tracker::_opt_bg_yellow()
{
  _p_leftWindow->setClearColor( 1.0, 1.0, 0.0, 1.0 );
  _p_rightWindow->setClearColor( 1.0, 1.0, 0.0, 1.0 );
}


void Tracker::_tool_pdicOrb()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Period: ", qhb1 );
        QString stl;
        stl.setNum(4);
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( "Period of Orbit" );
  wpu->adjustSize();

  int returnCode = wpu->exec();


  // Calculate the orbit
  // Maybe this belongs in a separate method.
 
  if( returnCode == QDialog::Accepted ) {

    bool ok = true;
    uint iterate = (qle->text()).toInt( &ok );
    if( !ok ) {
      QMessageBox::information( this, 
          "CHEF::Tracker",
          "Sorry: the period was not recognized.\n"
          "Periodic orbit will not be calculated." );
      return;
    }

    Vector w(6);                            // ??? Change this
    // w(0) = _bmlConPtr->_proton.State(0); // ??? Change this
    // w(3) = _bmlConPtr->_proton.State(3); // ??? Change this
    w = _bmlConPtr->_proton.State();

    const uint order = 5;  // ??? Change this
    const uint ul    = 5;  // ??? Change this
  
    Vector z(w);
  
    beamline* bmlPtr = (beamline*) (_bmlConPtr->cheatBmlPtr());
    EnvPtr<double>::Type storedEnv = Jet::_lastEnv;
    double energy = _bmlConPtr->_proton.Energy();

    for( unsigned int iterCount = 0; iterCount < ul; iterCount++ ) {
      Jet__environment::BeginEnvironment( order );
  	coord  x(w(0)),  y(w(1)),  q(0.0),
  	      px(w(3)), py(w(4)), qq(0.0);
      Jet__environment::EndEnvironment();
  
      Jet xsq;
      Jet x1 = x;
      xsq = x1*x1;
  
      JetProton jpr( energy );
      Mapping stuff;
      cout << "Begin n-th Henon iterate for n = "
  	   << iterate
  	   << endl;
      for( unsigned int i = 0; i < iterate; i++ ) {
  	cout << "No." << (i+1) << endl;
  	bmlPtr->propagate( jpr );
      }
      cout << "End n-th Henon iterate" << endl;
  
      stuff = jpr.State();
  
      MatrixD M(6,6);
      M = stuff.Jacobian();

      for( int i = 0; i < 6; i++ ) {
        M(i,2) = 0.0;
        M(2,i) = 0.0;
        M(i,5) = 0.0;
        M(5,i) = 0.0;
      }

      for( int i = 0; i < 6; i++ ) { M( i, i ) -= 1.0; }
      M = M.inverse();
  
      Vector z(w);
      z = stuff(z);
  
      w = w + M*( w - stuff(w) );
  
      cout << w << endl;
    }

    Jet::_lastEnv = storedEnv;

  
    // A final test ...

    _bmlConPtr->_proton.setState(w);
  
    cout << "\nTest the results: " << endl;
  
    for( int j = 0; j < 10; j++ ) {
      for( unsigned int i = 0; i < iterate; i++ ) {
  	bmlPtr->propagate(_bmlConPtr->_proton);
      }
      cout << _bmlConPtr->_proton.State() << endl;
    }

  }

  delete wpu;
}


void Tracker::_tool_dppMod()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "dp/p: ", qhb1 );
        QString stl;
        stl.setNum( _bmlConPtr->_proton.get_ndp() );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( "Period of Orbit" );
  wpu->adjustSize();

  int returnCode = wpu->exec();


  if( returnCode == QDialog::Accepted ) {
    bool ok = true;
    double newdpp = (qle->text()).toDouble( &ok );

    if( !ok ) {
      QMessageBox::information( this, 
          "CHEF::Tracker",
          "Sorry: did not recognize new dp/p." );
      return;
    }

    _bmlConPtr->_proton.set_ndp( newdpp );
  }

  delete wpu;
}


void Tracker::_new_x( double x )
{
  _bmlConPtr->_proton.set_x(x);
}


void Tracker::_new_xp( double xp )
{
  _bmlConPtr->_proton.set_npx(xp);
}


void Tracker::_new_y( double y )
{
  _bmlConPtr->_proton.set_y(y);
}


void Tracker::_new_yp( double yp )
{
  _bmlConPtr->_proton.set_npy(yp);
}


void Tracker::_makeNewOrbit()
{
  if( _isIterating ) {
    _p_currOrb = new Orbit( _bmlConPtr->_proton.State() );
    _myWheel.increment();
    _p_currOrb->setColor( _myWheel.red(), _myWheel.green(), _myWheel.blue() );
    _orbits.append( _p_currOrb );
  }
  else {
    _p_currOrb = 0;
  }
}


void Tracker::_cnvFromView( double a, double b, const OrbitTransformer* otPtr )
{
  otPtr->toState( a, b, &(_bmlConPtr->_proton) );
  _makeNewOrbit();
}


void Tracker::setState( const Vector& s )
{
  if( s.Dim() == 6 ) {
    _bmlConPtr->_proton.setState( s );
  }
  else {
    ostringstream uic;
    uic  << "File: " << __FILE__ << ", Line: " << __LINE__
         << "\nvoid Tracker::setState( const Vector& s )"
         << "\nArgument s has dimension " << (s.Dim()) << " != 6";
    QMessageBox::information( this, "CHEF::Tracker", uic.str().c_str() );
  }
}


void Tracker::_iterate()
{
  static beamline* bmlPtr;
  static Proton*   protonPtr;
  bmlPtr = (beamline*) (_bmlConPtr->cheatBmlPtr());
  protonPtr = &(_bmlConPtr->_proton);

  if( _isIterating ) 
  {
    for( int i = 0; i < _number; i++ ) {
      bmlPtr->propagate( *protonPtr );
      _p_currOrb->add( protonPtr->State() );
      // REMOVE: _history.add( protonPtr->State() );
    }

    _p_leftWindow->updateGL();
    _p_rightWindow->updateGL();

    _p_timer->start( 100, true );
  }
  else {
    _p_timer->stop();

    _p_x_input ->_set_first ( protonPtr->get_x(), protonPtr->get_npx() ); // Probably should
    _p_xp_input->_set_second( protonPtr->get_x(), protonPtr->get_npx() ); // be done by emitting
    _p_y_input ->_set_first ( protonPtr->get_y(), protonPtr->get_npy() ); // a signal of some
    _p_yp_input->_set_second( protonPtr->get_y(), protonPtr->get_npy() ); // sort.
  }
}


void Tracker::_start_callback()
{
  // Reset the colors in the PointEdit objects.
  _p_x_input->unsetPalette();
  _p_x_input->repaint();
  _p_y_input->unsetPalette();
  _p_y_input->repaint();
  _p_xp_input->unsetPalette();
  _p_xp_input->repaint();
  _p_yp_input->unsetPalette();
  _p_yp_input->repaint();
  QApplication::flushX();
  
  _isIterating = !( _isIterating );

  if( _isIterating ) {
    _p_startBtn->setText( "Stop" );
    if( 0 == _p_currOrb ) {
      _makeNewOrbit();
    }
    _p_timer->start( 100, true );
  }
  else {
    _p_leftWindow->uploadBuffer();
    _p_rightWindow->uploadBuffer();
    _p_startBtn->setText( "Track" );
  }
}
