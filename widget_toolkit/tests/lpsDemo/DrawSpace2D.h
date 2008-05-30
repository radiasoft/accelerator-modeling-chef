////////////////////////////////////////////////////////////
// 
// File:          DrawSpace2D.h
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
// Thursday. May 29, 2008.       (this file)
// 
////////////////////////////////////////////////////////////
// 
// This file is an amalgam drawn from a number of programs going back
// to 2001. (See headers of other files in this directory.  After further
// development, a successor of this file will eventually replace those
// implementations.
// 
////////////////////////////////////////////////////////////

#ifndef DRAWSPACE2D_H
#define DRAWSPACE2D_H

#include <qpixmap.h>
#include <qwidget.h>
#include <qhbox.h>

namespace AESOP_domain
{

class Tracker;
class DrawSpace2D;

typedef void (*DrawFunc) ( DrawSpace2D* );

class DrawSpace2D : public QWidget
{
Q_OBJECT

public:
  DrawSpace2D( Tracker*, QHBox*, const char* = 0 );
  ~DrawSpace2D();

  void setColors( double, double, double );
  void setClearColor( double, double, double );
  void setDraw( DrawFunc );

  void clear();

  void paintEvent( QPaintEvent* );

  static void drawH_ViewRect  ( DrawSpace2D* );
  static void drawH_ViewNorm  ( DrawSpace2D* );
  static void drawH_ViewActAng( DrawSpace2D* );

  void multScaleBy( double );
  void setScaleTo ( double );
  void setPointSize( int );
  void setRange( double, double, double, double );

  void setSize( int w, int h );
  void mark( double x, double y, QPen* z );

  // REMOVE: int handle( int );
  bool ViewIs( DrawFunc );

  void activateZoom();
  void resetZoom();

  void redrawOrbits();


public slots:
  void deactivateZoom();

signals:
  void _new_point( double, double );
  void _startedZoom();

private:
  Tracker* _topTracker;
  DrawFunc _myFunc;
  int      _pointSize;
  double _r, _g, _b;
  double _rClr, _gClr, _bClr;
  char _myName[20];

  bool   _isZooming;
  bool   _zoomActive;
  bool   _zoomed;

  double _xLo, _xHi, _yLo, _yHi;
  double _xbf, _ybf;
  int    _ixbf, _iybf;

  QPixmap _imago;

  // Mouse handling functions
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );
};

};  // end namespace AESOP

#endif // DRAWSPACE2D_H
