/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      RayTrace.h
******  Version:   3.2
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
******  Partial Revision History:
******
******  June, 2005 
******  - Axes, grids, and tic labels added with the assistance
******  of Summer student employee Oleg Mokhov.
******  
******  Usage, modification, and redistribution are subject to terms
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef RAYTRACE_H
#define RAYTRACE_H

#include <iostream>

#include <qpushbutton.h>
#include <qmenubar.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgl.h>

#include "slist.h"
#include "ColorWheel.h"
#include "VectorD.h"

// Predeclaration of classes...
class beamline;
class BeamlineContext;

#ifndef MONITOR_H
#include "monitor.h"
#endif

#ifndef LATTFUNCSAGE_H
#include "LattFuncSage.h"
#endif


class RayDrawSpace;
class RayTrace;
class PointEdit;

typedef void (*RayDrawFunc) ( RayDrawSpace* );

struct Ray
{
  Ray();
  ~Ray();

  double s;
  Vector z;
};


class RayTrace : public QVBox
{
Q_OBJECT

public:
  RayTrace( beamline* ,       QWidget* parent=0, const char* name=0, WFlags f=0);
  RayTrace( BeamlineContext*, QWidget* parent=0, const char* name=0, WFlags f=0);
  ~RayTrace();
  int run();

  slist                   _history;
  LattFuncSage::lattFunc* _p_info;

  void      setBeamline( const beamline* );
  void      _process ( RayDrawSpace*, double, double );
  void      setState( const Vector& );

  int       numberOfMonitors() const;
  int       maxHistory() const;

public slots:
  // Conversion slots: connected to RayDrawSpaces
  void   _appendToHistory ( double, const Vector& );

  // Slot connected to track button
  void   _start_callback();

  // Slots connected to menu items
  void   _fileClose       ();
  void   _do_nothing      ();
  void   _edit_clear      ();

  void   _view_zoom_out   ();
  void   _view_zoom_in    ();
  void   _view_zoom_reset ();

  void   _opt_dsp_axes    ();
  void   _opt_dsp_grid    ();
  void   _opt_dsp_nums    ();
  void   _opt_largePoints ();
  void   _opt_smallPoints ();
  void   _opt_setIter     ();
  void   _opt_setHistory  ();
  void   _opt_bg_black    ();
  void   _opt_bg_white    ();
  void   _opt_bg_yellow   ();
  void   _opt_contmode    ();
  void   _opt_snglmode    ();

  // Slots connected to PointEdit objects
  void   _new_x           ( double );
  void   _new_xp          ( double );
  void   _new_y           ( double );
  void   _new_yp          ( double );

  // Slot connected to timer ...
  void   _iterate();
  // ... and its helpers.
  void   _pushParticle();
  void   _pushBunch();

private:
  void             _finishConstructor();

  int              _number;
  int              _n_monitor;
  int              _maxHistory;

  QTimer*          _p_timer;
  RayDrawSpace*       _p_leftWindow;
  RayDrawSpace*       _p_rightWindow;
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
  bool             _continuous;
  bool             _isIterating;
};


class RayDrawSpace : public QGLWidget
{
Q_OBJECT

public:
  RayDrawSpace( RayTrace*, QHBox*, const char* = 0 );
  ~RayDrawSpace();

  void setColors( GLdouble, GLdouble, GLdouble );
  void setClearColor( GLclampf, GLclampf, GLclampf, GLclampf );
  void setDraw( RayDrawFunc );

  void toggleAxisDisplay();
  void toggleGridDisplay();
  void toggleNumberDisplay();

  void paintGL();

  static void drawH_ViewRect( RayDrawSpace* );
  static void drawV_ViewRect( RayDrawSpace* );

  void doDrawAxes();
  void doDrawGrid();
  void doDrawNums();

  void multScaleBy( double );
  void setScaleTo ( double );
  void setPointSize( int );
  void setRange( double, double, double, double );

  int handle( int );

public slots:
  void _incrementColor();

private:
  RayTrace*   _topRayTrace;
  RayDrawFunc _myFunc;
  int         _pointSize;
  GLdouble _r, _g, _b;
  GLclampf _rClr, _gClr, _bClr, _aClr;
  char _myName[20];

  double _xLo, _yLo, _xHi, _yHi;

  ColorWheel _myWheel;

  void mousePressEvent( QMouseEvent* );

  void _computeGridBounds( double&, double&, double&, double&,
			   double&, double&, double& ) const;

  bool _drawAxes;
  bool _drawGrid;
  bool _drawNums;
};



inline int RayTrace::numberOfMonitors() const
{
  return _n_monitor;
}


inline int RayTrace::maxHistory() const
{
  return _maxHistory;
}


#endif // RAYTRACE_H

