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

  void              setList( std::list<std::string> const& bmllist, const char* use_name=0);
  std::list<std::string> getSelected();
  void                   setBeamParameters(bmlfactory const& bf);
  double                 getBRHO() {return brho_;}

 private:

  void computeBeamParameters( double value, int datatype);
  void refreshBeamParameters();

 private slots:

  void particleCheck(int id); 
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

  virtual void cancel();

 private:

  double momentum_;
  double et_;
  double ek_;
  double gamma_;
  double brho_;
  double mass_;

};



#endif
