////////////////////////////////////////////////////////////
// 
// File:          Tracker.h
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. May 24, 2001.    (original version)
// * Originally written for a precursor of chef.
// 
// August, 2002 - March, 2003.
// * Modified slightly and used while studying
//   tomography in longitudinal phase space.
// 
// April - October, 2004.
// * Incremental changes and incorporation into chef.
// 
// Thursday. May 29, 2008.    (this file)
// * Specialized version: probably temporary.
//   * Built by cutting and pasting pieces from older Tracker.cc 
//     and a program called fermiModel.cc
//   * Still needs to be cleaned up.
// 
////////////////////////////////////////////////////////////


#ifndef TRACKER_H
#define TRACKER_H

#include <iostream>
#include <stdlib.h>

#include <list>

#include <qpushbutton.h>
#include <qmenubar.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>

#include <basic_toolkit/ColorWheel.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/mxyzptlk.h>

namespace AESOP_domain
{
class RFMap;
class Orbit;

class DrawSpace2D;
class PointEdit;

class Tracker : public QVBox
{
Q_OBJECT

public:
  Tracker( RFMap*, int = 1 );
  ~Tracker();

  // The following variables are public just for convenience.
  // They should only be modified by class Tracker!
  std::list<Orbit*> _orbits;

  MatrixC          _E;    // matrix of eigenvectors
  MatrixC          _invE; // and its inverse
  CLieOperator*    _XG;   // it turns out that _N is predefined
  CLieOperator*    _T;    // by compiler
  MappingC*        _f;
  MappingC*        _g;

  void      setState( const Vector& );
  void      cnvDataToNormalForm( Vector& );

  bool      isIterating();

public slots:
  // Conversion slots: connected to DrawSpace2Ds
  void   _cnvFromHViewRect( double, double );
  void   _cnvFromHViewNorm( double, double );
  void   _cnvFromHViewActAng( double, double );

  // Slot connected to track button
  void   _start_callback();

  // Slots connected to menu items
  void   _do_nothing      ();
  void   _edit_clear      ();
  void   _file_exit       ();

  void   _view_rect       ();
  void   _view_norm       ();
  void   _view_ActAng     ();
  void   _view_zoom_out   ();
  void   _view_zoom_in    ();
  void   _view_zoom_reset ();
  void   _view_zoom       ();

  void   _opt_largePoints ();
  void   _opt_smallPoints ();
  void   _opt_setIter     ();
  void   _opt_bg_black    ();
  void   _opt_bg_grey     ();
  void   _opt_bg_white    ();

  void   _tool_pdicOrb    ();

  // Slots connected to PointEdit objects
  void   _new_x           ( double );
  void   _new_xp          ( double );

  void   _iterate();

  void   _changeOrder();

private:
  Vector           _state;

  int              _order;
  bool             _initialized;
  Jet__environment_ptr _p_je;
 
  void             _makeNewOrbit();

  int              _number;
  ColorWheel       _myWheel;

  QHBox*           _p_trackBox;
  QPushButton*     _p_startBtn;
  QLabel*          _p_blankLabel;

  DrawSpace2D*     _p_leftWindow;
  QPen*            _penPtr;

  QHBox*           _p_numberDisplay;
  PointEdit*       _p_x_input;
  PointEdit*       _p_xp_input;
  QLabel*          _p_x_label;
  QLabel*          _p_xp_label;

  RFMap*           _p_rf;
  bool             _deleteContext;
  bool             _is_iterating;
  Orbit*           _p_currOrb;
};

};  // end namespace AESOP_domain

#endif // TRACKER_H
