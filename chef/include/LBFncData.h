#ifndef LBFNCDATA_H
#define LBFNCDATA_H

#include <qobject.h>

#include "beamline.h"
#include "LattFuncSage.h"
#include "chefplotdata.h"
#include <boost/shared_array.hpp>


class BeamlineContext;

class LBFncData : public CHEFPlotData
{
 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;
    double           _currentTune[2];

    int     _arraySize;

    // The following representation is named
    //   to agree with Lebedev & Bogacz, 
    //   "Betatron Motion with Coupling ..."
    boost::shared_array<double> _azimuth;
    boost::shared_array<double> _beta_1x;
    boost::shared_array<double> _beta_1y;
    boost::shared_array<double> _beta_2x;
    boost::shared_array<double> _beta_2y;
    boost::shared_array<double> _alpha_1x;
    boost::shared_array<double> _alpha_1y;
    boost::shared_array<double> _alpha_2x;
    boost::shared_array<double> _alpha_2y;
    boost::shared_array<double> _u1;
    boost::shared_array<double> _u2;
    boost::shared_array<double> _u3;
    boost::shared_array<double> _u4;
    boost::shared_array<double> _nu_1;
    boost::shared_array<double> _nu_2;

    char*   _name;

    void _finishConstructor();

 public:
    LBFncData( const Particle&, beamline*, std::ostream* = &std::cout, std::ostream* = &std::cerr);
    LBFncData( BeamlineContext*, std::ostream* = &std::cout, std::ostream* = &std::cerr);
    ~LBFncData();
    
    void resetErrorStream( std::ostream* );
    void resetOutputStream( std::ostream* );

    double getHorTune();
    double getVerTune();

    void doCalc();
    void makeCurves();

 protected:
    ostream* _errorStreamPtr;
    ostream* _outputStreamPtr;
};


inline void LBFncData::resetErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}


inline void LBFncData::resetOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}

#endif // LBFNCPLT_H
