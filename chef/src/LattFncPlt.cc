/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      LattFncPlt.cc
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


#include <fstream>
#include <iomanip>
#include <string>   // needed for strcat
#include <math.h>

#include <qapp.h>
#include <qpalette.h>
#include <qlabel.h>
#include <qprinter.h>
#include <qpdevmet.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>

#include <qwt/qwt_plot_printfilter.h>
#include <qwt/qwt_counter.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_math.h>
#include <qwt/qwt_autoscl.h>

#include "complexAddon.h"
#include "bmlfactory.h"
#include "LattFncPlt.h"
#include "BeamlineContext.h"

// This undef is needed because of the compiler.
#undef connect

using namespace CHEF_domain;
using namespace std;

QString zoomInfo("Zoom: Press mouse button and drag");
QString cursorInfo("Cursor Pos: Press mouse button in plot region");


// External variable that has to be defined for the MAD parser
// Already defined in CHEF.cc. This is a problem!!
// madparser* mp = 0;


LattFncPlt::InputFrame::InputFrame(QWidget *p, const char* name)
: QFrame(p, name)
{
    _lbl_H_Tune = new QLabel( QString("Tunes:    Horizontal:"), this);
    _lbl_V_Tune = new QLabel( QString("Vertical:"), this);
    lblInfo = new QLabel(cursorInfo, this);
    lblInfo->setFont(QFont("Helvetica", 8));

    _lbl_H_Tune->setGeometry( 20, 20, 250, 20);
    _lbl_V_Tune->setGeometry(280, 20, 150, 20);
    lblInfo->setGeometry(20, 5, 230, 10);

}

LattFncPlt::InputFrame::~InputFrame()
{
  delete _lbl_H_Tune;
  delete _lbl_V_Tune;
  delete lblInfo;
}



LattFncPlt::LattFncPlt( BeamlineContext* bcp, QWidget* p, const char* name )
: QWidget( p, name ),  
  _bmlConPtr(bcp), _deleteContext(false),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _psi_H(0),   _psi_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0)
{
  this->_finishConstructor();
}


LattFncPlt::LattFncPlt( /* const */ beamline* pBml, QWidget* p , const char* name )
: QWidget( p, name ),
  _bmlConPtr(0), _deleteContext(true),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _psi_H(0),   _psi_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0)
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor();
}

