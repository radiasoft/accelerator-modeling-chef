/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      RayTrace.cc
******  Version:   3.3
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
                                                                
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
******  Partial Revision History:
******
******  June, 2005 
******  - Axes, grids, and tic labels added with the assistance
******  of Summer student employee Oleg Mokhov.
******  Mar 2007   ostiguy@fnal.gov
******  -eliminated references to slist/dlist
******                                                                
**************************************************************************
*************************************************************************/


// ************************************
// 
// File: RayTrace.cc
// 
// Author: Leo Michelotti
// Date: September 13, 2001
// 
// ************************************


#include <fstream>

#include <qapplication.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <typeinfo>

#include <GenericException.h>
#include <RayTrace.h>
#include <PointEdit.h>
#include <Tracker.h>   // ??? Is this needed?
#include <QtMonitor.h>

#include <GL/glut.h>
/// FIX ME !#include <qwt_math.h>

#include <beamline/Particle.h>  // This line should not be necessary!!!
#include <beamline/beamline.h>
#include <physics_toolkit/BeamlineContext.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>


// This undef is needed because of the compiler.
#undef connect
// ??? which compiler ???
// ??? still true     ???


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// --------------------------
// Implementation: struct Ray
// --------------------------

Ray::Ray()
: s(0.0), z(6)
{
}

Ray::~Ray()
{
}

// -------------------------------
// Implementation: class RayDrawSpace
// -------------------------------

RayDrawSpace::RayDrawSpace( RayTrace* p, QHBox* parent, const char* m )
: QGLWidget(parent), 
  _topRayTrace(p), 
  _pointSize(3),
  _r(1.0), _g(1.0), _b(1.0),
  _xLo( -0.0020 ), 
  _yLo( -0.0020 ), 
  _xHi( 0.0020 ), 
  _yHi( 0.0020 ),
  _myWheel(0.0),
  _drawAxes(false),
  _drawGrid(false),
  _drawNums(false)
{
  // *** _myWheel.setIncrement( 252.0 );  // = 7*36, will provide ten colors
  _myWheel.setIncrement( 30.0 ); 

  _myFunc = drawH_ViewRect;  // This must be changed in the 
                             // RayTrace constructor
  strcpy( _myName, m );
}


RayDrawSpace::~RayDrawSpace()
{
  // cout << _myName << " destructor called." << endl;
}


void RayDrawSpace::multScaleBy( double x )
{
  double u = fabs(x);
  // _xLo /= u;
  // _xHi /= u;
  _yLo /= u;
  _yHi /= u;
}


void RayDrawSpace::setScaleTo( double x )
{
  double u = fabs(x);
  _xLo = -u;
  _xHi =  u;
  _yLo = -u;
  _yHi =  u;
}


void RayDrawSpace::setDraw( RayDrawFunc x )
{
  _myFunc = x;
}


void RayDrawSpace::toggleAxisDisplay()
{
  _drawAxes = !_drawAxes;
  if( !_drawAxes ) { _drawNums = false; }
  updateGL();
}


void RayDrawSpace::toggleGridDisplay()
{
  _drawGrid = !_drawGrid;
  updateGL();
}


void RayDrawSpace::toggleNumberDisplay()
{
  _drawNums = !_drawNums;
  if( !_drawAxes ) { _drawNums = false; }
  updateGL();
}


void RayDrawSpace::setColors( GLdouble r, GLdouble g, GLdouble b )
{
  if( 0.0 <= r && r <= 1.0 ) _r = r;
  if( 0.0 <= g && g <= 1.0 ) _g = g;
  if( 0.0 <= b && b <= 1.0 ) _b = b;
}



void RayDrawSpace::setClearColor( GLclampf r, GLclampf g, GLclampf b, GLclampf a )
{
  _rClr = r;
  _gClr = g;
  _bClr = b;
  _aClr = a;
  updateGL();
}


void RayDrawSpace::_incrementColor()
{
  _myWheel.increment();
  _r = _myWheel.red();
  _g = _myWheel.green();
  _b = _myWheel.blue();
}


void RayDrawSpace::paintGL()
{
  _myFunc( this );
}


