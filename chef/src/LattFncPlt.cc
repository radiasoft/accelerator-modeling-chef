/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      LattFncPlt.cc
******  Version:   4.0
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

#include "complexAddon.h"
#include "bmlfactory.h"
#include "LattFncPlt.h"
#include "BeamlineContext.h"

#include <qapplication.h>
#include <qwt/qwt_plot.h>
#include "chefplotmain.h"

// This undef is needed because of the compiler.
#undef connect

using namespace CHEF_domain;
using namespace std;

// External variable that has to be defined for the MAD parser
// Already defined in CHEF.cc. This is a problem!!
// madparser* mp = 0;


LattFncPlt::LattFncPlt( BeamlineContext* bcp, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(bcp), _deleteContext(false),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _psi_H(0),   _psi_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0), 
  _name(0)
{
  this->_finishConstructor( name );
}


LattFncPlt::LattFncPlt( /* const */ beamline* pBml, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(0), _deleteContext(true),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _psi_H(0),   _psi_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0), 
  _name(0)
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor( name );
}

void LattFncPlt::_finishConstructor( const char* name )
{
  if( 0 != name ) {
    int len;
    if( 0 < ( len = strlen(name) ) ) {
      _name = new char[len+1];
      strcpy( _name, name );
    }
  }

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

  _recalc();


  // Convert to "curves"
  CHEFCurve* c1 
    = new CHEFCurve( _azimuth, _beta_H, _arraySize, "Horizontal Beta" );
  CHEFCurve* c2 
    = new CHEFCurve( _azimuth, _beta_V, _arraySize, "Vertical Beta" );
  CHEFCurve* c3 
    = new CHEFCurve( _azimuth, _disp_H, _arraySize, "Horizontal Dispersion" );
  CHEFCurve* c4 
    = new CHEFCurve( _azimuth, _disp_V, _arraySize, "Vertical Dispersion" );

  c1->setPen( QPen( "blue", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "blue", 1, Qt::SolidLine ) );
  c3->setPen( QPen( "red",  0, Qt::SolidLine ) );
  c4->setPen( QPen( "red",  0, Qt::SolidLine ) );

  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c3->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c4->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  // c4->setStyle( QwtCurve::NoCurve ); // this turns off a curve

  _di = new CHEFPlotData;
  _di->addCurve( c1 );
  _di->addCurve( c2 );
  _di->addCurve( c3 );
  _di->addCurve( c4 );
  // *_di now has the responsibility of deleting
  // the curves when it is destroyed

  _di->setXLabel( "Arc Length [m]"                  );
  _di->setYLabel( "Beta [m]",       QwtPlot::yLeft  );
  _di->setYLabel( "Dispersion [m]", QwtPlot::yRight );

  _di->setScaleMagRight( 5.0 ); // ??? fix this!
  _di->setBeamline( bmlPtr ); // ??? What if *bmlPtr is deleted?


  // Prepare the plotting widget
  _plotter = new CHEFPlotMain(0, 0, Qt::WDestructiveClose );
  // This will delete itself using a "File/Close" menu item.

  char theTitle[1024];
  for( int i = 0; i < 1024; i++ ) {
    theTitle[i] = '\0';
  }
  strcat( theTitle, "Uncoupled lattice functions: " );
  strcat( theTitle, _bmlConPtr->name() );

  _plotter->setCaption( theTitle );

  _plotter->addData( *(_di) );
  _plotter->resize( 85*(QApplication::desktop()->width())/100,
                    85*(QApplication::desktop()->width())/200  );

  _plotter->show();
}


LattFncPlt::~LattFncPlt()
{
  std::cout << "DGN: Inside LattFncPlt::~LattFncPlt()" << std::endl;
  if( _name ) { delete [] _name; }

  delete    _di;
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
  std::cout << "DGN: Leaving LattFncPlt::~LattFncPlt()" << std::endl;
}


void LattFncPlt::_recalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalFracTune();
  _currentTune[1] = _bmlConPtr->getVerticalFracTune();


  int i = 0;
  const LattFuncSage::lattFunc* infoPtr = _bmlConPtr->getLattFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void LattFncPlt::_recalc()"
           "\n*** WARNING *** Too many lattice functions read."
           "\n*** WARNING *** Am resetting to " << _arraySize << " in all."
        << "\n*** WARNING *** "
        << endl;
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
    *(_errorStreamPtr) 
         << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "LattFncPlt::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


void LattFncPlt::setCaption( const char* cpt )
{
  _plotter->setCaption( cpt );
}


void LattFncPlt::_selfDestruct()
{
  delete this;
}


