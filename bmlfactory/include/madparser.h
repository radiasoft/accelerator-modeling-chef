/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      madparser.h
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

#ifndef madparser_h
#define madparser_h

#include <glib.h>

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

typedef struct madparser_ madparser;

#ifdef __cplusplus
extern "C" {
#endif

madparser*    madparser_init( const char* filename_in, const char* filename_out );
int           madparser_parse( madparser* mp );
int           madparser_delete( madparser* mp );

fb_allocator* madparser_expr_alloc( madparser* mp );
fb_allocator* madparser_ident_alloc( madparser* mp );

GHashTable*   madparser_const_table( madparser* mp );
fb_allocator* madparser_const_alloc( madparser* mp );

GHashTable*   madparser_var_table( madparser* mp );
fb_allocator* madparser_var_alloc( madparser* mp );

GHashTable*   madparser_bel_table( madparser* mp );
fb_allocator* madparser_bel_alloc( madparser* mp );
fb_allocator* madparser_matrix_alloc( madparser* mp );

GHashTable*   madparser_bml_table( madparser* mp );
fb_allocator* madparser_bml_alloc( madparser* mp );

GPtrArray*    madparser_comment_arr( madparser* mp );
fb_allocator* madparser_comment_alloc( madparser* mp );
  
FILE*         madparser_file_out( madparser* mp );

const char*   madparser_current_filename( const madparser* mp );
void*         madparser_current_buffer( const madparser* mp );
int           madparser_has_input( const madparser* mp );

FILE*         madparser_new_yybuff( madparser* mp, const char* filename );
void*         madparser_restore_yybuff( madparser* mp );
void          madparser_save_yybuff( madparser* mp, void* yyb );

int           madparser_linenum( const madparser* mp );
int           madparser_local_linenum( const madparser* mp );
void          madparser_inc_linenum( madparser* mp );

void          madparser_comment_at_eof_set( madparser* mp );
void          madparser_comment_at_eof_clear( madparser* mp );
int           madparser_comment_at_eof_get( const madparser* mp );

void          madparser_comment_mode_inc( madparser* mp );
void          madparser_comment_mode_dec( madparser* mp );
int           madparser_comment_mode( const madparser* mp );

beam_element* madparser_current_bel( madparser* mp );
char*         madparser_current_bel_type( madparser* mp );
void          madparser_set_bel_type(madparser* mp, const char* );
void          madparser_new_bel( madparser* mp );

#ifdef __cplusplus
};
#endif

#endif /* madparser_h */
