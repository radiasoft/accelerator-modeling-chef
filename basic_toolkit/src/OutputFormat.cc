#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      OutputFormat.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <OutputFormat.h>

OutputFormat::OutputFormat( const char* pad )
{
  width     =   10;
  precision =    3;
  flags     =    OutputFormat::scientific;
  dumpbase  =    0;
 
  if( pad ) {
    padding = new char [ strlen( pad ) + 1 ];
    strcpy( padding, pad );
  }
  else {
    padding = new char [2];
    padding[0] = '\t';
    padding[1] = '\0';
  }
}
 
OutputFormat::OutputFormat( const OutputFormat& x)
{
  width     = x.width;
  precision = x.precision;
  flags     = x.flags;
  padding   = new char [ strlen( x.padding ) + 1 ];
  strcpy( padding, x.padding );
  dumpbase =  x.dumpbase;
}

OutputFormat::~OutputFormat()
{
  delete [] padding;
}

OutputFormat& OutputFormat::operator=( const OutputFormat& x )
{
  if( this != &x ) {
    width = x.width;
    precision = x.precision;
    flags = x.flags;
    if ( padding != NULL ) delete [] padding;
    padding = new char [ strlen( x.padding ) + 1 ];
    strcpy( padding, x.padding );
    dumpbase = x.dumpbase;
  }
  return *this;
}


void OutputFormat::setPadding( const char* pad )
{
  if ( padding != NULL ) delete [] padding;
  padding = new char [ strlen( pad ) + 1 ];
  strcpy( padding, pad );
}


const char* OutputFormat::getPadding()
{
  return padding;
}
