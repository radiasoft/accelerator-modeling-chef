/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      SiteViewer.cc
******  Version:   3.2
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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

#include <values.h>
#include <fstream>
#include <iomanip>

#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>

#ifndef MAXFLOAT
#define MAXFLOAT 1.0e30
#endif

#include "SiteViewer.h"
#include "beamline.h"
#include "FramePusher.h"
#include "BeamlineContext.h"
#include "QueryDialog.h"
#include "BeamlineExpressionTree.h"

// This undef is needed because of the compiler.
#undef connect

using namespace std;


// Static global variable
const BoolNullNode SiteViewer::nada;


SiteViewer::SiteViewer( BeamlineContext& bmlCP, QWidget* parent, const char* name, WFlags f)
: 
  QVBox(parent,name,f),
  _x(0), _y(0), _z(0), 
  _xmin(1.0e10), _xmax(-1.0e10),
  _ymin(1.0e10), _ymax(-1.0e10),
  _zmin(1.0e10), _zmax(-1.0e10),
  _bmlConPtr( &bmlCP ), 
  _deleteContext( false ),
  _showStart( false ),
  _element(0),
  _filterPtr(0)
{
  this->_finishConstructor();
}


SiteViewer::SiteViewer( beamline* x, QWidget* parent, const char* name, WFlags f)
: 
  QVBox(parent,name,f),
   _x(0), _y(0), _z(0), 
  _xmin(1.0e10), _xmax(-1.0e10),
  _ymin(1.0e10), _ymax(-1.0e10),
  _zmin(1.0e10), _zmax(-1.0e10),
  _bmlConPtr( 0 ), 
  _deleteContext( true ),
  _showStart( false ),
  _element(0),
  _filterPtr(0)
{
  if( 0 == x ) {
    QMessageBox::information( 0, "CHEF::SiteViewer",
                              "Must specify a beamline first." );
  }
  else {
    _bmlConPtr = new BeamlineContext( false, x );
    this->_finishConstructor();
  }
}


void SiteViewer::_finishConstructor()
{
  // Initialize filterPtr
  _filterPtr = SiteViewer::nada.Clone();

  // Change representation of the beamline
  // from list to array.
  _n = _bmlConPtr->countHowManyDeeply();
  _x = new double [_n];
  _y = new double [_n];
  _z = new double [_n];

  _element = new bmlnElmnt* [_n];

  // Find site coordinates ...
  DeepBeamlineIterator dbi(_bmlConPtr->cheatBmlPtr());
  bmlnElmnt* q;
  FramePusher fp;
  Vector r(3);
  int i = 0;

  while( ( 0 != (q = dbi++) ) && ( i < _n ) ) {
    // The second check, i < _n, 
    // is simply paranoia; 
    // it should not be necessary.

    _element[i] = q;

    q->accept( fp );
    r = fp.getFrame().getOrigin();
    
    // _x[i] = - r(0);
    // _y[i] = r(2);
    // _z[i] = r(1);

    _x[i] = r(0);
    _y[i] = r(2);
    _z[i] = - r(1);

    if( _x[i] < _xmin ) _xmin = _x[i];
    if( _x[i] > _xmax ) _xmax = _x[i];
    if( _y[i] < _ymin ) _ymin = _y[i];
    if( _y[i] > _ymax ) _ymax = _y[i];
    if( _z[i] < _zmin ) _zmin = _z[i];
    if( _z[i] > _zmax ) _zmax = _z[i];

    i++;
  }


  // Construct menu ...
  QMenuBar* myMenuPtr = new QMenuBar( this );

    QPopupMenu* fileMenu  = new QPopupMenu( this );
    fileMenu->insertItem( "Write site coordinates", this, SLOT(_fileSite()) );
    fileMenu->insertSeparator();
    fileMenu->insertItem( "Close", this, SLOT(_fileClose()) );
  myMenuPtr->insertItem( "File", fileMenu );

    _viewMenuPtr = new QPopupMenu( this );
      _id_viewZoom = 
    _viewMenuPtr->insertItem( "Mouse zoom", this, SLOT(_viewZoom()) );
    _viewMenuPtr->insertItem( "Zoom out (x2)", this, SLOT(_viewZoomOut()) );
    _viewMenuPtr->insertItem( "Reset", this, SLOT(_viewReset()) );
    fileMenu->insertSeparator();
    _id_optHilit =
    _viewMenuPtr->insertItem( "Highlight", this, SLOT(_viewHilite()) );
  myMenuPtr->insertItem( "View", _viewMenuPtr );

    _optionMenuPtr = new QPopupMenu( this );
    _id_optShoOrig =
    _optionMenuPtr->insertItem( "Show origin", this, SLOT(_optionOrigin()) );
    // REMOVE: _optionMenuPtr->insertItem( "Highlight sextupoles", this, SLOT(_optionHiltSext()) );
  myMenuPtr->insertItem( "Options", _optionMenuPtr );

  _myGLwindow = new Wndw( this );
  _myGLwindow->show();

  // ------------------------------------------------------
  // From chef-mdi/src/SiteViewer.cpp:
  // _myGLwindow->resize(width(), height());
  // ------------------------------------------------------
  _myGLwindow->setMinimumSize(QSize(  (2*QApplication::desktop()->height())/3
                                    , (2*QApplication::desktop()->height())/3 ));
  this->adjustSize();
  _myGLwindow->setMinimumSize(QSize( 10, 10 ));
  // ------------------------------------------------------
  // Note: _myGLwindow will be
  // deleted automatically by Qt.
}