void RayDrawSpace::doDrawAxes()
{
  double xPos;
  double yPos;
  double xGridMin, xGridMax, yGridMin, yGridMax;
  double interval, xInterval, yInterval;

  _computeGridBounds( xGridMin, xGridMax,
                      yGridMin, yGridMax,
		      interval, xInterval, yInterval );

  glLineWidth( 2 );
  glColor3f( 0.5, 0.5, 0.5 );

  // X-axis line
  glBegin( GL_LINES );
    glVertex3d( _xLo, (_yLo + _yHi) / 2.0, 1.0 );
    glVertex3d( _xHi, (_yLo + _yHi) / 2.0, 1.0 );
  glEnd();

  // Y-axis line
  glBegin( GL_LINES );
    glVertex3d( (_xLo + _xHi) / 2.0, _yLo, 1.0 );
    glVertex3d( (_xLo + _xHi) / 2.0, _yHi, 1.0 );
  glEnd();

  // X-axis tic marks
  for( xPos = (_xHi + _xLo) / 2.0 - xInterval * 1.5 * interval; xPos < _xHi; xPos += xInterval )
  {
    if( xPos != (_xLo + _xHi) / 2.0 )
    {
      glLineWidth( 3 );
      glColor3f( 0.5, 0.5, 0.5 );

      glBegin( GL_LINES );
        glVertex3d( xPos, (_yLo + _yHi) / 2.0 + _yLo * (1.0 / 30.0), 1.0 );
        glVertex3d( xPos, (_yLo + _yHi) / 2.0 + _yHi * (1.0 / 30.0), 1.0 );
      glEnd();
    }   
  }

  // Y-axis tic marks
  //for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval; yPos < _yHi; yPos += yInterval )
  for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval - yGridMax * 1.5; yPos < _yHi; yPos += yInterval )
  {
    if( yPos != (_yLo + _yHi) / 2.0 )
    {
      glLineWidth( 3 );
      glColor3f( 0.5, 0.5, 0.5 );

      glBegin( GL_LINES );
        glVertex3d( (_xLo + _xHi) / 2.0 - _xHi * (1.0 / 50.0), yPos, 1.0 );
        glVertex3d( (_xLo + _xHi) / 2.0 + _xHi * (1.0 / 50.0), yPos, 1.0 );
      glEnd(); 
    }
  }
}


void RayDrawSpace::doDrawGrid()
{
  double xPos, yPos;
  double xGridMin, xGridMax, yGridMin, yGridMax;
  double interval, xInterval, yInterval;

  _computeGridBounds( xGridMin, xGridMax,
                      yGridMin, yGridMax,
		      interval, xInterval, yInterval );

  glLineWidth( 1 );
  glColor3f( 0.5, 0.5, 0.5 );

  // Vertical grid lines
  for( xPos = (_xHi + _xLo) / 2.0 - xInterval * 1.5 * interval; xPos < _xHi; xPos += xInterval )
  {  
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, 0x00FF );

    glBegin( GL_LINES );
      glVertex3d( xPos, _yLo, 1.0 );
      glVertex3d( xPos, _yHi, 1.0 );
    glEnd();

    glDisable( GL_LINE_STIPPLE );   
  }

  // Horizontal grid lines
  //for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval; yPos < _yHi; yPos += yInterval )
  for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval - yGridMax * 1.5; yPos < _yHi; yPos += yInterval )
  {
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, 0x00FF );

    glBegin( GL_LINES );
      glVertex3d( _xLo, yPos, 1.0 );
      glVertex3d( _xHi, yPos, 1.0 );
    glEnd();

    glDisable( GL_LINE_STIPPLE );
  }
}


void RayDrawSpace::doDrawNums()
{
  double xPos;
  double yPos;
  
  stringstream ss;
  char c;

  double xGridMin, xGridMax, yGridMin, yGridMax;
  double interval, xInterval, yInterval;

  _computeGridBounds( xGridMin, xGridMax,
                      yGridMin, yGridMax,
		      interval, xInterval, yInterval );

  glColor3f ( 1.0, 1.0, 0.0 );

  // X-axis numbers
  for( xPos = (_xHi + _xLo) / 2.0 - xInterval * 1.5 * interval; xPos < _xHi; xPos += xInterval )
  {
    // if( xPos != (_xHi + _xLo) / 2.0 )
    if( 1.0e-14 < std::abs( xPos - (_xHi + _xLo) / 2.0) )
    {
      glRasterPos3d( xPos, (_yLo * (9.0 / 10.0) + _yHi) / 2.0, 1.0 );
      ss << xPos << '\0';
      c = ss.get();
      while (c != '\0' )
      {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
        c = ss.get();
      }
    }
  }

  // Y-axis numbers
  //for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval; yPos < _yHi; yPos += yInterval )
  for( yPos = (_yLo + _yHi) / 2.0 - yInterval * interval - yGridMax * 1.5; yPos < _yHi; yPos += yInterval )
  {
    // if( yPos != (_yLo + _yHi) / 2.0 )
    if( 1.0e-14 < std::abs( yPos - (_yHi + _yLo) / 2.0) )
    {
      glRasterPos3d( (_xLo + _xHi) / 2.0, yPos, 1.0 );
      ss <<  "    " << yPos << '\0';
      c = ss.get();
      while( c != '\0' )
      {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
        c = ss.get();
      }    
    }
  }
}


