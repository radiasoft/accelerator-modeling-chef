/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      ETFncPlt.h
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


#ifndef ETFNCPLT_H
#define ETFNCPLT_H

#include <qframe.h>

#include "beamline.h"
#include "LattFuncSage.h"

class QMenuBar;
class QPopupMenu;
class QwtPlot;
class QwtCounter;
class QLabel;
class QwtPlot;
class QLineEdit;

class BeamlineContext;

// ++----++++----++++----++++----++++----++++----++++----++
namespace CHEF_domain
{

class ETFncPlt : public QWidget
{
    Q_OBJECT

 public:

class InputFrame : public QFrame
{

 public:

    QLabel* _lbl_H_Tune;
    QLabel* _lbl_V_Tune;
    // REMOVE QwtCounter *cntDamp;
    QLineEdit *_tuneEditor;
    QLabel *lblInfo;

    InputFrame(QWidget *p = 0, const char *name = 0);
    ~InputFrame();

};

 private:
    BeamlineContext* _bmlConPtr;
    bool             _deleteContext;

    double        _currentTune[2];
    double        _targetHorTune;
    double        _dnu_x;

    int    _arraySize;
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

    // Widget set .....
    enum { betaPlot=0, invPlot, rootPlot } _plotType;

    QMenuBar* _myMenuPtr;
    QPopupMenu* _fileMenu;
    QPopupMenu* _viewMenu;
    QPopupMenu* _varMenu;
    int         _zoomItemID;
    QwtPlot *plt;
    InputFrame *frmInp;
    QPoint p1;
    int d_zoom;
    int d_zoomActive;

    long crv1, crv2, crv3, crv4;
    long mrk1, mrk2;
    
    void _finishConstructor();

public:
    
    ETFncPlt( beamline*, QWidget* = 0, const char* = 0 );
    ETFncPlt( BeamlineContext*, QWidget* = 0, const char* = 0 );
    ~ETFncPlt();

protected:
    
    void recalc();
    void resizeEvent(QResizeEvent *e);

    private slots:

    void plotMousePressed(const QMouseEvent &e);
    void plotMouseReleased(const QMouseEvent &e);
    void plotMouseMoved(const QMouseEvent &e);
    
    void _print();
    void _fileClose();
    void _zoom();
    void _setBetaPlot();
    void _setInvPlot();
    void _setRootPlot();
    void _fileSaveAs();
    void _do_nothing();
};

}; // end namespace CHEF

#endif // ETFNCPLT_H
