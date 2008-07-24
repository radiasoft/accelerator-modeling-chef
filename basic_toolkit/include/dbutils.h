/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      dbutils.h
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
#ifndef DBUTILS_H
#define DBUTILS_H

#include <string>

namespace sqlite {
  class connection;
}


bool table_exists( sqlite::connection& con, std::string const& tname );
int  table_nrows ( sqlite::connection& con, std::string const& tname );
bool table_empty ( sqlite::connection& con, std::string const& tname );

#endif // DBUTILS_H