SiteViewer::~SiteViewer()
{
  if(_x)             { delete [] _x; }
  if(_y)             { delete [] _y; }
  if(_z)             { delete [] _z; }
  if(_element)       { delete [] _element; }
  if(_deleteContext) { delete _bmlConPtr; _bmlConPtr = 0; }
  if(_filterPtr)     { _filterPtr->eliminate(); _filterPtr = 0; }
}


bool SiteViewer::filter( const bmlnElmnt* x ) const
{  
  if( _filterPtr ) { return _filterPtr->evaluate(x); }
  return false;
}


void SiteViewer::_fileSite()
{
  QString fileName = QFileDialog::getSaveFileName(
    QString(_bmlConPtr->name()) + QString("_site.txt"),
    QString("Text Files (*.txt)"),
    0,
    0,
    QString("CHEF: Site Coordinate File") );

  if( fileName.isNull() ) {
    QMessageBox::information( 0, "CHEF::SiteViewer",
                              "Sorry. It didn't work." );
  }

  else {
    int i;
    ofstream outputFileStream( fileName );
    outputFileStream << setiosflags(ios::fixed);
    outputFileStream << setw(18) << "Element"
                     << setw(18) << "Name"
                     << setw(18) << "Azimuth"
                     << setw(18) << "x[m]"
                     << setw(18) << "y[m]"
                     << setw(18) << "z[m]"
                     << endl;
    for( i = 0; i < (6*15); i++ ) {
      outputFileStream << "-";
    }
    outputFileStream << endl;
    
    double az = 0.0;
    double xx = 0.0, yy = 0.0, zz = 0.0;
    double s;
    for( i = 0; i < _n; i++ ) {
      s = 0.0;
      s += (_x[i] - xx)*(_x[i] - xx);
      s += (_y[i] - yy)*(_y[i] - yy);
      s += (_z[i] - zz)*(_z[i] - zz);
      az += sqrt(s);
      xx = _x[i];
      yy = _y[i];
      zz = _z[i];
      outputFileStream << setw(18) << _element[i]->Type() 
                       << setw(18) << _element[i]->Name()
                       << setw(18) << az
                       << setw(18) << setprecision(6) << _x[i]
                       << setw(18) << setprecision(6) << _y[i]
                       << setw(18) << setprecision(6) << _z[i]
                       << endl;
    }

    outputFileStream.close();
  }
}


void SiteViewer::_fileClose()
{
  // REMOVE: delete this;
  close();
}


