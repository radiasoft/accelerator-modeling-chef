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

#include <qtable.h>
#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>


class DataBrowser : public QTable {

 Q_OBJECT

 public:

  DataBrowser( sqlite::connection& db, QWidget* parent=0, const char* name=0 );
 ~DataBrowser();

  void closeEvent( QCloseEvent *event );

 protected:

  void init( sqlite::connection& db, std::string const& sql);

  sqlite::connection&  db_;  

 private:   

  sqlite::result_type               res_; 
  bool                              table_complete_;  
  
  void get_rows();

};

// ------------------------------------------------------------------------------
// Derived Classes 
// ------------------------------------------------------------------------------

class CS4DDataBrowser: public DataBrowser {

 public:

  CS4DDataBrowser( sqlite::connection& db, QWidget* parent, char const* name );
};


class CS2DDataBrowser: public DataBrowser {

 public:

  CS2DDataBrowser( sqlite::connection& db, QWidget* parent, char const* name );
};


class DispersionDataBrowser: public DataBrowser {

 public:

  DispersionDataBrowser( sqlite::connection& db, QWidget* parent, char const* name );
};


class CovarianceDataBrowser: public DataBrowser {

 public:

  CovarianceDataBrowser( sqlite::connection& db, QWidget* parent, char const* name );
};


#endif // DATABROWSER_H

