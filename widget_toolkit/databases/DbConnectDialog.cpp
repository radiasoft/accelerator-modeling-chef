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

#include <iostream>

DbConnectDialog::DbConnectDialog(QWidget* parent, const char* name, WFlags f):
  DbConnectDialogBase(parent,name,f) 
{

  // ... FIXME !
  // ... the defaults should be read from a separate config file ... 

  listBoxDBDriver->clear();
  listBoxDBDriver->insertItem("QODBC3");
  listBoxDBDriver->insertItem("QPSQL7");
  listBoxDBDriver->setCurrentItem(0);
 
  lineEditDBName->setText("blastman");
  lineEditHostName->setText("mrkrabs.fnal.gov");
  lineEditUserName->setText("nobody");
  lineEditPassword->setText("");

  connect ( buttonOk, SIGNAL( clicked() ), this, SLOT( handleOkClicked() )  );

}


// ......................................................................................................................


DbConnectDialog::~DbConnectDialog() 
{


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


void DbConnectDialog::handleOkClicked() {



 DbConnectStruct st    = { listBoxDBDriver->selectedItem()->text(),
                           lineEditDBName->text(), 
                           lineEditUserName->text(),  
                           lineEditPassword->text(), 
                           lineEditHostName->text() }; 

 emit buttonOkClicked( st );

}

