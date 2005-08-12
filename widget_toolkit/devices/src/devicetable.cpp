***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  Widget Toolkit:                                              ******
******                                                               ******
******  A collection of Qt widgets layered on the Beamline/mxyzptlk  ****** 
******  class libraries.                                             ****** 
******                                                               ****** 
******  File:  devicetable.cpp                                       ****** 
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

#include <iostream>
#include <sstream>
#include <string>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qheader.h>
#include <qtable.h>
#include <qtimer.h>
#include <qutexr/xrclient.h>
#include <iostream>

#include <devicetable.h>


#define NAME       "XML-RPC  get device settings "
#define VERSION    "0.1"
#define SERVER_URL "http://www-bd.fnal.gov/xmlrpc/Accelerator"


DeviceTable::DeviceTable(QWidget* parent, const char* name): DeviceTableBase(parent,name), _maxrow(0), _maxcol(0) 
{ 

  connect( device_table, SIGNAL( valueChanged(int,int) ),   this, SLOT( valueChanged(int,int) ) );
  connect( this,         SIGNAL( deviceDataReady() ),       this, SLOT( handleDeviceData() ) );

  device_table->horizontalHeader()->setLabel( 0, tr( "Device Name" ) );
  device_table->setNumRows(100);
  device_table->setNumCols(10);

  _server_url     = new QUrl ( QString(SERVER_URL) );
  _xrclient       = new XRClient  ( *_server_url, 0, 0);
  _xrclient->acceptCompressed(true);
  
  connect( _xrclient,    SIGNAL( methodResponse(int,const QVariant&) ), this, SLOT( handleResponse(int,const QVariant&) ) );
  connect( _xrclient,    SIGNAL( fault(int, int, const QString &) ),    this, SLOT( handleFault(int, int,const QString &) ) );

  _refresh_timer = new QTimer(this);
 
  connect( _refresh_timer, SIGNAL( timeout() ),  this,  SLOT( refreshAll() ) );  

  
}



DeviceTable::~DeviceTable()
{

  if ( _server_url )  delete _server_url;
  if ( _xrclient   )  delete _xrclient;

}



int DeviceTable::get_device_setting( XRClient& xrc, std::string dname) {

  QValueList< QVariant >  param_list;
  QString  device_name( dname.c_str() );
  param_list.append( device_name  );
       
  return xrc.call(  QString("getReading"),  param_list );

}



void  DeviceTable::handleResponse( int response_num, const QVariant & response) {

   _xmlrpc_response = &response;
    emit deviceDataReady();
    
  
}


void  DeviceTable::handleFault( int response_num, int fault_num, const QString & fault_string) {

  QMessageBox::information( this,"Warning", fault_string, QMessageBox::Ok  );  
}


void  DeviceTable::valueChanged(int row, int col)  
{

_changed_row = row;
_changed_col = col;

QTableItem* tmp_item  =  device_table->item(_changed_row, _changed_col);
int response =  get_device_setting( *_xrclient,  std::string( tmp_item->text().ascii() ) );

}



