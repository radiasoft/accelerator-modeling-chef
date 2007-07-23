/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                    
******  File:      TrbWidget.cc
******  Version:   2.1
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
****** 
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                         
******  Authors:   Leo Michelotti
******             Nadezhda Shemyakina  (Summer, 2001)
******             
******             with ideas taken from a simple trackball
******             widget written by Josef Wilgen.
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** REVISION HISTORY
****** Mar 2007     ostiguy@fnal.gov
****** - removed dependencies on dlist/slist
******                                                                
**************************************************************************
*************************************************************************/


// In header of original version:
// File: TrbWidget.cc
// 
// Modifed from animator.cc, uses
// trackball to orient viewer.
//
// Leo Michelotti
// July 13, 2001
//
//

#include <values.h>
#include <stdlib.h>
#include <iostream>  // Amazingly, without *this* line, cout is undeclared,
#include <sstream>   //   even though *this* line is here.
#include <math.h>

#ifndef MAXFLOAT
#define MAXFLOAT 1.0e32
#endif

#include <qapplication.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qgrid.h>

#include <qwt_math.h>
#include <GL/glut.h>

#include "MathConstants.h"

#include "Tracker.h"  // ??? Needed only for class Orbit
#include "TrbWidget.h"

using namespace std;


DrawSpace3D::DrawSpace3D( TrbWidget* p )
: QGLWidget(p), _topTrbWidget(p), 
  _isIterating(false), _cameraInitialized(false),
  _theta(0.0),  _phi(0.0),
  _x_min(-1.0), _x_max(1.0), _x_center(0.0), _x_scale(1.0), 
  _y_min(-1.0), _y_max(1.0), _y_center(0.0), _y_scale(1.0), 
  _z_min(-1.0), _z_max(1.0), _z_center(0.0), _z_scale(1.0), 
  _x_cell_width(0.1), 
  _y_cell_width(0.1), 
  _sx_cell_width(0.1), 
  _sy_cell_width(0.1)
  // REMOVE: _sc_size_x(0.0), _sc_size_y(0.0), 
  // REMOVE: _nop_scale(0.0)
{
  // REMOVE: if( strlen(m) < 20 ) {
  // REMOVE:   strcpy( _myName, m );  // ??? _myName not needed ???
  // REMOVE: }
  // REMOVE: else {
  // REMOVE:   strncpy( _myName, m, (20-1) );
  // REMOVE:   _myName[(20-1)] = '\0';
  // REMOVE: }

  _cameraFrom[0] = 0.0;   _cameraFrom[1] = 0.0;   _cameraFrom[2] = 10.0;
  _cameraTo  [0] = 0.0;   _cameraTo  [1] = 0.0;   _cameraTo  [2] = 0.0; 
  _cameraUp  [0] = 0.0;   _cameraUp  [1] = 1.0;   _cameraUp  [2] = 0.0; 

  _light_position[0] = 0.0;
  _light_position[1] = 0.0;
  _light_position[2] = 10.0;
  _light_position[3] = 1.0;

  _light_ambience[0] = 1.0;
  _light_ambience[1] = 1.0;
  _light_ambience[2] = 1.0;
  _light_ambience[3] = 1.0;

  _yellow_color[0] = 1.0;
  _yellow_color[1] = 1.0;
  _yellow_color[2] = 0.0;
  _yellow_color[3] = 1.0;

  _dark_yellow_color[0] = 0.4;
  _dark_yellow_color[1] = 0.4;
  _dark_yellow_color[2] = 0.0;
  _dark_yellow_color[3] = 1.0;


  // From main function of doubles.cc
  // Prepare scaling parameters

  // Begin by finding _x_min, _y_min, _x_max, _y_max, 
  // _x_cell_width, and _y_cell_width
  _x_min =   MAXFLOAT;
  _x_max = - MAXFLOAT;
  _y_min =   MAXFLOAT;
  _y_max = - MAXFLOAT;
  _z_min =   MAXFLOAT;
  _z_max = - MAXFLOAT;
  
  const Orbit* orbitPtr = 0;

  for ( std::list<Orbit*>::iterator orbit_it  = _topTrbWidget->_myOrbits.begin();
	orbit_it != _topTrbWidget->_myOrbits.end(); ++orbit_it )
  {
    orbitPtr = *orbit_it;

    for( Orbit::const_iterator it  = orbitPtr->begin();
	                       it != orbitPtr->end(); ++it ) {

      _x_min = std::min(_x_min, (**it)(0) );
      _x_max = std::max(_x_max, (**it)(0) );

      _y_min = std::min(_y_min, (**it)(1) );
      _y_max = std::max(_y_max, (**it)(1) );

      _z_min = std::min(_z_min, (**it)(2) );
      _z_max = std::max(_z_max, (**it)(2) );
    }
  }

  _x_cell_width = ( _x_max - _x_min ) / 10.0;
  _y_cell_width = ( _y_max - _y_min ) / 10.0;

  double zCellWidth = ( _z_max - _z_min ) / 10.0;

  _x_center = ( _x_min + _x_max ) / 2.0;
  _y_center = ( _y_min + _y_max ) / 2.0;
  _z_center = ( _z_min + _z_max ) / 2.0;

  if( std::abs(_x_center) <= 2.0*_x_cell_width ) { _x_center = 0.0; }
  if( std::abs(_y_center) <= 2.0*_y_cell_width ) { _y_center = 0.0; }
  if( std::abs(_z_center) <= 2.0*zCellWidth    ) { _z_center = 0.0; }

  _x_scale = std::abs( std::abs(_x_min) > std::abs(_x_max) ? 
                       _x_min : _x_max
                     );
  _y_scale = std::abs( std::abs(_y_min) > std::abs(_y_max) ? 
                       _y_min : _y_max
                     );
  _z_scale = std::abs( std::abs(_z_min) > std::abs(_z_max) ? 
                       _z_min : _z_max
                     );
  _sx_cell_width = (_x_cell_width/(_x_scale));
  _sy_cell_width = (_y_cell_width/(_y_scale)); 
}