void RayDrawSpace::_computeGridBounds( double& xGridMin, double& xGridMax,
                                       double& yGridMin, double& yGridMax,
                                       double& interval, double& xInterval, double& yInterval ) const
{
  // The interval between every tick, grid line and number
  interval = 4.0;

  xGridMin = 0.9 * _xLo;
  xGridMax = 0.9 * _xHi;
  yGridMin = 0.9 * _yLo;
  yGridMax = 0.9 * _yHi;
            
  // FIX ME !!
  //xInterval = qwtFloor125( ((xGridMax - xGridMin) / 2.0) / interval);
  //yInterval = qwtFloor125( ((yGridMax - yGridMin) / 2.0) / interval);
  xInterval = (xGridMax - xGridMin) / 2.0 / interval;
  yInterval = (yGridMax - yGridMin) / 2.0 / interval;

  double xAverage = (xGridMin + xGridMax) / 2.0;
  double yAverage = (yGridMin + yGridMax) / 2.0;

  xGridMin = xAverage - interval * xInterval;
  xGridMax = xAverage + interval * xInterval;
  yGridMin = yAverage - interval * yInterval;
  yGridMax = yAverage + interval * yInterval;
}


void RayDrawSpace::drawH_ViewRect( RayDrawSpace* x )
{
  
  glClearColor( x->_rClr, x->_gClr, x->_bClr, x->_aClr );
  glClear( GL_COLOR_BUFFER_BIT );
  glLoadIdentity();
  glOrtho( x->_xLo, x->_xHi, x->_yLo, x->_yHi, -1., 1. );
  // glPointSize(x->_pointSize);
  glLineWidth(1);

  static int i;
  static int j;
  static int m;
  static int n;
  static Ray* q;
  
  n = x->_topRayTrace->numberOfMonitors();
  if( n <= 0 )  { return; }

  m = ( x->_topRayTrace->_history ).size();
  if( m > (x->_topRayTrace->maxHistory())*n ) {
    m -= (x->_topRayTrace->maxHistory())*n;

    for( i = 0; i < m; i++ ) {
     delete x->_topRayTrace->_history.front();
     x->_topRayTrace->_history.pop_front();
    }
  }

  std::list<Ray*>::iterator it = x->_topRayTrace->_history.begin();

  m = ( x->_topRayTrace->_history ).size() / n;
  for( i = 0; i < m-1; i++ ) {
    glColor3f( ((i+1)*(x->_r))/m, 
               ((i+1)*(x->_g))/m, 
               ((i+1)*(x->_b))/m );
    glBegin( GL_LINE_STRIP );


    for( j = 0; j < n; j++ ) {

      q = (it != x->_topRayTrace->_history.end()) ? *(it++) : 0;

      if(q) { glVertex3f( q->s, (q->z)(0), 0.0 ); }

      else {
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      }
    }
    glEnd();
  }

  // Draw axes, tic-marks, and grids
  if( x->_drawAxes ) { x->doDrawAxes(); }
  if( x->_drawGrid ) { x->doDrawGrid(); }
  if( x->_drawNums ) { x->doDrawNums(); }

  // The last ray is painted white.
  if( m > 0 ) {
    glColor3f( 1, 1, 1 );
    glLineWidth(3);
    glBegin( GL_LINE_STRIP );

    for( j = 0; j < n; j++ ) {

      q = (it != x->_topRayTrace->_history.end()) ? *(it++) : 0;

      if(q) { glVertex3f( q->s, (q->z)(0), 0.0 ); }
      else {
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      }
    }
    glEnd();
  }

  glFlush();
};


