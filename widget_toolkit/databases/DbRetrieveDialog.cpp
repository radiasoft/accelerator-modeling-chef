/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application Framework
******             using the BEAMLINE class library.
******                                                                
******  File:      DbRetrieveDialog.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab, Batavia, IL                                
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

#include <DbRetrieveDialog.h>
#include <DbRetrieveQueries.h>

#include <qmessagebox.h>
#include <qsqlcursor.h>
#include <qdatatable.h>
#include <qdatetime.h>
#include <qsqlquery.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <qvariant.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>
#include <qvalidator.h>
#include <qtextview.h>
#include <iostream>

#include <vector>


DbRetrieveDialog::DbRetrieveDialog(QWidget* parent, const char* name, WFlags f):
  DbRetrieveDialogBase(parent,name,f) 
{

 listBoxMachines->setHScrollBarMode(QScrollView::AlwaysOff);
 listBoxSeries->setHScrollBarMode(QScrollView::AlwaysOff);
 listBoxSeries->setEnabled(false);
 listBoxSeries->setSelectionMode(QListBox::Extended); 


 dateEditMachineState->setDate( QDate::currentDate() );
 dateEditMachineState->setRange(QDate(1970,1,1), QDate::currentDate() );

 lineEditOriginValue->setValidator( new QDoubleValidator( 0.0, 1.0e5, 6, lineEditOriginValue) ); 

 // ... signal-slots connections 

 connect( buttonOk,            SIGNAL( clicked() ),               this, SLOT( _handleButtonOkClicked()) );  
 connect( checkBoxXYZ,         SIGNAL( toggled(bool) ),           this, SLOT( _handleSurveyTypeCheckBoxToggled(bool) ) );  
 connect( checkBoxXYH,         SIGNAL( toggled(bool) ),           this, SLOT( _handleSurveyTypeCheckBoxToggled(bool) ) );  
 connect( listBoxMachines,     SIGNAL( clicked(QListBoxItem*)  ), this, SLOT( _handleListBoxMachinesClicked(QListBoxItem*) ) );  
 connect( checkBoxDipoleBends, SIGNAL( toggled(bool)           ), this, SLOT( _handleDipoleBendsChecked(bool) ) );  
 connect( checkBoxMainQuads,   SIGNAL( toggled(bool)           ), this, SLOT( _handleMainQuadsChecked(bool) ) );  
 connect( tabWidget,           SIGNAL( currentChanged(QWidget*)), this, SLOT( _handleCurrentTabChanged(QWidget*) ) );  
 connect( checkBoxAllMoments,  SIGNAL( toggled(bool)),            this, SLOT( _handleAllMomentsChecked(bool)) );  
 connect( checkBoxSeries,      SIGNAL( toggled(bool)),            this, SLOT( _handleSeriesChecked(bool)) );  
 connect( listBoxSeries,       SIGNAL( selectionChanged()),       this, SLOT( _handleSeriesSelectionChanged()) );  


}


// ......................................................................................................................


DbRetrieveDialog::~DbRetrieveDialog() 
{


 QMessageBox::warning(0, "WARNING", "destructor called");

}


// ......................................................................................................................

void DbRetrieveDialog::showEvent (QShowEvent * )
{

 _refreshMachineSelection( ); 
 _refreshSeriesSelection( listBoxMachines->selectedItem()->text() ); 
 _refreshSurveyDataSelection(  listBoxMachines->selectedItem()->text()  );

}

// ......................................................................................................................

void DbRetrieveDialog::_refreshMachineSelection( ) 
{

 listBoxMachines->clear();

 QSqlQuery query( valid_machines_query_string );
 if ( query.isActive() ) { 
      while ( query.next() ) {
	listBoxMachines->insertItem( query.value(0).toString());       
  
      }          
 }
 listBoxMachines->setSelected(0,true);

}

// ......................................................................................................................



void DbRetrieveDialog::_refreshSurveyDataSelection(const QString& machine )
{

  QSqlQuery query;
 
  listBoxSurveyDateXYH->clear();

  query.prepare(available_survey_xyh_dates_query_string);
  query.bindValue(":machine_name", machine );
  query.exec();
  if ( query.isActive() ) { 
      while ( query.next() ) 
         listBoxSurveyDateXYH->insertItem( query.value(0).toString());       
  }
  listBoxSurveyDateXYH->setSelected(0,true);

  listBoxSurveyDateXYZ->clear();

  query.prepare(available_survey_xyz_dates_query_string);
  query.bindValue(":machine_name", machine );
  query.exec();
  if ( query.isActive() ) { 
      while ( query.next() ) 
         listBoxSurveyDateXYZ->insertItem( query.value(0).toString());       
  }

  listBoxSurveyDateXYZ->setSelected(0,true);



  //..................................................................................

} 