DrawSpace3D::~DrawSpace3D()
{
}


void DrawSpace3D::startIterating()
{
  _isIterating = true;
}


void DrawSpace3D::stopIterating()
{
  _isIterating = false;
}


double DrawSpace3D::getTheta()
{
  return _theta;
}


double DrawSpace3D::getPhi()
{
  return _phi;
}


void DrawSpace3D::setTheta( double x )
{
  _theta = x;

  while( _theta < 0.0 )      { _theta += M_TWOPI; }
  while( _theta >= M_TWOPI ) { _theta -= M_TWOPI; }
  if( _theta > M_PI ) { _theta = M_TWOPI - _theta; }

  _light_position[0] = 10.0*sin(_theta)*cos(_phi);
  _light_position[1] = 10.0*sin(_theta)*sin(_phi);
  _light_position[2] = 10.0*cos(_theta);
}


void DrawSpace3D::setPhi( double x )
{
  _phi = x;

  while( _phi < 0.0 )      { _phi += M_TWOPI; }
  while( _phi >= M_TWOPI ) { _phi -= M_TWOPI; }

  _light_position[0] = 10.0*sin(_theta)*cos(_phi);
  _light_position[1] = 10.0*sin(_theta)*sin(_phi);
  _light_position[2] = 10.0*cos(_theta);
}


void TrbWidget::_file_close()
{
  delete this;
}


