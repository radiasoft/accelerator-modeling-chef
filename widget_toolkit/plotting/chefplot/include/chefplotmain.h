#ifndef CHEFPLOTMAIN_H
#define CHEFPLOTMAIN_H

#include "chefplotmainbase.h" 
#include "chefplotdata.h" 
#include <qcanvas.h>
#include <qwmatrix.h>

class CHEFPlot;
class beamline;
class bmlnElmnt;
class LegoPlot;


class CHEFPlotMain: public CHEFPlotMainBase {

  Q_OBJECT
    
  public:
  
  CHEFPlotMain(QWidget * parent = 0, const char* name = 0, Qt::WFlags f = 0);
  virtual ~CHEFPlotMain();

  void displayLattice(beamline* bml);
  void addData(CHEFPlotData& cpdata);
  void showDataPoints(bool set);
  void enableLegend(bool set);
  void enableInterpolation(bool set);
  void enableCoordinatesDisplay(bool set);
    
    
  public slots:

  virtual void filePrint();
  virtual void enableGrid(bool set);
  virtual void enableThumbWheels(bool set);
  virtual void enableLegoPlot(bool set);
  virtual void zoomUseRightAxis();
  virtual void zoomUseLeftAxis();
  virtual void exit();
  virtual void clear();
  
  private:

  
  CHEFPlot*     _chefplot;


};

#endif
