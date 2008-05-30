////////////////////////////////////////////////////////////
// 
// File:          DrawSpace2D.cc
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. May 29, 2008.       (this file)
// 
////////////////////////////////////////////////////////////
// 
// This file is an amalgam drawn from a number of programs going back
// to 2001. (See headers of other files in this directory.  After further
// development, a successor of this file will eventually replace those
// implementations.
// 
////////////////////////////////////////////////////////////

#include <math.h>
#include <basic_toolkit/MathConstants.h>

#include <qpixmap.h>
#include <qpainter.h>

#include <Orbit.h>
#include <Tracker.h>
#include <DrawSpace2D.h>

#define I_PHI 0
#define I_E   1
#define DIM   2

using namespace std;
using namespace AESOP_domain;

// -------------------------------
// Implementation: class DrawSpace2D
// -------------------------------

DrawSpace2D::DrawSpace2D( Tracker* p, QHBox* parent, const char* m )
  : QWidget(parent)
  , _topTracker(p)
  , _r(1.0)
  , _g(1.0)
  , _b(1.0)
  , _rClr(0.0)
  , _gClr(0.0)
  , _bClr(0.0)
  , _xLo( - M_TWOPI )
  , _xHi( M_TWOPI )
  , _yLo( - 0.02 )
  , _yHi( 0.02  )
  , _pointSize(3)
  , _zoomActive( false )
  , _isZooming( false )
  , _zoomed( false )
{
  _myFunc = drawH_ViewRect;  // This must be changed in the 
                             // Tracker constructor
  strcpy( _myName, m );

  // REMOVE: _imago.fill( black );
  _imago.fill( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );

  setBackgroundMode( QWidget::FixedColor );
  // REMOVE: setEraseColor( black );
  setEraseColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  // REMOVE: setBackgroundColor( black );
  setBackgroundColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
}


DrawSpace2D::~DrawSpace2D()
{
  // cout << _myName << " destructor called." << endl;
}


void DrawSpace2D::multScaleBy( double x )
{
  double u = fabs(x);
  if( 0 != u ) {
    double avg = ( _xHi + _xLo )/2.0;
    double d = ( ( _xHi - _xLo )/2.0 )/u;
    _xLo = avg - d;
    _xHi = avg + d;

    avg = ( _yHi + _yLo )/2.0;
    d = ( ( _yHi - _yLo )/2.0 )/u;
    _yLo = avg - d;
    _yHi = avg + d;
  }

  this->redrawOrbits();
}


void DrawSpace2D::setScaleTo( double x )
{
  double u = fabs(x);
  _xLo = -u;
  _xHi =  u;
  _yLo = -u;
  _yHi =  u;
}


