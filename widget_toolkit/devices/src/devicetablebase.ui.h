/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <sstream>
#include <string>
#include "devicetablebase.h"





void  DeviceTableBase::handleResponse( int response_num, const QVariant & response) {

  
  
}


void  DeviceTableBase::handleFault( int response_num, int fault_num, const QString & fault_string) {

}


void  DeviceTableBase::valueChanged(int row, int col)  
{


}


void DeviceTableBase::handleDeviceData() {


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

