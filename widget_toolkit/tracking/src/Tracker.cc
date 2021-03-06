/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      Tracker.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Leo Michelotti                                     
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  (PARTIAL) REVISION HISTORY
******  --------------------------
******  May, 2001    michelotti@fnal.gov
******  - Original version: part of AESOP
******  
******  March, 2007  ostiguy@fnal.gov
******  - removed dependencies on dlist/slist
******  support for reference-counted elements and beamlines 
******
******  December, 2009  michelotti@fnal.gov
******  - class Orbit removed to its own files in order to
******  eliminate circular dependencies in widget_toolkit.
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
#include "Orbit.h"
#include "Tracker.h"
#include "TrbWidget.h"
#include "beamline.h"
#include "BeamlineContext.h"

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

// This undef is needed because of the compiler.
// #undef connect

using namespace std;

// ---------------------------------------
// Implementation: class OrbitTransformer
// ---------------------------------------


OrbitTransformer::OrbitTransformer()
: _x_o(0.0),_xp_o(0.0),_y_o(0.0),_yp_o(0.0),_cdt_o(0.0),_dpp_o(0.0)
{}


void RectH::toDynamics( Vector const& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state[Particle::xIndex];
  *yPtr = state[Particle::npxIndex];
  *zPtr = 0.0;
}


void RectH::toState( double a, double b, Particle& ptr ) const
{
  ptr.set_x(a);
  ptr.set_npx(b);
}

void RectV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state[Particle::yIndex];
  *yPtr = state[Particle::npyIndex];
  *zPtr = 0.0;
}


void RectV::toState( double a, double b, Particle& ptr ) const
{
  ptr.set_y(a);
  ptr.set_npy(b);
}


void NormH::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state[Particle::xIndex];
  *yPtr = _alpha*state[Particle::xIndex] + _beta*state[Particle::npxIndex];
  *zPtr = state[Particle::yIndex];  // Somewhat arbitrary. Fix this.
}


void NormH::toState( double a, double b, Particle& ptr ) const
{
  ptr.set_x(a);
  ptr.set_npx(( b - a*_alpha )/_beta);
}


void NormV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  *xPtr = state[Particle::yIndex];
  *yPtr = _alpha*state[Particle::yIndex] + _beta*state[Particle::npyIndex];
  *zPtr = state[Particle::xIndex];  // Somewhat arbitrary. Fix this.
}


void NormV::toState( double a, double b, Particle& ptr ) const
{
  ptr.set_y(a);
  ptr.set_npy(( b - a*_alpha )/_beta);
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
  double u, v;

  // Horizontal
  u = state[Particle::xIndex] - _x_o;
  v = _alphaH*u + _betaH*( state[Particle::npxIndex] - _xp_o );
  *xPtr = ( u*u + v*v )/( 2.0*_betaH );
  
  // Vertical
  u = state[Particle::yIndex] - _y_o;
  v = _alphaV*u + _betaV*( state[Particle::npyIndex] - _yp_o );
  *yPtr = ( u*u + v*v )/( 2.0*_betaV );

  *zPtr = 0.0;
}