void DrawSpace2D::setSize( int w, int h )
{
  _imago.resize(w,h);
  // REMOVE: _imago.fill(black);
  _imago.fill( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  setEraseColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  setBackgroundColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  this->QWidget::setFixedSize(w,h);
}


void DrawSpace2D::mark( double x, double y, QPen* z )
{
  static QPainter localPainter;

  x = this->width()  * (( x - _xLo )/( _xHi - _xLo ));
  y = this->height() * (( _yHi - y )/( _yHi - _yLo ));

  localPainter.begin( this );
  localPainter.setPen( *z );
  localPainter.drawPoint( ((int) x), ((int) y) );
  localPainter.end();

  localPainter.begin( &_imago );
  localPainter.setPen( *z );
  localPainter.drawPoint( ((int) x), ((int) y) );
  localPainter.end();
}


void DrawSpace2D::setDraw( DrawFunc x )
{
  _myFunc = x;
}


void DrawSpace2D::setColors( double r, double g, double b )
{
  if( 0.0 <= r && r <= 1.0 ) _r = r;
  if( 0.0 <= g && g <= 1.0 ) _g = g;
  if( 0.0 <= b && b <= 1.0 ) _b = b;
}



void DrawSpace2D::setClearColor( double r, double g, double b )
{
  _rClr = r;
  _gClr = g;
  _bClr = b;
}


void DrawSpace2D::paintEvent( QPaintEvent* x )
{
  bitBlt ( this, x->rect().topLeft(), &_imago, x->rect(), CopyROP );
  this->QWidget::paintEvent(x);
}


void DrawSpace2D::drawH_ViewRect( DrawSpace2D* dsPtr )
{
  dsPtr->clear();

  QPen localPen;
  std::list<Orbit*>::iterator it = (dsPtr->_topTracker->_orbits).begin();
  while( it != (dsPtr->_topTracker->_orbits).end() ) {
    Orbit* q = *it;
    QColor orbitColor(   int(255.0*q->Red())
                       , int(255.0*q->Green())
                       , int(255.0*q->Blue())  );
    localPen.setColor( orbitColor );
    localPen.setStyle ( SolidLine );
    localPen.setWidth ( 10 );
    for( std::list<Vector*>::iterator iter = (q->_history).begin();
         iter != (q->_history).end();
         ++iter ) {
      Vector* vecPtr = *iter;
      dsPtr->mark( (*vecPtr)(0), (*vecPtr)(1), &localPen );
    }
    ++it;
  }  

  #if 0
  // Paint one white point
  q = dsPtr->_topTracker->_orbits.empty() ? 0 : dsPtr->_topTracker->_orbits.back();
  if(q) { 
    vec = q->lastPoint();
    glColor3f( 1., 1., 1. );
    glVertex3f( (*vec)(I_PHI), (*vec)(I_E), 0.0 );
  }
  #endif
};


void DrawSpace2D::drawH_ViewNorm( DrawSpace2D* dsPtr )
{
  dsPtr->clear();

  QPen localPen;
  std::list<Orbit*>::iterator it = (dsPtr->_topTracker->_orbits).begin();
  while( it != (dsPtr->_topTracker->_orbits).end() ) {
    Orbit* q = *it;
    QColor orbitColor(   int(255.0*q->Red())
                       , int(255.0*q->Green())
                       , int(255.0*q->Blue())  );
    localPen.setColor( orbitColor );
    localPen.setStyle ( SolidLine );
    localPen.setWidth ( 10 );
    for( std::list<Vector*>::iterator iter = (q->_history).begin();
         iter != (q->_history).end();
         ++iter ) {
      Vector vec_copy( *(*iter) );
      dsPtr->_topTracker->cnvDataToNormalForm( vec_copy );
      dsPtr->mark( vec_copy(0), vec_copy(1), &localPen );
    }
    ++it;
  }

  #if 0
  // Paint one white point
  q = dsPtr->_topTracker->_orbits.empty() ? 0 : dsPtr->_topTracker->_orbits.back();
  if(q) { 
    vec = q->lastPoint();
    glColor3f( 1., 1., 1. );
    glVertex3f( (*vec)(I_PHI), (*vec)(I_E), 0.0 );
  }
  #endif
};


void DrawSpace2D::drawH_ViewActAng( DrawSpace2D* dsPtr )
{
  dsPtr->clear();

  QPen localPen;
  std::list<Orbit*>::iterator it = (dsPtr->_topTracker->_orbits).begin();
  double angle, action;
  while( it != (dsPtr->_topTracker->_orbits).end() ) {
    Orbit* q = *it;
    QColor orbitColor(   int(255.0*q->Red())
                       , int(255.0*q->Green())
                       , int(255.0*q->Blue())  );
    localPen.setColor( orbitColor );
    localPen.setStyle ( SolidLine );
    localPen.setWidth ( 10 );
    for( std::list<Vector*>::iterator iter = (q->_history).begin();
         iter != (q->_history).end();
         ++iter ) {
      Vector vec_copy( *(*iter) );
      dsPtr->_topTracker->cnvDataToNormalForm( vec_copy );
      angle = atan2( vec_copy(1), vec_copy(0) );
      action = vec_copy(0)*vec_copy(0) + vec_copy(1)*vec_copy(1);
      dsPtr->mark( angle, action, &localPen );
    }
    ++it;
  }

  #if 0
  // Paint one white point
  q = dsPtr->_topTracker->_orbits.empty() ? 0 : dsPtr->_topTracker->_orbits.back();
  if(q) { 
    vec = q->lastPoint();
    glColor3f( 1., 1., 1. );
    glVertex3f( (*vec)(I_PHI), (*vec)(I_E), 0.0 );
  }
  #endif
};


void DrawSpace2D::mousePressEvent( QMouseEvent* qme )
{
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

      QPainter painter( this );
      painter.setPen ( myPen );

      bitBlt( this, 0, 0, &_imago, 0, 0, this->width(), this->height(), Qt::CopyROP );
      painter.drawEllipse (  _ixbf - 3, _iybf - 3, 6, 6 );
    }
    emit _new_point( _xbf, _ybf );
  }
}


void DrawSpace2D::mouseMoveEvent( QMouseEvent* qme )
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


void DrawSpace2D::mouseReleaseEvent( QMouseEvent* qme )
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
    // _xHi = _xLo + w;
    _xHi = _xLo + dx;
  
    if( _ybf < y ) { _yLo = _ybf; }
    else           { _yLo = y; }
    // _yHi = _yLo + w;
    _yHi = _yLo + dy;

    // Finished
    _isZooming = false;
    _zoomActive = false;
    _zoomed = true;

    this->redrawOrbits();
  }
}


void DrawSpace2D::setPointSize( int x )
{
  _pointSize = x;
}


void DrawSpace2D::setRange( double xl, double xh, double yl, double yh )
{
  _xLo = xl;
  _xHi = xh;
  _yLo = yl;
  _yHi = yh;
  // redrawOrbits() is purposely NOT invoked here.
}


bool DrawSpace2D::ViewIs( DrawFunc df ) 
{
  return (df == _myFunc);
}


void DrawSpace2D::activateZoom()
{
  _zoomActive = true;
}


void DrawSpace2D::deactivateZoom()
{
  _zoomed     = false;
  _zoomActive = false;
  _isZooming  = false;
}


void DrawSpace2D::resetZoom()
{
  this->deactivateZoom();
  if( _myFunc == DrawSpace2D::drawH_ViewRect ) {
    _xLo = - M_TWOPI;
    _xHi =   M_TWOPI;
    _yLo = - 0.02   ;
    _yHi =   0.02   ;
  }
  else if( _myFunc == DrawSpace2D::drawH_ViewNorm ) {
    _xLo = - 0.20;
    _xHi =   0.20;
    _yLo = - 0.20   ;
    _yHi =   0.20   ;
  }

  this->redrawOrbits();
}


void DrawSpace2D::clear()
{
  // REMOVE: _imago.fill(black);
  _imago.fill( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  setEraseColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  setBackgroundColor( QColor( int(255.0*_rClr), int(255.0*_gClr), int(255.0*_bClr) ) );
  this->erase();
}


void DrawSpace2D::redrawOrbits()
{
  _myFunc( this );
}
