#ifndef DEVICETABLE_H
#define DEVICETABLE_H

#include <devicetablebase.h>
#include <fstream>
#include <string>

class QTimer;

class DeviceTable: public DeviceTableBase {

 Q_OBJECT 

 public:

  DeviceTable(QWidget* parent, const char* name);
  virtual ~DeviceTable();

  virtual int get_device_setting( XRClient& xrc, std::string dname);
  void readDeviceList( std::ifstream & is);    
  void clearSelection();
  void setAutoRefresh(bool set);
  public slots:

  virtual void valueChanged(int row, int col);  

  public slots:

  void refreshAll();
 
  private slots:

  virtual void handleResponse( int response_num, const QVariant & response);
  virtual void handleFault( int response_num, int fault_num, const QString & fault_string);
  virtual void handleDeviceData();
  virtual void handleDeviceData(int row, int col);

  

 private:

  QTimer*  _refresh_timer;
  bool     _waiting_for_reply; 
  int      _maxrow; 
  int      _maxcol;



};

#endif
