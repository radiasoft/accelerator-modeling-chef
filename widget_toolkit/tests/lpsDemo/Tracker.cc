////////////////////////////////////////////////////////////
// 
// File:          Tracker.cc
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
// Thursday. May 24, 2001.    (original version)
// * Originally written for a precursor of chef.
// 
// August, 2002 - March, 2003.
// * Modified slightly and used while studying
//   tomography in longitudinal phase space.
// 
// April - October, 2004.
// * Incremental changes and incorporation into chef.
// 
// Thursday. May 29, 2008.    (this file)
// * Specialized version: probably temporary.
//   * Built by cutting and pasting pieces from older Tracker.cc 
//     and a program called fermiModel.cc
//   * Still needs to be cleaned up.
// 
// Thursday. January 13, 2011.
// * Corrected error in loop in function
//   void Tracker::cnvDataToNormalForm( Vector& vec )
// 
////////////////////////////////////////////////////////////


#include <fstream>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qradiobutton.h>

#define I_PHI 0
#define I_E   1
#define DIM   2

#include <PointEdit.h>
#include <RFMap.h>
#include <DrawSpace2D.h>
#include <Orbit.h>
#include <Tracker.h>

#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>

// This undef is needed because of the compiler.
// #undef connect

extern void twod_normalForm( const Mapping& theMapping, /* input */
                             int            maxOrder,   /* input */
                             MatrixC*       Bptr,
                             CLieOperator*  N,
                             CLieOperator*  T );
extern int filterTransverseTunes( /* const */ MatrixD& mtrx, Vector& nu );
extern int ConvertNtoTunes( MappingC& nu, /* const */ CLieOperator& N );


using namespace AESOP_domain;
using namespace std;

// -----------------------------
// Implementation: class Tracker
// -----------------------------

