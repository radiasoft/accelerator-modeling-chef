#ifndef CHEFPLOT_H
#define CHEFPLOT_H

#include "chefplotdata.h" 
#include <qwidget.h>
#include <qwmatrix.h>

class Plot;
class beamline;
class bmlnElmnt;
class LegoPlot;


class CHEFPlot: public QWidget {

  friend class CHEFPlotMain;
  
  Q_OBJECT
    
  public:
  
  CHEFPlot(QWidget * parent = 0, const char* name = 0, Qt::WFlags f = 0);
  virtual ~CHEFPlot();

  void displayLattice(beamline* bml);
  void addData(CHEFPlotData& cpdata);
  
  protected:

  void resizeEvent (QResizeEvent * e);
  

  public slots:

  void updateLatticeDisplay();
  void enableGrid(bool set);
  void enableThumbWheels(bool set);
  void enableLegoPlot(bool set);
  void zoomUseLeftAxis();
  void zoomUseRightAxis();
  
  private slots:
  
  void resizeLego();

  private:

  
  Plot*          _plot;
  LegoPlot*      _lego;
  QWMatrix       _canvas_transformation;
  int            _lego_height;
  

};

#endif
