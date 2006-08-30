/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      str_to_upper.c
******                                                                
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******   
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******  URA/FNAL reserves all rights.
******                                          
******                                                                
******  Authors:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******
******  Contacts:            Leo Michelotti michelotti@fnal.gov
******             Jean-Francois Ostiguy    ostiguy@fnal.gov
******                                                              
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

   /* -*- C -*- */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <bmlfactory/str_to_upper.h>


char*
str_to_upper( char* str ) {
  char* p;
  
  assert( str != NULL );
    
  for ( p = str; *p != '\0'; ++p ) {
    *p = toupper(*p);
  }
  return str;
}
