#ifndef MOMENTSFNCDATA_H
#define MOMENTSFNCDATA_H

#include "beamline.h"
#include "LattFuncSage.h"
#include <chefplotdata.h>
#include <boost/shared_array.hpp>

class BeamlineContext;


class MomentsFncData : public CHEFPlotData
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

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor();

 public:
    MomentsFncData( beamline*        ,  ostream* stdoutstream = &std::cout, ostream* stderrstream=&std::cerr );
    MomentsFncData( BeamlineContext* ,  ostream* stdoutstream = &std::cout, ostream* stderrstream=&std::cerr );
    ~MomentsFncData();

    void resetErrorStream(  ostream* );
    void resetOutputStream( ostream* );

    double getHorTune();
    double getVerTune();

    void doCalc();
    void makeCurves();

 protected:
    ostream* _errorStreamPtr;
    ostream* _outputStreamPtr;
};


inline void MomentsFncData::resetErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}

inline void MomentsFncData::resetOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}

#endif // MOMENTSFNDATA_H
