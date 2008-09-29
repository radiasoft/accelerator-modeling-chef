/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:      TuneDiagram.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TUNEDIAGRAM_H
#define TUNEDIAGRAM_H

#include <TuneDiagramBase.h>
#include <qwt_data.h>
#include <cstdlib>

class QwtPlotCurve;

class TuneDiagram: public TuneDiagramBase {

 Q_OBJECT
 
 class ResonanceLine;

 public:

  TuneDiagram( QWidget* parent= 0, char const* = 0, WFlags = 0 );
 ~TuneDiagram();

  void replot();

  void   setRange( int min, int max, bool showdiffs=false);
  void   setTunes( double nu1, double nu2);
  void setChromas( double xi1, double xi2);

 private slots:

  void setMaxOrder( int ); 
  void setMinOrder( int ); 
  void showSums();
  void showDifferences();

 private:

  void plotResonances();
  void clear();

  TuneDiagram( TuneDiagram const& );

  double nu1_; 
  double nu2_; 

};

class TuneDiagram::ResonanceLine : public QwtData {
 
  struct Point {
    Point() : xp(0.0), yp(0.0) {}
    Point( double x, double y) : xp(x), yp(y) {}
    Point( Point const& o ): xp(o.xp), yp(o.yp) {}
    double xp;
    double yp;

    double x() const { return xp; }
    double y() const { return yp; }
    bool operator==( Point const& rhs) const { return ( (abs(this->xp-rhs.xp) < 1.0e-6)  && ( abs(this->yp-rhs.yp)<1.0e-6 )) ; }
    bool operator< ( Point const& rhs) const {   if (  (abs(this->xp-rhs.xp) < 1.0e-6) ) { 
                                                       return (this->yp < rhs.yp) ; 
                                                 }
                                                 else { 
                                                     return (this->xp < rhs.xp) ; 
					         }
                                              }
  };                          
 
 public: 

  ResonanceLine( int nx, int ny, int n, double xoffset=0.0, double yoffset=0.0 ); 
  ResonanceLine(  ResonanceLine const& o); 
 ~ResonanceLine();

  ResonanceLine* copy () const;

  size_t         size ()         const;
  double  	 x (size_t i)    const;
  double 	 y (size_t i)    const;

  QwtDoubleRect  boundingRect () const;
 
  bool isVisible() const; 
 
 private:
  
  double    nx_;
  double    ny_;
  double    n_;

  Point point1_;
  Point point2_;

  double xoffset_;
  double yoffset_;
};

#endif // TUNEDIAGRAM_H
