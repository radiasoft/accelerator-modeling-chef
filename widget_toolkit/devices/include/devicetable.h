/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  Widget Toolkit:                                              ******
******                                                               ******
******  A collection of Qt widgets layered on the Beamline/mxyzptlk  ****** 
******  class libraries.                                             ****** 
******                                                               ****** 
******  File:  devicetable.h                                         ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:    Jean-Francois Ostiguy                             ******
******             Fermilab                                          ****** 
******             ostiguy@fnal.gov                                  ****** 
******                                                               ******  
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
**************************************************************************/

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
