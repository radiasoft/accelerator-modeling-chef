/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      cfg_output.h
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
#ifndef CFG_OUTPUT_H
#define CFG_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <glib.h>  
#include <bmlfactory/madparser_types.h>

void  const_def_output( madparser* mp, FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  var_def_output  ( madparser* mp, FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bel_def_output  ( madparser* mp, FILE* out, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bml_def_output  ( FILE* out, GHashTable* bml_table, GPtrArray* comment_arr );

#ifdef __cplusplus
};
#endif

#endif /* CFG_OUTPUT_H */
