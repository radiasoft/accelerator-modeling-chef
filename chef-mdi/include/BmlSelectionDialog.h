#ifndef BMLSELECTIONDIALOG_H
#define BMLSELECTIONDIALOG_H

#include "bmlselectiondialogbase.h"

#include <list>
#include <string>

class bmlfactory;

class BmlSelectionDialog: public BmlSelectionDialogBase {

 enum beamparameter { MOMENTUM, ET, EK, GAMMA, BRHO };

 Q_OBJECT

 public:

  BmlSelectionDialog(QWidget* parent=0, const char* name=0, WFlags f=0);
  void setList( std::list<std::string>& bmllist, const char* use_name=0);
  void getSelected();
  void setBeamParameters(const bmlfactory& bf); 
  double getBRHO() {return _brho;}

 private:

  void computeBeamParameters( double value, int datatype);
  void refreshBeamParameters();

 private slots:

  void momentumCheck(bool set); 
  void ETCheck(bool set); 
  void EKCheck(bool set); 
  void GammaCheck(bool set); 
  void BrhoCheck(bool set); 

  void momentumChanged(); 
  void ETChanged(); 
  void EKChanged(); 
  void GammaChanged(); 
  void BrhoChanged(); 
  void updateBeamParameters();

  virtual void _cancel();

 private:

  std::list<std::string>* _bmllist; 

  double _momentum;
  double _et;
  double _ek;
  double _gamma;
  double _brho;

};



#endif
