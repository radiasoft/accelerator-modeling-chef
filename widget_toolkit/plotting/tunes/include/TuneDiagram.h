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

#include <qwt_data.h>
#include <TuneDiagramBase.h>

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

  void maxOrderValueChanged( int ); 
  void minOrderValueChanged( int ); 
  void checkBoxSumsClicked();
  void checkBoxDifferencesClicked();

 private:

  TuneDiagram( TuneDiagram const& );
  QwtPlotCurve* tunescrv_;

};

class TuneDiagram::ResonanceLine : public QwtData {
 
 public: 

  ResonanceLine( int nx, int ny, int n ); 
  ResonanceLine(  ResonanceLine const& o); 
 ~ResonanceLine();

  ResonanceLine* copy () const;

  size_t         size ()         const;
  double  	 x (size_t i)    const;
  double 	 y (size_t i)    const;

  // QwtDoubleRect  boundingRect ();

 private:
  
  double    nx_;
  double    ny_;
  double    n_;
};

#endif // TUNEDIAGRAM_H
