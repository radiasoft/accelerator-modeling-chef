/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      MomentsFncPlt.cc
******  Version:   2.0
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
#include "MomentsFncPlt.h"
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


MomentsFncPlt::MomentsFncPlt( BeamlineContext* bcp, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(bcp), _deleteContext(false),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0),
  _name(0)
{
  this->_finishConstructor( name );
}


MomentsFncPlt::MomentsFncPlt( /* const */ beamline* pBml, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(0), _deleteContext(true),
  _plotType( betaPlot ),
  _arraySize(0), _azimuth(0),
  _beta_H(0),  _beta_V(0),
  _alpha_H(0), _alpha_V(0),
  _inv_beta_H(0), _inv_beta_V(0),
  _root_beta_H(0), _root_beta_V(0),
  _disp_H(0), _disp_V(0),
  _name(0)
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor( name );
}

void MomentsFncPlt::_finishConstructor( const char* name )
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
  _beta_V      = new double[_arraySize];
  _alpha_V     = new double[_arraySize];
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

  c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "red",   1, Qt::SolidLine ) );

  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );

  // c4->setStyle( QwtCurve::NoCurve ); // this turns off a curve

  _di = new CHEFPlotData;
  _di->addCurve( c1 );
  _di->addCurve( c2 );
  // *_di now has the responsibility of deleting
  // the curves when it is destroyed

  _di->setXLabel( "Arc Length [m]"                  );
  _di->setYLabel( "Beta [m]",       QwtPlot::yLeft  );

  // _di->setScaleMagRight( 5.0 ); // ??? fix this!
  _di->setBeamline( bmlPtr ); // ??? What if *bmlPtr is deleted?


  // Prepare the plotting widget
  _plotter = new CHEFPlotMain(0, 0, 0 );
  // This will delete itself using a "File/Close" menu item.

  char theTitle[1024];
  for( int i = 0; i < 1024; i++ ) {
    theTitle[i] = '\0';
  }
  strcat( theTitle, "Moments based lattice functions: " );
  strcat( theTitle, _bmlConPtr->name() );

  _plotter->setCaption( theTitle );

  _plotter->addData( *(_di) );
  _plotter->resize( 85*(QApplication::desktop()->width())/100,
                    85*(QApplication::desktop()->width())/200  );

  _plotter->show();
}


MomentsFncPlt::~MomentsFncPlt()
{
  if( _name ) { delete [] _name; }

  delete    _di;
  delete [] _azimuth;
  delete [] _beta_H;
  delete [] _alpha_H;
  delete [] _beta_V;
  delete [] _alpha_V;
  delete [] _inv_beta_H;
  delete [] _inv_beta_V;
  delete [] _root_beta_H;
  delete [] _root_beta_V;
  delete [] _disp_H;
  delete [] _disp_V;

  if(_deleteContext) { delete _bmlConPtr; }
}


void MomentsFncPlt::_recalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
  _currentTune[1] = _bmlConPtr->getVerticalEigenTune();


  int i = 0;
  const CovarianceSage::Info* infoPtr = _bmlConPtr->getCovFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void MomentsFncPlt::_recalc()"
           "\n*** WARNING *** Too many lattice functions read."
           "\n*** WARNING *** Am resetting to " << _arraySize << " in all."
        << "\n*** WARNING *** "
        << endl;
    }
    else {
      _azimuth[i]      = infoPtr->arcLength;
      _beta_H[i]       = infoPtr->beta.hor;
      _alpha_H[i]      = infoPtr->alpha.hor;
      _beta_V[i]       = infoPtr->beta.ver;
      _alpha_V[i]      = infoPtr->alpha.ver;
      _inv_beta_H[i]   = 1.0/infoPtr->beta.hor;
      _inv_beta_V[i]   = 1.0/infoPtr->beta.ver;
      _root_beta_H[i]  = sqrt(infoPtr->beta.hor);
      _root_beta_V[i]  = sqrt(infoPtr->beta.ver);
      // _disp_H[i]       = infoPtr->dispersion.hor;
      // _disp_V[i]       = infoPtr->dispersion.ver;
      _disp_H[i]       = 0.0;
      _disp_V[i]       = 0.0;

      infoPtr = _bmlConPtr->getCovFuncPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "MomentsFncPlt::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


void MomentsFncPlt::setCaption( const char* cpt )
{
  _plotter->setCaption( cpt );
}


void MomentsFncPlt::_selfDestruct()
{
  delete this;
}
