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
******  File:      SiteViewer.h
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


#ifndef SITEVIEWER_H
#define SITEVIEWER_H

#include <qvbox.h>
#include <qgl.h>

class beamline;
class bmlnElmnt;
class BeamlineContext;
class QPopupMenu;
class QString;
class QMouseEvent;

#ifndef BMLEXPTREE_H
#include "BeamlineExpressionTree.h"
#endif

class SiteViewer : public QVBox
{
Q_OBJECT

// Nested class SiteViewer::Wndw
class Wndw : public QGLWidget
{
 public:
  Wndw( SiteViewer* );
  ~Wndw();

  void paintGL();
  void activateZoom();
  void deactivateZoom();
  void zoomOut();
  void resetZoom();
  void toggleHighlight();

 private:
  SiteViewer* _parent;
  double _xLo, _xHi, _yLo, _yHi;
  double _xbf, _ybf;
  double _xLobf, _xHibf, _yLobf, _yHibf;
  int    _ixbf, _iybf;

  bool   _isZooming;
  bool   _zoomActive;
  bool   _startedZoom;
  bool   _startedScroll;
  bool   _zoomed;
  bool   _hilights;

  // Mouse handling functions
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );

};


public:
  SiteViewer( const Particle&, beamline*, QWidget* = 0, const char* = 0, WFlags = 0 );
  SiteViewer( BeamlineContext&, QWidget* = 0, const char* = 0, WFlags = 0 );
  ~SiteViewer();

  bool filter( const bmlnElmnt* ) const;

  int    N();
  double X( int );
  double Y( int );
  double Z( int );

  friend class Wndw;

public slots:
  void _fileSite();
  void _fileClose();
  void _viewZoom();
  void _viewZoomOut();
  void _viewReset();
  void _viewHilite();
  void _optionOrigin();
  void _optionHilt( const BoolNode& );

private:
  static const BoolNullNode nada;
  void _finishConstructor();

  Wndw* _myGLwindow;

  int     _n;
  double* _x;
  double* _y;
  double* _z;
  double  _xmin;
  double  _xmax;
  double  _ymin;
  double  _ymax;
  double  _zmin;
  double  _zmax;

  BeamlineContext* _bmlConPtr;
  bool             _deleteContext;
  bool             _showStart;
  bmlnElmnt**      _element;

  BoolNode*  _filterPtr; // owned

  QPopupMenu* _viewMenuPtr;
  QPopupMenu* _optionMenuPtr;
  int         _id_viewZoom;
  int         _id_optHilit;
  int         _id_optShoOrig;
};


// From SiteViewer.icc

inline int SiteViewer::N()
{
  return _n;
}


inline double SiteViewer::X( int i )
{
  if( 0 <= i && i < _n ) return _x[i];
  else                   return 0.0;
}


inline double SiteViewer::Y( int i )
{
  if( 0 <= i && i < _n ) return _y[i];
  else                   return 0.0;
}


inline double SiteViewer::Z( int i )
{
  if( 0 <= i && i < _n ) return _z[i];
  else                   return 0.0;
}

#endif // SITEVIEWER_H
