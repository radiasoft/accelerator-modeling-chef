/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      comment_arr.h
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
#ifndef COMMENT_ARR_H
#define COMMENT_ARR_H

#include <stdlib.h>
#include <glib.h>

#define COMMENT_OK           0
#define COMMENT_INIT_ERR     1

typedef  struct {
    char*       svalue_;
    int         linenum_;
    const char* filename_;
    int         local_linenum_;
} comment;

GPtrArray*   comment_arr_init( void );
int          comment_arr_delete( GPtrArray* comment_arr );
void         comment_arr_display( FILE* out, GPtrArray* comment_arr );

gpointer     comment_arr_lookup( int linenum, GPtrArray* comment_arr );
int          comment_arr_add( const char* sptr, int linenum, const char* filename, int local_linenum, GPtrArray* comment_arr, fb_allocator* comment_alloc );

#endif /* COMMENT_ARR_H */
