/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      matrix.h
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

#ifndef matrix_h
#define matrix_h

#include <glib.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator_h */

#ifdef __cplusplus
extern "C" {
#endif

#define MATRIX_NOF_PARAMS 6

typedef struct {
    GNode* rm_[MATRIX_NOF_PARAMS][MATRIX_NOF_PARAMS];
    GNode* tm_[MATRIX_NOF_PARAMS][MATRIX_NOF_PARAMS][MATRIX_NOF_PARAMS];
} matrix;
  
matrix*  matrix_init( fb_allocator* matrix_alloc );
int      matrix_set_default( matrix* mtx, fb_allocator* expr_alloc );
int      matrix_delete( matrix* mtx, fb_allocator* expr_alloc, fb_allocator* matrix_alloc );

#ifdef __cplusplus
};
#endif

#endif /* multipole_h */
