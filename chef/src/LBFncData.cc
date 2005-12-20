#include "LBFncData.h"
#include "BeamlineContext.h"
#include <qwt/qwt_plot.h>
#include <boost/shared_ptr.hpp>

// This undef is (still??) needed because of the compiler.
#undef connect

double* dnull = 0;

using namespace std;

// External variable that has to be defined for the MAD parser
// Already defined in CHEF.cc. This is a problem!!
// madparser* mp = 0;


LBFncData::LBFncData( beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream )
: _bmlConPtr( new BeamlineContext( false, pBml ) ),
  _deleteContext(false),
  _arraySize(0),
  _azimuth(dnull),
  _beta_1x(dnull),
  _beta_1y(dnull),
  _beta_2x(dnull),
  _beta_2y(dnull),
  _alpha_1x(dnull),
  _alpha_1y(dnull),
  _alpha_2x(dnull),
  _alpha_2y(dnull),
  _u1(dnull),
  _u2(dnull),
  _u3(dnull),
  _u4(dnull),
  _nu_1(0),
  _nu_2(0),
  _name(0),
  _errorStreamPtr(stderrstream),
  _outputStreamPtr(stdoutstream)
{
  this->_finishConstructor();
}


LBFncData::LBFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream )
: _bmlConPtr(bcp),
  _deleteContext(false),
  _arraySize(0),
  _azimuth(dnull),
  _beta_1x(dnull),
  _beta_1y(dnull),
  _beta_2x(dnull),
  _beta_2y(dnull),
  _alpha_1x(dnull),
  _alpha_1y(dnull),
  _alpha_2x(dnull),
  _alpha_2y(dnull),
  _u1(dnull),
  _u2(dnull),
  _u3(dnull),
  _u4(dnull),
  _nu_1(dnull),
  _nu_2(dnull),
  _name(0),
  _errorStreamPtr(stderrstream),
  _outputStreamPtr(stdoutstream)
{
  this->_finishConstructor();
}


void LBFncData::_finishConstructor( )
{
  const beamline* bmlPtr = _bmlConPtr->cheatBmlPtr();

  // Create the lattice function arrays.
  _arraySize   = bmlPtr->countHowManyDeeply();

  _azimuth     = boost::shared_array<double>( new double[_arraySize] );
  _beta_1x     = boost::shared_array<double>( new double[_arraySize] );
  _beta_1y     = boost::shared_array<double>( new double[_arraySize] );
  _beta_2x     = boost::shared_array<double>( new double[_arraySize] );
  _beta_2y     = boost::shared_array<double>( new double[_arraySize] );
  _alpha_1x    = boost::shared_array<double>( new double[_arraySize] );
  _alpha_1y    = boost::shared_array<double>( new double[_arraySize] );
  _alpha_2x    = boost::shared_array<double>( new double[_arraySize] );
  _alpha_2y    = boost::shared_array<double>( new double[_arraySize] );
  _u1          = boost::shared_array<double>( new double[_arraySize] );
  _u2          = boost::shared_array<double>( new double[_arraySize] );
  _u3          = boost::shared_array<double>( new double[_arraySize] );
  _u4          = boost::shared_array<double>( new double[_arraySize] );
  _nu_1        = boost::shared_array<double>( new double[_arraySize] );
  _nu_2        = boost::shared_array<double>( new double[_arraySize] );

  for( int i = 0; i < _arraySize; i++ ) {
    _azimuth[i] = 0.0;
    _beta_1x[i] = 0.0;
    _beta_1y[i] = 0.0;
    _beta_2x[i] = 0.0;
    _beta_2y[i] = 0.0;
    _alpha_1x[i] = 0.0;
    _alpha_1y[i] = 0.0;
    _alpha_2x[i] = 0.0;
    _alpha_2y[i] = 0.0;
    _u1[i] = 0.0;
    _u2[i] = 0.0;
    _u3[i] = 0.0;
    _u4[i] = 0.0;
    _nu_1[i] = 0.0;
    _nu_2[i] = 0.0;
  }
}


void LBFncData::makeCurves()
{
  // Convert to "curves"
  boost::shared_ptr<CHEFCurve> 
  c1x = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( _azimuth, _beta_1x, _arraySize), "<hor|beta|hor>" ) );

  boost::shared_ptr<CHEFCurve>
  c1y = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( _azimuth, _beta_1y, _arraySize), "<ver|beta|hor>" ) );

  boost::shared_ptr<CHEFCurve>
  c2x = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( _azimuth, _beta_2x, _arraySize), "<hor|beta|ver>" ) );

  boost::shared_ptr<CHEFCurve>
  c2y = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( _azimuth, _beta_2y, _arraySize), "<ver|beta|ver>" ) );


  c1x->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c1y->setPen( QPen( "black", 1, Qt::DashLine  ) );
  c2y->setPen( QPen( "red",   1, Qt::SolidLine ) );
  c2x->setPen( QPen( "red",   1, Qt::DashLine  ) );

  c1x->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2y->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2x->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c1y->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  addCurve( c1x );
  addCurve( c2x );
  addCurve( c1y );
  addCurve( c2y );
  

  setXLabel( "Arc Length [m]" );
  setYLabel( "Beta [m]",  QwtPlot::yLeft );
  setYLabel( "Beta [m]",  QwtPlot::yRight );

  setBeamline( _bmlConPtr->cheatBmlPtr(), false ); // false = do not clone line   
}


LBFncData::~LBFncData()
{
  if( _name )        { delete [] _name;        _name        = 0; }
  if(_deleteContext&&(0 != _bmlConPtr))
                     { delete _bmlConPtr;      _bmlConPtr   = 0; }
}


void LBFncData::doCalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
  _currentTune[1] = _bmlConPtr->getVerticalEigenTune();

  int i = 0;
  const LBSage::Info* infoPtr = _bmlConPtr->getLBFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void LBFncData::_docalc()"
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
         << "LBFncData::docalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


double LBFncData::getHorTune()
{
  return _currentTune[0];
}


double LBFncData::getVerTune()
{
  return _currentTune[1];
}
