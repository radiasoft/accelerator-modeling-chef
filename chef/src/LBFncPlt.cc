/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      LBFncPlt.cc
******  Version:   1.0
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

#include "LBFncPlt.h"
#include "BeamlineContext.h"

#include <qapplication.h>
#include <qwt/qwt_plot.h>
#include "chefplotmain.h"

// This undef is (still??) needed because of the compiler.
#undef connect

using namespace CHEF_domain;
using namespace std;

// External variable that has to be defined for the MAD parser
// Already defined in CHEF.cc. This is a problem!!
// madparser* mp = 0;


LBFncPlt::LBFncPlt( beamline* pBml, const char* name )
  : _plotter(0),
  _di(0),
  _bmlConPtr( new BeamlineContext( false, pBml ) ),
  _deleteContext(false),
  _arraySize(0),
  _azimuth(0),
  _beta_1x(0),
  _beta_1y(0),
  _beta_2x(0),
  _beta_2y(0),
  _alpha_1x(0),
  _alpha_1y(0),
  _alpha_2x(0),
  _alpha_2y(0),
  _u1(0),
  _u2(0),
  _u3(0),
  _u4(0),
  _nu_1(0),
  _nu_2(0),
  _name(0),
  _errorStreamPtr(&cerr),
  _outputStreamPtr(&cout)
{
  this->_finishConstructor( name );
}


LBFncPlt::LBFncPlt( BeamlineContext* bcp, const char* name )
  : _plotter(0),
  _di(0),
  _bmlConPtr(bcp),
  _deleteContext(false),
  _arraySize(0),
  _azimuth(0),
  _beta_1x(0),
  _beta_1y(0),
  _beta_2x(0),
  _beta_2y(0),
  _alpha_1x(0),
  _alpha_1y(0),
  _alpha_2x(0),
  _alpha_2y(0),
  _u1(0),
  _u2(0),
  _u3(0),
  _u4(0),
  _nu_1(0),
  _nu_2(0),
  _name(0),
  _errorStreamPtr(&cerr),
  _outputStreamPtr(&cout)
{
  this->_finishConstructor( name );
}


void LBFncPlt::_finishConstructor( const char* name )
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
  _beta_1x     = new double[_arraySize];
  _beta_1y     = new double[_arraySize];
  _beta_2x     = new double[_arraySize];
  _beta_2y     = new double[_arraySize];
  _alpha_1x    = new double[_arraySize];
  _alpha_1y    = new double[_arraySize];
  _alpha_2x    = new double[_arraySize];
  _alpha_2y    = new double[_arraySize];
  _u1          = new double[_arraySize];
  _u2          = new double[_arraySize];
  _u3          = new double[_arraySize];
  _u4          = new double[_arraySize];
  _nu_1        = new double[_arraySize];
  _nu_2        = new double[_arraySize];


  // Load the arrays
  _recalc();


  // Convert to "curves"
  CHEFCurve* c1x
    = new CHEFCurve( _azimuth, _beta_1x, _arraySize, "<hor|beta|hor>" );
  CHEFCurve* c1y
    = new CHEFCurve( _azimuth, _beta_1y, _arraySize, "<ver|beta|hor>" );
  CHEFCurve* c2x
    = new CHEFCurve( _azimuth, _beta_2x, _arraySize, "<hor|beta|ver>" );
  CHEFCurve* c2y
    = new CHEFCurve( _azimuth, _beta_2y, _arraySize, "<ver|beta|ver>" );

  c1x->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c1y->setPen( QPen( "black", 1, Qt::DashLine  ) );
  c2y->setPen( QPen( "red",   1, Qt::SolidLine ) );
  c2x->setPen( QPen( "red",   1, Qt::DashLine  ) );

  c1x->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2y->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2x->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c1y->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  _di = new CHEFPlotData;
  _di->addCurve( c1x );
  _di->addCurve( c2x );
  _di->addCurve( c1y );
  _di->addCurve( c2y );
  // *_di now has the responsibility of deleting
  // the curves when it is destroyed

  _di->setXLabel( "Arc Length [m]" );
  _di->setYLabel( "Beta [m]",  QwtPlot::yLeft );

  // _di->setScaleMagRight( 5.0 ); // ??? fix this!
  _di->setBeamline( bmlPtr ); // ??? What if *bmlPtr is deleted?


  // Prepare the plotting widget
  _plotter = new CHEFPlotMain(0, 0, Qt::WDestructiveClose );
  // This will delete itself using a "File/Close" menu item.

  char theTitle[1024];
  for( int i = 0; i < 1024; i++ ) {
    theTitle[i] = '\0';
  }
  strcat( theTitle, "Lebedev-Bogacz representation." );
  strcat( theTitle, _bmlConPtr->name() );

  _plotter->setCaption( theTitle );

  _plotter->addData( *(_di) );
  _plotter->resize( 85*(QApplication::desktop()->width())/100,
                    85*(QApplication::desktop()->width())/200  );

  _plotter->show();
}


LBFncPlt::~LBFncPlt()
{
  if( _name )        { delete [] _name;        _name        = 0; }

  if( _di )          { delete    _di;          _di          = 0; }
  if( _azimuth )     { delete [] _azimuth;     _azimuth     = 0; }

  if( _beta_1x )     { delete [] _beta_1x;     _beta_1x     = 0; }
  if( _beta_1y )     { delete [] _beta_1y;     _beta_1y     = 0; }
  if( _beta_2x )     { delete [] _beta_2x;     _beta_2x     = 0; }
  if( _beta_2y )     { delete [] _beta_2y;     _beta_2y     = 0; }
  if( _alpha_1x )    { delete [] _alpha_1x;    _alpha_1x    = 0; }
  if( _alpha_1y )    { delete [] _alpha_1y;    _alpha_1y    = 0; }
  if( _alpha_2x )    { delete [] _alpha_2x;    _alpha_2x    = 0; }
  if( _alpha_2y )    { delete [] _alpha_2y;    _alpha_2y    = 0; }
  if( _u1 )          { delete [] _u1;          _u1          = 0; }
  if( _u2 )          { delete [] _u2;          _u2          = 0; }
  if( _u3 )          { delete [] _u3;          _u3          = 0; }
  if( _u4 )          { delete [] _u4;          _u4          = 0; }
  if( _nu_1 )        { delete [] _nu_1;        _nu_1        = 0; }
  if( _nu_2 )        { delete [] _nu_2;        _nu_2        = 0; }

  if(_deleteContext&&(0 != _bmlConPtr))
                     { delete _bmlConPtr;      _bmlConPtr   = 0; }
}


void LBFncPlt::_recalc()
{
  int i = 0;
  const LBSage::Info* infoPtr = _bmlConPtr->getLBFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void LBFncPlt::_recalc()"
           "\n*** WARNING *** Too many lattice functions read."
           "\n*** WARNING *** Am resetting to " << _arraySize << " in all."
        << "\n*** WARNING *** "
        << endl;
    }
    else {
      _azimuth[i] = infoPtr->arcLength;
      _beta_1x[i] = infoPtr->beta_1x;
      _beta_1y[i] = infoPtr->beta_1y;
      _beta_2x[i] = infoPtr->beta_2x;
      _beta_2y[i] = infoPtr->beta_2y;

      infoPtr = _bmlConPtr->getLBFuncPtr(++i);
    }
  }

  if( i != _arraySize ) {
    (*_errorStreamPtr)
         <<  "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "LBFncPlt::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


void LBFncPlt::setCaption( const char* cpt )
{
  _plotter->setCaption( cpt );
}


void LBFncPlt::_selfDestruct()
{
  delete this;
}
