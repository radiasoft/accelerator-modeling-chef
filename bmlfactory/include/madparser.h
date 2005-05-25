/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.3
******                                    
******  File:      madparser.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab   
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
******

******  Authors:   Dmitri Mokhov, Oleg Krivosheev, Jean-Francois Ostiguy                  
******                                                                
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

#if !defined(beam_line_h)
#include "beam_line.h"
#endif /* beam_line_h */

typedef struct madparser_ madparser;

#ifdef __cplusplus
extern "C" {
#endif

madparser*    madparser_init( const char* filename_in, const char* filename_out);

int           madparser_parse( madparser* mp, const char* stringbuffer);
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
void          madparser_set_bml_ref_energy(madparser* mp, beam_line* bml);

GPtrArray*    madparser_comment_arr( madparser* mp );
fb_allocator* madparser_comment_alloc( madparser* mp );

void          madparser_use_parameters ( madparser* mp, const char* s);
const char*   madparser_get_use_statement_beamline_name( madparser* mp );

void          madparser_set_beam_particle_mass( madparser* mp, double mass);
void          madparser_set_beam_particle_charge( madparser* mp, double charge); 
void          madparser_set_beam_energy( madparser* mp, double energy); 
void          madparser_set_beam_momentum( madparser* mp, double momentum);
void          madparser_set_beam_gamma( madparser* mp, double gamma);
void          madparser_set_beam_particle_type( madparser* mp, char* type);

double        madparser_get_brho( madparser* mp );  

FILE*         madparser_file_out( madparser* mp );

const char*   madparser_current_filename( const madparser* mp );
void*         madparser_current_buffer( const madparser* mp );
int           madparser_has_input( const madparser* mp );

void          madparser_new_yybuff( madparser* mp, const char* filename );
void          madparser_restore_yybuff( madparser* mp );
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


FILE*         madparser_call_include( madparser* mp, char* newfile, void* yybuffer);
void*         madparser_return_from_include(madparser* mp );

  int         madparser_includestack_empty( madparser* mp );
void          madparser_push_input_buffer(madparser* mp, void* yybuffer); 
void*         madparser_pop_input_buffer(madparser* mp);
  int         madparser_is_reading_from_memory(madparser* mp);
void          madparser_set_inmemory_flag(madparser* mp, int);


#ifdef __cplusplus
};
#endif

#endif /* madparser_h */
