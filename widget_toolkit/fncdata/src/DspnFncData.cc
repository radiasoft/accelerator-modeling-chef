/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DspnFncData.cc                                    ****** 
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


#include <iomanip>
#include <string>   // needed for strcat
#include <math.h>

#include <DspnFncData.h>

#include <CHEFCurve.h>
#include <CurveData.h>
#include <complexAddon.h>
#include <bmlfactory.h>
#include <BeamlineContext.h>
#include <GenericException.h>
#include <iosetup.h>

#include <qwt/qwt_plot.h>
#include <boost/shared_ptr.hpp>

using FNAL::pcerr;
using FNAL::pcout;


// This undef is needed because of the compiler.
#undef connect

static double* dnull = 0;

using namespace std;

DspnFncData::DspnFncData( BeamlineContext* bcp, std::ostream*  stdoutstream, std::ostream* stderrstream )
: _bmlConPtr(bcp), 
  _deleteContext(false),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)

{}


DspnFncData::DspnFncData( const Particle& prt, beamline* pBml, std::ostream*  stdoutstream, std::ostream* stderrstream )
: _bmlConPtr(0), 
  _deleteContext(true),
  _errorStreamPtr(stderrstream), 
  _outputStreamPtr(stdoutstream)
{
  _bmlConPtr = new BeamlineContext( prt, pBml, false );
}


void DspnFncData::makeCurves()
{
  boost::shared_ptr<CHEFCurve> 
    c1( new CHEFCurve( CurveData( &_azimuth[0], &_disp_H[0],   _azimuth.size()), "Hor Dispersion" ) );
  boost::shared_ptr<CHEFCurve>
    c2( new CHEFCurve( CurveData( &_azimuth[0], &_disp_V[0],   _azimuth.size()), "Ver Dispersion" ) );
  boost::shared_ptr<CHEFCurve>
    c3( new CHEFCurve( CurveData( &_azimuth[0], &_dPrime_H[0], _azimuth.size()),  "Hor Dispersion Derivative" ) );
  boost::shared_ptr<CHEFCurve>
    c4( new CHEFCurve( CurveData( &_azimuth[0], &_dPrime_V[0], _azimuth.size()),  "Ver Dispersion Derivative" ) );
  boost::shared_ptr<CHEFCurve> 
    c5( new CHEFCurve( CurveData( &_azimuth[0], &_clo_H[0],    _azimuth.size()),  "Hor Closed Orbit" ) );
  boost::shared_ptr<CHEFCurve>
    c6( new CHEFCurve( CurveData( &_azimuth[0], &_clo_V[0],    _azimuth.size()),  "Ver Closed Orbit" ) );


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
  setBeamline( *_bmlConPtr->cheatBmlPtr()); 

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

  std::vector<DispersionSage::Info> const& dispersion_array =  _bmlConPtr->getDispersionArray();

  for ( std::vector<DispersionSage::Info>::const_iterator it  =  dispersion_array.begin(); 
                                                          it !=  dispersion_array.end(); ++it ) {

      _azimuth.push_back(   it->arcLength             );
        _clo_H.push_back(   it->closedOrbit.hor / mm  );
        _clo_V.push_back(   it->closedOrbit.ver / mm  );
       _disp_H.push_back(   it->dispersion.hor        );
       _disp_V.push_back(   it->dispersion.ver        );
     _dPrime_H.push_back(   it->dPrime.hor            );
     _dPrime_V.push_back(   it->dPrime.ver            );


  }
}


double DspnFncData::getHorTune()
{
  return -1.0;
}


double DspnFncData::getVerTune()
{
  return -1.0;
}