void LattFncPlt::_finishConstructor()
{
  const beamline* bmlPtr = _bmlConPtr->cheatBmlPtr();

  // Create the lattice function arrays.
  _arraySize   = bmlPtr->countHowManyDeeply();
  _azimuth     = new double[_arraySize];
  _beta_H      = new double[_arraySize];
  _alpha_H     = new double[_arraySize];
  _psi_H       = new double[_arraySize];
  _beta_V      = new double[_arraySize];
  _alpha_V     = new double[_arraySize];
  _psi_V       = new double[_arraySize];
  _inv_beta_H  = new double[_arraySize];
  _inv_beta_V  = new double[_arraySize];
  _root_beta_H = new double[_arraySize];
  _root_beta_V = new double[_arraySize];
  _disp_H      = new double[_arraySize];
  _disp_V      = new double[_arraySize];


  // Now do the widget stuff.
    d_zoomActive = d_zoom = 0;
    
    _myMenuPtr = new QMenuBar( this );

      _fileMenu = new QPopupMenu;
      _fileMenu->insertItem( "Save as...", this, SLOT(_fileSaveAs()) );
      _fileMenu->insertItem( "Print...",   this, SLOT(_print()) );
      _fileMenu->insertSeparator();
      _fileMenu->insertItem( "Close", this, SLOT(_fileClose()) );
    _myMenuPtr->insertItem( "File", _fileMenu );
    
      _viewMenu = new QPopupMenu;
      _zoomItemID = _viewMenu->insertItem( "Zoom", this, SLOT(_zoom()) );
        _varMenu = new QPopupMenu;
        _varMenu->insertItem( "beta",       this, SLOT(_setBetaPlot()) );
        _varMenu->insertItem( "1/beta",     this, SLOT(_setInvPlot())  );
        _varMenu->insertItem( "sqrt(beta)", this, SLOT(_setRootPlot()) );
      _viewMenu->insertItem( "Variables", _varMenu );
    _myMenuPtr->insertItem( "View", _viewMenu );


    char theTitle[1024];
    for( int i = 0; i < 1024; i++ ) {
      theTitle[i] = '\0';
    }
    strcat( theTitle, "Lattice functions for " );
    strcat( theTitle, _bmlConPtr->name() );

    plt = new QwtPlot( theTitle, this );
    
    plt->enableAxis( QwtPlot::yRight, true );
    // Set the bottom of the beta axis to zero.
    plt->setAxisReference( QwtPlot::yLeft, 0.0 );
    // plt->d_as[QwtPlot::yLeft].setOptions( QwtAutoscale::IncludeRef );
    plt->changeAxisOptions( QwtPlot::yLeft, QwtAutoScale::IncludeRef, true );

    crv1 = plt->insertCurve( "H Beta" );
    crv2 = plt->insertCurve( "V Beta" );
    crv3 = plt->insertCurve( "H Dispersion" );
    crv4 = plt->insertCurve( "V Dispersion" );

    plt->setCurvePen(crv1, QPen(black,1));
    plt->setCurvePen(crv2, QPen(red,1));
    plt->setCurvePen(crv3, QPen(blue,1));
    plt->setCurvePen(crv4, QPen(darkGreen,1));
    // plt->setCurvePen(crv1, QPen(black,3));
    // plt->setCurvePen(crv2, QPen(red,3));

    plt->enableGridX(true);
    plt->enableGridXMin(false);
    plt->enableGridY(true);
    plt->enableGridYMin(false);
    // plt->setGridMajPen(QPen(white, 0, DotLine));
    // plt->setGridMinPen(QPen(gray, 0 , DotLine));

    plt->setAxisTitle(QwtPlot::xBottom, "azimuth [m]");
    plt->setAxisTitle(QwtPlot::yLeft,   "beta [m]");
    plt->setAxisTitle(QwtPlot::yRight,  "dispersion [m]");

    // plt->setAxisScale( QwtPlot::xBottom, 0.0, 40.0, 2.0 );
    
    plt->setCurveYAxis(crv1, QwtPlot::yLeft);
    plt->setCurveYAxis(crv2, QwtPlot::yLeft);
    plt->setCurveYAxis(crv3, QwtPlot::yRight);
    plt->setCurveYAxis(crv4, QwtPlot::yRight);
    
    mrk1 = plt->insertMarker();
    plt->setMarkerLineStyle(mrk1, QwtMarker::VLine);
    plt->setMarkerPos(mrk1, 0.0,0.0);
    plt->setMarkerLabelAlign(mrk1, AlignRight|AlignBottom);
    plt->setMarkerPen(mrk1, QPen(green, 0, DashDotLine));
    plt->setMarkerFont(mrk1, QFont("Helvetica", 10, QFont::Bold));

    mrk2 = plt->insertLineMarker("", QwtPlot::yLeft);
    plt->setMarkerLabelAlign(mrk2, AlignRight|AlignBottom);
    plt->setMarkerPen(mrk2, QPen(QColor(200,150,0), 0, DashDotLine));
    plt->setMarkerFont(mrk2, QFont("Helvetica", 10, QFont::Bold));
    plt->setMarkerSymbol(mrk2, QwtSymbol(QwtSymbol::Diamond, yellow, green, QSize(7,7)));
    
    plt->setCanvasBackground(white);
    plt->enableLegend(TRUE);
    plt->setLegendPos(Qwt::Bottom);
    plt->setLegendFrameStyle(QFrame::Box|QFrame::Sunken);

    plt->enableOutline(TRUE);
    plt->setOutlinePen(green);

    plt->adjustSize();


    frmInp = new InputFrame(this);
    frmInp->setFrameStyle(QFrame::Panel|QFrame::Raised);
    frmInp->setLineWidth(2);

    QString qstr;
    qstr.setNum( _currentTune[0] );
    frmInp->_lbl_H_Tune->setText( QString("Tunes:    Horizontal: ") + qstr );
    qstr.setNum( _currentTune[1] );
    frmInp->_lbl_V_Tune->setText( QString("Vertical: ") + qstr );


    connect(plt, SIGNAL(plotMouseMoved(const QMouseEvent&)),
            SLOT(plotMouseMoved( const QMouseEvent&)));
    connect(plt, SIGNAL(plotMousePressed(const QMouseEvent &)),
            SLOT(plotMousePressed( const QMouseEvent&)));
    connect(plt, SIGNAL(plotMouseReleased(const QMouseEvent &)),
            SLOT(plotMouseReleased( const QMouseEvent&)));
    

    this->adjustSize();


  // Finally, do the calculation ...
  recalc();
  // _setBetaPlot();
}