void IHIV::toState( double IH, double IV, Particle& particle ) const
{
  double  u     = 0;
  double  v     = 0;
  double  amp   = 0;
  double  phi   = 0;
  double  alpha = 0;
  double  beta  = 0;

  // If orbit has diverged, bring it back.
  if( isnan(particle.get_x()) || isnan(particle.get_npx()) || 
      isnan(particle.get_y()) || isnan(particle.get_npy())    ) 
  {
    particle.set_x  (0.);
    particle.set_y  (0.);
    particle.set_cdt(0.);
    particle.set_npx(0.);
    particle.set_npy(0.);
    particle.set_ndp(0.);
    return;
  }

  // Horizontal
  beta  = _betaH;
  alpha = _alphaH;

  u = particle.get_x() - _x_o;
  v = alpha*u + beta*( particle.get_npx() - _xp_o );
  phi = atan2(u,v);
  if( phi > M_PI ) phi -= M_TWOPI;

  if( IH >= 0.0 ) {
    amp = sqrt(2.0*beta*IH);
    u = amp*sin(phi);
    v = amp*cos(phi);
    particle.set_x( _x_o + u );
    particle.set_npx( _xp_o + (( v - alpha*u )/beta) );
  }
  else {
    particle.set_x( 0. );
    particle.set_npx( 0. );
  }

  // Vertical
  beta  = _betaV;
  alpha = _alphaV;

  u = particle.get_y() - _y_o;
  v = alpha*u + beta*( particle.get_npy() - _yp_o );
  phi = atan2(u,v);
  if( phi > M_PI ) phi -= M_TWOPI;

  if( IV >= 0.0 ) {
    amp = sqrt(2.0*beta*IV);
    u = amp*sin(phi);
    v = amp*cos(phi);
    particle.set_y( _y_o + u );
    particle.set_npy( _yp_o + (( v - alpha*u )/beta) );
  }
  else {
    particle.set_y( 0. );
    particle.set_npy( 0. );
  }
}


void PhiHPhiV::toDynamics( const Vector& state, double* xPtr, double* yPtr, double* zPtr ) const
{
  double u, v, phiH, phiV;

  // Horizontal
  u = state[Particle::xIndex] - _x_o;
  v = _alphaH*u + _betaH*( state[Particle::npxIndex] - _xp_o );
  phiH = atan2(u,v);
  if( phiH > M_PI ) phiH -= M_TWOPI;
  
  // Vertical
  u = state[Particle::yIndex] - _y_o;
  v = _alphaV*u + _betaV*( state[Particle::npyIndex] - _yp_o );
  phiV = atan2(u,v);
  if( phiV > M_PI ) phiV -= M_TWOPI;
  
  *xPtr = phiH;
  *yPtr = phiV;
  *zPtr = 0.0;
}


