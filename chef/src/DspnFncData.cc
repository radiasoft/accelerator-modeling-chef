#include <iomanip>
#include <string>   // needed for strcat
#include <math.h>

#include "complexAddon.h"
#include "bmlfactory.h"
#include "DspnFncData.h"
#include "BeamlineContext.h"
#include "GenericException.h"

#include <qwt/qwt_plot.h>
#include <boost/shared_ptr.hpp>


// This undef is needed because of the compiler.
#undef connect

static double* dnull = 0;

DspnFncData::DspnFncData( BeamlineContext* bcp, std::ostream*  stdoutstream, std::ostream* stderrstream )
: _errorStreamPtr(stderrstream), _outputStreamPtr(stdoutstream),
  _bmlConPtr(bcp), _deleteContext(false),
  _arraySize(0), 
  _azimuth(dnull),
  _clo_H(dnull), _clo_V(dnull), 
  _disp_H(dnull), _disp_V(dnull)

{
  this->_finishConstructor();
}


DspnFncData::DspnFncData( /* const */ beamline* pBml, std::ostream*  stdoutstream, std::ostream* stderrstream )
: _errorStreamPtr(stderrstream), _outputStreamPtr(stdoutstream),
  _bmlConPtr(0), _deleteContext(true),
  _arraySize(0), 
  _azimuth(dnull),
  _clo_H(dnull), _clo_V(dnull), 
  _disp_H(dnull), _disp_V(dnull)
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor();
}

void DspnFncData::_finishConstructor( )
{
  const beamline* bmlPtr = _bmlConPtr->cheatBmlPtr();

  // Create the lattice function arrays.
  _arraySize   = bmlPtr->countHowManyDeeply();
 
  _azimuth     = boost::shared_array<double>( new double[_arraySize] );
  _disp_H      = boost::shared_array<double>( new double[_arraySize] );
  _disp_V      = boost::shared_array<double>( new double[_arraySize] );
  _dPrime_H    = boost::shared_array<double>( new double[_arraySize] );
  _dPrime_V    = boost::shared_array<double>( new double[_arraySize] );
  _clo_H       = boost::shared_array<double>( new double[_arraySize] );
  _clo_V       = boost::shared_array<double>( new double[_arraySize] );

  for( int i = 0; i < _arraySize; i++ ) {
    _azimuth[i] = 0.0;
    _disp_H[i] = 0.0;
    _disp_V[i] = 0.0;
    _dPrime_H[i] = 0.0;
    _dPrime_V[i] = 0.0;
    _clo_H[i] = 0.0;
    _clo_V[i] = 0.0;
  }

}


void DspnFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve> 
    c1( new CHEFCurve( CurveData( _azimuth, _disp_H, _arraySize), "Hor Dispersion" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve( CurveData( _azimuth, _disp_V, _arraySize), "Ver Dispersion" ) );
  boost::shared_ptr<CHEFCurve>
    c3( new CHEFCurve( CurveData( _azimuth, _dPrime_H, _arraySize),  "Hor Dispersion Derivative" ) );
  boost::shared_ptr<CHEFCurve>
    c4( new CHEFCurve( CurveData( _azimuth, _dPrime_V, _arraySize),  "Ver Dispersion Derivative" ) );
  boost::shared_ptr<CHEFCurve> 
    c5( new CHEFCurve( CurveData( _azimuth, _clo_H, _arraySize),  "Hor Closed Orbit" ) );
  boost::shared_ptr<CHEFCurve>
    c6( new CHEFCurve( CurveData( _azimuth, _clo_V, _arraySize),  "Ver Closed Orbit" ) );


  c1->setPen( QPen( "black",  1, Qt::SolidLine ) );
  c2->setPen( QPen( "blue",   1, Qt::SolidLine ) );
  c3->setPen( QPen( "cyan",   0, Qt::SolidLine ) );
  c4->setPen( QPen( "magenta",0, Qt::SolidLine ) );
  c5->setPen( QPen( "red",    0, Qt::SolidLine ) );
  c6->setPen( QPen( "green",  0, Qt::SolidLine ) );


  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c3->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c4->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c5->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c6->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  addCurve( c1 );
  addCurve( c2 );
  addCurve( c3 );
  addCurve( c4 );
  addCurve( c5 );
  addCurve( c6 );

  setXLabel( "Arc Length [m]" );
  setYLabel( "Dispersion [m] & Dispersion Derivative",    QwtPlot::yLeft  );
  setYLabel( "Closed Orbit [mm]", QwtPlot::yRight );

  setScaleMagRight( 5.0 ); 
  setBeamline( _bmlConPtr->cheatBmlPtr(), false ); // false = do not clone line   
}


DspnFncData::~DspnFncData()
{
  // data held by shared_array are deleted when
  // the shared_ptr go out of scope. 

  if(_deleteContext) { delete _bmlConPtr; }
}


void DspnFncData::doCalc()
{
  const double mm = 0.001;
  int i = 0;

  const DispersionSage::Info* infoPtr;
  try {
    infoPtr = _bmlConPtr->getDispersionPtr(i);
  }
  catch( const GenericException& ge ) {
    throw ge;
  }

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void DspnFncData::doCalc()"
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
      _dPrime_H[i]     = infoPtr->dPrime.hor;
      _dPrime_V[i]     = infoPtr->dPrime.ver;

      // REMOVE: // Zero out closed orbits of less than 1 Angstrom
      // REMOVE: if( std::abs(_clo_H[i]) < 1.0e-7 ) { _clo_H[i] = 0.0; }
      // REMOVE: if( std::abs(_clo_V[i]) < 1.0e-7 ) { _clo_V[i] = 0.0; }

      infoPtr = _bmlConPtr->getDispersionPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "DspnFncData::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }

  _arraySize = i;
}


double DspnFncData::getHorTune()
{
  return -1.0;
}


double DspnFncData::getVerTune()
{
  return -1.0;
}