void SiteViewer::_viewZoom()
{
  bool checked = !( _viewMenuPtr->isItemChecked(_id_viewZoom) );
  _viewMenuPtr->setItemChecked( _id_viewZoom, checked );

  if( checked ) 
  { _myGLwindow->activateZoom(); }
  else 
  { _myGLwindow->deactivateZoom(); }
}


void SiteViewer::_viewZoomOut()
{
  _myGLwindow->zoomOut();
}


void SiteViewer::_viewReset()
{
  _viewMenuPtr->setItemChecked( _id_viewZoom, false );
  _myGLwindow->resetZoom();
}


void SiteViewer::_viewHilite()
{
  bool checked = !( _optionMenuPtr->isItemChecked(_id_optHilit) );
  _optionMenuPtr->setItemChecked( _id_optHilit, checked );
  _myGLwindow->toggleHighlight();

  if( checked ) {
    QueryDialog* qdl = new QueryDialog( 0, 0, false );
    connect( qdl,  SIGNAL(_useThis( const BoolNode& )), 
             this, SLOT  (_optionHilt( const BoolNode& )) );
    qdl->show();
  }
}


void SiteViewer::_optionOrigin()
{
  bool checked = !( _optionMenuPtr->isItemChecked(_id_optShoOrig) );
  _optionMenuPtr->setItemChecked( _id_optShoOrig, checked );

  _showStart = !_showStart;
  this->_myGLwindow->updateGL();
}


void SiteViewer::_optionHilt( const BoolNode& x )
{
  _filterPtr = x.Clone();
  this->_myGLwindow->updateGL();
}


SiteViewer::Wndw::Wndw( SiteViewer* pt)
: QGLWidget( (QWidget*) pt ), 
  _xbf(0.0), _ybf(0.0),
  _isZooming( false ), 
  _zoomActive( false ), 
  _startedZoom( false ),
  _zoomed( false ), 
 _hilights( false )
{
  _parent = pt;
}


SiteViewer::Wndw::~Wndw()
{
}


void SiteViewer::Wndw::paintGL()
{
  int w = this->width();
  int h = this->height();

  if( !_zoomed ) {
    double xRange = _parent->_xmax - _parent->_xmin;
    double yRange = _parent->_ymax - _parent->_ymin;
    double zRange = _parent->_zmax - _parent->_zmin;
    double range = 0.0;
    if( xRange > range ) range = xRange;
    if( yRange > range ) range = yRange;
    if( zRange > range ) range = zRange;
    range *= 1.1;

    if( h < w ) {
      yRange = range;
      xRange = (((double) w)/((double) h))*range;
    }
    else {
      xRange = range;
      yRange = (((double) h)/((double) w))*range;
    }

    _xHi = ( _parent->_xmin + _parent->_xmax + xRange )/2.0;
    _xLo = ( _parent->_xmin + _parent->_xmax - xRange )/2.0;
    _yHi = ( _parent->_ymin + _parent->_ymax + yRange )/2.0;
    _yLo = ( _parent->_ymin + _parent->_ymax - yRange )/2.0;
  }

  bmlnElmnt* q;

  glClearColor( 0, 0, 0, 1 );
  glClear( GL_COLOR_BUFFER_BIT );
  glLoadIdentity();
  glOrtho( _xLo, _xHi, _yLo, _yHi, -1., 1. );
  glViewport( 0, 0, w, h );
  glLineWidth(3);
  glBegin( GL_LINES );

  // The first point ...
  double xx = 0.0, yy = 0.0, zz = 0.0;
  if( _parent->_showStart ) {
    glColor3f( 1., 1., 0. );
    glVertex3f( xx, yy, zz );
    glVertex3f( xx + 0.1*(_xHi - _xLo), yy, zz );
    glVertex3f( xx, yy, zz );
    glVertex3f( xx, yy + 0.1*(_yHi - _yLo), zz );
  }

  // ... the other points.
  for( int i = 0; i < _parent->_n; i++ ) 
  {
    q = _parent->_element[i];
    if( !(0 == strstr(q->Type(), "bend")) ) {
      glColor3f( 0., 1., 0. );
    }
    else if( !(0 == strstr(q->Type(), "sex")) ) {
      glColor3f( 1., 1., 0. );
    }
    else if( !(0 == strstr(q->Type(), "quad")) || 
             !(0 == strstr(q->Type(), "Quad"))     ) {
      glColor3f( 1., 0., 0. );
    }
    else {
      glColor3f( 1., 1., 1. );
    }

    glVertex3f( xx, yy, zz );
    glVertex3f( _parent->_x[i], _parent->_y[i], 0.0 );

    xx = _parent->_x[i];
    yy = _parent->_y[i];
  }

  glEnd();


  // Draw selected elements.
  if( _hilights ) {
    glPointSize(9);
    glBegin( GL_POINTS );

    // The first point ...
    xx = 0.0, yy = 0.0, zz = 0.0;

    // ... the other points.
    for( int i = 0; i < _parent->_n; i++ ) 
    {
      q = _parent->_element[i];
      if( _parent->filter(q) ) {

        glColor3f( 1., 1., 0. );  // yellow
        glVertex3f( xx, yy, zz );
      }
      xx = _parent->_x[i];
      yy = _parent->_y[i];
    }

    glEnd();
    glPointSize(1);
  }

  glFlush();
}


