/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DataBrowser.cc                                    ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:                                                      ******
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
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

#include <DataBrowser.h>
#include <sstream>
#include <iostream>
#include <string>

#undef emit // Qt3 defines this as a macro ;-(

#include <sqlite/query.hpp>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DataBrowser::DataBrowser( sqlite::connection& db, QWidget* parent, const char* name )
: QTable( parent, name), db_(db), table_complete_(false)
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::init( sqlite::connection& db, std::string const& sql )
{ 
  
  sqlite::query q(db, sql);
  res_ = q.emit_result();
 
  unsigned int colsize = res_->get_column_count(); 

  setNumRows( 0 );
  setNumCols( colsize);

  if (!res_)  return;  //  error ... no data  !

  get_rows(); 
 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::get_rows()
{

  unsigned int colsize = numCols();

  for (unsigned int i=0; i<colsize; ++i) {
     horizontalHeader()->setLabel( i,  res_->get_column_name(i) );
  }

  if ( !res_ )  return;

  if ( table_complete_)  return;
  
  unsigned int row_end   = 0; 

  while ( !table_complete_ ) {

    unsigned int row_start  =  (numRows() == 0 ) ? 0 : numRows(); // the index of the old last row 
 
    setNumRows(  (row_start == 0) ? 1000 : numRows()*2  );
    
    row_end   = numRows();              // the index of the new last row 

    for (int row=row_start; row<row_end; ++row ) {
      
         for ( unsigned int col=0; col < numCols(); ++col) {
           setText(row, col, res_->get_string(col) );
         }

         if ( !res_->next_row() ) { table_complete_ = true;  row_end = row;  break; }

    } 

  }

  setNumRows( row_end+1);

  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::closeEvent( QCloseEvent *event )
{
  // For some reason, QTable does not support the destructive close flag ... 
  // Therefore, we explicitly delete on a close event.

    std::cout << " received closeEvent " << std::endl;
    event->accept();
    delete this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DataBrowser::~DataBrowser()
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CS4DDataBrowser::CS4DDataBrowser( sqlite::connection& db, QWidget* parent, char const* name )
  : DataBrowser( db, parent,name)
{

std::string const sql = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta1x, alpha1x, beta1y, alpha1y, psi1,"
       + "beta2x, alpha2x, beta2y, alpha2y, psi2,"
       + "eta_x, etap_x, eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER_4D, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER_4D.iseq ) " 
       +"   AND (ELEMENTS.iseq =      REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =        DISPERSION.iseq ) ";

  
  init( db, sql );  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CS2DDataBrowser::CS2DDataBrowser( sqlite::connection& db, QWidget* parent, char const* name )
  : DataBrowser( db, parent,name)
{

std::string const sql = std::string("") +  
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta_x, alpha_x, psi_x, eta_x, etap_x, "
       + "beta_y, alpha_y, psi_y  eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";
  init( db, sql );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DispersionDataBrowser::DispersionDataBrowser( sqlite::connection& db, QWidget* parent, char const* name )
  : DataBrowser( db, parent,name)
{

std::string const sql = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, x, xp, y,  yp, eta_x, etap_x "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

  init( db, sql );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CovarianceDataBrowser::CovarianceDataBrowser( sqlite::connection& db, QWidget* parent, char const* name )
  : DataBrowser( db, parent,name)
{

std::string const sql= std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COVARIANCE, DISPERSION "
       +" WHERE (ELEMENTS.iseq =      COVARIANCE.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

  init( db, sql );  
}