LattFncPlt::~LattFncPlt()
{
  delete plt;
  delete frmInp;
  delete _fileMenu;
  delete _varMenu;
  delete _viewMenu;
  delete _myMenuPtr;
  
  delete [] _azimuth;
  delete [] _beta_H;
  delete [] _alpha_H;
  delete [] _psi_H;
  delete [] _beta_V;
  delete [] _alpha_V;
  delete [] _psi_V;
  delete [] _inv_beta_H;
  delete [] _inv_beta_V;
  delete [] _root_beta_H;
  delete [] _root_beta_V;
  delete [] _disp_H;
  delete [] _disp_V;

  if(_deleteContext) { delete _bmlConPtr; }
}



void LattFncPlt::_fileClose()
{
  delete this;
}


void LattFncPlt::_fileSaveAs()
{
  QString fileName = QFileDialog::getSaveFileName(
    (QString(_bmlConPtr->name())+QString(".txt")),
    "Text Files (*.txt);;Data files (*.dat)",
    this,
    0,
    QString("CHEF: Lattice Functions.") );
  if( fileName.isNull() ) {
    QMessageBox::information( this, "CHEF: Lattice Functions.",
                              "Sorry. It didn't work." );
  }
  else {
    int i = 0;
    const bmlnElmnt* q;
    ofstream outputFileStream( fileName );
    outputFileStream << setiosflags(ios::fixed);
    DeepBeamlineIterator dbi(_bmlConPtr->cheatBmlPtr());

    if( fileName.right(4) == ".dat" ) {
      // Write the data ...
      i = 0;
      while((  q = dbi++  )) {
        if( i >= _arraySize ) {
          cerr << "*** WARNING ***                               \n"
                  "*** WARNING *** LattFncPlt::_fileSaveAs       \n"
                  "*** WARNING *** Array bounds exceeded: value = "
               << i
               << ".\n"
                  "*** WARNING ***                               \n"
             << endl;
          return;
        }
        else {
          outputFileStream << setw(10) << setprecision(3) << _azimuth[i]
                           << setw(10) << setprecision(3) << _beta_H[i]
                           << setw(10) << setprecision(3) << _beta_V[i]
                           << setw(10) << setprecision(3) << _disp_H[i]
                           << endl;
        }
        i++;
      }
    }

    else if( fileName.right(4) == ".txt" ) {
      // Write the labels ...
      outputFileStream << setw(15) << "Element"
                       << setw(15) << "Name"
                       << setw(10) << "Azimuth"
                       << setw(10) << "Hor.Beta"
                       << setw(10) << "Hor.Alpha"
                       << setw(10) << "Hor.Psi"
                       << setw(10) << "Hor.Disp"
                       << setw(10) << "Ver.Beta"
                       << setw(10) << "Ver.Alpha"
                       << setw(10) << "Ver.Psi"
                       << setw(10) << "Ver.Disp"
                       << endl;
      for( i = 0; i < (2*15+7*10); i++ ) {
        outputFileStream << "-";
      }
      outputFileStream << endl;
    
      // Write the data ...
      i = 0;
      while((  q = dbi++  )) {
        if( i >= _arraySize ) {
          cerr << "*** WARNING ***                               \n"
                  "*** WARNING *** LattFncPlt::_fileSaveAs          \n"
                  "*** WARNING *** Array bounds exceeded: value = "
               << i
               << ".\n"
                  "*** WARNING ***                               \n"
             << endl;
          return;
        }
        else {
          outputFileStream << setw(15) << q->Type() 
                           << setw(15) << q->Name()
                           << setw(10) << setprecision(3) << _azimuth[i]
                           << setw(10) << setprecision(3) << _beta_H[i]
                           << setw(10) << setprecision(3) << _alpha_H[i]
                           << setw(10) << setprecision(3) << (_psi_H[i]/M_TWOPI)
                           << setw(10) << setprecision(3) << _disp_H[i]
                           << setw(10) << setprecision(3) << _beta_V[i]
                           << setw(10) << setprecision(3) << _alpha_V[i]
                           << setw(10) << setprecision(3) << (_psi_V[i]/M_TWOPI)
                           << setw(10) << setprecision(3) << _disp_V[i]
                           << endl;
        }
        i++;
      }
    }

    outputFileStream.close();
  }
}


void LattFncPlt::_do_nothing()
{
  QMessageBox::information( this, "CHEF: Lattice Functions.",
                            "Sorry. This function is not implemented." );
  // cerr << "LattFncPlt::_do_nothing() invoked." << endl;
}



