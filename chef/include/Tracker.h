/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      Tracker.h
******  Version:   4.0
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
    Orbit( const Orbit& );   // Does a deep copy
    ~Orbit();

    void add( const Vector& );
    void add( const Vector* );

    const Vector* lastPoint();
    const Vector* firstPoint();

    GLdouble Red() const;
    GLdouble Green() const;
    GLdouble Blue() const;
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


class OrbitTransformer
{
  public: 
    OrbitTransformer();
    virtual ~OrbitTransformer() {}

    virtual void toState( double, double, Proton* ) const = 0;
    virtual void toDynamics( const Vector&, double*, double*, double* ) const = 0;
    // This assumes, without checking, that the dimension
    //   of the Vector is correct: i.e. it is the same as the
    //   dimension of a proton state.

    inline void set_x_center  ( double u ) { _x_o   = u; }
    inline void set_xp_center ( double u ) { _xp_o  = u; }
    inline void set_y_center  ( double u ) { _y_o   = u; }
    inline void set_yp_center ( double u ) { _yp_o  = u; }
    inline void set_cdt_center( double u ) { _cdt_o = u; }
    inline void set_dpp_center( double u ) { _dpp_o = u; }
    void set_center( const Vector& );
    void set_center( const Particle& );

    inline double get_x_center()   const { return _x_o;   }
    inline double get_xp_center()  const { return _xp_o;  }
    inline double get_y_center()   const { return _y_o;   }
    inline double get_yp_center()  const { return _yp_o;  }
    inline double get_cdt_center() const { return _cdt_o; }
    inline double get_dpp_center() const { return _dpp_o; }
    Vector get_center() const;
    void load_center_into( Particle* ) const;
    void load_center_into( Particle& ) const;

    void copyCenterFrom( const OrbitTransformer* );
    void copyCenterFrom( const OrbitTransformer& );

  protected:
    // Central values of the proton state. 
    //   Typically, these represent a closed or periodic orbit.
    //   The last two are added in anticipation of future upgrades.
    double _x_o;
    double _xp_o;
    double _y_o;
    double _yp_o;
    double _cdt_o;
    double _dpp_o;
};


class RectH : public OrbitTransformer
{
  public: 
    RectH() {}
    ~RectH() {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;
};


class RectV : public OrbitTransformer
{
  public: 
    RectV() {}
    ~RectV() {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;
};


class NormH : public OrbitTransformer
{
  public: 
    NormH() : _alpha(0.0), _beta(1.0) {}
    NormH( double a, double b ) : _alpha(a), _beta(b) {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;

  private:
    double _alpha;
    double _beta;
};


class NormV : public OrbitTransformer
{
  public: 
    NormV() : _alpha(0.0), _beta(1.0) {}
    NormV( double a, double b ) : _alpha(a), _beta(b) {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;

  private:
    double _alpha;
    double _beta;
};


class IHIV : public OrbitTransformer
{
  public: 
    IHIV( double a1, double b1, double a2, double b2 );
    IHIV( double a1, double b1, double a2, double b2, const Particle& );
    // _alphaH(a1), _betaH(b1), _alphaV(a2), _betaV(b2)
    // Last argument used as central particle to determine origin.
    IHIV(); 
    ~IHIV() {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;

  private:
    double _alphaH;
    double _betaH;
    double _alphaV;
    double _betaV;
};


class PhiHPhiV : public OrbitTransformer
{
  public: 
    PhiHPhiV( double a1, double b1, double a2, double b2 );
    PhiHPhiV( double a1, double b1, double a2, double b2, const Particle& );
    PhiHPhiV();
    ~PhiHPhiV() {}

    void toState( double, double, Proton* ) const;
    void toDynamics( const Vector&, double*, double*, double* ) const;

  private:
    double _alphaH;
    double _betaH;
    double _alphaV;
    double _betaV;
};


class Tracker : public QVBox
{
Q_OBJECT

public:
  Tracker( beamline*,        QWidget* parent=0, const char* name=0, WFlags f=0 );
  Tracker( BeamlineContext*, QWidget* parent=0, const char* name=0, WFlags f=0 );
  ~Tracker();
  int run();

  slist                   _orbits;
  LattFuncSage::lattFunc* _p_info;

  // REMOVE void      setBeamline( const beamline* );
  // REMOVE void      _process ( DrawSpace*, double, double );
  void setState( const Vector& );

  bool isIterating();

public slots:
  // Conversion slots: connected to DrawSpaces
  void   _cnvFromView( double, double, const OrbitTransformer* );

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
  void   _view_center     ();

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
  Particle*        _centralParticlePtr;
  bool             _deleteContext;
  bool             _isIterating;
  Orbit*           _p_currOrb;
};


class DrawSpace : public QGLWidget
{
Q_OBJECT

  struct Context {
    double _xLo, _yLo, _xHi, _yHi;
    OrbitTransformer* _transformPtr;
    // Not owned; just held for future reference.

    Context() : _xLo(0.),_yLo(0.),_xHi(0.),_yHi(0.),_transformPtr(0) {}
    ~Context() { _transformPtr = 0; }
  };

public:
  DrawSpace( Tracker*, QHBox*, const char* = 0 );
  ~DrawSpace();

  void setColors( GLdouble, GLdouble, GLdouble );
  void setClearColor( GLclampf, GLclampf, GLclampf, GLclampf );
  void setTransformer( OrbitTransformer* );
  // DrawSpace takes responsibility for the argument.
  //   It must not be deleted or go out of scope after
  //   calling this function.
  void setCenterOn( const Particle& );
  void paintGL();

  void multScaleBy( double );
  void setScaleTo ( double );
  void setCenterTo( double, double );
  void setPointSize( int );
  void setRange( double, double, double, double );

  // REMOVE: int handle( int );
  const OrbitTransformer* getTransformer();

  void storeCurrentContext();
  void setRectContext();
  void setNormContext();
  void setActAngContext();

  void activateZoom();
  void resetZoom();

public:
  static const double DEF_RANGE;
  static const double DEF_X_CENTER;
  static const double DEF_Y_CENTER;

public slots:
  void deactivateZoom();

signals:
  void _new_point( double, double, const OrbitTransformer* );
  void _startedZoom();

private:
  Tracker*     _topTracker;
  OrbitTransformer* _transformPtr;

  Context _rectContext;
  Context _normContext;
  Context _actangContext;
  Context* _currentContextPtr;
  // Not to be deleted: points to one of the above.

  int          _pointSize;
  GLdouble     _r, _g, _b;
  GLclampf     _rClr, _gClr, _bClr, _aClr;
  char         _myName[20];

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

inline GLdouble Orbit::Red() const
{
  return _red;
}

inline GLdouble Orbit::Green() const
{
  return _green;
}

inline GLdouble Orbit::Blue() const
{
  return _blue;
}

inline bool Tracker::isIterating()
{
  return _isIterating;
}

inline const OrbitTransformer* DrawSpace::getTransformer()
{
  return _transformPtr;
}

};  // end namespace CHEF

#endif // TRACKER_H