void RayDrawSpace::drawV_ViewRect( RayDrawSpace* x )
{

  glClearColor( x->_rClr, x->_gClr, x->_bClr, x->_aClr );
  glClear( GL_COLOR_BUFFER_BIT );
  glLoadIdentity();
  glOrtho( x->_xLo, x->_xHi, x->_yLo, x->_yHi, -1., 1. );
  // glPointSize(x->_pointSize);
  glLineWidth(1);

  static int i;
  static int j;
  static int m;
  static int n;
  static Ray* q;
  
  n = x->_topRayTrace->numberOfMonitors();
  if( n <= 0 )  { return; }

  m = ( x->_topRayTrace->_history ).size();
  if( m > (x->_topRayTrace->maxHistory())*n ) {
    m -= (x->_topRayTrace->maxHistory())*n;
    for( i = 0; i < m; i++ ) {
      delete x->_topRayTrace->_history.front();
      x->_topRayTrace->_history.pop_front();
    }
  }

  std::list<Ray*>::iterator it = x->_topRayTrace->_history.begin();

  m = ( x->_topRayTrace->_history ).size() / n;
  for( i = 0; i < m-1; i++ ) {
    glColor3f( ((i+1)*(x->_r))/m, 
               ((i+1)*(x->_g))/m, 
               ((i+1)*(x->_b))/m );
    glBegin( GL_LINE_STRIP );

    for( j = 0; j < n; j++ ) {

      q = (it != x->_topRayTrace->_history.end()) ? *(it++) : 0;

      if(q) { glVertex3f( q->s, (q->z)(1), 0.0 ); }
      else {
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      }
    }
    glEnd();
  }

  // Draw axes, tic-marks, and grids
  if( x->_drawAxes ) { x->doDrawAxes(); }
  if( x->_drawGrid ) { x->doDrawGrid(); }
  if( x->_drawNums ) { x->doDrawNums(); }

  // The last ray is painted white.
  if( m > 0 ) {
    glColor3f( 1, 1, 1 );
    glLineWidth(3);
    glBegin( GL_LINE_STRIP );

    for( j = 0; j < n; j++ ) {

      q = (it != x->_topRayTrace->_history.end()) ? *(it++) : 0;

      if(q) { glVertex3f( q->s, (q->z)(1), 0.0 ); }
      else {
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      }
    }
    glEnd();
  }

  glFlush();
};


void RayDrawSpace::mousePressEvent( QMouseEvent* /*qme*/ )
{
}


void RayDrawSpace::setPointSize( int x )
{
  _pointSize = x;
}


void RayDrawSpace::setRange( double xl, double xh, double yl, double yh )
{
  _xLo = xl;
  _xHi = xh;
  _yLo = yl;
  _yHi = yh;
}


// -----------------------------
// Implementation: class RayTrace
// -----------------------------

RayTrace::RayTrace( BmlContextPtr bmlCP, QWidget* parent, const char* name, WFlags f )
: QVBox( parent, name, f),
  _p_info(0), 
  _number(1),
  _maxHistory(64),
  _bmlConPtr( bmlCP ), 
  _continuous(true),
  _isIterating(false)
{
  _finishConstructor();
}


RayTrace::RayTrace( Particle const& prt, BmlPtr x, 
                    QWidget* parent, const char* name, WFlags f )
: QVBox(parent, name, f),
  _p_info(0), 
  _number(1),
  _maxHistory(64),
  _bmlConPtr(), 
  _continuous(true),
  _isIterating(false)
{
  if( 0 == x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "RayTrace::RayTrace( /* const */ beamline* x )", 
           "null argument passed to constructor." ) );
  }

  _bmlConPtr = BmlContextPtr( new BeamlineContext( prt, x ));
  _finishConstructor();
}