void SiteViewer::Wndw::activateZoom()
{
  _zoomActive = true;
}


void SiteViewer::Wndw::deactivateZoom()
{
  _zoomActive = false;
  _isZooming  = false;
}


void SiteViewer::Wndw::zoomOut()
{
  static double xc, yc, dx, dy;

  dx = _xHi - _xLo;
  dy = _yHi - _yLo;

  if( fabs(dx-dy) > 0.00001*(dx+dy) ) {
    cerr << "\n*** WARNING *** " << __FILE__ << ": line" << __LINE__
         << "\n*** WARNING *** void SiteViewer::Wndw::zoomOut()" 
         << "\n*** WARNING *** Squaring up the viewport.\n"
         << endl;
    dy = dx;
  }

  xc = ( _xLo + _xHi )/2.0;
  yc = ( _yLo + _yHi )/2.0;
  
  _xLo = xc - dx;
  _xHi = xc + dx;
  _yLo = yc - dy;
  _yHi = yc + dy;

  this->updateGL();
}


void SiteViewer::Wndw::resetZoom()
{
  _zoomed     = false;
  _zoomActive = false;   // These two are 
  _isZooming  = false;   // not necessary.

  this->updateGL();
}


void SiteViewer::Wndw::toggleHighlight()
{
  _hilights = !_hilights;
}


void SiteViewer::Wndw::mousePressEvent( QMouseEvent* pme )
{
  static double ix, iy;
  static double dmin, d, dx, dy;
  static double x, y, xx, yy;
  static double s, azimuth;
  static int i, j;

  // If either button was pressed, we may be
  // starting a zoom ... or not.
  _ixbf = pme->pos().x();
  _iybf = pme->pos().y();

  ix = ((double)(pme->pos().x())) / ((double) this->width() );
  iy = ((double)(pme->pos().y())) / ((double) this->height());

  _xbf = ( _xHi*ix + _xLo*( 1.0 - ix ) );
  _ybf = ( _yLo*iy + _yHi*( 1.0 - iy ) );

  _xLobf = _xLo;
  _xHibf = _xHi;
  _yLobf = _yLo;
  _yHibf = _yHi;

  _isZooming     = false;
  _startedZoom   = _zoomActive && ( pme->button() == LeftButton );
  _startedScroll = (!_zoomActive) && ( pme->button() == LeftButton );

  // If the right button was pressed,
  // then search for nearest beamline element.
  // if( !_zoomActive ) {
  if( pme->button() == RightButton ) {
    const Particle& particleRef = _parent->_bmlConPtr->_proton;
    j = 0;
    s = 0.0;
    dmin = MAXFLOAT;
    xx = 0.0;
    yy = 0.0;
    for( i = 0; i < _parent->_n; i++ ) {
      x = _parent->_x[i];
      y = _parent->_y[i];

      // REMOVE: dx = x - xx;
      // REMOVE: dy = y - yy;
      // REMOVE: s += sqrt( dx*dx + dy*dy );
      s += _parent->_element[i]->OrbitLength( particleRef );

      dx = _xbf - ((x+xx)/2.);
      dy = _ybf - ((y+yy)/2.);
      d = dx*dx + dy*dy;
      if( d < dmin ) {
        azimuth = s;
        dmin = d;
        j = i;
      }

      xx = x;
      yy = y;
    }

    bmlnElmnt* q = _parent->_element[j];

    // This code is similar to what appears in 
    // BeamlineBrowser::infoWriter::visitBmlnElmnt
    // Keep it that way!
    QString cap( q->Type() );
    cap += QString( " " );
    cap += QString( q->Name() );

    QString msg( "Length = " );
    QString stl;
    stl.setNum( q->Length() );
    msg += stl;
    stl.setNum( q->Strength() );
    msg += QString( "\nStrength = " );
    msg += stl;
    stl.setNum( azimuth );
    msg += QString( "\nAzimuth = " );
    msg += stl;
  
    QMessageBox::information( 0, cap, msg );
  }
}


