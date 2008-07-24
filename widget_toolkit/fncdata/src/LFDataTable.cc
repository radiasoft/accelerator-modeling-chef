/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:    LFDataTable.cc                                      ****** 
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

#include <LFDataTable.h>
#include <sstream>
#include <iostream>
#include <string>

#undef emit // Qt defines this as a macro ;-(

#include <sqlite/query.hpp>

std::string const CS2DQuery = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta_x, alpha_x, psi_x, eta_x, etap_x, "
       + "beta_y, alpha_y, psi_y  eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

std::string const CS4DQuery = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta1x, alpha1x, beta1y, alpha1y, psi1,"
       + "beta2x, alpha2x, beta2y, alpha2y, psi2,"
       + "eta_x, etap_x, eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER_4D, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER_4D.iseq ) " 
       +"   AND (ELEMENTS.iseq =      REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =        DISPERSION.iseq ) LIMIT 100";

std::string const DispersionQuery = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, x, xp, y,  yp, eta_x, etap_x "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

std::string const CovarianceQuery = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COVARIANCE, DISPERSION "
       +" WHERE (ELEMENTS.iseq =      COVARIANCE.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

LFDataTable::LFDataTable( sqlite::connection& db, std::string const& data_query, QWidget* parent, const char* name )
: QTable( parent, name)
{ 
  
  std::ostringstream sql;

  std::cout << data_query << std::endl;

  sqlite::query q(db, data_query );

  sqlite::result_type res = q.emit_result();
 
  if (!res)  return; //  error ... no data  !

  // count the no of rows

  unsigned int rowsize = 0;

  do { ++rowsize; } while ( res->next_row() );

  sqlite::query q2(db, data_query );
  res = q2.emit_result();

  unsigned int colsize = res->get_column_count(); 

  setNumRows( rowsize );
  setNumCols( colsize );
  
  for (unsigned int i=0; i<colsize; ++i) {
     horizontalHeader()->setLabel( i,  res->get_column_name(i) );
  }

  int row = 0;
  if ( res ) {
    do {
      for ( unsigned int col=0; col<colsize; ++col) {
         setText(row, col, res->get_string(col) );
      }
      ++row;  
   } while( res->next_row() );
  } 

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void LFDataTable::closeEvent( QCloseEvent *event )
{
  // QTable does not seem to support the destructive close
  // flag ... so we explicitly delete on a close event.

    event->accept();
    delete this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

LFDataTable::~LFDataTable()
{} 