void RayTrace::_finishConstructor()
{
  // Initialize the QtMonitors
  _n_monitor = QtMonitor::setAzimuth( *_bmlConPtr->cheatBmlPtr() );
  if( 0 == _n_monitor ) {
    (*pcerr) << "\n*** WARNING *** "
             << "\n*** WARNING *** " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** RayTrace::_finishConstructor()"
             << "\n*** WARNING *** No QtMonitors in beamline "
             << _bmlConPtr->name()
             << "\n*** WARNING *** \n"
             << endl;
  }


  // Set mode based on beamline mode
  _continuous = _bmlConPtr->isTreatedAsRing();


  // Connect QtMonitor signals to the _appendToHistory slot
  
  for (beamline::deep_iterator it  = boost::const_pointer_cast<beamline>(_bmlConPtr->cheatBmlPtr() )->deep_begin();
                               it != boost::const_pointer_cast<beamline>(_bmlConPtr->cheatBmlPtr() )->deep_end(); ++it ) {
    if( typeid(**it) ==  typeid(QtMonitor) ) {
      QObject::connect( dynamic_cast<const QtMonitor*>((*it).get()),    
                          SIGNAL(ping(double,const Vector&)),
                        this, 
                          SLOT(_appendToHistory(double,const Vector&)) 
                      );
    }
  }


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
  myMenuPtr->insertItem( "Edit", editMenu );
 
    QPopupMenu*  optionMenu = new QPopupMenu;
      QPopupMenu*  zoomMenu  = new QPopupMenu;
      zoomMenu->insertItem( "Out", this, SLOT(_view_zoom_out()) );
      zoomMenu->insertItem( "In",  this, SLOT(_view_zoom_in()) );
      zoomMenu->insertItem( "Reset", this, SLOT(_view_zoom_reset()) );
    optionMenu->insertItem( "Zoom", zoomMenu );
    //   ----------------------
    //   QPopupMenu* pointsizeMenu = new QPopupMenu;
    //   pointsizeMenu->insertItem( "Large", this, SLOT(_opt_largePoints()) );
    //   pointsizeMenu->insertItem( "Small", this, SLOT(_opt_smallPoints()) );
    // optionMenu->insertItem( "Point Size", pointsizeMenu );
    // ---------------------------------------------
      QPopupMenu* modeMenu = new QPopupMenu;
      modeMenu->insertItem( "Continuous  ",  this, SLOT(_opt_contmode()) );
      modeMenu->insertItem( "Single shot  ", this, SLOT(_opt_snglmode()) );
    optionMenu->insertItem( "Mode  ", modeMenu );
    optionMenu->insertItem( "Maximum Traces  ", this, SLOT(_opt_setHistory()) );
    optionMenu->insertItem( "Strobe Period  ", this, SLOT(_opt_setIter()) );
      QPopupMenu* displayMenu = new QPopupMenu;
      displayMenu->insertItem( "Axes", this, SLOT(_opt_dsp_axes()) );
      displayMenu->insertItem( "Grid", this, SLOT(_opt_dsp_grid()) );
      displayMenu->insertItem( "Numbers", this, SLOT(_opt_dsp_nums()) );
    optionMenu->insertItem( "Display", displayMenu );
      QPopupMenu* bgColorMenu = new QPopupMenu;
      bgColorMenu->insertItem( "Black",  this, SLOT(_opt_bg_black()) );
      bgColorMenu->insertItem( "White",  this, SLOT(_opt_bg_white()) );
      bgColorMenu->insertItem( "Yellow", this, SLOT(_opt_bg_yellow()) );
    optionMenu->insertItem( "Background Color  ", bgColorMenu );
  myMenuPtr->insertItem( "Options", optionMenu );

  // Construct tracking toggle button; align it to left.
  _p_trackBox = new QHBox( this );
  _p_startBtn = new QPushButton( "Trace", _p_trackBox );
  _p_startBtn->setToggleButton( true );
  _p_startBtn->setMaximumWidth( 100 );
  _p_blankLabel = new QLabel( _p_trackBox );
  this->setStretchFactor( _p_blankLabel, 10 );

  // Construct drawing windows ...
  _p_phaseSpaceViews = new QHBox( this );
  _p_phaseSpaceViews->setSpacing(5);

  _p_leftWindow = new RayDrawSpace( this, _p_phaseSpaceViews, "First" );
    _p_leftWindow->show();
    _p_leftWindow->setDraw( RayDrawSpace::drawH_ViewRect );
    _p_leftWindow->setColors( 1., 0., 1. );
    int drawArea = (25*QApplication::desktop()->height()*
                       QApplication::desktop()->width())/100;
    int fixedWidth = (int) sqrt( (double) drawArea );
    _p_leftWindow->setFixedSize( fixedWidth, fixedWidth );
    // _p_leftWindow->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );
    _p_leftWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
    _p_leftWindow->update();

  _p_rightWindow = new RayDrawSpace( this, _p_phaseSpaceViews, "Second" );
    _p_rightWindow->show();
    _p_rightWindow->setDraw( RayDrawSpace::drawV_ViewRect );
    _p_rightWindow->setColors( 0., 1., 0. );
    _p_rightWindow->setFixedSize( fixedWidth, fixedWidth );
    _p_rightWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
    _p_rightWindow->update();


    // Scale the window displays.
    double lng = _bmlConPtr->sumLengths();
    double bff = 0.05*lng;
    _p_leftWindow ->setRange( -bff, lng+bff, -0.005, 0.005 );
    _p_rightWindow->setRange( -bff, lng+bff, -0.005, 0.005 );


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

  const Particle& particleRef = _bmlConPtr->getParticle();
  _p_x_input ->_set_first ( particleRef.get_x(), particleRef.get_npx() ); // Probably should
  _p_xp_input->_set_second( particleRef.get_x(), particleRef.get_npx() ); // be done by emitting
  _p_y_input ->_set_first ( particleRef.get_y(), particleRef.get_npy() ); // a signal of some
  _p_yp_input->_set_second( particleRef.get_y(), particleRef.get_npy() ); // sort.

  _p_x_input ->setMaxLength( 14 );
  _p_xp_input->setMaxLength( 14 );
  _p_y_input ->setMaxLength( 14 );
  _p_yp_input->setMaxLength( 14 );


  // Make connections...
  QObject::connect( _p_x_input,  SIGNAL(textChanged( const QString& )),
                    _p_x_input,  SLOT(_txtchg( const QString& )) );
  QObject::connect( _p_x_input,  SIGNAL(returnPressed()),
                    _p_x_input,  SLOT(_retprs()) );
  QObject::connect( _p_x_input,  SIGNAL(_new_value( double )),
                          this,  SLOT(_new_x( double )) );

  QObject::connect( _p_xp_input, SIGNAL(textChanged( const QString& )),
                    _p_xp_input, SLOT(_txtchg( const QString& )) );
  QObject::connect( _p_xp_input, SIGNAL(returnPressed()),
                    _p_xp_input, SLOT(_retprs()) );
  QObject::connect( _p_xp_input, SIGNAL(_new_value( double )),
                           this, SLOT(_new_xp( double )) );

  QObject::connect( _p_y_input,  SIGNAL(textChanged( const QString& )),
                    _p_y_input,  SLOT(_txtchg( const QString& )) );
  QObject::connect( _p_y_input,  SIGNAL(returnPressed()),
                    _p_y_input,  SLOT(_retprs()) );
  QObject::connect( _p_y_input,  SIGNAL(_new_value( double )),
                          this,  SLOT(_new_y( double )) );

  QObject::connect( _p_yp_input, SIGNAL(textChanged( const QString& )),
                    _p_yp_input, SLOT(_txtchg( const QString& )) );
  QObject::connect( _p_yp_input, SIGNAL(returnPressed()),
                    _p_yp_input, SLOT(_retprs()) );
  QObject::connect( _p_yp_input, SIGNAL(_new_value( double )),
                           this, SLOT(_new_yp( double )) );

  QObject::connect( _p_startBtn,   SIGNAL(clicked()),
                    this,          SLOT(_start_callback()));

  QObject::connect( _p_timer, SIGNAL(timeout()),
                    this,     SLOT(_iterate()));
}