Tracker::Tracker( RFMap* rfPtr, int theOrder )
  : _state(DIM)
  , _E(DIM,DIM)
  , _invE(DIM,DIM)
  , _order(theOrder)
  , _initialized(false)
  , _p_je(0)
  , _XG(0)
  , _T(0)
  , _f(0)
  , _g(0)
  , _p_rf( rfPtr )
  , _is_iterating(false)
  , _number(1)
  , _deleteContext( false )
  , _p_currOrb(0)
  , _myWheel(0.0)
{
  if(    ( _order != 1 )
      && ( _order != 3 )
      && ( _order != 5 )
      && ( _order != 7 ) ) 
  {
    _order = 1;
  }

  _penPtr = new QPen;
  _penPtr->setColor( white );
  _penPtr->setStyle ( SolidLine );
  _penPtr->setWidth ( 2 );
  
  _myWheel.setIncrement( 195.0 ); 

  // _state(I_PHI) = rfPtr->getPhi_s();
  _state(I_PHI) = 0.0;
  _state(I_E)   = 0.0;

  // Separate child widgets
  this->setSpacing(5);

  // Construct the main menu
  QMenuBar*    myMenuPtr = new QMenuBar( this );

    QPopupMenu*  fileMenu  = new QPopupMenu;
    fileMenu->insertItem( "Exit", qApp, SLOT(quit()) );
  myMenuPtr->insertItem( "File", fileMenu );

    QPopupMenu*  editMenu  = new QPopupMenu;
    editMenu->insertItem( "P.Th. order", this, SLOT(_changeOrder()) );
    editMenu->insertItem( "Clear", this, SLOT(_edit_clear()));
  myMenuPtr->insertItem( "Edit", editMenu );
 
    QPopupMenu*  viewMenu  = new QPopupMenu;
    viewMenu->insertItem( "Rectangular", this, SLOT(_view_rect()) );
    viewMenu->insertItem( "Normal", this, SLOT(_view_norm()) );
    viewMenu->insertItem( "Action Angle", this, SLOT(_view_ActAng()) );
        QPopupMenu*  zoomMenu  = new QPopupMenu;
        zoomMenu->insertItem( "Mouse", this, SLOT(_view_zoom()) );
        zoomMenu->insertItem( "Out", this, SLOT(_view_zoom_out()) );
        zoomMenu->insertItem( "In",  this, SLOT(_view_zoom_in()) );
        zoomMenu->insertItem( "Reset", this, SLOT(_view_zoom_reset()) );
    viewMenu->insertItem( "Zoom", zoomMenu );
  myMenuPtr->insertItem( "View", viewMenu );

  #if 0
    QPopupMenu*  optionMenu = new QPopupMenu;
    optionMenu->insertItem( "P T. order", this, SLOT(_changeOrder()) );
      QPopupMenu* pointsizeMenu = new QPopupMenu;
      pointsizeMenu->insertItem( "Large", this, SLOT(_opt_largePoints()) );
      pointsizeMenu->insertItem( "Small", this, SLOT(_opt_smallPoints()) );
    optionMenu->insertItem( "Point Size", pointsizeMenu );
    optionMenu->insertItem( "Strobe ...", this, SLOT(_opt_setIter()) );
      QPopupMenu* bgColorMenu = new QPopupMenu;
      bgColorMenu->insertItem( "Black",  this, SLOT(_opt_bg_black()) );
      bgColorMenu->insertItem( "Grey",  this, SLOT(_opt_bg_grey()) );
      bgColorMenu->insertItem( "White", this, SLOT(_opt_bg_white()) );
    optionMenu->insertItem( "Background Color", bgColorMenu );
  myMenuPtr->insertItem( "Options", optionMenu );
  #endif

 #if 0
    QPopupMenu*  toolMenu  = new QPopupMenu;
    toolMenu->insertItem( "Periodic Orbit", this, SLOT(_tool_pdicOrb()));
  myMenuPtr->insertItem( "Tools", toolMenu );
  #endif

  // Construct tracking toggle button; align it to left.
  _p_trackBox = new QHBox( this );
  _p_startBtn = new QPushButton( "&Track", _p_trackBox );
  _p_startBtn->setToggleButton( true );
  _p_startBtn->setMaximumWidth( 100 );
  _p_blankLabel = new QLabel( _p_trackBox );
  this->setStretchFactor( _p_blankLabel, 10 );

  // Construct drawing windows ...
  _p_leftWindow = new DrawSpace2D( this, this, "First" );
    _p_leftWindow->show();
    _p_leftWindow->setDraw( DrawSpace2D::drawH_ViewRect );
    _p_leftWindow->setColors( 1., 0., 1. );
    // _p_leftWindow->qglColor( QColor(255,0,255) );
    int drawArea = (40*QApplication::desktop()->height()*
                       QApplication::desktop()->width())/100;
    int fixedWidth = (int) sqrt( (double) drawArea );
    _p_leftWindow->setSize( fixedWidth, fixedWidth );
    // _p_leftWindow->setFixedSize( fixedWidth, fixedWidth );
    // _p_leftWindow->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );
    _p_leftWindow->update();

  // Construct text displays
  _p_numberDisplay = new QHBox( this );

  _p_x_label  = new QLabel   ( " phi - phi_s   ", _p_numberDisplay );
  _p_x_input  = new PointEdit( "0.0"  ,           _p_numberDisplay );
  _p_xp_label = new QLabel   ( " E - E_s "      , _p_numberDisplay );
  _p_xp_input = new PointEdit( "0.0"  ,           _p_numberDisplay );

  _p_x_input ->setMaxLength( 14 );
  _p_xp_input->setMaxLength( 14 );


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

  connect( _p_leftWindow, SIGNAL(_new_point (double,double)),
           _p_x_input,    SLOT  (_set_first (double,double)) );
  connect( _p_leftWindow, SIGNAL(_new_point (double,double)),
           _p_xp_input,   SLOT  (_set_second(double,double)) );

  connect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
           this,          SLOT  (_cnvFromHViewRect(double,double)) );

  connect( _p_startBtn,   SIGNAL(clicked()),
           this,          SLOT(_start_callback()));
}


Tracker::~Tracker()
{
  delete _p_xp_input;
  delete _p_xp_label;
  delete _p_x_input;
  delete _p_x_label;
  delete _p_numberDisplay;

  delete _p_leftWindow;
  delete _p_blankLabel;
  delete _p_startBtn;
  delete _p_trackBox;

  for ( std::list<Orbit*>::iterator it  = _orbits.begin();  
                                    it != _orbits.end();  ++it ) {
    delete (*it);
  }

  if( _XG ) { delete [] _XG;}
  if( _T  ) { delete [] _T;}
  if( _f  ) { delete [] _f; }
  if( _g  ) { delete [] _g; }
}


void Tracker::_file_exit()
{
}


void Tracker::_do_nothing()
{
  QMessageBox::information( this, "Tracker",
                            "Sorry. This function is not implemented." );
}


void Tracker::_edit_clear()
{
  _is_iterating = false;
  _p_currOrb = 0;

  for ( std::list<Orbit*>::iterator it  = _orbits.begin();  
                                    it != _orbits.end();  ++it ) {
    delete (*it);
  }
  _orbits.clear();

  _p_leftWindow->clear();
}


