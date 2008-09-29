/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DataBrowser.h                                     ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:                                                      ******
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              ostiguy@fnal.gov                                 ****** 
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
***************************************************************************
***************************************************************************
**************************************************************************/

#ifndef DATABROWSER_H
#define DATABROWSER_H

#include <DataBrowserBase2.h>
#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>
#include <ostream>
#include <string>

class DataBrowser;
class QMenuBar;
class QPopupMenu;

std::ostream& operator<<(  std::ostream& os, DataBrowser const& o); 

class DataBrowser : public DataBrowserBase {

 Q_OBJECT

 class FloatItem;

 public:

  DataBrowser( char const* dbname, QWidget* parent=0, const char* name=0 , WFlags fl = 0);
  virtual  ~DataBrowser();

  void print ( std::ostream& os) const;

 protected:

  void closeEvent( QCloseEvent *event );
  void init( sqlite::connection& db, std::string const& sql);

  mutable sqlite::connection   db_;  
  std::string                 sql_;  

 private:   

  void get_rows( sqlite::result_type res);
  void clear();

  QMenuBar*   menubar_;
  QPopupMenu* col_display_menu_;

 private slots:

  void handleColumnSelection( int id); 
  void handleSetPrecision( int );
  void saveAs();

};

// ------------------------------------------------------------------------------
// Derived Classes 
// ------------------------------------------------------------------------------

class CS4DDataBrowser: public DataBrowser {

 public:

  CS4DDataBrowser( char const* dbname, QWidget* parent, char const* name, WFlags fl = 0  );
};


class CS2DDataBrowser: public DataBrowser {

 public:

  CS2DDataBrowser( char const* dbname, QWidget* parent, char const* name, WFlags fl = 0);
};


class DispersionDataBrowser: public DataBrowser {

 public:

  DispersionDataBrowser( char const* dbname, QWidget* parent, char const* name,  WFlags fl = 0 );
};


class CovarianceDataBrowser: public DataBrowser {

 public:

  CovarianceDataBrowser( char const* dbname, QWidget* parent, char const* name,   WFlags fl = 0);
};


#endif // DATABROWSER_H

