#ifndef LEGO_H
#define LEGO_H

#include <qwidget.h>

class  beamline;
class  QPainter;
class  QPixmap;

class LegoPlot: public QWidget {

  Q_OBJECT

  public:

  
  LegoPlot(QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  ~LegoPlot();
  
  void setBeamline(const beamline* bml);
  const beamline* getBeamline();
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

  beamline*  _beamline;     // the displayed beamline
  QPainter*  _painter;
  QPixmap*   _pixmap;

  double     _x0;           // the beamline display origin in physical coordinates
  double     _w;            // the beamline display extent in physical coordinates
  double     _scale;        
  int        _loffset;  
  int        _roffset;  
   
  
};

#endif
