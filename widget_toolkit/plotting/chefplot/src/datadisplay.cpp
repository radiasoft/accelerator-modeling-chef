/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      datadisplay.cpp
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

#include <DataDisplay.h>
#include <qtable.h>
#include <qfiledialog.h>
#include <qdatetime.h>
#include <string>
#include <list>

#include <fstream>
#include <iostream>


DataDisplay::DataDisplay(QWidget* parent, const char* name, WFlags f, bool showbml):
  DataDisplayBase(parent, name, f), _showbml(showbml) 
{


}

// ..................................................................................  

DataDisplay::~DataDisplay() 
{


}

// ..................................................................................  

void 
DataDisplay::filePrint()
{

  //  _output( std::cout );

}

// ..................................................................................  

void 
DataDisplay::_output( std::ostream& os)
{

 int rows = DataTable->numRows(); 
 int cols = DataTable->numCols(); 


 os << "! LEGEND:"<< std::endl;
 os << "! "       << std::endl;
 
 int k = 0;
 for (int i=0; i<cols; i++)
 {
   if  ( ! DataTable->isColumnHidden(i) ) {
     os << "! Column " << ++k << " : " 
        << DataTable->horizontalHeader( )->label(i).replace('\n', ',') << std::endl;
   }
 }

 os << "!" << std::endl;

 k = 0 ;
 QString tmp_heading;
 
 for ( int i=0; i <cols; i++)
 {
   if  ( ! DataTable->isColumnHidden(i) ) {
     tmp_heading = "Column " + QString::number(++k); 

     if ( k == 1){
       os.width(1); os <<"!"; os.width(17);
       os << tmp_heading;            
     }
     else {
       os.width(18);
       os.setf(std::ios_base::right | std::ios_base::scientific);
       os << tmp_heading;       
     }
   }
 }

 os << std::endl;
 os << "!" << std::endl;

 

 for (int i=0; i< rows; i++)
 {
   for (int j=0; j< cols; j++)
   {
     if  ( !DataTable->isColumnHidden(j) ) {

     os.width(18);
     os.setf(std::ios_base::right | std::ios_base::scientific);
     os << DataTable->text(i,j);
     } 
   }
   os << std::endl; 

 }
}

// ..................................................................................  

void 
DataDisplay::fileSave()
{

  QString s = caption()+QString("-")+(QDateTime::currentDateTime()).toString();
  s.replace ( " ", "-", false );
  s.replace ( ":", "-", true );
 

  std::ofstream fs ( s.ascii() );
 
  _output( fs );


}

// ..................................................................................  

void DataDisplay::fileSaveAs()
{

  QString f = caption()+QString("-")+(QDateTime::currentDateTime()).toString();

  f.replace ( " ", "-", false );
  f.replace ( ":", "-", true );
 
  QString s = QFileDialog::getSaveFileName(
                    QString("./") + f,
                    "",
                    this,
                    "Save File Dialog",
                    "Save Data File As" );

  std::ofstream fs ( s.ascii() );
  _output( fs );
 
}

void 
DataDisplay::viewMerge( bool set)
{

  int cols      = DataTable->numCols(); 
  int rows      = DataTable->numRows(); 
  int rowoffset = 0;

  if (_showbml) rowoffset = 4;  

  if (set) {
   
    for (int i = std::max(rowoffset, 1) ; i<cols; i++) {
    if ( (i - (i/2)*2) == 0 )
      { 
        DataTable->hideColumn ( i );
      }
    }
  }
  else
  {
    for (int i=1; i<cols; i++) 
    {
      if ( DataTable->isColumnHidden(i) )  DataTable->showColumn ( i ); 
    }
  };

  return; 

}

// ..................................................................................  

void 
DataDisplay::fileExit()
{

 close();

}


