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
******  Copyright (c) 1999  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


   /* -*- C -*- */

#ifndef cfg_output_h
#define cfg_output_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <glib.h>  

void  const_def_output( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  var_def_output( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bel_def_output( FILE* out, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bml_def_output( FILE* out, GHashTable* bml_table, GPtrArray* comment_arr );

#ifdef __cplusplus
};
#endif

#endif /* cfg_output_h */
