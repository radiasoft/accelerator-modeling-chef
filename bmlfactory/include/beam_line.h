/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      beam_line.h
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
