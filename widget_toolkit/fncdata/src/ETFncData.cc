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
#include <CHEFCurve.h>
#include <CurveData.h>
#include <BeamlineContext.h>
#include <boost/shared_ptr.hpp>
#include <qwt/qwt_plot.h>

// This undef is needed because of the compiler.
#undef connect

using namespace std;



ETFncData::ETFncData( BeamlineContext* bcp,  std::ostream* stdoutstream, std::ostream* stderrstream )
: 
  _bmlConPtr(bcp), 
  _deleteContext(false),
  _name(0),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)
{}


ETFncData::ETFncData( const Particle& prt, beamline* pBml,  std::ostream* stdoutstream, std::ostream* stderrstream)
:
  _bmlConPtr(0), 
  _deleteContext(true),
  _name(0),
  _plotType( betaPlot ),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)
{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );
}


void ETFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve> 
    c1 ( new CHEFCurve( CurveData( &_azimuth[0],  &_beta_H[0], _azimuth.size() ), "Horizontal Beta" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve(  CurveData( &_azimuth[0],  &_beta_V[0], _azimuth.size() ), "Vertical Beta" ) );

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
  setBeamline( *_bmlConPtr->cheatBmlPtr() ); // false = do not clone line   
}


ETFncData::~ETFncData()
{

  if(_deleteContext) { delete _bmlConPtr; }
}



void ETFncData::doCalc()
{
  _currentTune[0] = _bmlConPtr->getHorizontalEigenTune();
  _currentTune[1] = _bmlConPtr->getVerticalEigenTune();



  std::vector<EdwardsTengSage::Info> const&  et_array = _bmlConPtr->getETArray();

  for ( std::vector<EdwardsTengSage::Info>::const_iterator it = et_array.begin();  
	it != et_array.end();  ++it) {

       _azimuth.push_back(     it->arcLength      );
        _beta_H.push_back(     it->beta.hor       );
       _alpha_H.push_back(     it->alpha.hor      );
        _beta_V.push_back(     it->beta.ver       );
       _alpha_V.push_back(     it->alpha.ver      );
    _inv_beta_H.push_back( 1.0/it->beta.hor       );
    _inv_beta_V.push_back( 1.0/it->beta.ver       );
   _root_beta_H.push_back(sqrt(it->beta.hor)      );
   _root_beta_V.push_back(sqrt(it->beta.ver)      );
//      _disp_H.push_back(     it->dispersion.hor );
//      _disp_V.push_back(     it->dispersion.ver );
        _disp_H.push_back(     0.0                     );
        _disp_V.push_back(     0.0                     );

   
  }

}


double ETFncData::getHorTune()
{
  return _currentTune[0];
}


double ETFncData::getVerTune()
{
  return _currentTune[1];
}
