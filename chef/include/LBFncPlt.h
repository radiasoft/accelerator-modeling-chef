/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      LBFncPlt.h
******  Version:   1.0
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


#ifndef LBFNCPLT_H
#define LBFNCPLT_H

#include <qobject.h>

#include "beamline.h"
#include "LattFuncSage.h"

class CHEFPlotMain;
class CHEFPlotData;
class BeamlineContext;

// ++----++++----++++----++++----++++----++++----++++----++
namespace CHEF_domain
{

class LBFncPlt : public QObject
{
    Q_OBJECT

 private:
    CHEFPlotMain*    _plotter;
    CHEFPlotData*    _di;
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    int     _arraySize;
    double* _azimuth;

    // The following representation is named
    //   in agreement with Lebedev & Bogacz, 
    //   "Betatron Motion with Coupling ..."
    double* _beta_1x;
    double* _beta_1y;
    double* _beta_2x;
    double* _beta_2y;
    double* _alpha_1x;
    double* _alpha_1y;
    double* _alpha_2x;
    double* _alpha_2y;
    double* _u1;
    double* _u2;
    double* _u3;
    double* _u4;
    double* _nu_1;
    double* _nu_2;

    char*   _name;

    void _finishConstructor( const char* name );

    void setErrorStream( ostream* );
    void setOutputStream( ostream* );

public:
    LBFncPlt( beamline*, const char* = 0 );
    LBFncPlt( BeamlineContext*, const char* = 0 );
    ~LBFncPlt();

    void setCaption( const char* );

protected:
    ostream* _errorStreamPtr;
    ostream* _outputStreamPtr;
    void _recalc();

private slots:
  void _selfDestruct();
};


inline void LBFncPlt::setErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}

inline void LBFncPlt::setOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}

}; // end namespace CHEF

#endif // LBFNCPLT_H
