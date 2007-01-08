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
#include <CHEFPlotMain.h>
#include <CHEFCurve.h>
#include <CurveData.h>

// This undef is needed because connect is defined by qt as a macro. Unfortunately, the string "connect" is 
// also used in the std c++ library code.  

#undef connect

using namespace std;



MomentsFncData::MomentsFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream)
: 
  _bmlConPtr(bcp), 
  _deleteContext(false),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)

{}


MomentsFncData::MomentsFncData( Particle const& prt, beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream)
: 
  _bmlConPtr(0), 
  _deleteContext(true),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)

{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );
}


void MomentsFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve>  
    c1( new CHEFCurve( CurveData(  &_azimuth[0], &_beta_H[0],  _azimuth.size()), "Horizontal Beta" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve( CurveData(  &_azimuth[0], &_beta_V[0],  _azimuth.size()), "Vertical Beta" ) );
  boost::shared_ptr<CHEFCurve> 
    c3( new CHEFCurve(  CurveData( &_azimuth[0], &_alpha_H[0], _azimuth.size()), "Horizontal Alpha" ) );
  boost::shared_ptr<CHEFCurve>  
    c4( new CHEFCurve(  CurveData( &_azimuth[0], &_alpha_V[0], _azimuth.size()), "Vertical Alpha" ) );

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

  setBeamline( *_bmlConPtr->cheatBmlPtr() ); 
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


  std::vector<CovarianceSage::Info> const&  covariance_array = _bmlConPtr->getCovarianceArray(); 

  for (std::vector<CovarianceSage::Info>::const_iterator it = covariance_array.begin(); it != covariance_array.end(); ++it ) {

      _azimuth.push_back(	it->arcLength       );
       _beta_H.push_back(       it->beta.hor        );
      _alpha_H.push_back(       it->alpha.hor       );
       _beta_V.push_back(       it->beta.ver        );
      _alpha_V.push_back(       it->alpha.ver       );
   _inv_beta_H.push_back(   1.0/it->beta.hor        );
   _inv_beta_V.push_back(   1.0/it->beta.ver        );
  _root_beta_H.push_back(  sqrt(it->beta.hor)       );
  _root_beta_V.push_back(  sqrt(it->beta.ver)       );
     // disp_H.push_back(       it->dispersion.hor  );
     // disp_V.push_back(       it->dispersion.ver  );
       _disp_H.push_back(       0.0                 );
       _disp_V.push_back(       0.0                 );

  }
	
}		


double MomentsFncData::getHorTune()
{
  return _currentTune[0];
}


double MomentsFncData::getVerTune()
{
  return _currentTune[1];
}