void DbRetrieveDialog::_handleListBoxMachinesClicked(QListBoxItem* item) {

  _refreshSurveyDataSelection(  item->text()  );
  _refreshSeriesSelection( item->text() );


}

// .....................................................................................................................


void DbRetrieveDialog::_refreshSeriesSelection(QString s) {

  listBoxSeries->clear();
  QSqlQuery query;

  query.prepare(  magnet_series_query_string );

  query.bindValue(0 , s );
  query.exec();

  if ( query.isActive() ) 
  { 
     while ( query.next() ) {

           listBoxSeries->insertItem( query.value(0).toString() );
     }
  }

  listBoxSeries->setSelected(0,true);

}


// .....................................................................................................................


void DbRetrieveDialog::_handleDipoleBendsChecked( bool set ) {

  if (!set) return;
 
  const int numpoles = 2; 

 
  QSqlQuery query;

  query.prepare( available_magnet_currents_query_string );
  query.bindValue(4 , numpoles );

  query.bindValue(0 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(1 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(2 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(3 , listBoxMachines->selectedItem()->text() );


  listBoxMeasurementCurrent->clear();

  query.exec();

  if ( query.isActive() ) 
  { 
     while ( query.next() ) {

           listBoxMeasurementCurrent->insertItem( query.value(0).toString() );
     }
  }


  listBoxMeasurementCurrent->setSelected(0,true);

}

// ......................................................................................................................


void DbRetrieveDialog::_handleMainQuadsChecked(bool set ) {

  if (!set) return;

  const int numpoles = 4; 

  QSqlQuery query;

  query.prepare( available_magnet_currents_query_string );
  query.bindValue(4 , numpoles );

  query.bindValue(0 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(1 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(2 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(3 , listBoxMachines->selectedItem()->text() );


  listBoxMeasurementCurrent->clear();
  
  query.exec();
  
  if ( query.isActive() ) 
  { 
     while ( query.next() ) {

           listBoxMeasurementCurrent->insertItem( query.value(0).toString() );
     }
  }

  listBoxMeasurementCurrent->setSelected(0,true);


}

// ......................................................................................................................


void DbRetrieveDialog::_handleButtonOkClicked() {



  if ( tabWidget->currentPage()->name() ==  QString("TabLocations") ) {

    _displayLocationsInfo(); 
  }

  if ( tabWidget->currentPage()->name() ==  QString("TabMultipoles") ) {
    if ( listBoxMeasurementCurrent->numRows() == 0) {
       QMessageBox::information(this, "Database Information", "No available data.");
       return;
    }
    _displayMomentsInfo();
  }

  if ( tabWidget->currentPage()->name() ==  QString("TabSurvey") ) {

    if ( checkBoxXYH->isChecked()  ) { 
       if ( listBoxSurveyDateXYH->numRows() == 0) {
	 QMessageBox::information(this, "Database Information", "No available data.");
         return;
       }
       _displaySurveyXYHInfo();
    } 

    if ( checkBoxXYZ->isChecked()  ) {
       if ( listBoxSurveyDateXYZ->numRows() == 0) {
	 QMessageBox::information(this, "Database Information", "No available data.");
         return;
       } 
       _displaySurveyXYZInfo();
    }
  }
} 


//......................................................................................................................................


void DbRetrieveDialog::_displaySurveyXYHInfo() {

Many query_params;

column_item data_table_columns[] = {   {"c_magnet_name",      "Magnet Name"}, 
                                       {"c_magnet_location",  "Magnet Location"},
				       {"c_calc_date",        "Date" },
				       {"c_x",                "X [m] FSCS"},
				       {"c_y",                "Y [m] FSCS"},
				       {"c_h",                "H [m] FSCS"},
				       {"c_roll",             "Roll [mrad]"},
				       {"c_pitch",            "Pitch [mrad]"},
				       {"c_yaw",              "Yaw [mrad]"},
				       {0, 0}
                                     };


 if ( checkBoxSeries->isChecked() ) {

     QString query_string( survey_xyh_data_by_series_query_string );
     query_string.replace("%%LIST%%", _getSelectedSeriesList() ); 
 
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxMachines->selectedItem()->text()  );                 // machine name
 
     _displayDataTable(query_string, query_params, data_table_columns ); 

     return;
 }
 
 if ( checkBoxAllMagnets->isChecked() ) {

    query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
    query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
    query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
    query_params.push_back( listBoxMachines->selectedItem()->text()  );                 // machine name

    _displayDataTable( survey_xyh_data_query_string, query_params, data_table_columns ); 
 }

}

//......................................................................................................................................



void DbRetrieveDialog::_displaySurveyXYZInfo() {

  Many query_params;

  column_item data_table_columns[] = { {"c_magnet_name",      "Magnet Name"}, 
                                       {"c_magnet_location",  "Magnet Location"},
				       {"c_calc_date",        "Date" },
				       {"c_x",                "X [m] FSCS"},
				       {"c_y",                "Y [m] FSCS"},
				       {"c_z",                "Z [m] FSCS"},
				       {"c_roll",             "Roll [mrad]"},
				       {"c_pitch",            "Pitch [mrad]"},
				       {"c_yaw",              "Yaw [mrad]"},
				       {0,                    0}
                                     } ;


 if ( checkBoxSeries->isChecked() ) {

     QString query_string( survey_xyz_data_by_series_query_string );
     query_string.replace("%%LIST%%", _getSelectedSeriesList() ); 
 
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYH->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxMachines->selectedItem()->text()  );                 // machine name
 
     _displayDataTable(query_string, query_params, data_table_columns ); 

     return;
 }

 if ( checkBoxAllMagnets->isChecked() ) {
  
     query_params.push_back( listBoxSurveyDateXYZ->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYZ->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxSurveyDateXYZ->selectedItem()->text().toInt() );     // date
     query_params.push_back( listBoxMachines->selectedItem()->text()  );     // machine name


     _displayDataTable( survey_xyz_data_query_string, query_params, data_table_columns ); 

     return;
 }

}

//.......................................................................................................................................


void DbRetrieveDialog::_displayMomentsInfo() {


column_item columns_info[ ] = 
{ 
   {"c_magnet_name",     "Name"          }, 
   {"c_magnet_location", "Location"      },
   {"c_location_date",   "Observed Date" },
   {"c_moment_number",   "pole no"       },
   {"c_b",               "B(n)"          },
   {"c_a",               "A(n)"          },
   {"c_target_amperage", "Target Current"},
   {"c_step_index",      "Step Index"    },  
   {"c_magnet_type",     "Type"          },
   {0,0}
};

Many query_params;
 

if ( checkBoxSeries->isChecked() ) {

  if (! listBoxMeasurementCurrent->selectedItem() ) { // No data available

     QMessageBox::information(this, "Database Information", "No available data.");
     return;  
  }

  QString query_string( magnet_moments_by_series_query_string );
  query_string.replace("%%LIST%%", _getSelectedSeriesList() ); 

  query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
  query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
  query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
  query_params.push_back( listBoxMachines->selectedItem()->text()  );                  // machine name
  query_params.push_back( listBoxMeasurementCurrent->selectedItem()->text().toInt() ); // current

  _displayDataTable( query_string, query_params, columns_info );
 
  return; 
}

// 

int numpoles = 0;

if  ( checkBoxDipoleBends->isChecked() ) { 
       numpoles = 2; 
} 
else if (checkBoxMainQuads->isChecked() ) {
       numpoles = 4;
} 


query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
query_params.push_back( dateEditMachineState->date().toString(Qt::ISODate) );        // date
query_params.push_back( listBoxMachines->selectedItem()->text()  );                  // machine name
query_params.push_back( listBoxMeasurementCurrent->selectedItem()->text().toInt() ); // current
query_params.push_back( numpoles );


_displayDataTable( magnet_moments_query_string, query_params, columns_info );

  
}


// ......................................................................................................................

void DbRetrieveDialog::_displayLocationsInfo() {


column_item columns_info[ ] = 
{ 
   {"Location",          "Location"            },
   {"azimuth_up",        "Upstream Azimuth"    },
   {"azimuth_down",      "Downstream Azimuth"  }, 
   {0,0}
};


Many query_params;

query_params.push_back( listBoxMachines->selectedItem()->text() ); 

_displayDataTable( machine_locations_query_string, query_params, columns_info );
  
}


// ......................................................................................................................


void DbRetrieveDialog::_handleSeriesChecked(bool set) {

  listBoxSeries->setEnabled(set);
  if (!set) return;
  listBoxMeasurementCurrent->clear();

  
}

// ......................................................................................................................


void DbRetrieveDialog::_handleSeriesSelectionChanged() {

  QSqlQuery query;

  QString query_string( available_magnet_currents_by_series_query_string );  
  query_string.replace("%%LIST%%", _getSelectedSeriesList() ); 
  query.prepare( query_string );

  query.bindValue(0 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(1 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(2 , dateEditMachineState->date().toString(Qt::ISODate) );
  query.bindValue(3 , listBoxMachines->selectedItem()->text());

  
  query.exec();

  // DBG std::cout << query.executedQuery() << std::endl;

  listBoxMeasurementCurrent->clear();

  if ( query.isActive() ) 
  { 
     while ( query.next() ) {

           listBoxMeasurementCurrent->insertItem( query.value(0).toString() );
     }
  }

  listBoxMeasurementCurrent->setSelected(0, true);

}

// ......................................................................................................................


void DbRetrieveDialog::_handleAllMomentsChecked(bool set) {

    spinBoxMinOrder->setEnabled(!set);
    spinBoxMaxOrder->setEnabled(!set);

}
//........................................................................................................................

void DbRetrieveDialog::_handleCurrentTabChanged(QWidget* w) {


  if (   w->name() == QString("TabLocations")) {

    buttonGroupMagnetSelection->setEnabled(false);

  }
  
  else if (   w->name() == QString("TabSurvey") ) {

    buttonGroupMagnetSelection->setEnabled(true);
    checkBoxAllMagnets->setEnabled(true);
    checkBoxDipoleBends->setEnabled(false);
    checkBoxMainQuads->setEnabled(false);

  }
  
  else if (   w->name() == QString("TabMultipoles") ) {
    spinBoxMinOrder->setEnabled(false);
    spinBoxMaxOrder->setEnabled(false);
  }

  if (buttonGroupMagnetSelection->isEnabled() ) { 
    if ( checkBoxDipoleBends->isChecked() ) _handleDipoleBendsChecked(true); 
    if ( checkBoxMainQuads->isChecked()   ) _handleMainQuadsChecked(true);
    if ( checkBoxAllMagnets->isChecked()  ) _handleAllMomentsChecked(true);
    if ( checkBoxSeries->isChecked()      ) _handleSeriesChecked(true);
  }



}


//.........................................................................


void DbRetrieveDialog::_handleSurveyTypeCheckBoxToggled(bool set) {

// This is UGLY. Should use an exclusive group 
// Preferably, the widget design layout should be modified 

if ( checkBoxXYH->isChecked() )
  listBoxSurveyDateXYH->setEnabled( true);
else
  listBoxSurveyDateXYH->setEnabled(false);

if ( checkBoxXYZ->isChecked()  ) 
  listBoxSurveyDateXYZ->setEnabled(true);
else
  listBoxSurveyDateXYZ->setEnabled(false);
          

}

//.........................................................................


void DbRetrieveDialog::_displayDataTable( QString query_string, Many query_params, column_item col_info[] ) {

QString   tmp_table_name   =  "tmp_table";
QSqlQuery dml_query;

//  A temporary kludge to work around the fact that TEMP TABLE Qt cursors are 
//  broken on WIN32 with pgsql ODBC drivers. Not sure whether the pb lies with Qt or
//  pgsql ODBC. 

#ifdef WIN32

int timestamp   = QTime::currentTime().msec();
tmp_table_name += QString().arg( timestamp );
 
dml_query.prepare( QString("CREATE TABLE ") + QString(tmp_table_name) + QString(" AS ") + QString( query_string ));

#else

dml_query.prepare( QString("CREATE TEMP TABLE ") + QString(tmp_table_name) + QString(" AS ") + QString( query_string ));

#endif

 for ( unsigned int i = 0; i < query_params.size(); ++i) 
 { 
   dml_query.bindValue( i,  query_params[i] );       // date
 };

 if( !dml_query.exec() ) {
 
   QMessageBox::warning(this, "Create TMP TABLE query Error", dml_query.lastError().databaseText() );
   return;
 } 


 QSqlCursor* cursor       = new QSqlCursor( tmp_table_name );
 QDataTable* data_table   = new QDataTable( cursor );
 
  
 column_item* cptr = col_info; 
 do {
       data_table->addColumn( cptr->name, cptr->label );
 } 
 while ( (++cptr)->name !=  0 );
    
 data_table->refresh();
 data_table->show();
  
 QSqlQuery ddl_query;
 QString q = "DROP TABLE "+ tmp_table_name;
 ddl_query.prepare( q );

 if( !ddl_query.exec() ) {
    QMessageBox::warning(this, "Drop Tmp Table Error", dml_query.lastError().databaseText() );
    return;
 }

}

//.................................................................................


QString DbRetrieveDialog::_getSelectedSeriesList() {

  QString selected_series;

  for ( unsigned int i=0; i < listBoxSeries->count(); ++i ) {
         
       if ( listBoxSeries->item(i)->isSelected() ) 
	  selected_series += "'" + listBoxSeries->item(i)->text() + "', "; 
  }
  
  selected_series.remove( selected_series.length()-2, 2); // remove last ", " 
  return selected_series;

} 
  


