#include <fstream>
#include <iomanip>
#include <string>   // needed for strcat
#include <math.h>

#include "complexAddon.h"
#include "bmlfactory.h"
#include "LattFncData.h"
#include "BeamlineContext.h"
#include "chefplotmain.h"
#include <boost/shared_ptr.hpp>
#include <qwt/qwt_plot.h>

static double* dnull = 0;

// This undef is needed because of the compiler.
#undef connect

LattFncData::LattFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream )
: _errorStreamPtr(stdoutstream), _outputStreamPtr(stderrstream),
  _bmlConPtr(bcp),  _deleteContext(false),
  _plotType( betaPlot ),
  _arraySize(   0     ),     
  _azimuth( dnull),
  _beta_H(      dnull ),      _beta_V( dnull),
  _alpha_H(     dnull ),     _alpha_V( dnull),
  _psi_H(       dnull ),       _psi_V( dnull),
  _inv_beta_H(  dnull ),  _inv_beta_V( dnull),
  _root_beta_H( dnull ), _root_beta_V( dnull),
  _disp_H(      dnull ),      _disp_V( dnull), 
  _name("undefined")
{
  this->_finishConstructor();
}


LattFncData::LattFncData( beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream)
: _errorStreamPtr(stdoutstream), _outputStreamPtr(stderrstream),
  _bmlConPtr(0),  _deleteContext(true),
  _plotType( betaPlot ),
  _arraySize  ( 0     ),     
  _azimuth( dnull ),
  _beta_H     ( dnull ),      _beta_V( dnull ),
  _alpha_H    ( dnull ),     _alpha_V( dnull ),
  _psi_H      ( dnull ),       _psi_V( dnull ),
  _inv_beta_H ( dnull ),  _inv_beta_V( dnull ),
  _root_beta_H( dnull ), _root_beta_V( dnull ),
  _disp_H     ( dnull ),      _disp_V( dnull ), 
  _name("undefined")
{
  _bmlConPtr = new BeamlineContext( false, pBml );
  this->_finishConstructor();
}

void LattFncData::_finishConstructor()
{
  const beamline* bmlPtr = _bmlConPtr->cheatBmlPtr();

  // Create the lattice function arrays.
  _arraySize   = bmlPtr->countHowManyDeeply();

  _azimuth     = boost::shared_array<double>( new double[_arraySize] );
  _beta_H      = boost::shared_array<double>( new double[_arraySize] );
  _alpha_H     = boost::shared_array<double>( new double[_arraySize] );
  _psi_H       = boost::shared_array<double>( new double[_arraySize] );
  _beta_V      = boost::shared_array<double>( new double[_arraySize] );
  _alpha_V     = boost::shared_array<double>( new double[_arraySize] );
  _psi_V       = boost::shared_array<double>( new double[_arraySize] );
  _inv_beta_H  = boost::shared_array<double>( new double[_arraySize] );
  _inv_beta_V  = boost::shared_array<double>( new double[_arraySize] );
  _root_beta_H = boost::shared_array<double>( new double[_arraySize] );
  _root_beta_V = boost::shared_array<double>( new double[_arraySize] );
  _disp_H      = boost::shared_array<double>( new double[_arraySize] );
  _disp_V      = boost::shared_array<double>( new double[_arraySize] );

  for( int i = 0; i < _arraySize; i++ ) {
    _azimuth[i] = 0.0;
    _beta_H[i] = 0.0;
    _alpha_H[i] = 0.0;
    _psi_H[i] = 0.0;
    _beta_V[i] = 0.0;
    _alpha_V[i] = 0.0;
    _psi_V[i] = 0.0;
    _inv_beta_H[i] = 0.0;
    _inv_beta_V[i] = 0.0;
    _root_beta_H[i] = 0.0;
    _root_beta_V[i] = 0.0;
    _disp_H[i] = 0.0;
    _disp_V[i] = 0.0;
  }
}

LattFncData::~LattFncData()
{
  //std::cout << "LattFncData::~LattFncData()" << std::endl;
  if(_deleteContext) { delete _bmlConPtr; }
}


void LattFncData::doCalc()
{
  if( _bmlConPtr->isTreatedAsRing() ) {
    _currentTune[0] = _bmlConPtr->getHorizontalFracTune();
    _currentTune[1] = _bmlConPtr->getVerticalFracTune();
  }
  else {
    _currentTune[0] = -1.0;
    _currentTune[1] = -1.0;
  }

  int i = 0;
  const LattFuncSage::lattFunc* infoPtr = _bmlConPtr->getLattFuncPtr(i);
 
  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** voidLattFncData::doCalc()"
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
         << "LattFncData::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }

  _arraySize = i;
}


void LattFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve>  
    c1( new CHEFCurve(  CurveData(_azimuth, _beta_H, _arraySize), "Horizontal Beta" ) );
  boost::shared_ptr<CHEFCurve>  
    c2( new CHEFCurve(  CurveData( _azimuth, _beta_V, _arraySize), "Vertical Beta" ) );
  boost::shared_ptr<CHEFCurve> 
    c3( new CHEFCurve(  CurveData( _azimuth, _disp_H, _arraySize), "Horizontal Dispersion" ) );
  boost::shared_ptr<CHEFCurve>  
    c4( new CHEFCurve(  CurveData(_azimuth, _disp_V, _arraySize), "Vertical Dispersion" ) );

  c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "red", 1, Qt::SolidLine ) );
  c3->setPen( QPen( "black",  0, Qt::DashLine ) );
  c4->setPen( QPen( "red",  0, Qt::DashLine ) );

  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c3->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
  c4->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

  // c4->setStyle( QwtCurve::NoCurve ); // this turns off a curve

  addCurve( c1 );
  addCurve( c2 );
  addCurve( c3 );
  addCurve( c4 );

  setXLabel( "Arc Length [m]"                  );
  setYLabel( "Beta [m]",       QwtPlot::yLeft  );
  setYLabel( "Dispersion [m]", QwtPlot::yRight );

  setScaleMagRight( 5.0 ); 
  setBeamline( _bmlConPtr->cheatBmlPtr(), false ); // false = do not clone line   
}


double LattFncData::getHorTune()
{
  return _currentTune[0];
}


double LattFncData::getVerTune()
{
  return _currentTune[1];
}

