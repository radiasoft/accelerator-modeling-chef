/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application Framework 
******             using the BEAMLINE class library 
******                                                                
******  File:      DbConnectDialog.cpp
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

#include <DbConnectDialog.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qsqldatabase.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qstringlist.h>

#include <iostream>

DbConnectDialog::DbConnectDialog(QWidget* parent, const char* name, WFlags f):
  DbConnectDialogBase(parent,name,f) 
{
  listBoxDBDriver->clear();

  QStringList drvlist = QSqlDatabase::drivers (); 

  if (drvlist.size() < 1 ) {
     QMessageBox::warning( this ,"Error", "No Database Driver installed.");
     return;
  }

  

  QStringList::Iterator it = drvlist.begin();

  while( it != drvlist.end() ) {

        listBoxDBDriver->insertItem( *it );
        ++it;
  }

  listBoxDBDriver->setCurrentItem(0);
 
  // ... FIXME !
  // ... the defaults should be read from a separate config file ... 

  // ** default values ***  

  lineEditDBName->setText("blastman");
  lineEditHostName->setText("mrkrabs.fnal.gov");
  lineEditUserName->setText("nobody");
  lineEditPassword->setText("");

  _handleDriverSelectionChanged();

  connect ( listBoxDBDriver, SIGNAL( selectionChanged() ), this, SLOT( _handleDriverSelectionChanged() )  );
  connect ( buttonOk,        SIGNAL( clicked() ),          this, SLOT( _handleOkClicked() )               );

}


// ......................................................................................................................


DbConnectDialog::~DbConnectDialog() 
{


}

// ...........................................................................................................

void DbConnectDialog::_handleDriverSelectionChanged() {

  

  if ( listBoxDBDriver->currentText().contains('ODBC') ) { 
 
    lineEditHostName->setEnabled(false);
    lineEditUserName->setEnabled(false);
    lineEditPassword->setEnabled(false);   

  } else {

    lineEditHostName->setEnabled(true);
    lineEditUserName->setEnabled(true);
    lineEditPassword->setEnabled(true);   

 }
}

// ...........................................................................................................

QString DbConnectDialog::databaseName() 
{ 
  return lineEditDBName->text();   
}

// ...........................................................................................................


QString DbConnectDialog::userName()     
{ 
  return lineEditUserName->text(); 
}


// ...........................................................................................................


QString DbConnectDialog::password()
{ 
  return lineEditPassword->text(); 
}

// ...........................................................................................................


QString DbConnectDialog::hostName()
{ 
  return lineEditHostName->text(); 

}

// ...........................................................................................................


void DbConnectDialog::_handleOkClicked() {



 DbConnectStruct st    = { listBoxDBDriver->selectedItem()->text(),
                           lineEditDBName->text(), 
                           lineEditUserName->text(),  
                           lineEditPassword->text(), 
                           lineEditHostName->text() }; 

 emit buttonOkClicked( st );

}

