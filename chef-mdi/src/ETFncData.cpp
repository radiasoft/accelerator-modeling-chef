/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      ETFFncData.cpp                                    ****** 
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

#include <ETFncData.h>
#include <BeamlineContext.h>
#include <boost/shared_ptr.hpp>
#include <qwt/qwt_plot.h>

// This undef is needed because of the compiler.
#undef connect

using namespace std;

static double* dnull = 0;


ETFncData::ETFncData( BeamlineContext* bcp,  std::ostream* stdoutstream, std::ostream* stderrstream )
: 
  _bmlConPtr(bcp), 
  _deleteContext(false),
  _arraySize(0), 
  _azimuth(dnull),
  _beta_H( dnull),  
  _beta_V(dnull),
  _inv_beta_H( dnull), 
  _inv_beta_V(dnull),
  _root_beta_H(dnull), 
  _root_beta_V(dnull),
  _alpha_H(dnull), 
  _alpha_V(dnull),
  _disp_H(dnull), 
  _disp_V(dnull), 
  _name(0),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)
{
  this->_finishConstructor();
}


ETFncData::ETFncData( const Particle& prt, beamline* pBml,  std::ostream* stdoutstream, std::ostream* stderrstream)
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
  _name(0),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)
{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );
   this->_finishConstructor();
}


void ETFncData::_finishConstructor()
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

   double* azimuth     = _azimuth.get();
   double* beta_H      = _beta_H.get();     
   double* alpha_H     = _alpha_H.get();     
   double* beta_V      = _beta_V.get();      
   double* alpha_V     = _alpha_V.get();     
   double* inv_beta_H  = _inv_beta_H.get();  
   double* inv_beta_V  = _inv_beta_V.get();  
   double* root_beta_H = _root_beta_H.get(); 
   double* root_beta_V = _root_beta_V.get(); 
   double* disp_H      = _disp_H.get();        
   double* disp_V      = _disp_V.get();        

  for( int i = 0; i < _arraySize; i++ ) {
    *(azimuth++)     = 0.0;
    *(beta_H++)      = 0.0;
    *(alpha_H++)     = 0.0;
    *(beta_V++)      = 0.0;
    *(alpha_V++)     = 0.0;
    *(inv_beta_H++)  = 0.0;
    *(inv_beta_V++)  = 0.0;
    *(root_beta_H++) = 0.0;
    *(root_beta_V++) = 0.0;
    *(disp_H++)      = 0.0;
    *(disp_V++)      = 0.0;
  }
}


void ETFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve> 
    c1 ( new CHEFCurve( CurveData(_azimuth, _beta_H, _arraySize), "Horizontal Beta" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve(  CurveData(_azimuth, _beta_V, _arraySize), "Vertical Beta" ) );

  c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
  c2->setPen( QPen( "red",   1, Qt::SolidLine ) );

  c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
  c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );

  // c4->setStyle( QwtCurve::NoCurve ); // this turns off a curve

  addCurve( c1 );
  addCurve( c2 );

  setXLabel( "Arc Length [m]"                  );
  setYLabel( "Beta [m]",       QwtPlot::yLeft  );

  // setScaleMagRight( 5.0 ); // ??? fix this!
  setBeamline( _bmlConPtr->cheatBmlPtr(), false ); // false = do not clone line   
}


ETFncData::~ETFncData()
{

  if(_deleteContext) { delete _bmlConPtr; }
}



void ETFncData::doCalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
  _currentTune[1] = _bmlConPtr->getVerticalEigenTune();

  int i = 0;
  const EdwardsTengSage::Info* infoPtr = _bmlConPtr->getETFuncPtr(i);

  while( 0 != infoPtr ) {
    if( i >= _arraySize ) {
      i = _arraySize;
      infoPtr = 0;
      *(_errorStreamPtr) 
        << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
        << "\n*** WARNING *** void ETFncData::_recalc()"
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

      infoPtr = _bmlConPtr->getETFuncPtr(++i);
    }
  }

  if( i != _arraySize ) {
    cerr << "\n*** WARNING *** "
         << __FILE__ << ", " << __LINE__ << ": "
         << "ETFncData::recalc(): "
         << "\n*** WARNING ***  _arraySize is being reset from "
         << _arraySize << " to " << i << "."
         << "\n*** WARNING *** "
         << endl;
  }
  _arraySize = i;
}


double ETFncData::getHorTune()
{
  return _currentTune[0];
}


double ETFncData::getVerTune()
{
  return _currentTune[1];
}