void TrbWidget::_light_sel()
{
  QDialog* wpu = new QDialog( 0, 0, true );
  // QDialog* wpu = new QDialog( 0, 0, true, Qt::WDestructiveClose );
    QVBox* qvb = new QVBox( wpu );

      QGrid* qgr = new QGrid( 2, qvb );
        QLabel* qlb1 = new QLabel( "Theta [deg]", qgr );
        qlb1->setTextFormat( Qt::PlainText );
        QString stl1;
        stl1.setNum( ((180./M_PI)*_p_glWindow->getTheta()) );
        QLineEdit* qle1 = new QLineEdit( stl1, qgr );
     /* QLabel* qlb2 = */ new QLabel( "Phi [deg]", qgr );
        // qlb2->setTextFormat( Qt::PlainText );
        // qlb2->setFixedSize(qlb1->size());
        // qlb2->resize(qlb1->size());
        QString stl2;
        stl2.setNum( ((180./M_PI)*_p_glWindow->getPhi()) );
        QLineEdit* qle2 = new QLineEdit( stl2, qgr );
      qgr->setMargin(5);
      qgr->setSpacing(3);
      qgr->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb3 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();

  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, because of the flag Qt::WDestructiveClose.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  wpu->setCaption( "Light Position" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stl1 != qle1->text() ) {
      bool ok;
      double newTheta = (qle1->text()).toDouble( &ok );
      newTheta *= (M_PI/180.);
      if( ok ) {
        _p_glWindow->setTheta( newTheta );
      }
    }
    if( stl2 != qle2->text() ) {
      bool ok;
      double newPhi = (qle2->text()).toDouble( &ok );
      newPhi *= (M_PI/180.);
      if( ok ) {
        _p_glWindow->setPhi( newPhi );
      }
    }
  }

  delete wpu;
  _p_glWindow->updateGL();  // Necessary ???
}


void TrbWidget::_do_nothing()
{
}


TrbWidget::TrbWidget( list<Orbit*>& theOrbits )
: QVBox(), _isIterating(false)
{
  // The TrbWidget constructor assumes ownership
  //   of all Orbits in orbits3D. In the process
  //   it empties the list.

  // Process the orbits

  for ( std::list<Orbit*>::iterator it = theOrbits.begin();  it != theOrbits.end(); ++it ) {
     _myOrbits.push_back( *it );
  }

  // Separate child widgets
  this->setSpacing(5);

  // Construct the timer
  _p_timer = new QTimer( this );

  // Construct the main menu
  QMenuBar*    myMenuPtr = new QMenuBar( this );

    QPopupMenu*  fileMenu  = new QPopupMenu;
    fileMenu->insertItem( "Close", this, SLOT(_file_close()), CTRL+Key_E );
  myMenuPtr->insertItem( "File", fileMenu );

    QPopupMenu*  optionsMenu = new QPopupMenu;
    optionsMenu->insertItem( "Light", this, SLOT(_light_sel()), CTRL+Key_L );
  myMenuPtr->insertItem( "Options", optionsMenu );

  // Construct tracking toggle button; align it to left.
  _p_trackBox = new QHBox( this );
  _p_startBtn = new QPushButton( "&Go!", _p_trackBox );
  _p_startBtn->setToggleButton( true );
  _p_startBtn->setMaximumWidth( 100 );
  _p_blankLabel = new QLabel( _p_trackBox );
  this->setStretchFactor( _p_blankLabel, 10 );

  // Construct drawing windows ...
  _p_glWindow = new DrawSpace3D( this );
  // REMOVE: _p_glWindow->show();
  int fixedWidth = ((60*(QApplication::desktop()->height()))/100);
  _p_glWindow->setFixedSize(fixedWidth,fixedWidth);
  _p_glWindow->update();

  // Make connections...
  connect( _p_startBtn, SIGNAL(clicked()),
           this,        SLOT(_start_callback()));
  connect( _p_timer, SIGNAL(timeout()),
           this,     SLOT(_iterate()));
}


TrbWidget::~TrbWidget()
{
  for ( std::list<Orbit*>::iterator it = _myOrbits.begin();   it != _myOrbits.end(); ++it ){
    delete (*it);
  }
}



void TrbWidget::_iterate()
{
  if( _isIterating ) 
  {
    _p_glWindow->updateGL();
    _p_timer->start( 100, true );
  }
  else {
    _p_timer->stop();
  }
}


