/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                    
******  File:      TrbWidget.h
******  Version:   2.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Authors:   Leo Michelotti
******             Nadezhda Shemyakina  (Summer, 2001)
******             
******             with ideas taken from a simple trackball
******             widget written by Josef Wilgen.
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


// In header of original version:
// File: TrbWidget.h
// 
// Modifed from animator.h, uses 
// trackball to orient viewer.
//
// Leo Michelotti
// July 13, 2001
//
//

#ifndef TRBWIDGET_H
#define TRBWIDGET_H

#undef connect

#include <qpushbutton.h>
#include <qmenubar.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgl.h>

#include "slist.h"

namespace CHEF_domain
{
// typedef void (*DrawFunc) ( DrawSpace3D* );

// Forward declaration
class TrbWidget;
class Orbit;

// Q_OBJECTs cannot be a nested class
//   so this must be defined independently.
class DrawSpace3D : public QGLWidget
{
Q_OBJECT

public:
  DrawSpace3D( TrbWidget* );
  ~DrawSpace3D();

  void paintGL();
  void startIterating();
  void stopIterating();

  double getTheta();
  double getPhi();
  void setTheta( double );
  void setPhi( double );

private:
  TrbWidget* _topTrbWidget;
  // REMOVE: char _myName[20];
  bool _isIterating;
  bool _cameraInitialized;

  // Light angles
  double _theta;
  double _phi;

  // Basic drawing functions
  void _drawOneLine( double, double, double, double );
  void _output(double, double, double );
  void _zoutput(double, double, double, double );
  void _choutput(double, double, char* );
  void _drawGrid();

  // Overloaded mouse handling functions
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );

  // Camera parameters
  double           _cameraFrom[3];
  double           _cameraTo[3];
  double           _cameraUp[3];

  // Light parameters
  GLfloat _light_position[4];
  GLfloat _light_ambience[4];
  GLfloat _yellow_color[4];
  GLfloat _dark_yellow_color[4];


  // Formally global quantities
  //   controlling the plotting coordinates
  double _x_min, _x_max, _x_center, _x_scale;
  double _y_min, _y_max, _y_center, _y_scale;
  double _z_min, _z_max, _z_center, _z_scale;

  double _x_cell_width;
  double _y_cell_width;
  double _sx_cell_width;
  double _sy_cell_width;

  // REMOVE: double _sc_size_x, _sc_size_y;
  // REMOVE: double _nop_scale;
};


class TrbWidget : public QVBox
{
Q_OBJECT

public:
  TrbWidget( slist& orbitList );
  // The TrbWidget constructor assumes ownership
  //   of all Orbits in orbits3D. In the process
  //   it empties the list.

  ~TrbWidget();

  const Orbit* nextOrbit();

  friend class DrawSpace3D;

// Slots
public slots:
  // Slot connected to animation button
  void   _start_callback();

  // Slots connected to menu items
  void _do_nothing();
  void _file_close();
  void _light_sel();

  // Slot connected to timer.
  void   _iterate();


// Private data
private:
  slist            _myOrbits;
  QTimer*          _p_timer;
  QMenuBar*        _p_menu;
  DrawSpace3D*     _p_glWindow;
  QHBox*           _p_trackBox;
  QPushButton*     _p_startBtn;
  QLabel*          _p_blankLabel;
  bool             _isIterating;
};


};  // end namespace CHEF

#endif // TRBWIDGET_H
