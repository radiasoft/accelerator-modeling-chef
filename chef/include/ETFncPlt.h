/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      ETFncPlt.h
******  Version:   2.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef ETFNCPLT_H
#define ETFNCPLT_H

#include <qobject.h>

#include "beamline.h"
#include "LattFuncSage.h"

class CHEFPlotMain;
class CHEFPlotData;
class BeamlineContext;

// ++----++++----++++----++++----++++----++++----++++----++
namespace CHEF_domain
{

class ETFncPlt : public QObject
{
    Q_OBJECT

 private:
    CHEFPlotMain*    _plotter;
    CHEFPlotData*    _di;
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double        _currentTune[2];
    double        _targetHorTune;
    double        _dnu_x;

    int     _arraySize;
    double* _azimuth;
    double* _beta_H;
    double* _alpha_H;
    double* _beta_V;
    double* _alpha_V;
    double* _inv_beta_H;
    double* _inv_beta_V;
    double* _root_beta_H;
    double* _root_beta_V;
    double* _disp_H;
    double* _disp_V;

    char*   _name;

    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor( const char* name );

    void setErrorStream( ostream* );
    void setOutputStream( ostream* );

public:
    
    ETFncPlt( beamline*, const char* = 0 );
    ETFncPlt( BeamlineContext*, const char* = 0 );
    ~ETFncPlt();

    void setCaption( const char* );

protected:
    ostream* _errorStreamPtr;
    ostream* _outputStreamPtr;
    void _recalc();

private slots:
  void _selfDestruct();
};


inline void ETFncPlt::setErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}

inline void ETFncPlt::setOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}

}; // end namespace CHEF

#endif // ETFNCPLT_H
