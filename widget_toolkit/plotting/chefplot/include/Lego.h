////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       Lego.h                                                                         //
//                                                                                            //
// AUTHOR(S):  Jean-Francois Ostiguy                                                          // 
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
//             Accelerator Division / Accelerator Integration Dept                            //
//             Fermi National Laboratory, Batavia, IL                                         //
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
// DATE:       September 2004                                                                 //
//                                                                                            //
// COPYRIGHT: Universities Research Association                                               //
//                                                                                            //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef LEGO_H
#define LEGO_H

#include <qwidget.h>
#include <beamline/beamline.h>

class  QPainter;
class  QPixmap;

class LegoPlot: public QWidget {

  Q_OBJECT

  public:

  
  LegoPlot(QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  ~LegoPlot();
  
  void        setBeamline(  ConstBmlPtr bml);
  ConstBmlPtr getBeamline();

  void setBeamlineDisplayLimits(double x, double w, int loffset, int roffset);
  void enableBeamlineDisplay(bool enabled);
  
  protected:
  
  void paintEvent ( QPaintEvent  * );
  void resizeEvent( QResizeEvent * );
  
  void drawDrift(double l, double  s);
  void drawQuad(double l, double  s, bool focusing );
  void drawBend(double l, double  s);
  void drawSext(double l, double  s, bool focusing );

  private:

  ConstBmlPtr  bml_;     // the displayed beamline
  QPainter*    painter_;
  QPixmap*     pixmap_;

  double     x0_;           // the beamline display origin in physical coordinates
  double     w_;            // the beamline display extent in physical coordinates
  double     scale_;        
  int        loffset_;  
  int        roffset_;  
   
  
};

#endif
