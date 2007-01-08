/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:  CHEFGUI.cpp                                           ****** 
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

#ifndef LATTFNCDATA_H
#define LATTFNCDATA_H

#include <beamline.h>
#include <LattFuncSage.h>
#include <CHEFPlotData.h>

#include <ostream>
#include <vector>
#include <boost/shared_ptr.hpp>


class BeamlineContext;

class LattFncData: public CHEFPlotData 
{
 public:
    LattFncData( const Particle&, beamline*, std::ostream* = &std::cout, std::ostream* = &std::cerr );
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

    std::vector<double>   _azimuth;
    std::vector<double>   _beta_H;
    std::vector<double>   _beta_V;
    std::vector<double>   _inv_beta_H;
    std::vector<double>   _inv_beta_V;
    std::vector<double>   _root_beta_H;
    std::vector<double>   _root_beta_V;
    std::vector<double>   _alpha_H;
    std::vector<double>   _alpha_V;
    std::vector<double>   _psi_H;
    std::vector<double>   _psi_V;
    std::vector<double>   _disp_H;
    std::vector<double>   _disp_V;

    std::string                 _name;

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
};


inline void LattFncData::resetErrorStream( std::ostream* x )
{
  _errorStreamPtr = x;
}

inline void LattFncData::resetOutputStream( std::ostream* x )
{
  _outputStreamPtr = x;
}


#endif // LATTFNCDATA_H
