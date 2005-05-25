/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******                                    
******  File:      beam_line.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab   
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
******  Authors:   Dmitri Mokhov and Oleg Krivosheev                  
******             Jean-Francois Ostiguy
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
**************************************************************************
*************************************************************************/


   /* -*- C -*- */

#ifndef beam_line_h
#define beam_line_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>  
#include <glib.h>

#if !defined(fb_allocator_h)  
#include "fb_allocator.h"
#endif /* fb_allocator_h */

#define BEAM_LINE_DELETE_OK  0
#define BEAM_LINE_ADD_OK     0
#define BEAM_LINE_ADD_ERR    1
#define BML_NAME_LENGTH    128

typedef struct {
    char        name_[BML_NAME_LENGTH];
    GList*      bel_list_;
    int         counter_;
    int         linenum_;
    const char* filename_;
    int         local_linenum_;
    double      reference_energy_; 
} beam_line;

beam_line*  beam_line_init( fb_allocator* bml_alloc );
int         beam_line_add_bel( beam_line* bml, char* name, GHashTable* bel_table, GHashTable* bml_table );
int         beam_line_add_bml( beam_line* bml1, const beam_line* bml2 );
void        beam_line_set_name( beam_line* bml, const char* name );
void        beam_line_set_linenum( beam_line* bml, int linenum, const char* filename, int linenum_local );
int         beam_line_delete( beam_line* bml, fb_allocator* bml_alloc );
void        beam_line_display( FILE* out, beam_line* bml, GHashTable* bml_table );

#ifdef __cplusplus
};
#endif
  
#endif /* beam_line_h */