void LattFncPlt::_print()
{

    // ??? Why is this here??? QBusrh br(red);
    // ??? Why is this here??? QPen pn(yellow);
    // ??? Why is this here??? 
    // ??? Why is this here??? QwtSymbol sym1;   
    // ??? Why is this here??? sym1.setBrush(br);
    // ??? Why is this here??? sym1.setPen(pn);
    // ??? Why is this here??? sym1.setSize(11);

    QPrinter p;

    if ( p.setup( /* parent = */ 0 ) )
    {
      plt->print( p );
    }
    else {
      cerr << "*** WARNING ***                               \n"
              "*** WARNING *** LattFncPlt::_print               \n"
              "*** WARNING *** QPrinter::setup() returned    \n"
              "*** WARNING *** a value of false.             \n"
              "*** WARNING ***                               \n"
           << endl;
    }
}

void LattFncPlt::_zoom()
{
    if (d_zoomActive)
    {
        // Disable Zooming.
        plt->setAxisAutoScale(QwtPlot::yLeft);
        plt->setAxisAutoScale(QwtPlot::xBottom);
        plt->replot();
        d_zoom = false;
        d_zoomActive = 0;
    }
    else {
       d_zoom = !d_zoom;
    }
    
    if (d_zoom)
    {
        _viewMenu->changeItem( _zoomItemID, QString("Unzoom") );
        frmInp->lblInfo->setText(zoomInfo);
    }
    else
    {
        _viewMenu->changeItem( _zoomItemID, QString("Zoom") );
        frmInp->lblInfo->setText(cursorInfo);
    }
    
}


void LattFncPlt::_setBetaPlot()
{
  _plotType = betaPlot;
  plt->setCurveData( crv1, _azimuth, _beta_H, _arraySize );
  plt->setCurveData( crv2, _azimuth, _beta_V, _arraySize );
  plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
  plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
  plt->setAxisTitle(QwtPlot::yLeft,  "beta  [m]"     );
  plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
  plt->replot();
}

void LattFncPlt::_setInvPlot()
{
  _plotType = invPlot;
  plt->setCurveData( crv1, _azimuth, _inv_beta_H, _arraySize );
  plt->setCurveData( crv2, _azimuth, _inv_beta_V, _arraySize );
  plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
  plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
  plt->setAxisTitle(QwtPlot::yLeft, "1/beta  [m**-1]");
  plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
  plt->replot();
}

void LattFncPlt::_setRootPlot()
{
  _plotType = rootPlot;
  plt->setCurveData( crv1, _azimuth, _root_beta_H, _arraySize );
  plt->setCurveData( crv2, _azimuth, _root_beta_V, _arraySize );
  plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
  plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
  plt->setAxisTitle(QwtPlot::yLeft, "root(beta)  [m**1/2]");
  plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
  plt->replot();
}


void LattFncPlt::plotMouseMoved(const QMouseEvent &e)
{
    
    // Not used ??? QPainter p;
    // QString lbl = "s = ";
    // QString lbl2;

    static QString lbl2;
    static QString lbl;
    static int     xpos;
    static int     ypos;

    xpos = e.pos().x();
    ypos = e.pos().y();

    if( xpos < 0 ) xpos = 0;
    if( ypos < 0 ) ypos = 0;
    if( xpos > plt->width() )  xpos = plt->width();
    if( ypos > plt->height() ) ypos = plt->height();

    lbl = "s = ";
    
    lbl2.setNum(plt->invTransform(QwtPlot::xBottom, xpos ), 'g', 3);

    if( _plotType == betaPlot ) {
      lbl += lbl2 + ",  beta = ";
    }
    else if( _plotType == invPlot ) {
      lbl += lbl2 + ",  1/beta = ";
    }
    else {
      lbl += lbl2 + ",  root(beta) = ";
    }
    
    lbl2.setNum(plt->invTransform(QwtPlot::yLeft, ypos ), 'g', 3);
    lbl += lbl2;
    
    frmInp->lblInfo->setText(lbl);
}


void LattFncPlt::plotMousePressed(const QMouseEvent &e)
{
    // store position
    p1 = e.pos();
    
    // update cursor pos display
    plotMouseMoved(e);
    
    if (d_zoom && (!d_zoomActive))
    {
        plt->setOutlineStyle(Qwt::Rect); 
    }
    else
    {
        plt->setOutlineStyle(Qwt::Cross);
    } 
    
}