void SiteViewer::Wndw::mouseMoveEvent( QMouseEvent* pme )
{
  static int w, h;
  static int myW, myH;
  static QPixmap pic;
  static double ix, iy, x, y, dx, dy;

  if( _startedZoom )
  {
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
  
    w = pme->pos().x() - _ixbf;
    h = pme->pos().y() - _iybf;
  
    QPainter paint( this );
    paint.setPen( Qt::blue );
    paint.drawRect( _ixbf, _iybf, w, h );
  }

  else if( _startedScroll )
  {
    ix = ((double)(pme->pos().x())) / ((double) this->width() );
    iy = ((double)(pme->pos().y())) / ((double) this->height());

    x = ( _xHibf*ix + _xLobf*( 1.0 - ix ) );
    y = ( _yLobf*iy + _yHibf*( 1.0 - iy ) );

    dx = x - _xbf;
    dy = y - _ybf;

    _xLo = _xLobf - dx;
    _xHi = _xHibf - dx;
    _yLo = _yLobf - dy;
    _yHi = _yHibf - dy;

    this->updateGL();
  }
}


void SiteViewer::Wndw::mouseReleaseEvent( QMouseEvent* pme )
{
  static double ix, iy;
  static double x, y;
  static double dx, dy, w;

  if( _isZooming )
  {
    ix = ((double)(pme->pos().x())) / ((double) this->width() );
    iy = ((double)(pme->pos().y())) / ((double) this->height());
  
    x = ( _xHi*ix + _xLo*( 1.0 - ix ) );
    y = ( _yLo*iy + _yHi*( 1.0 - iy ) );
  
    // REMOVE: // Square up the edges
    // REMOVE: dx = fabs( x - _xbf );
    // REMOVE: dy = fabs( y - _ybf );
    // REMOVE: 
    // REMOVE: if( dx < dy ) { w = dx; }
    // REMOVE: else          { w = dy; }
    // REMOVE: 
    // REMOVE: if( _xbf < x ) { _xLo = _xbf; }
    // REMOVE: else           { _xLo = x; }
    // REMOVE: _xHi = _xLo + w;
    // REMOVE: 
    // REMOVE: if( _ybf < y ) { _yLo = _ybf; }
    // REMOVE: else           { _yLo = y; }
    // REMOVE: _yHi = _yLo + w;
    
    // Square up the edges
    dx = fabs( x - _xbf );
    dy = fabs( y - _ybf );
  
    if( dy < dx ) { w = dx; }
    else          { w = dy; }
  
    _xLo = ( x + _xbf - w )/2.0;
    _xHi = _xLo + w;
  
    _yLo = ( y + _ybf - w )/2.0;
    _yHi = _yLo + w;

    // Finished
    _isZooming = false;
    _startedZoom = false;
    _startedScroll = false;
    // REMOVE: _zoomActive = false;
    _zoomed = true;

    this->updateGL();
  }
}
