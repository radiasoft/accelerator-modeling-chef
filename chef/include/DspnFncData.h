#ifndef DSPNFNDATA_H
#define DSPNFNDATA_H

#include "beamline.h"
#include "DispersionSage.h"
#include <chefplotdata.h>
#include <ostream>
#include <boost/shared_array.hpp>


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

    boost::shared_array<double>          _azimuth;
    boost::shared_array<double>          _clo_H;
    boost::shared_array<double>          _clo_V;
    boost::shared_array<double>          _disp_H;
    boost::shared_array<double>          _disp_V;
    boost::shared_array<double>          _dPrime_H;
    boost::shared_array<double>          _dPrime_V;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor( );

 public:
    DspnFncData( beamline*,        std::ostream* = &std::cout, std::ostream* = &std::cerr );
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
    ostream*   _errorStreamPtr;
    ostream*   _outputStreamPtr;
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
