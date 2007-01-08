#ifndef ETFNCDATA_H
#define ETFNCDATA_H

#include <beamline/beamline.h>
#include <physics_toolkit/LattFuncSage.h>
#include <CHEFPlotData.h>
#include <ostream>
#include <vector>


class Particle;
class BeamlineContext;

class ETFncData: public CHEFPlotData 
{
 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double           _currentTune[2];
    double           _targetHorTune;
    double           _dnu_x;

   std::vector<double>           _azimuth;
   std::vector<double>           _beta_H;
   std::vector<double>           _alpha_H;
   std::vector<double>           _beta_V;
   std::vector<double>           _alpha_V;
   std::vector<double>           _inv_beta_H;
   std::vector<double>           _inv_beta_V;
   std::vector<double>           _root_beta_H;
   std::vector<double>           _root_beta_V;
   std::vector<double>           _disp_H;
   std::vector<double>           _disp_V;

    char*   _name;

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    

 public:
    ETFncData( const Particle&, beamline*, std::ostream* = &std::cout, std::ostream* = &std::cerr);
    ETFncData( BeamlineContext*, std::ostream* = &std::cout, std::ostream* = &std::cerr);
   ~ETFncData();

    void resetErrorStream ( std::ostream* stderrstream );
    void resetOutputStream( std::ostream* stdoutstream );

    double getHorTune();
    double getVerTune();

    void doCalc();
    void makeCurves();

 protected:
    std::ostream* _errorStreamPtr;
    std::ostream* _outputStreamPtr;
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