void DeviceTable::handleDeviceData(int row, int col) {

  QTableItem* new_item = 0;
  QTableItem* old_item = 0;
  
  const QVariant scaled_setting =  (_xmlrpc_response->toMap())["scaled"];

  QVariant::Type xmltype  =  scaled_setting.type();

  int idev = 0;
           
  switch ( xmltype ) {

      case QVariant::Double:  // this is a scalar device

         old_item = device_table->item(row,col+1);

         if ( old_item ) { 
	        old_item->setText(  _xmlrpc_response->toMap()["units"].toString() );
                device_table->updateCell( row, col+1);
	 } else {

               new_item =  new QTableItem(device_table, QTableItem::Never, _xmlrpc_response->toMap()["units"].toString() );
               device_table->setItem(row, col+1,    new_item);
               _maxrow = std::max(_maxrow, row);
               _maxcol = std::max(_maxcol, col+1);
	 };

         old_item = device_table->item(row,col+2);
          
         if ( old_item ) { 
	        old_item->setText(  QString::number(scaled_setting.toDouble())  );
                device_table->updateCell( row, col+2);
	 } else {

               new_item    =  new QTableItem(device_table, QTableItem::Never,  QString::number(scaled_setting.toDouble()));
               device_table->setItem(row, col+2,    new_item);
               _maxrow = std::max(_maxrow, row);
               _maxcol = std::max(_maxcol, col+2);     
	 };
         break;
                  

     case QVariant::List:  // this is an array device

         old_item = device_table->item(row,col+1);

         if ( old_item ) {

	   old_item->setText( _xmlrpc_response->toMap()["units"].toString() );
           device_table->updateCell( row,col+1 );

	 } else {

           new_item =  new QTableItem(device_table, QTableItem::Never,
                                              _xmlrpc_response->toMap()["units"].toString() );
   
           device_table->setItem(row, col+1,    new_item);
           _maxrow = std::max(_maxrow, row);
           _maxcol = std::max(_maxcol, col+1);	 
         };


         for ( QValueList<QVariant>::const_iterator it = (scaled_setting.toList()).begin();
             it != (scaled_setting.toList()).end(); ++it ) 
          {

                 old_item = device_table->item(row,col+2+idev);

                 if (old_item) {
                    
                   old_item->setText( QString::number( (*it).toDouble() ) );
                   device_table->updateCell( row,col+2+idev );
                 } else {

                 new_item =  new QTableItem(device_table, QTableItem::Never,
                                              QString::number( (*it).toDouble() ));
                 
    	         device_table->setItem(row, col+2+idev, new_item);
                 _maxrow = std::max(_maxrow, row);
                 _maxcol = std::max(_maxcol, col+2+idev);	 
                };
 
                idev++;

           };
        
         break;

      default:

	std::cout << "Unexpected type = " << xmltype << std::endl;
           
	break;       
   };
             
     _waiting_for_reply = false;
}


void DeviceTable::handleDeviceData() {

  // this slot is called when new device data has been returned;

  handleDeviceData( _changed_row, _changed_col);

}


void DeviceTable::readDeviceList( std::ifstream& is) {

  char* inbuffer = new char[1024]; 
  std::stringstream ss;

  int row = 0; 

  while ( is.getline(inbuffer, 1024), !is.eof() ) 
  {
   
    device_table->setText(row, 0, QString ( inbuffer ) );
    valueChanged( row,0 );
    _waiting_for_reply = true;
    while ( _waiting_for_reply ) qApp->processEvents(1); // process events for 1 ms 
    row++;

  };

   _waiting_for_reply = false;
   device_table->repaint();

  

  //  delete inbuffer;

}

void  DeviceTable::clearSelection() 
{

  int ns = device_table->numSelections();

  for (int i=0; i < ns; i++) 
  {
     QTableSelection sel = device_table->selection(i);
     
     int row_t = sel.topRow();
     int row_b = sel.bottomRow();
     int col_l = sel.leftCol();
     int col_r = sel.rightCol();

     for ( int row = row_t; row <= row_b; row++ ) 
      {
        for ( int col = col_l; col <= col_r; col++ )
	  {

            QTableItem* item =  device_table->item( row,col); 
            device_table->takeItem( item );
            delete item;
	  }
      }
     device_table->clearSelection(); 
  }
}


void  DeviceTable::refreshAll() 
{


  QTableSelection sel = device_table->selection(1);

  int row_t = 0;
  int row_b = _maxrow;
  
  for ( int row = row_t; row <= row_b; row++ ) 
  {

    if ( device_table->item(row,0) ) {
       valueChanged(row,0);
      _waiting_for_reply = true;
      while ( _waiting_for_reply ) qApp->processEvents(1); 
    }
  
  };

   _waiting_for_reply = false;
   device_table->repaint(); 

} 


void  DeviceTable::setAutoRefresh( bool set) 
{

  if (set) 
  {
     _refresh_timer->start(30000, false);  

  }
  else
  {
     _refresh_timer->stop();  

  }
}
 


#if 0
  switch ( response.type() ) {
    
    case QVariant::String:  std::cout << "QVariant::String" << std::endl;
      break;

    case  QVariant::Int:    std::cout << "QVariant::Int" << std::endl;
      break;

    case QVariant::Double:  std::cout << "QVariant::Double" << std::endl;
      break;
      
    case QVariant::List:    std::cout << "QVariant::List" << std::endl;    // XML-RPC Array
      break;
      
    case QVariant::Map:     std::cout << "QVariant::Map" << std::endl;     // XML-RPC Struct
      break;
      
    case QVariant::DateTime: std::cout << "QVariant::DateTime" << std::endl; 
      break;

  default:                  std::cout << "Unknown Type" << std::endl;
  }

#endif

