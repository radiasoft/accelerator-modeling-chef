#ifndef ETFNCDATA_H
#define ETFNCDATA_H

#include "beamline.h"
#include "LattFuncSage.h"
#include "chefplotdata.h"
#include <ostream>
#include <boost/shared_array.hpp>


class BeamlineContext;

class ETFncData: public CHEFPlotData 
{
 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double           _currentTune[2];
    double           _targetHorTune;
    double           _dnu_x;

    int              _arraySize;

    boost::shared_array<double>          _azimuth;
    boost::shared_array<double>          _beta_H;
    boost::shared_array<double>          _alpha_H;
    boost::shared_array<double>          _beta_V;
    boost::shared_array<double>          _alpha_V;
    boost::shared_array<double>          _inv_beta_H;
    boost::shared_array<double>          _inv_beta_V;
    boost::shared_array<double>          _root_beta_H;
    boost::shared_array<double>          _root_beta_V;
    boost::shared_array<double>          _disp_H;
    boost::shared_array<double>          _disp_V;

    char*   _name;

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor();

 public:
    ETFncData( beamline*,        ostream* = &std::cout, ostream* = &std::cerr);
    ETFncData( BeamlineContext*, ostream* = &std::cout, ostream* = &std::cerr);
   ~ETFncData();

    void resetErrorStream ( std::ostream* stderrstream );
    void resetOutputStream( std::ostream* stdoutstream );

    double getHorTune();
    double getVerTune();

    void doCalc();
    void makeCurves();

 protected:
    ostream* _errorStreamPtr;
    ostream* _outputStreamPtr;
};


inline void ETFncData::resetErrorStream( std::ostream* x )
{
  _errorStreamPtr = x;
}

inline void ETFncData::resetOutputStream( std::ostream* x )
{
  _outputStreamPtr = x;
}


#endif // ETFNCPLT_H