void Tracker::_view_rect()
{
  if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewRect) ) {
    return;
  }

  _p_leftWindow->setDraw( DrawSpace2D::drawH_ViewRect );

  disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewNorm(double,double)) );
  disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewActAng(double,double)) );
     connect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewRect(double,double)) );

  _p_leftWindow  ->setRange( - M_TWOPI, M_TWOPI, -0.02, 0.02 );

  _p_leftWindow->redrawOrbits();
}


void Tracker::_view_norm()
{
  // Normal form stuff

  if( !_initialized ) {
    double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
    Jet__environment::BeginEnvironment(_order);
    // coord phi( _p_rf->getPhi_s() ), E( 0.0 );
    coord phi( 0.0 ), E( 0.0 );
    _p_je = Jet__environment::EndEnvironment(scale);
    JetC__environment::setLastEnv( _p_je ); 
    Mapping M;
  
    M.SetComponent( I_PHI, phi );
    M.SetComponent( I_E,   E   );
  
    (*_p_rf)( M );
  
    MatrixD jac(DIM,DIM);
    jac = M.Jacobian();

    _E = jac.eigenVectors();
    _invE = _E.inverse();

    if( _XG ) { delete [] _XG;}
    _XG = new CLieOperator [_order];
    if( _T  ) { delete [] _T;}
    _T  = new CLieOperator [_order];

    #if 0
    cout << "DGN: " << __FILE__ << "," << __LINE__
         << ": Before twod_normalForm( M, _order, &_E, _XG, _T );"
         << endl;
    cout << _T[3] << endl;
    #endif
    twod_normalForm( M, _order, &_E, _XG, _T );
    #if 0
    cout << "DGN: " << __FILE__ << "," << __LINE__
         << ": After twod_normalForm( M, _order, &_E, _XG, _T );"
         << endl;
    cout << _T[3] << endl;
    #endif

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


  // Widget stuff

  if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewNorm) ) {
    return;
  }
  _p_leftWindow->setDraw( DrawSpace2D::drawH_ViewNorm );

  disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewRect(double,double)) );
  disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewActAng(double,double)) );
     connect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
              this,          SLOT  (_cnvFromHViewNorm(double,double)) );

  _p_leftWindow->setRange( -0.20, 0.20, -0.20, 0.20 );

  _p_leftWindow->redrawOrbits();
}


void Tracker::_view_ActAng()
{
  if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewNorm) ) {
    _p_leftWindow->setDraw( DrawSpace2D::drawH_ViewActAng );

    disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
                this,          SLOT  (_cnvFromHViewRect(double,double)) );
    disconnect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
                this,          SLOT  (_cnvFromHViewNorm(double,double)) );
       connect( _p_leftWindow, SIGNAL(_new_point       (double,double)),
                this,          SLOT  (_cnvFromHViewActAng(double,double)) );

    _p_leftWindow  ->setRange( -M_PI, M_PI, 0.0, 0.04 );
  
    _p_leftWindow->redrawOrbits();
  }
}


void Tracker::_view_zoom()
{
  _p_leftWindow->activateZoom();
}


void Tracker::_view_zoom_out()
{
  _p_leftWindow->multScaleBy( 0.5 );
}


void Tracker::_view_zoom_in()
{
  _p_leftWindow->multScaleBy( 2.0 );
}



void Tracker::_view_zoom_reset()
{
  _p_leftWindow->resetZoom();
}


void Tracker::_opt_largePoints()
{
  _p_leftWindow ->setPointSize( 3 );
  _p_leftWindow->redrawOrbits();
}

void Tracker::_opt_smallPoints()
{
  _p_leftWindow ->setPointSize( 1 );
  _p_leftWindow->redrawOrbits();
}


void Tracker::_opt_setIter()
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


void Tracker::_opt_bg_black()
{
  _p_leftWindow->setClearColor( 0.0, 0.0, 0.0 );
  _p_leftWindow->redrawOrbits();
}

void Tracker::_opt_bg_grey()
{
  _p_leftWindow->setClearColor( 0.552, 0.552, 0.552 );
  _p_leftWindow->redrawOrbits();
}

void Tracker::_opt_bg_white()
{
  _p_leftWindow->setClearColor( 0.960, 0.960, 0.941 );
  _p_leftWindow->redrawOrbits();
}


