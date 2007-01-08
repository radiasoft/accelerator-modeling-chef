/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DspnFncData.h                                     ****** 
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

#ifndef DSPNFNDATA_H
#define DSPNFNDATA_H

#include <beamline.h>
#include <DispersionSage.h>
#include <CHEFPlotData.h>
#include <ostream>
#include <vector>


class BeamlineContext;

class DspnFncData : public CHEFPlotData
{
 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double           _currentTune[2];
    double           _targetHorTune;
    double           _dnu_x;

    int              _arraySize;

    std::vector<double>          _azimuth;
    std::vector<double>          _clo_H;
    std::vector<double>          _clo_V;
    std::vector<double>          _disp_H;
    std::vector<double>          _disp_V;
    std::vector<double>          _dPrime_H;
    std::vector<double>          _dPrime_V;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    

 public:
    DspnFncData( const Particle&, beamline*, std::ostream* = &std::cout, std::ostream* = &std::cerr );
    DspnFncData( BeamlineContext*, std::ostream* = &std::cout, std::ostream* = &std::cerr );
    ~DspnFncData();

    void resetErrorStream(  std::ostream* );
    void resetOutputStream( std::ostream* );

    double getHorTune();
    double getVerTune();

    void doCalc();
    void doCalc( const DispersionSage::Info& );
    void makeCurves();

 protected:
    std::ostream*   _errorStreamPtr;
    std::ostream*   _outputStreamPtr;
};


inline void DspnFncData::resetErrorStream( std::ostream* x )
{
  _errorStreamPtr = x;
}

inline void DspnFncData::resetOutputStream( std::ostream* x )
{
  _outputStreamPtr = x;
}

#endif // DSPNFNCDATA_H
