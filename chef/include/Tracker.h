/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      Tracker.h
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


// ************************************
// 
// File: Tracker.cc
// 
// Author: Leo Michelotti
// Date: May 24, 2001
// 
// ************************************


#ifndef TRACKER_H
#define TRACKER_H

#include <iostream>

#include <qpushbutton.h>
#include <qmenubar.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qgl.h>

#include "slist.h"
#include "ColorWheel.h"
#include "VectorD.h"

// Predeclaration of classes...
class beamline;
class BeamlineContext;

#ifndef LATTFUNCSAGE_H
#include "LattFuncSage.h"
#endif


namespace CHEF_domain
{
class DrawSpace;
class Tracker;
class PointEdit;

typedef void (*DrawFunc) ( DrawSpace* );

class Orbit
{
  public: 
    Orbit( const Vector& );  // Arguments are always cloned.
    Orbit( const Vector* );
    ~Orbit();

    void add( const Vector& );
    void add( const Vector* );

    const Vector* lastPoint();
    const Vector* firstPoint();

    GLdouble Red();
    GLdouble Blue();
    GLdouble Green();
    void setColor( GLdouble, GLdouble, GLdouble );

    class Iterator 
    {
      public:
        Iterator( const Orbit& );
        Iterator( const Orbit* );
        Iterator( const Iterator& );
        ~Iterator();
      
        const Vector* operator++( int );    // postfix increment
        // ??? Should use PhaseSpacePoint rather than Vector.
        void reset();
        void goBack( int = 1 );
      
      private:
        dlist_iterator* _getNext;
    };
    friend class Iterator;

  private:
    dlist    _history;
    GLdouble _red;
    GLdouble _green;
    GLdouble _blue;
};


class Tracker : public QVBox
{
Q_OBJECT

public:
  Tracker( beamline* );
  Tracker( BeamlineContext* );
  ~Tracker();
  int run();

  slist                   _orbits;
  LattFuncSage::lattFunc* _p_info;

  // REMOVE void      setBeamline( const beamline* );
  // REMOVE void      _process ( DrawSpace*, double, double );
  void      setState( const Vector& );

public slots:
  // Conversion slots: connected to DrawSpaces
  void   _cnvFromHViewRect( double, double );
  void   _cnvFromVViewRect( double, double );
  void   _cnvFromHViewNorm( double, double );
  void   _cnvFromVViewNorm( double, double );
  void   _cnvFromIHIV     ( double, double );
  void   _cnvFromPhiHPhiV ( double, double );

  // Slot connected to track button
  void   _start_callback();

  // Slots connected to menu items
  void   _fileClose       ();
  void   _do_nothing      ();
  void   _edit_clear      ();
  void   _edit_select     ();
  void   _file_exit       ();

  void   _view_rect       ();
  void   _view_norm       ();
  void   _view_actang     ();
  void   _view_zoom_out   ();
  void   _view_zoom_in    ();
  void   _view_zoom_s     ();
  void   _view_zoom_reset ();
  void   _view_zoom       ();

  void   _opt_largePoints ();
  void   _opt_smallPoints ();
  void   _opt_setIter     ();
  void   _opt_bg_black    ();
  void   _opt_bg_white    ();
  void   _opt_bg_yellow   ();

  void   _tool_pdicOrb    ();
  void   _tool_dppMod     ();

  // Slots connected to PointEdit objects
  void   _new_x           ( double );
  void   _new_xp          ( double );
  void   _new_y           ( double );
  void   _new_yp          ( double );

  // Slot connected to timer.
  void   _iterate();

private:
  void             _finishConstructor();
  void             _makeNewOrbit();

  int              _number;
  ColorWheel       _myWheel;

  QTimer*          _p_timer;
  DrawSpace*       _p_leftWindow;
  DrawSpace*       _p_rightWindow;
  QHBox*           _p_trackBox;
  QPushButton*     _p_startBtn;
  QLabel*          _p_blankLabel;

  QHBox*           _p_numberDisplay;
  QHBox*           _p_phaseSpaceViews;
  PointEdit*       _p_x_input;
  PointEdit*       _p_xp_input;
  PointEdit*       _p_y_input;
  PointEdit*       _p_yp_input;
  QLabel*          _p_x_label;
  QLabel*          _p_xp_label;
  QLabel*          _p_y_label;
  QLabel*          _p_yp_label;

  BeamlineContext* _bmlConPtr;
  bool             _deleteContext;
  char             _isIterating;
  Orbit*           _p_currOrb;
};


class DrawSpace : public QGLWidget
{
Q_OBJECT

public:
  DrawSpace( Tracker*, QHBox*, const char* = 0 );
  ~DrawSpace();

  void setColors( GLdouble, GLdouble, GLdouble );
  void setClearColor( GLclampf, GLclampf, GLclampf, GLclampf );
  void setDraw( DrawFunc );
  void paintGL();

  static void drawH_ViewRect( DrawSpace* );
  static void drawV_ViewRect( DrawSpace* );
  static void drawH_ViewNorm( DrawSpace* );
  static void drawV_ViewNorm( DrawSpace* );
  static void drawIHIV      ( DrawSpace* );
  static void drawPhiHPhiV  ( DrawSpace* );

  void multScaleBy( double );
  void setScaleTo ( double );
  void setPointSize( int );
  void setRange( double, double, double, double );

  // REMOVE: int handle( int );
  bool ViewIs( DrawFunc );

  void activateZoom();
  void resetZoom();

public slots:
  void deactivateZoom();

signals:
  void _new_point( double, double );
  void _startedZoom();

private:
  Tracker*   _topTracker;
  DrawFunc _myFunc;
  int      _pointSize;
  GLdouble _r, _g, _b;
  GLclampf _rClr, _gClr, _bClr, _aClr;
  char _myName[20];

  bool   _isZooming;
  bool   _zoomActive;
  bool   _zoomed;

  double _xLo, _yLo, _xHi, _yHi;
  double _xbf, _ybf;
  int    _ixbf, _iybf;

  // Mouse handling functions
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );
};


// Inlined functions

inline GLdouble Orbit::Red()
{
  return _red;
}

inline GLdouble Orbit::Green()
{
  return _green;
}

inline GLdouble Orbit::Blue()
{
  return _blue;
}

};  // end namespace CHEF

#endif // TRACKER_H