void Tracker::_tool_pdicOrb()
{
  /*
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb = new QLabel( "Period: ", qhb1 );
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
    int i, j;

    bool ok = true;
    uint iterate = (qle->text()).toInt( &ok );
    if( !ok ) {
      QMessageBox::information( this, 
          "AESOP::Tracker",
          "Sorry: the period was not recognized.\n"
          "Periodic orbit will not be calculated." );
      return;
    }

    Vector w(6);
    w(0) = _bmlConPtr->_proton.State(0); // ??? Change this
    w(3) = _bmlConPtr->_proton.State(3); // ??? Change this

    const uint order = 5;  // ??? Change this
    const uint ul    = 5;  // ??? Change this
  
    Vector z(w);
  

    beamline* bmlPtr = (beamline*) (_bmlConPtr->cheatBmlPtr());
    Jet__environment* storedEnv = Jet::lastEnv;
    double energy = _bmlConPtr->_proton.Energy();
  

    for( int iterCount = 0; iterCount < ul; iterCount++ ) {
      Jet::BeginEnvironment( order );
        coord x(  w(0) ),  y(0.0),  q(0.0),
              px( w(3) ), py(0.0), qq(0.0);
      Jet::EndEnvironment();
  
      Jet xsq;
      xsq = x*x;
  
      JetProton jpr( energy );
      Mapping stuff;
      cout << "Begin n-th Henon iterate for n = "
           << iterate
           << endl;
      for( i = 0; i < iterate; i++ ) {
        cout << "No." << (i+1) << endl;
        bmlPtr->propagate( jpr );
      }
      cout << "End n-th Henon iterate" << endl;
  
      stuff = jpr.State();
  
      MatrixD M(6,6), jac(6,6);
      jac = stuff.Jacobian();
      M(0,0) = jac(0,0);
      M(0,3) = jac(0,3);
      M(3,0) = jac(3,0);
      M(3,3) = jac(3,3);
  
      for( i = 0; i < 6; i++ ) M( i, i ) -= 1.0;
      M = M.inverse();
  
      cout << "M = " << M << endl;
      cout << "w = " << w << endl;
      Vector z(w);
      z = stuff(z);
      cout << "stuff(w) = " << z << endl;
  
      w = w + M*( w - stuff(w) );
  
      cout << w << endl;
    }

    Jet::lastEnv = storedEnv;

  
    // A final test ...

    _bmlConPtr->_proton.setState(w);
  
    cout << "\nTest the results: " << endl;
  
    for( j = 0; j < 10; j++ ) {
      for( i = 0; i < iterate; i++ ) {
        bmlPtr->propagate(_bmlConPtr->_proton);
      }
      cout << _bmlConPtr->_proton.State() << endl;
    }

  }

  delete wpu;
  */
}


void Tracker::_new_x( double x )
{
  _state(I_PHI) = x;
}


void Tracker::_new_xp( double xp )
{
  _state(I_E) = xp;
}


void Tracker::_makeNewOrbit()
{
  if( _is_iterating ) {
    _p_currOrb = new Orbit( _state );
    _myWheel.increment();
    _p_currOrb->setColor( _myWheel.red(), _myWheel.green(), _myWheel.blue() );
    _orbits.push_back( _p_currOrb );
  }
  else {
    _p_currOrb = 0;
  }
}


void Tracker::_cnvFromHViewRect( double a, double b )
{
  _state(I_PHI) = a;
  _state(I_E)   = b;
  _makeNewOrbit();
}


#if 0
void Tracker::_cnvFromHViewNorm( double a, double b )
{
  // !!! ??? To be finished.
  std::complex<double> z( a, b );
  MatrixC s( DIM, 1 );
  s(0,0) = z;  
  s(1,0) = conj(z);
  s = _E*s;
  _state(I_PHI) = real(s(0,0));
  _state(I_E)   = real(s(1,0));
  _makeNewOrbit();
}
#endif

#if 1
void Tracker::_cnvFromHViewNorm( double re_a, double im_a )
{
  static MatrixC u(DIM,1);
  u(0) = std::complex<double>( re_a, im_a );
  u(1) = conj( u(0) );

  std::vector<std::complex<double> > a(DIM);
  static int i, j;

  for( i = _order - 2; i >= 0; --i ) {
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    for( j = 0; j < DIM; j++ ) {
      u(j) = (_g[i](j))(a);
    }
  }

  u = _E*u;

  _state(I_PHI) = real(u(0));
  _state(I_E)   = real(u(1));
}
#endif


void Tracker::_cnvFromHViewActAng( double a, double b )
{
  // !!! ??? To be finished.
}


void Tracker::setState( const Vector& s )
{
  if( s.Dim() != DIM ) {
    cout << "Error 988" << endl;
    exit(988);
  }

  _state = s;
}


