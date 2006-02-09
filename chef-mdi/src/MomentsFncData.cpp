/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:  CHEFGUI.cpp                                           ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
******                                                               ******  
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
**************************************************************************/

#include <fstream>
#include <iomanip>
#include <string>   // needed for strcat
#include <math.h>

#include <complexAddon.h>
#include <bmlfactory.h>
#include <MomentsFncData.h>
#include <BeamlineContext.h>

#include <qapplication.h>
#include <qwt/qwt_plot.h>
#include <boost/shared_ptr.hpp>
#include <chefplotmain.h>

// This undef is needed because connect is defined by qt as a macro. Unfortunately, the string "connect" is 
// also used in the std c++ library code.  

#undef connect

static double* dnull = 0;

using namespace std;



MomentsFncData::MomentsFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream)
: 
  _bmlConPtr(bcp), 
  _deleteContext(false),
  _arraySize(0), 
  _azimuth(dnull),
  _beta_H(dnull),  
  _beta_V(dnull),
  _inv_beta_H(dnull), 
  _inv_beta_V(dnull),
  _root_beta_H(dnull), 
  _root_beta_V(dnull),
  _alpha_H(dnull), 
  _alpha_V(dnull),
  _disp_H(dnull), 
  _disp_V(dnull),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)

{
  this->_finishConstructor();
}


MomentsFncData::MomentsFncData( const Particle& prt, beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream)
: 
  _bmlConPtr(0), 
  _deleteContext(true),
  _arraySize(0), 
  _azimuth(dnull),
  _beta_H(dnull),  
  _beta_V(dnull),
  _inv_beta_H(dnull), 
  _inv_beta_V(dnull),
  _root_beta_H(dnull), 
  _root_beta_V(dnull),
  _alpha_H(dnull), 
  _alpha_V(dnull),
  _disp_H(dnull), 
  _disp_V(dnull),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)

{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );
  this->_finishConstructor();
}

void 
MomentsFncData::_finishConstructor( )
{

  const beamline* bmlPtr = _bmlConPtr->cheatBmlPtr();

  // Create the lattice function arrays.
  _arraySize   = bmlPtr->countHowManyDeeply();

  _azimuth     = boost::shared_array<double>( new double[_arraySize] );
  _beta_H      = boost::shared_array<double>( new double[_arraySize] );
  _alpha_H     = boost::shared_array<double>( new double[_arraySize] );
  _beta_V      = boost::shared_array<double>( new double[_arraySize] );
  _alpha_V     = boost::shared_array<double>( new double[_arraySize] );
  _inv_beta_H  = boost::shared_array<double>( new double[_arraySize] );
  _inv_beta_V  = boost::shared_array<double>( new double[_arraySize] );
  _root_beta_H = boost::shared_array<double>( new double[_arraySize] );
  _root_beta_V = boost::shared_array<double>( new double[_arraySize] );
  _disp_H      = boost::shared_array<double>( new double[_arraySize] );
  _disp_V      = boost::shared_array<double>( new double[_arraySize] );

  for( int i = 0; i < _arraySize; i++ ) {
    _azimuth[i]     = 0.0;
    _beta_H[i]      = 0.0;
    _alpha_H[i]     = 0.0;
    _beta_V[i]      = 0.0;
    _alpha_V[i]     = 0.0;
    _inv_beta_H[i]  = 0.0;
    _inv_beta_V[i]  = 0.0;
    _root_beta_H[i] = 0.0;
    _root_beta_V[i] = 0.0;
    _disp_H[i]      = 0.0;
    _disp_V[i]      = 0.0;
  }
}


void MomentsFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve>  
    c1( new CHEFCurve( CurveData( _azimuth, _beta_H, _arraySize), "Horizontal Beta" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve( CurveData( _azimuth, _beta_V, _arraySize), "Vertical Beta" ) );
  boost::shared_ptr<CHEFCurve> 
    c3( new CHEFCurve(  CurveData( _azimuth, _alpha_H, _arraySize), "Horizontal Alpha" ) );
  boost::shared_ptr<CHEFCurve>  
    c4( new CHEFCurve(  CurveData(_azimuth, _alpha_V, _arraySize), "Vertical Alpha" ) );

  c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "red",   1, Qt::SolidLine ) );
  c3->setPen( QPen( "black", 1, Qt::DashLine  ) );
  c4->setPen( QPen( "red",   1, Qt::DashLine  ) );

  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c3->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c4->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  addCurve( c1 );
  addCurve( c2 );
  addCurve( c3 );
  addCurve( c4 );

  setXLabel( "Arc Length [m]" );
  setYLabel( "Beta [m]", QwtPlot::yLeft  );
  setYLabel( "Alpha",    QwtPlot::yRight );

  setBeamline( _bmlConPtr->cheatBmlPtr(), false ); // false = do not clone line   
}


MomentsFncData::~MomentsFncData()
{
  if(_deleteContext) { delete _bmlConPtr; }
}


void MomentsFncData::doCalc()
{
  if( _bmlConPtr->isTreatedAsRing() ) {
    _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
    _currentTune[1] = _bmlConPtr->getVerticalEigenTune();
  }
  else {
    _currentTune[0] = -1.0;
    _currentTune[1] = -1.0;
  }

  int i = 0;
  const CovarianceSage::Info* infoPtr = _bmlConPtr->getCovFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void MomentsFncData::_recalc()"
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
      // disp_H[i]       = infoPtr->dispersion.hor;
      // disp_V[i]       = infoPtr->dispersion.ver;
      _disp_H[i]       = 0.0;
      _disp_V[i]       = 0.0;

      infoPtr = _bmlConPtr->getCovFuncPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "MomentsFncData::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


double MomentsFncData::getHorTune()
{
  return _currentTune[0];
}


double MomentsFncData::getVerTune()
{
  return _currentTune[1];
}
