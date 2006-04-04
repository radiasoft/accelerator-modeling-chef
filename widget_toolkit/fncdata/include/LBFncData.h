/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:       LBFncData.h                                      ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******                                                               ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
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

#ifndef LBFNCDATA_H
#define LBFNCDATA_H

#include <qobject.h>

#include <beamline.h>
#include <LattFuncSage.h>
#include <chefplotdata.h>
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
    std::ostream* _errorStreamPtr;
    std::ostream* _outputStreamPtr;
};


inline void LBFncData::resetErrorStream( std::ostream* x )
{
  _errorStreamPtr = x;
}


inline void LBFncData::resetOutputStream( std::ostream* x )
{
  _outputStreamPtr = x;
}

#endif // LBFNCPLT_H
