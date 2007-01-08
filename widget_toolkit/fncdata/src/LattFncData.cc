/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      LattFncData.cpp                                   ******
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

#include <LattFncData.h>

#include <complexAddon.h>
#include <bmlfactory.h>
#include <BeamlineContext.h>
#include <CHEFPlotMain.h>
#include <CHEFCurve.h>
#include <CurveData.h>
#include <qwt/qwt_plot.h>


// This undef is needed because of the compiler.
#undef connect

LattFncData::LattFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream )
: _errorStreamPtr(stdoutstream), 
  _outputStreamPtr(stderrstream),
  _bmlConPtr(bcp),  
  _deleteContext(false),
  _name("undefined"),
  _plotType( betaPlot )
{}


LattFncData::LattFncData( Particle const& prt, beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream)
: _errorStreamPtr(stdoutstream), _outputStreamPtr(stderrstream),
  _bmlConPtr(0),  
  _deleteContext( true ),
  _name("undefined"),
  _plotType( betaPlot )
{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );

}


LattFncData::~LattFncData()
{
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


  std::vector<LattFuncSage::lattFunc> const&  twiss_array = _bmlConPtr->getTwissArray();
 
  for ( std::vector<LattFuncSage::lattFunc>::const_iterator it =  twiss_array.begin();  it !=  twiss_array.end(); ++it ) {

      _azimuth.push_back( it->arcLength      );
       _beta_H.push_back(  it->beta.hor      );
      _alpha_H.push_back( it->alpha.hor      );
        _psi_H.push_back(   it->psi.hor      );
       _beta_V.push_back(  it->beta.ver      );
      _alpha_V.push_back( it->alpha.ver      );
        _psi_V.push_back(   it->psi.ver      );
   _inv_beta_H.push_back(  1.0/it->beta.hor  );
   _inv_beta_V.push_back(  1.0/it->beta.ver  );
  _root_beta_H.push_back( sqrt(it->beta.hor) );
  _root_beta_V.push_back( sqrt(it->beta.ver) );
       _disp_H.push_back( it->dispersion.hor );
       _disp_V.push_back( it->dispersion.ver );

  }

}


void LattFncData::makeCurves()
{
  // This test should be removed, but for now I retain it.   - lpjm

  if( _bmlConPtr->isTreatedAsRing() ) {


    boost::shared_ptr<CHEFCurve>  
      c1( new CHEFCurve(  CurveData( &_azimuth[0], &_beta_H[0], _azimuth.size() ), "Horizontal Beta" ) );
    boost::shared_ptr<CHEFCurve>  
      c2( new CHEFCurve(  CurveData( &_azimuth[0], &_beta_V[0], _azimuth.size() ), "Vertical Beta" ) );
    boost::shared_ptr<CHEFCurve> 
      c3( new CHEFCurve(  CurveData( &_azimuth[0], &_disp_H[0], _azimuth.size() ), "Horizontal Dispersion" ) );
    boost::shared_ptr<CHEFCurve>  
      c4( new CHEFCurve(  CurveData( &_azimuth[0], &_disp_V[0], _azimuth.size() ), "Vertical Dispersion" ) );

    c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
    c2->setPen( QPen( "red",   1, Qt::SolidLine ) );
    c3->setPen( QPen( "black", 0, Qt::DashLine ) );
    c4->setPen( QPen( "red",   0, Qt::DashLine ) );

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
    setBeamline( *_bmlConPtr->cheatBmlPtr() ); // false = do not clone line   
  }

  // The following is done if the line is really a line.
  else {

    boost::shared_ptr<CHEFCurve>  
      c1( new CHEFCurve(  CurveData(  &_azimuth[0], &_beta_H[0], _azimuth.size()),   "Horizontal Beta" ) );
    boost::shared_ptr<CHEFCurve>  
      c2( new CHEFCurve(  CurveData(  &_azimuth[0], &_beta_V[0], _azimuth.size()),   "Vertical Beta" ) );
    boost::shared_ptr<CHEFCurve> 
      c3( new CHEFCurve(  CurveData(  &_azimuth[0], &_alpha_H[0], _azimuth.size()),  "Horizontal Alpha" ) );
    boost::shared_ptr<CHEFCurve>  
      c4( new CHEFCurve(  CurveData(  &_azimuth[0], &_alpha_V[0], _azimuth.size()),   "Vertical Alpha" ) );

    c1->setPen( QPen( "black", 1, Qt::SolidLine ) );
    c2->setPen( QPen( "red",   1, Qt::SolidLine ) );
    c3->setPen( QPen( "black", 0, Qt::DashLine ) );
    c4->setPen( QPen( "red",   0, Qt::DashLine ) );

    c1->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
    c2->setAxis( QwtPlot::xBottom, QwtPlot::yLeft  );
    c3->setAxis( QwtPlot::xBottom, QwtPlot::yRight );
    c4->setAxis( QwtPlot::xBottom, QwtPlot::yRight );

    // c4->setStyle( QwtCurve::NoCurve ); // this turns off a curve

    addCurve( c1 );
    addCurve( c2 );
    addCurve( c3 );
    addCurve( c4 );

    setXLabel( "Arc Length [m]" );
    setYLabel( "Beta [m]", QwtPlot::yLeft  );
    setYLabel( "Alpha",    QwtPlot::yRight );

    // setScaleMagRight( 5.0 ); 
    setBeamline( *_bmlConPtr->cheatBmlPtr() ); // false = do not clone line   
  }
}


double LattFncData::getHorTune()
{
  return _currentTune[0];
}


double LattFncData::getVerTune()
{
  return _currentTune[1];
}

