/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      DspnFncPlt.cc
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
#include "DspnFncPlt.h"
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


DspnFncPlt::DspnFncPlt( BeamlineContext* bcp, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(bcp), _deleteContext(false),
  _arraySize(0), _azimuth(0),
  _clo_H(0), _clo_V(0), 
  _disp_H(0), _disp_V(0),
  _name(0)
{
  this->_finishConstructor( name );
}


DspnFncPlt::DspnFncPlt( /* const */ beamline* pBml, const char* name )
: _errorStreamPtr(&cerr), _outputStreamPtr(&cout),
  _bmlConPtr(0), _deleteContext(true),
  _arraySize(0), _azimuth(0),
  _clo_H(0), _clo_V(0), 
  _disp_H(0), _disp_V(0),
  _name(0)
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor( name );
}

void DspnFncPlt::_finishConstructor( const char* name )
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
  _disp_H      = new double[_arraySize];
  _disp_V      = new double[_arraySize];
  _clo_H       = new double[_arraySize];
  _clo_V       = new double[_arraySize];

  _recalc();


  // Convert to "curves"
  CHEFCurve* c1 
    = new CHEFCurve( _azimuth, _disp_H, _arraySize, "Horizontal Dispersion" );
  CHEFCurve* c2 
    = new CHEFCurve( _azimuth, _disp_V, _arraySize, "Vertical Dispersion" );
  CHEFCurve* c3 
    = new CHEFCurve( _azimuth, _clo_H, _arraySize, "Horizontal Closed Orbit" );
  CHEFCurve* c4 
    = new CHEFCurve( _azimuth, _clo_V, _arraySize, "Vertical Closed Orbit" );

  c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "blue",  1, Qt::SolidLine ) );
  c3->setPen( QPen( "red",   0, Qt::SolidLine ) );
  c4->setPen( QPen( "green", 0, Qt::SolidLine ) );

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
  _di->setYLabel( "Dispersion [m]",   QwtPlot::yLeft  );
  _di->setYLabel( "Closed Orbit [mm]", QwtPlot::yRight );

  _di->setScaleMagRight( 5.0 ); // ??? fix this!
  _di->setBeamline( bmlPtr ); // ??? What if *bmlPtr is deleted?


  // Prepare the plotting widget
  _plotter = new CHEFPlotMain(0, 0, 0 );
  // This will delete itself using a "File/Close" menu item.

  char theTitle[1024];
  for( int i = 0; i < 1024; i++ ) {
    theTitle[i] = '\0';
  }
  strcat( theTitle, "Closed orbit and dispersion: " );
  strcat( theTitle, _bmlConPtr->name() );

  _plotter->setCaption( theTitle );

  _plotter->addData( *(_di) );
  _plotter->resize( 85*(QApplication::desktop()->width())/100,
                    85*(QApplication::desktop()->width())/200  );

  _plotter->show();
}


DspnFncPlt::~DspnFncPlt()
{
  if( _name ) { delete [] _name; }

  delete    _di;
  delete [] _azimuth;  _azimuth = 0;
  delete [] _disp_H;   _disp_H = 0;
  delete [] _disp_V;   _disp_V = 0;
  delete [] _clo_H;    _clo_H = 0;
  delete [] _clo_V;    _clo_V = 0;

  if(_deleteContext) { delete _bmlConPtr; }
}



void DspnFncPlt::_recalc()
{
  const double mm = 0.001;

  _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
  _currentTune[1] = _bmlConPtr->getVerticalEigenTune();


  int i = 0;
  const DispersionSage::Info* infoPtr = _bmlConPtr->getDispersionPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void DspnFncPlt::_recalc()"
           "\n*** WARNING *** Too many lattice functions read."
           "\n*** WARNING *** Am resetting to " << _arraySize << " in all."
        << "\n*** WARNING *** "
        << endl;
    }
    else {
      _azimuth[i]      = infoPtr->arcLength;
      _clo_H[i]        = infoPtr->closedOrbit.hor / mm;
      _clo_V[i]        = infoPtr->closedOrbit.ver / mm;
      _disp_H[i]       = infoPtr->dispersion.hor;
      _disp_V[i]       = infoPtr->dispersion.ver;

      // Zero out closed orbits of less than 1 Angstrom
      if( std::abs(_clo_H[i]) < 1.0e-7 ) { _clo_H[i] = 0.0; }
      if( std::abs(_clo_V[i]) < 1.0e-7 ) { _clo_V[i] = 0.0; }

      infoPtr = _bmlConPtr->getDispersionPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "DspnFncPlt::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


void DspnFncPlt::setCaption( const char* cpt )
{
  _plotter->setCaption( cpt );
}


void DspnFncPlt::_selfDestruct()
{
  delete this;
}
