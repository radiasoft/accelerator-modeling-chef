#ifndef LATTFNCDATA_H
#define LATTFNCDATA_H

#include "beamline.h"
#include "LattFuncSage.h"
#include "chefplotdata.h"

#include <ostream>
#include <boost/shared_array.hpp>


class BeamlineContext;

class LattFncData: public CHEFPlotData 
{
 public:
    LattFncData( beamline*,        std::ostream* = &std::cout, std::ostream* = &std::cerr );
    LattFncData( BeamlineContext*, std::ostream* = &std::cout, std::ostream* = &std::cerr );
   ~LattFncData();

    void resetErrorStream ( std::ostream* );
    void resetOutputStream( std::ostream* );
   
    double getHorTune();
    double getVerTune();

    void doCalc();
    void makeCurves();

 protected:
    std::ostream* _errorStreamPtr;
    std::ostream* _outputStreamPtr;

 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double           _currentTune[2];
    double           _dnu_x;

    int              _arraySize;

    boost::shared_array<double>   _azimuth;
    boost::shared_array<double>   _beta_H;
    boost::shared_array<double>   _alpha_H;
    boost::shared_array<double>   _psi_H;
    boost::shared_array<double>   _beta_V;
    boost::shared_array<double>   _alpha_V;
    boost::shared_array<double>   _psi_V;
    boost::shared_array<double>   _inv_beta_H;
    boost::shared_array<double>   _inv_beta_V;
    boost::shared_array<double>   _root_beta_H;
    boost::shared_array<double>   _root_beta_V;
    boost::shared_array<double>   _disp_H;
    boost::shared_array<double>   _disp_V;

    std::string                 _name;

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor();
};


inline void LattFncData::resetErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}

inline void LattFncData::resetOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}


#endif // LATTFNCDATA_H