void TrbWidget::_start_callback()
{
  _isIterating = !( _isIterating );

  if( _isIterating ) {
    _p_startBtn->setText( "&Stop!" );
    _p_timer->start( 100, true );
    _p_glWindow->startIterating();
  }
  else {
    _p_startBtn->setText( "&Go!" );
    _p_glWindow->stopIterating();
  }
  
  // Not needed: QApplication::flushX();
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++
// Mouse event handling functions

void DrawSpace3D::mousePressEvent( QMouseEvent* )
{
}


void DrawSpace3D::mouseMoveEvent( QMouseEvent* mePtr )
{
  double v[3];
  double w, h, x, y, a;

  w = ((double) this->width());
  h = ((double) this->height());
  x = ((double)(mePtr->pos().x()));
  y = ((double)(mePtr->pos().y()));

  // Code copied from trackball
  v[0] = (2.0 * x - w) / w;
  v[1] = (h - 2.0 * y) / h;
  a = sqrt(v[0] * v[0] + v[1] * v[1]);
  v[2] = cos((M_PI) * ((a < 1.0) ? a : 1.0));
  a = 1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= a;
  v[1] *= a;
  v[2] *= a;

  if( v[2] >= 0.0 ) {
    _cameraFrom[0] = - 10.*v[0];
    _cameraFrom[1] = - 10.*v[1];
    _cameraFrom[2] = 10.*v[2];
    _cameraUp[1]    = 1.0;
  }
  else {
    _cameraFrom[0] = 10.*v[0];
    _cameraFrom[1] = - 10.*v[1];
    _cameraFrom[2] = 10.*v[2];
    _cameraUp[1]    = - 1.0;
  }

  updateGL();
}


void DrawSpace3D::mouseReleaseEvent( QMouseEvent* )
{
}


void DrawSpace3D::_drawOneLine( double x1, double y1, double x2, double y2 ) 
{
  glBegin(GL_LINES);
  glVertex2f (x1, y1); 
  glVertex2f (x2, y2); 
  glEnd();
}


// REMOVE: void DrawSpace3D::_output(double x, double y, double u)
// REMOVE: {
// REMOVE:   glRasterPos2f(x, y);
// REMOVE:   stringstream ss;
// REMOVE:   ss << u << '\0';
// REMOVE:   char c;
// REMOVE:   c = ss.get();
// REMOVE: 
// REMOVE:   while(( c != '\0' )) {
// REMOVE:     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
// REMOVE:     c = ss.get();
// REMOVE:   }
// REMOVE: }


void DrawSpace3D::_output(double x, double y, double u)
{
  glRasterPos2f(x, y);
  QString ss;
  ss.setNum(u);
  QChar qc;
  char c;
  int i = 0;
  qc = ss[i];

  while(( qc != QChar::null )) {
    c = qc.latin1();
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    ++i;
    qc = ss[i];
  }
}


void DrawSpace3D::_zoutput(double x, double y, double z, double u)
{
  glRasterPos3f(x, y, z);
  stringstream ss;
  ss << u << '\0';
  char c;
  c = ss.get();
  while(( c != '\0' )) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    c = ss.get();
  }
}

void DrawSpace3D::_choutput( double x, double y, char *str )
{
  int len, j;
  glRasterPos2f(x, y);
  len = (int) strlen(str);
  for (j = 0; j < len; j++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[j]);
  }
}


