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

#ifndef comment_arr_h
#define comment_arr_h

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

#endif /* comment_arr_h */