RayTrace::~RayTrace()
{
  if( _p_info ) delete _p_info;

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

  for ( std::list<Ray*>::iterator it = _history.begin();  it != _history.end(); ++it ) {
    delete (*it);
  }

}


void RayTrace::_fileClose()
{
  close();
}


void RayTrace::_do_nothing()
{
  QMessageBox::information( this, "RayTrace",
                            "Sorry. This function is not implemented." );
}


void RayTrace::_edit_clear()
{
  _isIterating = false;

  for ( std::list<Ray*>::iterator it = _history.begin();  it != _history.end(); ++it ) {
    delete (*it);
  }

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void RayTrace::_view_zoom_out()
{
  _p_leftWindow->multScaleBy( 0.5 );
  _p_rightWindow->multScaleBy( 0.5 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void RayTrace::_view_zoom_in()
{
  _p_leftWindow->multScaleBy( 2.0 );
  _p_rightWindow->multScaleBy( 2.0 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}



void RayTrace::_view_zoom_reset()
{
  _p_leftWindow->setScaleTo( 0.002 );
  _p_rightWindow->setScaleTo( 0.002 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void RayTrace::_opt_largePoints()
{
  _p_leftWindow ->setPointSize( 3 );
  _p_rightWindow->setPointSize( 3 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}

void RayTrace::_opt_smallPoints()
{
  _p_leftWindow ->setPointSize( 1 );
  _p_rightWindow->setPointSize( 1 );

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void RayTrace::_opt_setHistory()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Maximum number of traces", qhb1 );
        QString stl;
        stl.setNum( _maxHistory );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                            wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
                            wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();
    qvb->adjustSize();
  wpu->setCaption( "Setting trace size" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stl != qle->text() ) {
      bool ok;
      int j = (qle->text()).toInt( &ok );
      if( ok ) {
        _maxHistory = j;
      }
    }
  }

  delete wpu;
}


void RayTrace::_opt_setIter()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
      /* QLabel* qlb = */ new QLabel( "Strobe period", qhb1 );
        QString stl;
        stl.setNum( _number );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                            wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
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


void RayTrace::_opt_dsp_axes()
{
  _p_leftWindow->toggleAxisDisplay();
  _p_rightWindow->toggleAxisDisplay();
}


void RayTrace::_opt_dsp_grid()
{
  _p_leftWindow->toggleGridDisplay();
  _p_rightWindow->toggleGridDisplay();
}


void RayTrace::_opt_dsp_nums()
{
  _p_leftWindow->toggleNumberDisplay();
  _p_rightWindow->toggleNumberDisplay();
}


void RayTrace::_opt_bg_black()
{
  _p_leftWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
  _p_rightWindow->setClearColor( 0.0, 0.0, 0.0, 1.0 );
}

void RayTrace::_opt_bg_white()
{
  _p_leftWindow->setClearColor( 1.0, 1.0, 1.0, 1.0 );
  _p_rightWindow->setClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void RayTrace::_opt_bg_yellow()
{
  _p_leftWindow->setClearColor( 1.0, 1.0, 0.0, 1.0 );
  _p_rightWindow->setClearColor( 1.0, 1.0, 0.0, 1.0 );
}


void RayTrace::_opt_contmode()
{
  _continuous = true;
}


void RayTrace::_opt_snglmode()
{
  _continuous = false;
}


void RayTrace::_new_x( double x )
{
  _bmlConPtr->setParticle_x(x);
}


void RayTrace::_new_xp( double xp )
{
  _bmlConPtr->setParticle_npx(xp);
}


void RayTrace::_new_y( double y )
{
  _bmlConPtr->setParticle_y(y);
}


void RayTrace::_new_yp( double yp )
{
  _bmlConPtr->setParticle_npy(yp);
}


void RayTrace::_appendToHistory( double az, const Vector& state )
{
  Ray* rayPtr = new Ray;
  rayPtr->s = az;
  rayPtr->z = state;
  _history.push_back( rayPtr );
}


void RayTrace::setState( const Vector& s )
{
  if( s.Dim() == 6 ) {
    _bmlConPtr->setParticleState( s );
  }
  else {
    ostringstream uic;
    uic  << "File: " << __FILE__ << ", Line: " << __LINE__
         << "\nvoid RayTrace::setState( const Vector& s )"
         << "\nArgument s has dimension " << (s.Dim()) << " != 6";
    QMessageBox::information( this, "CHEF::Tracker", uic.str().c_str() );
  }
}


void RayTrace::_iterate()
{
  if( 0 == (_bmlConPtr->particleBunchPtr_ )) {
    _pushParticle(); 
  }
  else {
    if( _bmlConPtr->particleBunchPtr_->empty() ) { 
      _pushParticle(); 
    }
    else                                           { 
      _pushBunch();    
    }
  }
}


void RayTrace::_pushBunch()
{
  ParticleBunch* pbPtr = _bmlConPtr->particleBunchPtr_;

  if( _isIterating ) 
  {
    for ( ParticleBunch::iterator it = pbPtr->begin();  it != pbPtr->end(); ++it ) {
      _bmlConPtr->setParticleState(  it->State() );
      _pushParticle();
    }

    // _p_leftWindow->updateGL();
    // _p_rightWindow->updateGL();

    _isIterating = false;
    _p_timer->stop();  // Probably unnecessary
    _p_startBtn->setOn(false);
    _p_startBtn->setText( "Trace" );
  }
}


void RayTrace::_pushParticle()
{
  BmlPtr bmlPtr = boost::const_pointer_cast<beamline>(_bmlConPtr->cheatBmlPtr());

  Particle* particle = _bmlConPtr->particle_;

  // ??? This will eventually have to be changed.
  
  // Continuous operation
  // --------------------
  if( _continuous ) 
  {
    _p_timer->stop();

    if( _isIterating ) 
    {
      // ??? This code fragment must be improved.
      // ??? It is written as a hack to prevent
      // ??? the program from hanging if the orbit 
      // ??? diverges.

      for( int i = 0; i < _number; i++ ) {
        for ( beamline::deep_iterator it  = boost::const_pointer_cast<beamline>(bmlPtr)->deep_begin();
                                      it != boost::const_pointer_cast<beamline>(bmlPtr)->deep_end(); ++it ) {
      	  (*it)->propagate( *particle );
      	  if(    (0.1 < std::abs(particle->get_x())) 
      	      || (0.1 < std::abs(particle->get_y())) ) {

      	    _p_x_input ->_set_first ( particle->get_x(), particle->get_npx() );
      	    _p_xp_input->_set_second( particle->get_x(), particle->get_npx() );
      	    _p_y_input ->_set_first ( particle->get_y(), particle->get_npy() );
      	    _p_yp_input->_set_second( particle->get_y(), particle->get_npy() );

      	    _p_leftWindow->updateGL();
      	    _p_rightWindow->updateGL();

      	    return;
      	  }
      	}
      }
      _p_leftWindow->updateGL();
      _p_rightWindow->updateGL();

      _p_timer->start( 100, true );
    }
    else {
      _p_x_input ->_set_first ( particle->get_x(), particle->get_npx() ); // Probably should
      _p_xp_input->_set_second( particle->get_x(), particle->get_npx() ); // be done by emitting
      _p_y_input ->_set_first ( particle->get_y(), particle->get_npy() ); // a signal of some
      _p_yp_input->_set_second( particle->get_y(), particle->get_npy() ); // sort.
    }
  }

  // Single shot operation
  // ---------------------
  else
  {
    if( _isIterating ) 
    {
      _p_timer->stop();  // Probably unnecessary

      // ??? This code fragment must be improved.
      // ??? It is written as a hack to prevent
      // ??? the program from hanging if the orbit 
      // ??? diverges.

       for (beamline::deep_iterator it  = boost::const_pointer_cast<beamline>(bmlPtr)->deep_begin();
                                   it != boost::const_pointer_cast<beamline>(bmlPtr)->deep_end(); ++it ) {
        (*it)->propagate( *particle );
        if(    (0.1 < std::abs(particle->get_x())) 
            || (0.1 < std::abs(particle->get_y())) ) {
          _p_leftWindow->updateGL();
          _p_rightWindow->updateGL();

          _isIterating = false;
          _p_startBtn->setOn(false);
          _p_startBtn->setText( "Trace" );

          _p_x_input ->_set_first ( particle->get_x(), particle->get_npx() ); // Probably should
          _p_xp_input->_set_second( particle->get_x(), particle->get_npx() ); // be done by emitting
          _p_y_input ->_set_first ( particle->get_y(), particle->get_npy() ); // a signal of some
          _p_yp_input->_set_second( particle->get_y(), particle->get_npy() ); // sort.

          return;
        }
      }

      _p_leftWindow->updateGL();
      _p_rightWindow->updateGL();

      _isIterating = false;
      _p_timer->stop();  // Probably unnecessary
      _p_startBtn->setOn(false);
      _p_startBtn->setText( "Trace" );

      _p_x_input ->_set_first ( particle->get_x(), particle->get_npx() ); // Probably should
      _p_xp_input->_set_second( particle->get_x(), particle->get_npx() ); // be done by emitting
      _p_y_input ->_set_first ( particle->get_y(), particle->get_npy() ); // a signal of some
      _p_yp_input->_set_second( particle->get_y(), particle->get_npy() ); // sort.
    }
  }
}


void RayTrace::_start_callback()
{
  // char inputBuffer[30];

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
    _p_timer->start( 100, true );
  }
  else {
    _p_startBtn->setText( "Trace" );
  }
}