void DrawSpace3D::_drawGrid()
{
  double fx_min; 
  if (_x_min > 0) {fx_min = ((int)(_x_min/_x_cell_width))*_x_cell_width;}
  else {fx_min = ((int)(_x_min/_x_cell_width) - 1.0)*_x_cell_width;}
  double fy_min;
  if (_y_min > 0) {fy_min = ((int)(_y_min/_y_cell_width))*_y_cell_width;}
  else {fy_min = ((int)(_y_min/_y_cell_width) - 1.0)*_y_cell_width;} 
     
  double cx_max;
  if (_x_max < 0) {cx_max = ((int)(_x_max/_x_cell_width))*_x_cell_width;}
  else {cx_max = ((int)(_x_max/_x_cell_width) + 1.0)*_x_cell_width;} 
  double cy_max;
  if (_y_max < 0) {cy_max = ((int)(_y_max/_y_cell_width))*_y_cell_width;}
  else {cy_max = ((int)(_y_max/_y_cell_width) + 1.0)*_y_cell_width;} 
  
  double sx_min = fx_min/_x_scale;
  double sy_min = fy_min/_y_scale;
  double sx_max = cx_max/_x_scale;
  double sy_max = cy_max/_y_scale;

  double sx = sx_min - _sx_cell_width;
  double x = fx_min  - _x_cell_width;
  glColor3f(0.0, 1.0, 0.0);
  while( x < (cx_max) + 1.5*_x_cell_width )
  { 
    if ( fabs(x) < 1.e-10*_x_cell_width )
    {
      glColor3f(1.0, 0.0, 0.0);
      _drawOneLine(sx, sy_min - _sy_cell_width, sx, sy_max + _sy_cell_width);
      _choutput(sx, sy_min - 1.5*_sy_cell_width, "0(x)");
      glColor3f(0.0, 1.0, 0.0);

    }
    else
    {
      //draw lines parallel y-axis
      _drawOneLine( sx, sy_min - _sy_cell_width,sx, sy_max + _sy_cell_width );
      _output( sx, sy_min - 1.5*_sy_cell_width, x );
    }
    x += _x_cell_width;
    sx += _sx_cell_width;
  }

  double sy = sy_min - _sy_cell_width;
  double y = fy_min  - _y_cell_width;
  while( y < (cy_max) + 1.5*_y_cell_width )
  {
    if ( fabs(y) < 1.0e-10*_y_cell_width )
    {
      glColor3f(1.0, 0.0, 0.0);
      _drawOneLine( sx_min - _sx_cell_width, sy, sx_max + _sx_cell_width, sy ); 
      _choutput(sx_min - 1.55*_sx_cell_width, sy, "0(y)");
      glColor3f(0.0, 1.0, 0.0);
     }
    else
    {
      //draw lines parallel x-axis
      _drawOneLine( sx_min - _sx_cell_width, sy, sx_max + _sx_cell_width, sy ); 
      _output( sx_min - 1.5*_sx_cell_width, sy, y );
    }
    y += _y_cell_width;
    sy += _sy_cell_width;
  }
}


// Fundamental opengl paint function
void DrawSpace3D::paintGL()
{
  if( !_cameraInitialized ) {
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -0.3, 0.3,  -0.3, 0.3,  1.5, 20. );  
    _cameraInitialized = true;
  }

  static double xrot = 0.0;
  
  //
  // Enable depth testing and clear the color and depth
  //  buffers.
  //
  glEnable( GL_DEPTH_TEST );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  // glClear( GL_COLOR_BUFFER_BIT );
  

  //
  // Define the modelview transformation.
  //
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  gluLookAt( _cameraFrom[0], _cameraFrom[1], _cameraFrom[2],
             _cameraTo[0],   _cameraTo[1],   _cameraTo[2],
             _cameraUp[0],   _cameraUp[1],   _cameraUp[2] );


  if( _isIterating ) { xrot += 5.; }
  glRotatef(xrot, 0.,0.,1.);

  _drawGrid();
  
  // REMOVE: // Set up lighting
  // REMOVE: glEnable(GL_LIGHTING);
  // REMOVE: glEnable(GL_LIGHT0);
  // REMOVE: glLightfv(GL_LIGHT0, GL_POSITION,  _light_position);
  // REMOVE: glLightfv(GL_LIGHT0, GL_AMBIENT,   _light_ambience);
  // REMOVE: glMaterialfv(GL_FRONT, GL_AMBIENT, _dark_yellow_color);
  // REMOVE: glMaterialfv(GL_FRONT, GL_DIFFUSE, _yellow_color);
  
  // REMOVE: // Disable lighting
  glDisable(GL_LIGHTING);


  glPointSize(3);
  const Orbit* orbitPtr;

  glBegin( GL_POINTS );

  for (std::list<Orbit*>::iterator orbit_it  =  _topTrbWidget->_myOrbits.begin();
       orbit_it !=  _topTrbWidget->_myOrbits.end(); ++orbit_it ) 
  {

    orbitPtr = *orbit_it;

    glColor3f( orbitPtr->Red(), orbitPtr->Green(), orbitPtr->Blue() );

    for( Orbit::const_iterator it  = orbitPtr->begin();
	                       it != orbitPtr->end(); ++it ) {

      glVertex3f( ((**it)(0) - _x_center)/_x_scale,
                  ((**it)(1) - _y_center)/_y_scale,
                  ((**it)(2) - _z_center)/_z_scale  );
    }
  }
  glEnd();

  glFlush ();
}
 
