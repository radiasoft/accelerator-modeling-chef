/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      DbConnectDialog.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef DBCONNECTDIALOG_H
#define DBCONNECTDIALOG_H

#include <dbconnectdialogbase.h>
#include <qstring.h>

class QListBoxItem;

struct DbConnectStruct {
  QString driver; 
  QString dbname; 
  QString username;
  QString passwd; 
  QString hostname;
};


class DbConnectDialog: public DbConnectDialogBase {

Q_OBJECT

 public:

  DbConnectDialog(QWidget* parent, const char* name, WFlags f=0);
 ~DbConnectDialog();

 QString databaseName(); 
 QString userName();     
 QString password();     
 QString hostName();     



 private slots:

   void _handleDriverSelectionChanged();
   void _handleOkClicked();

 signals:
  
   void buttonOkClicked(const DbConnectStruct& st );

};

#endif
