/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      dbutils.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#include <basic_toolkit/dbutils.h>

#include <sstream>
#include <unistd.h>
#include <sqlite/connection.hpp>
#include <sqlite/query.hpp>
#include <sqlite/command.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/transaction.hpp>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool table_exists( sqlite::connection& con, std::string const& tname )
{
  std::ostringstream sql;

  sql << "SELECT name FROM sqlite_master WHERE type='table' AND name="
      << "'" << tname << "'" << std::ends;

  sqlite::query q( con, sql.str() );
  sqlite::result_type res = q.emit_result();
  
  return (!res) ? false : ( res->get_string(0) == tname );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int table_nrows( sqlite::connection& con, std::string const& tname )
{
  std::ostringstream sql;
  sql << "SELECT COUNT(*) FROM " << tname << std::ends;
  sqlite::query q( con, sql.str() );

  sqlite::result_type res = q.emit_result();
  return res->get_int(0);
}  
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool table_empty( sqlite::connection& con, std::string const& tname )
{
  std::ostringstream sql;

  sql << "SELECT EXISTS (SELECT * FROM " << tname <<")" << std::ends;

  //--------------------------------------------------------------------------
  // the query below is equivalent; it may be more efficient, but less portable 
  //sql <<" select (select id from "<< tname << 
  //      " limit 1) is not null " << std::ends;
  //-------------------------------------------------------------------------

  sqlite::query q( con, sql.str() );

  sqlite::result_type res = q.emit_result();
  return ! bool(res->get_int(0));
}  
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
