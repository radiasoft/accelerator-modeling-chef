#ifndef PLOTINTERFACE_H
#define PLOTINTERFACE_H

#include "displayinfo.h" 


class PlotInterface {
  
  public:
  
   virtual void addData(const DisplayInfo& dinfo);
      //double getCurrentXmin();
      //double getCurrentXmax();
      //double getCurrentYmin();
      //double getCurrentYmax();

  public slots:

       //void scaleChanged();
       //void leftWheelValueChanged(   double value);
       //void rightWheelValueChanged(  double value);
       // void bottomWheelValueChanged( double value);
       //void enableThumbWheel(bool set, int axiscode);
       //void enableThumbWheels(bool set);
       //void setZoomer(bool set, int axis);

  private:

     //double                     _xmin; 
     //double                     _xmax;
     //double                     _ylmin;
     //double                     _ylmax;
     //double                     _yrmin;
     //double                     _yrmax;

     //double                     _xrange;
     //double                     _ylrange;
     //double                     _yrrange;

};


#endif
