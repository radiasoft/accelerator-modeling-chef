/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      RayTrace.cc
******  Version:   3.1
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

#include "GenericException.h"
#include "RayTrace.h"
#include "PointEdit.h"
#include "Tracker.h"
#include "QtMonitor.h"

#include "Particle.h"  // This line should not be necessary!!!
#include "beamline.h"
#include "BeamlineContext.h"


// This undef is needed because of the compiler.
#undef connect


using namespace CHEF_domain;
using namespace std;

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
: QGLWidget(parent), _topRayTrace(p), _r(1.0), _g(1.0), _b(1.0),
  _xLo( -0.0020 ), _xHi( 0.0020 ), _yLo( -0.0020 ), _yHi( 0.0020 ),
  _myWheel(0.0), _pointSize(3)
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


void RayDrawSpace::drawH_ViewRect( RayDrawSpace* x )
{
  slist_iterator getNext( x->_topRayTrace->_history );
  
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
      delete ((Ray*)((x->_topRayTrace->_history).get()));
    }
  }

  m = ( x->_topRayTrace->_history ).size() / n;
  for( i = 0; i < m-1; i++ ) {
    glColor3f( ((i+1)*(x->_r))/m, 
               ((i+1)*(x->_g))/m, 
               ((i+1)*(x->_b))/m );
    glBegin( GL_LINE_STRIP );
    for( j = 0; j < n; j++ ) {
      q = (Ray*) getNext();
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

  // The last ray is painted white.
  if( m > 0 ) {
    glColor3f( 1, 1, 1 );
    glLineWidth(3);
    glBegin( GL_LINE_STRIP );
    for( j = 0; j < n; j++ ) {
      q = (Ray*) getNext();
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
  slist_iterator getNext( x->_topRayTrace->_history );

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
      delete ((Ray*)((x->_topRayTrace->_history).get()));
    }
  }

  m = ( x->_topRayTrace->_history ).size() / n;
  for( i = 0; i < m-1; i++ ) {
    glColor3f( ((i+1)*(x->_r))/m, 
               ((i+1)*(x->_g))/m, 
               ((i+1)*(x->_b))/m );
    glBegin( GL_LINE_STRIP );
    for( j = 0; j < n; j++ ) {
      q = (Ray*) getNext();
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

  // The last ray is painted white.
  if( m > 0 ) {
    glColor3f( 1, 1, 1 );
    glLineWidth(3);
    glBegin( GL_LINE_STRIP );
    for( j = 0; j < n; j++ ) {
      q = (Ray*) getNext();
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


void RayDrawSpace::mousePressEvent( QMouseEvent* qme )
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

RayTrace::RayTrace( BeamlineContext* bmlCP, QWidget* parent, const char* name, WFlags f )
: QVBox( parent, name, f),
  _p_info(0), 
  _number(1),
  _maxHistory(64),
  _bmlConPtr( bmlCP ), 
  _deleteContext( false ),
  _isIterating(0)
{
  this->_finishConstructor();
}


RayTrace::RayTrace( /* const */ beamline* x, QWidget* parent, const char* name, WFlags f )
: QVBox(parent, name, f),
  _p_info(0), 
  _number(1),
  _maxHistory(64),
  _bmlConPtr( 0 ), 
  _deleteContext( true ),
  _isIterating(0) 
{
  if( 0 == x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "RayTrace::RayTrace( /* const */ beamline* x )", 
           "null argument passed to constructor." ) );
  }

  _bmlConPtr = new BeamlineContext( false, x );
  this->_finishConstructor();
}


void RayTrace::_finishConstructor()
{
  // Initialize the QtMonitors
  _n_monitor = QtMonitor::setAzimuth( _bmlConPtr->cheatBmlPtr() );
  if( 0 == _n_monitor ) {
    cerr << "\n*** WARNING *** "
         << "\n*** WARNING *** " << __FILE__ << ", Line " << __LINE__
         << "\n*** WARNING *** RayTrace::_finishConstructor()"
         << "\n*** WARNING *** No QtMonitors in beamline "
         << _bmlConPtr->name()
         << "\n*** WARNING *** \n"
         << endl;
  }

  DeepBeamlineIterator dbi( _bmlConPtr->cheatBmlPtr() );
  bmlnElmnt* q;
  while((  q = dbi++  )) {
    if( typeid(*q) ==  typeid(QtMonitor) ) {
      QObject::connect( dynamic_cast<const QtMonitor*>(q),    
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
      QPopupMenu*  viewMenu  = new QPopupMenu;
      viewMenu->insertItem( "Rectangular", this, SLOT(_view_rect()) );
        QPopupMenu*  zoomMenu  = new QPopupMenu;
        zoomMenu->insertItem( "Out", this, SLOT(_view_zoom_out()) );
        zoomMenu->insertItem( "In",  this, SLOT(_view_zoom_in()) );
        zoomMenu->insertItem( "Reset", this, SLOT(_view_zoom_reset()) );
      viewMenu->insertItem( "Zoom", zoomMenu );
    optionMenu->insertItem( "View", viewMenu );
    //   ----------------------
    //   QPopupMenu* pointsizeMenu = new QPopupMenu;
    //   pointsizeMenu->insertItem( "Large", this, SLOT(_opt_largePoints()) );
    //   pointsizeMenu->insertItem( "Small", this, SLOT(_opt_smallPoints()) );
    // optionMenu->insertItem( "Point Size", pointsizeMenu );
    // ---------------------------------------------
    optionMenu->insertItem( "Maximum Traces", this, SLOT(_opt_setHistory()) );
    optionMenu->insertItem( "Strobe", this, SLOT(_opt_setIter()) );
      QPopupMenu* bgColorMenu = new QPopupMenu;
      bgColorMenu->insertItem( "Black",  this, SLOT(_opt_bg_black()) );
      bgColorMenu->insertItem( "White",  this, SLOT(_opt_bg_white()) );
      bgColorMenu->insertItem( "Yellow", this, SLOT(_opt_bg_yellow()) );
    optionMenu->insertItem( "Background Color", bgColorMenu );
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

  connect( _p_startBtn,   SIGNAL(clicked()),
           this,          SLOT(_start_callback()));

  connect( _p_timer, SIGNAL(timeout()),
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

  Ray* q;
  while(( q = (Ray*) _history.get() )) {
    delete q;
  }

  if(_deleteContext) { delete _bmlConPtr; }
}


void RayTrace::_file_exit()
{
}


void RayTrace::_fileClose()
{
  delete this;
}


void RayTrace::_do_nothing()
{
  QMessageBox::information( this, "RayTrace",
                            "Sorry. This function is not implemented." );
}


void RayTrace::_edit_clear()
{
  _isIterating = 0;

  Ray* q;
  while(( q = (Ray*) _history.get() )) {
    delete q;
  }

  _p_leftWindow->updateGL();
  _p_rightWindow->updateGL();
}


void RayTrace::_view_rect()
{
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
        QLabel* qlb = new QLabel( "Maximum number of traces", qhb1 );
        QString stl;
        stl.setNum( _maxHistory );
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
        QLabel* qlb = new QLabel( "Strobe period", qhb1 );
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


void RayTrace::_new_x( double x )
{
  _bmlConPtr->_proton.set_x(x);
}


void RayTrace::_new_xp( double xp )
{
  _bmlConPtr->_proton.set_npx(xp);
}


void RayTrace::_new_y( double y )
{
  _bmlConPtr->_proton.set_y(y);
}


void RayTrace::_new_yp( double yp )
{
  _bmlConPtr->_proton.set_npy(yp);
}


void RayTrace::_appendToHistory( double az, const Vector& state )
{
  Ray* rayPtr = new Ray;
  rayPtr->s = az;
  rayPtr->z = state;
  _history.append( rayPtr );
}


void RayTrace::setState( const Vector& s )
{
  if( s.Dim() == 6 ) {
    _bmlConPtr->_proton.setState( s );
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
  static beamline* bmlPtr;
  static Proton*   protonPtr;
  bmlPtr = (beamline*) (_bmlConPtr->cheatBmlPtr());
  protonPtr = &(_bmlConPtr->_proton);

  if( _isIterating ) 
  {
    for( int i = 0; i < _number; i++ ) {
      bmlPtr->propagate( *protonPtr );
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