// void LattFncPlt::_fileSaveAs()
// {
//   QString fileName = QFileDialog::getSaveFileName(
//     (QString(_bmlConPtr->name())+QString(".txt")),
//     "Text Files (*.txt);;Data files (*.dat)",
//     this,
//     0,
//     QString("CHEF: Lattice Functions.") );
//   if( fileName.isNull() ) {
//     QMessageBox::information( this, "CHEF: Lattice Functions.",
//                               "Sorry. It didn't work." );
//   }
//   else {
//     int i = 0;
//     const bmlnElmnt* q;
//     ofstream outputFileStream( fileName );
//     outputFileStream << setiosflags(ios::fixed);
//     DeepBeamlineIterator dbi(_bmlConPtr->cheatBmlPtr());
// 
//     if( fileName.right(4) == ".dat" ) {
//       // Write the data ...
//       i = 0;
//       while((  q = dbi++  )) {
//         if( i >= _arraySize ) {
//           cerr << "*** WARNING ***                               \n"
//                   "*** WARNING *** LattFncPlt::_fileSaveAs       \n"
//                   "*** WARNING *** Array bounds exceeded: value = "
//                << i
//                << ".\n"
//                   "*** WARNING ***                               \n"
//              << endl;
//           return;
//         }
//         else {
//           outputFileStream << setw(10) << setprecision(3) << _azimuth[i]
//                            << setw(10) << setprecision(3) << _beta_H[i]
//                            << setw(10) << setprecision(3) << _beta_V[i]
//                            << setw(10) << setprecision(3) << _disp_H[i]
//                            << endl;
//         }
//         i++;
//       }
//     }
// 
//     else if( fileName.right(4) == ".txt" ) {
//       // Write the labels ...
//       outputFileStream << setw(15) << "Element"
//                        << setw(15) << "Name"
//                        << setw(10) << "Azimuth"
//                        << setw(10) << "Hor.Beta"
//                        << setw(10) << "Hor.Alpha"
//                        << setw(10) << "Hor.Psi"
//                        << setw(10) << "Hor.Disp"
//                        << setw(10) << "Ver.Beta"
//                        << setw(10) << "Ver.Alpha"
//                        << setw(10) << "Ver.Psi"
//                        << setw(10) << "Ver.Disp"
//                        << endl;
//       for( i = 0; i < (2*15+7*10); i++ ) {
//         outputFileStream << "-";
//       }
//       outputFileStream << endl;
//     
//       // Write the data ...
//       i = 0;
//       while((  q = dbi++  )) {
//         if( i >= _arraySize ) {
//           cerr << "*** WARNING ***                               \n"
//                   "*** WARNING *** LattFncPlt::_fileSaveAs          \n"
//                   "*** WARNING *** Array bounds exceeded: value = "
//                << i
//                << ".\n"
//                   "*** WARNING ***                               \n"
//              << endl;
//           return;
//         }
//         else {
//           outputFileStream << setw(15) << q->Type() 
//                            << setw(15) << q->Name()
//                            << setw(10) << setprecision(3) << _azimuth[i]
//                            << setw(10) << setprecision(3) << _beta_H[i]
//                            << setw(10) << setprecision(3) << _alpha_H[i]
//                            << setw(10) << setprecision(3) << (_psi_H[i]/M_TWOPI)
//                            << setw(10) << setprecision(3) << _disp_H[i]
//                            << setw(10) << setprecision(3) << _beta_V[i]
//                            << setw(10) << setprecision(3) << _alpha_V[i]
//                            << setw(10) << setprecision(3) << (_psi_V[i]/M_TWOPI)
//                            << setw(10) << setprecision(3) << _disp_V[i]
//                            << endl;
//         }
//         i++;
//       }
//     }
// 
//     outputFileStream.close();
//   }
// }
// 
// 
// void LattFncPlt::_do_nothing()
// {
//   QMessageBox::information( this, "CHEF: Lattice Functions.",
//                             "Sorry. This function is not implemented." );
//   // cerr << "LattFncPlt::_do_nothing() invoked." << endl;
// }
// 
// 
// 
// void LattFncPlt::_print()
// {
// 
//     // ??? Why is this here??? QBusrh br(red);
//     // ??? Why is this here??? QPen pn(yellow);
//     // ??? Why is this here??? 
//     // ??? Why is this here??? QwtSymbol sym1;   
//     // ??? Why is this here??? sym1.setBrush(br);
//     // ??? Why is this here??? sym1.setPen(pn);
//     // ??? Why is this here??? sym1.setSize(11);
// 
//     QPrinter p;
// 
//     if ( p.setup( /* parent = */ 0 ) )
//     {
//       plt->print( p );
//     }
//     else {
//       cerr << "*** WARNING ***                               \n"
//               "*** WARNING *** LattFncPlt::_print               \n"
//               "*** WARNING *** QPrinter::setup() returned    \n"
//               "*** WARNING *** a value of false.             \n"
//               "*** WARNING ***                               \n"
//            << endl;
//     }
// }
// 
// 
// void LattFncPlt::_setBetaPlot()
// {
//   _plotType = betaPlot;
//   plt->setCurveData( crv1, _azimuth, _beta_H, _arraySize );
//   plt->setCurveData( crv2, _azimuth, _beta_V, _arraySize );
//   plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
//   plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
//   plt->setAxisTitle(QwtPlot::yLeft,  "beta  [m]"     );
//   plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
//   plt->replot();
// }
// 
// void LattFncPlt::_setInvPlot()
// {
//   _plotType = invPlot;
//   plt->setCurveData( crv1, _azimuth, _inv_beta_H, _arraySize );
//   plt->setCurveData( crv2, _azimuth, _inv_beta_V, _arraySize );
//   plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
//   plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
//   plt->setAxisTitle(QwtPlot::yLeft, "1/beta  [m**-1]");
//   plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
//   plt->replot();
// }
// 
// void LattFncPlt::_setRootPlot()
// {
//   _plotType = rootPlot;
//   plt->setCurveData( crv1, _azimuth, _root_beta_H, _arraySize );
//   plt->setCurveData( crv2, _azimuth, _root_beta_V, _arraySize );
//   plt->setCurveData( crv3, _azimuth, _disp_H, _arraySize );
//   plt->setCurveData( crv4, _azimuth, _disp_V, _arraySize );
//   plt->setAxisTitle(QwtPlot::yLeft, "root(beta)  [m**1/2]");
//   plt->setAxisTitle(QwtPlot::yRight, "dispersion [m]");
//   plt->replot();
// }
// 
// 