void Tracker::cnvDataToNormalForm( Vector& vec )
{
  static MatrixC u(DIM,1);
  std::vector<std::complex<double> > a(DIM);
  static int i, j;

  for( i = 0; i < DIM; i++ ) {
    u(i) = vec(i);
  }

  u = _invE*u;

  for( i = 0; i <= _order - 2; i++ ) {
    for( j = 0; j < DIM; j++ ) {
      a[j] = u(j);
    }
    for( j = 0; j < DIM; j++ ) {
      u(j) = (_f[i](j))(a);
      // u(j) = a[j];
    }
  }

  vec(0) = real(u(0));
  vec(1) = imag(u(0));
}


void Tracker::_iterate()
{
  static QPen localPen;   // ??? I don't like this.  Tracker should not have  ???
                          // ??? to know graphics details.                    ???
  Vector vec_copy(DIM);
  double action, angle;

  while( _is_iterating )
  {
    if( !(_p_currOrb->filled()) && std::abs(_state(I_E)) < 2.0 ) {
      (*_p_rf)(_state);
      _p_currOrb->add( _state );

      QColor orbitColor(   int(255.0*_p_currOrb->Red())
                         , int(255.0*_p_currOrb->Green())
                         , int(255.0*_p_currOrb->Blue())  );
      localPen.setColor( orbitColor );
      localPen.setStyle ( SolidLine );
      localPen.setWidth ( 2 );

      if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewRect) ) {
        _p_leftWindow->mark( _state(I_PHI), _state(I_E), &localPen );
      }
      else if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewNorm ) ) {
        vec_copy = _state;
        this->cnvDataToNormalForm( vec_copy );
        _p_leftWindow->mark( vec_copy(0), vec_copy(1), &localPen );
      }
      else if( _p_leftWindow->ViewIs(DrawSpace2D::drawH_ViewActAng ) ) {
        vec_copy = _state;
        this->cnvDataToNormalForm( vec_copy );
        angle = atan2( vec_copy(1), vec_copy(0) );
        action = vec_copy(0)*vec_copy(0) + vec_copy(1)*vec_copy(1);
        _p_leftWindow->mark( angle, action, &localPen );
      }
    }

    qApp->processEvents();
  }

  _p_x_input ->_set_first ( _state(I_PHI), _state(I_E) );
  _p_xp_input->_set_second( _state(I_PHI), _state(I_E) );
}


void Tracker::_start_callback()
{
  // Reset the colors in the PointEdit objects.
  _p_x_input->unsetPalette();
  _p_x_input->repaint();
  _p_xp_input->unsetPalette();
  _p_xp_input->repaint();
  QApplication::flushX();
  

  _is_iterating = !( _is_iterating );

  if( _is_iterating ) {
    _p_startBtn->setText( "&Stop!" );
    if( 0 == _p_currOrb ) {
      _makeNewOrbit();
      // REMOVE: _p_currOrb = new Orbit( protonPtr->State() );
      // REMOVE: _orbits.append( _p_currOrb );
    }
    this->_iterate();
  }
  else {
    _p_startBtn->setText( "&Track" );
    // ??? REMOVE: Fl::remove_timeout( Tracker::iterate, ae );
  }
}


void Tracker::_changeOrder()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QButtonGroup* qbgPtr = new QButtonGroup( 1, Qt::Vertical, QString("Order"), qvb, 0 );
        QRadioButton* qrbPtr[4];
        char* const arrgh [] = { "1", "3", "5", "7" };
        for( int i = 0; i < 4; ++i ) {
          qrbPtr[i] = new QRadioButton( QString( arrgh[i] ), qbgPtr );
        }
      qbgPtr->setFrameStyle(QFrame::NoFrame);
      qbgPtr->setRadioButtonExclusive(true);
      qbgPtr->setButton( qbgPtr->id( qrbPtr[(_order -1)/2] ) );
      // Note: This assumes _order is 1, 3, 5, or 7.
      qbgPtr->adjustSize();
    
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb2 );
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

  wpu->adjustSize();
  wpu->setFixedSize( wpu->width(), wpu->height() );

  int returnCode = wpu->exec();

  int newOrder = _order;
  if( returnCode == QDialog::Accepted ) {
    QButton* selectedPtr = qbgPtr->selected();
    for( int i = 0; i < 4; ++i ) {
      if( selectedPtr == qrbPtr[i] ) {
        newOrder = 2*i + 1;
        break;
      }
    }
  }

  delete wpu;

  if( _order != newOrder ) { 
    _order = newOrder;
    _initialized = false;
    this->_view_rect();
  }
}


bool Tracker::isIterating()
{
  return _is_iterating;
}


#undef I_PHI
#undef I_E
#undef DIM