void LattFncPlt::plotMouseReleased(const QMouseEvent &e)
{
    int x1, x2, y1, y2;
    int lim;
    
    // some shortcuts
    int axl= QwtPlot::yLeft, axb= QwtPlot::xBottom;
    
    if (d_zoom && (!d_zoomActive))
    {
        d_zoomActive = 1;
        
        // Don't invert any scales which aren't inverted
        x1 = qwtMin(p1.x(), e.pos().x());
        x2 = qwtMax(p1.x(), e.pos().x());
        y1 = qwtMin(p1.y(), e.pos().y());
        y2 = qwtMax(p1.y(), e.pos().y());
        
        // limit selected area to a minimum of 11x11 points
        lim = 5 - (y2 - y1) / 2;      // Why not y2 - y1 + 1???
        if (lim > 0)
        {
            y1 -= lim;
            y2 += lim;
        }
        lim = 5 - (x2 - x1 + 1) / 2;
        if (lim > 0)
        {
            x1 -= lim;
            x2 += lim;
        }
        
        // Set fixed scales
        plt->setAxisScale(axl, plt->invTransform(axl,y1), plt->invTransform(axl,y2));
        plt->setAxisScale(axb, plt->invTransform(axb,x1), plt->invTransform(axb,x2));
        plt->replot();
        

    }

    frmInp->lblInfo->setText(cursorInfo);
    plt->setOutlineStyle(Qwt::Triangle);
}



void LattFncPlt::recalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalFracTune();
  _currentTune[1] = _bmlConPtr->getVerticalFracTune();


  {
  QString qstr;
  qstr.setNum( _currentTune[0] );
  frmInp->_lbl_H_Tune->setText( QString("Tunes:    Horizontal: ") + qstr );
  qstr.setNum( _currentTune[1] );
  frmInp->_lbl_V_Tune->setText( QString("Vertical: ") + qstr );
  }

  int i = 0;
  const LattFuncSage::lattFunc* infoPtr = _bmlConPtr->getLattFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      QMessageBox::information( this, "CHEF: Lattice Functions.",
          "Too many lattice functions." );
      i = _arraySize;
      infoPtr = 0;
    }
    else {
      _azimuth[i]      = infoPtr->arcLength;
      _beta_H[i]       = infoPtr->beta.hor;
      _alpha_H[i]      = infoPtr->alpha.hor;
      _psi_H[i]        = infoPtr->psi.hor;
      _beta_V[i]       = infoPtr->beta.ver;
      _alpha_V[i]      = infoPtr->alpha.ver;
      _psi_V[i]        = infoPtr->psi.ver;
      _inv_beta_H[i]   = 1.0/infoPtr->beta.hor;
      _inv_beta_V[i]   = 1.0/infoPtr->beta.ver;
      _root_beta_H[i]  = sqrt(infoPtr->beta.hor);
      _root_beta_V[i]  = sqrt(infoPtr->beta.ver);
      _disp_H[i]       = infoPtr->dispersion.hor;
      _disp_V[i]       = infoPtr->dispersion.ver;

      infoPtr = _bmlConPtr->getLattFuncPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "LattFncPlt::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;


  // copies data (less efficient, but safe)
  if( _plotType == betaPlot ) {
    plt->setCurveData( crv1, _azimuth, _beta_H, _arraySize );
    plt->setCurveData( crv2, _azimuth, _beta_V, _arraySize );
    plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
    plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
  }
  else if( _plotType == invPlot ) {
    plt->setCurveData( crv1, _azimuth, _inv_beta_H, _arraySize );
    plt->setCurveData( crv2, _azimuth, _inv_beta_V, _arraySize );
    plt->setCurveData( crv3, _azimuth, _disp_H,     _arraySize );
    plt->setCurveData( crv4, _azimuth, _disp_V,     _arraySize );
  }
  else {
    plt->setCurveData( crv1, _azimuth, _root_beta_H, _arraySize );
    plt->setCurveData( crv2, _azimuth, _root_beta_V, _arraySize );
    plt->setCurveData( crv3, _azimuth, _disp_H,      _arraySize );
    plt->setCurveData( crv4, _azimuth, _disp_V,      _arraySize );
  }

  // attaches data (more efficient, but dangerous)
  // plt->setCurveRawData(crv2, frequency, phase, ArraySize);

  plt->replot();
}


void LattFncPlt::resizeEvent(QResizeEvent *e)
{
    QRect r( 0, 
             _myMenuPtr->height(), 
             e->size().width(), 
             e->size().height() - 50 - _myMenuPtr->height()
           );
    
    plt->setGeometry(r);
    frmInp->setGeometry(0, r.bottom() + 1, r.width(), 50);
    
}
