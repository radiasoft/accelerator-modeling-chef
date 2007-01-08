/**************************************************************************
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


#include <LBFncData.h>
#include <BeamlineContext.h>
#include <qwt/qwt_plot.h>
#include <boost/shared_ptr.hpp>
#include <CHEFCurve.h>
#include <CurveData.h>


// This undef is (still??) needed because of the compiler.
#undef connect



using namespace std;

// External variable that has to be defined for the MAD parser
// Already defined in CHEF.cc. This is a problem!!
// madparser* mp = 0;


LBFncData::LBFncData( const Particle& prt, beamline* pBml, std::ostream* stdoutstream, std::ostream* stderrstream )
: _bmlConPtr( new BeamlineContext( prt, pBml, false ) ),
  _deleteContext(false),
  _errorStreamPtr(stderrstream),
  _outputStreamPtr(stdoutstream)
{}


LBFncData::LBFncData( BeamlineContext* bcp, std::ostream* stdoutstream, std::ostream* stderrstream )
: _bmlConPtr(bcp),
  _deleteContext(false),
  _name(0),
  _errorStreamPtr(stderrstream),
  _outputStreamPtr(stdoutstream)
{}


void LBFncData::makeCurves()
{
  // Convert to "curves"
  boost::shared_ptr<CHEFCurve> 
    c1x = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( &_azimuth[0], &_beta_1x[0], _azimuth.size()), "<hor|beta|hor>" ) );

  boost::shared_ptr<CHEFCurve>
    c1y = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( &_azimuth[0], &_beta_1y[0], _azimuth.size()), "<ver|beta|hor>" ) );

  boost::shared_ptr<CHEFCurve>
    c2x = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( &_azimuth[0], &_beta_2x[0], _azimuth.size()), "<hor|beta|ver>" ) );

  boost::shared_ptr<CHEFCurve>
    c2y = boost::shared_ptr<CHEFCurve> ( new CHEFCurve( CurveData( &_azimuth[0], &_beta_2y[0], _azimuth.size()),  "<ver|beta|ver>" ) );


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

  setBeamline( *_bmlConPtr->cheatBmlPtr() ); // false = do not clone line   
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

  std::vector<LBSage::Info> const& lb_array = _bmlConPtr->getLBArray();

  for ( std::vector<LBSage::Info>::const_iterator it = lb_array.begin(); it != lb_array.end(); ++it ) {

      _azimuth.push_back( it->arcLength );
      _beta_1x.push_back( it->beta_1x   );
      _beta_1y.push_back( it->beta_1y   );
      _beta_2x.push_back( it->beta_2x   );
      _beta_2y.push_back( it->beta_2y   );

  }


}


double LBFncData::getHorTune()
{
  return _currentTune[0];
}


double LBFncData::getVerTune()
{
  return _currentTune[1];
}