void PhiHPhiV::toState( double phiH, double phiV, Particle& particle ) const
{
  double u     = 0;
  double v     = 0;
  double amp   = 0;
  double alpha = 0;
  double beta  = 0;

  // If orbit has diverged, bring it back.
  if( isnan(particle.get_x()) || isnan(particle.get_npx()) || 
      isnan(particle.get_y()) || isnan(particle.get_npy())    ) 
  {
    particle.set_x  (0.);
    particle.set_y  (0.);
    particle.set_cdt(0.);
    particle.set_npx(0.);
    particle.set_npy(0.);
    particle.set_ndp(0.);
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

  u = particle.get_x() - _x_o;
  v = alpha*u + beta*( particle.get_npx() - _xp_o );

  amp = sqrt( u*u + v*v );
  u = amp*sin(phiH);
  v = amp*cos(phiH);
  particle.set_x( _x_o + u );
  particle.set_npx( _xp_o + (( v - alpha*u )/beta) );

  // Vertical
  beta  = _betaV;
  alpha = _alphaV;

  u = particle.get_y() - _y_o;
  v = alpha*u + beta*( particle.get_npy() - _yp_o );

  amp = sqrt( u*u + v*v );
  u = amp*sin(phiV);
  v = amp*cos(phiV);
  particle.set_y( _y_o + u );
  particle.set_npy( _yp_o + (( v - alpha*u )/beta) );
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
    _x_o   = u[ Particle::xIndex  ];
    _xp_o  = u[ Particle::npxIndex];
    _y_o   = u[ Particle::yIndex  ];
    _yp_o  = u[ Particle::npyIndex];
    _cdt_o = u[ Particle::cdtIndex];
    _dpp_o = u[ Particle::ndpIndex];
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
  u[ Particle::xIndex  ] = _x_o  ;
  u[ Particle::npxIndex] = _xp_o ;
  u[ Particle::yIndex  ] = _y_o  ;
  u[ Particle::npyIndex] = _yp_o ;
  u[ Particle::cdtIndex] = _cdt_o;
  u[ Particle::ndpIndex] = _dpp_o;
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
  if( _rectContext._transformPtr )   
  { delete _rectContext._transformPtr;
    _rectContext._transformPtr = 0;      
  }
  if( _normContext._transformPtr )   
  { delete _normContext._transformPtr;
    _normContext._transformPtr = 0;      
  }
  if( _actangContext._transformPtr )   
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


void DrawSpace::clearBuffer( const QColor& c )
{
  int myW = this->width();
  int myH = this->height();
  _pic.resize( myW, myH );
  _pic.fill( c );
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
  this->updateGL();
  this->uploadBuffer();
}


void DrawSpace::paintGL()
{

  glClearColor( _rClr, _gClr, _bClr, _aClr );
  glClear( GL_COLOR_BUFFER_BIT );
  glLoadIdentity();
  glOrtho( _xLo, _xHi, _yLo, _yHi, -1., 1. );
  glPointSize(_pointSize);
  glBegin( GL_POINTS );

  const Vector* vec;
  double a, b, c;

  Orbit *q = 0;

  for ( std::list<Orbit*>::iterator orb_it  = _topTracker->orbits_.begin();
                                    orb_it != _topTracker->orbits_.end();
                                    ++orb_it )
  {
    q = *orb_it;
    glColor3f( q->Red(), q->Green(), q->Blue() );

    for ( Orbit::iterator it  = q->begin();
                          it != q->end();
                          ++it ) 
    {
      vec = *it;
      _transformPtr->toDynamics( *vec, &a, &b, &c );
      glVertex3f( a, b, c );
    }
  }

  // Paint one white point
  q = _topTracker->orbits_.empty() ? 0 : _topTracker->orbits_.back();

  if( q ) {
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

        CSLattFuncs const& lf = *(_topTracker->_p_info);
        alphaH = lf.alpha.hor;
        betaH  = lf.beta.hor;
        alphaV = lf.alpha.ver;
        betaV  = lf.beta.ver;

        list<Orbit*> orbits3D;
        Vector z(3);
        const Vector* vec = 0;
        Orbit* orbitPtr = 0;
        Orbit* newOrbitPtr = 0;

        if ( _topTracker->orbits_.empty() ) return;

        for ( std::list<Orbit*>::iterator it  = _topTracker->orbits_.begin(); 
                                          it != _topTracker->orbits_.end(); ++it )
        {
          orbitPtr = *it;
          for ( std::list<Vector*>::iterator oit   = orbitPtr->history_.begin();
                                             oit  != orbitPtr->history_.end(); ++oit ) {
            vec = *oit;
            // Horizontal

            u = (*vec)[0];
            v = alphaH*u + betaH*(*vec)[3];
            z[0] = atan2(u,v);
            if( z[0] > M_PI ) z[0] -= M_TWOPI;
            // Vertical
            u = (*vec)[1];
            v = alphaV*u + betaV*(*vec)[4];
            z[1] = atan2(u,v);
            if( z[1] > M_PI ) z[1] -= M_TWOPI;

            z[2] = 1000.0*sqrt( u*u + v*v );  // ??? Temporary kludge ???
  
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

          orbits3D.push_back( newOrbitPtr );
          newOrbitPtr = 0;
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
        double a, b, c;
        double xc = ( _xLo + _xHi )/2.0;
        double yc = ( _yLo + _yHi )/2.0;

        list<Orbit*> orbits3D;
        Vector z(3);
        const Vector* vec = 0;
        Orbit* orbitPtr = 0;
        Orbit* newOrbitPtr = 0;

        if ( _topTracker->orbits_.empty() ) return;

        for ( std::list<Orbit*>::iterator it  = _topTracker->orbits_.begin(); 
              it != _topTracker->orbits_.end(); ++it )
        {
          orbitPtr = *it;
          for ( std::list<Vector*>::iterator oit   = orbitPtr->history_.begin();
                                             oit  != orbitPtr->history_.end(); ++oit ) {
            vec = *oit;
            _transformPtr->toDynamics( *vec, &a, &b, &c );

            if( _xLo < a && a < _xHi && 
                _yLo < b && b < _yHi    ) 
            {
              z[0] = a - xc;
              z[1] = b - yc;
              z[2] = c;

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

          }

          orbits3D.push_back( newOrbitPtr );
          newOrbitPtr = 0;
        }

        TrbWidget* trbPtr = new TrbWidget( orbits3D );
        // The TrbWidget constructor assumes ownership
        //   of all Orbits in orbits3D. In the process
        //   it empties the list.
        trbPtr->show();

      }
    }
  }
}


void DrawSpace::mouseMoveEvent( QMouseEvent* qme )
{
  int w, h;
  int myW, myH;
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
  double ix, iy;
  double x, y;
  double dx, dy, w;

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
    this->uploadBuffer();
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
  this->uploadBuffer();
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

Tracker::Tracker( BmlContextPtr bmlCP, QWidget* parent, const char* name, WFlags f)
: QVBox(parent, name, f),
  _p_info(0),
  _number(1),
  _myWheel(0.0),
  _bmlConPtr( bmlCP ), 
  _centralParticlePtr(0),
  _isIterating(false), 
  _p_currOrb(0)
  {
  // *** _myWheel.setIncrement( 252.0 );  // = 7*36, will provide ten colors
  _myWheel.setIncrement( 195.0 ); 

  _finishConstructor();
}


Tracker::Tracker( const Particle& prt, BmlPtr x,
                  QWidget* parent, const char* name, WFlags f)
: QVBox(parent, name, f),
  _p_info(0), 
  _number(1),
  _myWheel(0.0),
  _bmlConPtr(), 
  _centralParticlePtr(0),
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
    _bmlConPtr = BmlContextPtr( new BeamlineContext( prt, *x) );
    _finishConstructor();
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
    _p_leftWindow->clearBuffer( QColor( 0, 0, 0 ) );

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
    _p_rightWindow->clearBuffer( QColor( 0, 0, 0 ) );


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

  for ( std::list<Orbit*>::iterator it  = orbits_.begin();  
                                    it != orbits_.end();  ++it ) {
    delete (*it);
  }

}


void Tracker::_file_exit()
{
}


void Tracker::_fileClose()
{
  close();
  // REMOVE: delete this;
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

  for ( std::list<Orbit*>::iterator it  = orbits_.begin();  
                                    it != orbits_.end();  ++it ) {
    delete (*it);
  }
  orbits_.clear();

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
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
}


void Tracker::_view_norm()
{
  if( typeid(*(_p_leftWindow->getTransformer())) == typeid(NormH) ) {
    return;
  }

  if( _p_info ) { delete _p_info; _p_info = 0; }

  Particle psave( _bmlConPtr->getParticle() );

  try {
    _p_info = new CSLattFuncs( _bmlConPtr->getInitial() );
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

  CSLattFuncs const& lf = (*_p_info);
  _p_leftWindow->storeCurrentContext();
  _p_leftWindow->setNormContext();
  if( 0 == _p_leftWindow->getTransformer() ) {
    _p_leftWindow->setTransformer( new NormH( lf.alpha.hor, lf.beta.hor ) );
    _p_leftWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                             - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }
  
  _p_rightWindow->storeCurrentContext();
  _p_rightWindow->setNormContext();
  if( 0 == _p_rightWindow->getTransformer() ) {
    _p_rightWindow->setTransformer( new NormV( lf.alpha.ver, lf.beta.ver ) );
    _p_rightWindow->setRange( - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE, 
                              - DrawSpace::DEF_RANGE, DrawSpace::DEF_RANGE );
  }

  if( _centralParticlePtr ) {
    _p_leftWindow->setCenterOn( *_centralParticlePtr );
    _p_rightWindow->setCenterOn( *_centralParticlePtr );
  }

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();

   // Reset before exiting
  _bmlConPtr->setParticle(psave);
}


void Tracker::_view_actang()
{
  if( typeid(*(_p_leftWindow->getTransformer())) == typeid(IHIV) ) {
    return;
  }

  if( _p_info ) { delete _p_info; _p_info = 0; }

  try {
    _p_info = new CSLattFuncs( _bmlConPtr->getInitial() );
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

  CSLattFuncs const& lf = (*_p_info);
  double a1 = lf.alpha.hor;
  double b1 = lf.beta.hor;
  double a2 = lf.alpha.ver;
  double b2 = lf.beta.ver;

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
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
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
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
}


void Tracker::_view_zoom_in()
{
  _p_leftWindow->multScaleBy( 2.0 );
  _p_rightWindow->multScaleBy( 2.0 );

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
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
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
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
  if( _centralParticlePtr ) { delete _centralParticlePtr; _centralParticlePtr = 0; }
  _centralParticlePtr = (_bmlConPtr->getParticle()).Clone();

  _p_leftWindow->setCenterOn( *_centralParticlePtr );
  _p_rightWindow->setCenterOn( *_centralParticlePtr );

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
}


void Tracker::_opt_largePoints()
{
  _p_leftWindow ->setPointSize( 3 );
  _p_rightWindow->setPointSize( 3 );

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
}


void Tracker::_opt_smallPoints()
{
  _p_leftWindow ->setPointSize( 1 );
  _p_rightWindow->setPointSize( 1 );

  _p_leftWindow->updateGL();
  _p_leftWindow->uploadBuffer();
  _p_rightWindow->updateGL();
  _p_rightWindow->uploadBuffer();
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

    Particle* dummyPtr = (_bmlConPtr->getParticle()).Clone();

    Vector w(6);                                    // ??? Change this
    // w(0) = (_bmlConPtr->getParticle()).State(0); // ??? Change this
    // w(3) = (_bmlConPtr->getParticle()).State(3); // ??? Change this
    w = dummyPtr->State();


    #if 0
    const uint order = 5;  // ??? Change this
    const uint ul    = 5;  // ??? Change this
    #endif
    #if 0
    const uint order = 7;   // ??? Change this
    const uint ul    = 10;  // ??? Change this
    #endif
    #if 1
    const uint order = 1;   // ??? Change this
    const uint ul    = 5;  // ??? Change this
    #endif
  
    Vector z(w);
  
    Jet__environment_ptr storedEnv = Jet__environment::getLastEnv();
    // REMOVE: double energy = dummyPtr->Energy();

    JetParticle* jpPtr = 0;
    try 
    { for( unsigned int iterCount = 0; iterCount < ul; iterCount++ ) {
        Jet__environment::BeginEnvironment( order );
          coord  x(w[0]),  y(w[1]),  q(0.0),
                px(w[3]), py(w[4]), qq(0.0);
        Jet__environment::EndEnvironment();
  
        dummyPtr->State() = w;
        jpPtr = new JetParticle(*dummyPtr);
        Mapping stuff;
        cout << "Begin n-th Henon iterate for n = "
             << iterate
             << endl;
        for( unsigned int i = 0;  i< iterate; i++ ) {
          cout << "No." << (i+1) << endl;
          _bmlConPtr->propagate( *jpPtr );
        }
        cout << "End n-th Henon iterate" << endl;
  
        stuff = jpPtr->State();
  
        MatrixD M(6,6), N(4,4);
        int k [] = { 0, 1, 3, 4 };

        M = stuff.jacobian();
        for( int i = 0; i < 4; i++ ) {
          for( int j = 0; j < 4; j++ ) {
            N[i][j] = M[k[i]][ k[j] ];
          }
        }
        for( int i = 0; i < 4; i++ ) { N[i][i] -= 1.0; }

        N = N.inverse();

        for( int i = 0; i < 4; i++ ) {
          for( int j = 0; j < 4; j++ ) {
            M[k[i]][ k[j] ] = N[i][j];
          }
        }
        for( int i = 0; i < 6; i++ ) {
          M[i][2] = 0.0;
          M[2][i] = 0.0;
          M[i][5] = 0.0;
          M[5][i] = 0.0;
        }

        Vector z(w);
        z = stuff(z);
 
        w = w + M*( w - stuff(w) );
        cout << w << endl;
      }
    }
    catch( const std::exception& ge ) 
    { ostringstream uic;
      uic << __FILE__ << ", line " << __LINE__
          << ": Exception was thrown during computation of periodic orbit."
          << "\nError message was:\n"
          << ge.what();
      QMessageBox::warning( 0, "CHEF: ERROR", uic.str().c_str() );

      if( 0 != dummyPtr ) { delete dummyPtr; dummyPtr = 0; }
      if( 0 != jpPtr    ) { delete jpPtr;    jpPtr    = 0; }
      Jet__environment::setLastEnv(storedEnv);
      delete wpu;
      return;
    }

    // A final test ...
    dummyPtr->State() = w;
    cout << "\nTest the results: " << endl;
    for( int j = 0; j < 10; j++ ) {
      for( unsigned int i = 0; i < iterate; i++ ) {
        _bmlConPtr->propagate(*dummyPtr);
      }
      cout << dummyPtr->State() << endl;
    }


    // Finish and clean up ...
    _bmlConPtr->setParticle(*dummyPtr);

    if( 0 != dummyPtr ) { delete dummyPtr; dummyPtr = 0; }
    if( 0 != jpPtr    ) { delete jpPtr;    jpPtr    = 0; }
    Jet__environment::setLastEnv(storedEnv);
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
        stl.setNum( _bmlConPtr->getParticle().State()[5] );
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

    Particle p =  _bmlConPtr->getParticle();
    p.set_ndp(newdpp); 
    _bmlConPtr->setParticle(p);
  }

  delete wpu;
}


void Tracker::_new_x( double x )
{
  Particle p = _bmlConPtr->getParticle();
  p.set_x(x);
  _bmlConPtr->setParticle(p);
}


void Tracker::_new_xp( double xp )
{
  Particle p = _bmlConPtr->getParticle();
  p.set_npx(xp);
  _bmlConPtr->setParticle(p);
}


void Tracker::_new_y( double y )
{
  Particle p = _bmlConPtr->getParticle();
  p.set_npy(y);
  _bmlConPtr->setParticle(p);
}


void Tracker::_new_yp( double yp )
{
  Particle p = _bmlConPtr->getParticle();
  p.set_npy(yp);
  _bmlConPtr->setParticle(p);
}


void Tracker::_makeNewOrbit()
{
  if( _isIterating ) {
    _p_currOrb = new Orbit( _bmlConPtr->getParticle().State() );
    _myWheel.increment();
    _p_currOrb->setColor( _myWheel.red(), _myWheel.green(), _myWheel.blue() );
    orbits_.push_back( _p_currOrb );
  }
  else {
    _p_currOrb = 0;
  }
}


void Tracker::_cnvFromView( double a, double b, const OrbitTransformer* otPtr )
{
  otPtr->toState( a, b, *_bmlConPtr->particle_ );   // ??? WARNING: VERY DANGEROUS! ???
  _makeNewOrbit();
}


void Tracker::setState( const Vector& s )
{
  if( s.Dim() == 6 ) {
    Particle p = _bmlConPtr->getParticle();
    p.State() = s;
    _bmlConPtr->setParticle(p);
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

  Particle* particle = _bmlConPtr->particle_;

  // ??? WARNING: TWO VERY DANGEROUS LINES! ???

  if( _isIterating ) 
  {
    for( int i = 0; i < _number; i++ ) {
      _bmlConPtr->propagate( *particle );
      _p_currOrb->add( particle->State() );
      // REMOVE: history_.add( particle->State() );
    }

    _p_leftWindow->updateGL();
    _p_leftWindow->uploadBuffer();
    _p_rightWindow->updateGL();
    _p_rightWindow->uploadBuffer();

    _p_timer->start( 100, true );
  }
  else {
    _p_timer->stop();

    _p_x_input ->_set_first ( particle->get_x(), particle->get_npx() ); // Probably should
    _p_xp_input->_set_second( particle->get_x(), particle->get_npx() ); // be done by emitting
    _p_y_input ->_set_first ( particle->get_y(), particle->get_npy() ); // a signal of some
    _p_yp_input->_set_second( particle->get_y(), particle->get_npy() ); // sort.
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
