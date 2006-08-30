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
#ifndef BMLFACTORY_MATRIX_H
#define BMLFACTORY_MATRIX_H

#include <glib.h>
#include <bmlfactory/fb_allocator.h>

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

#endif /* BMLFACTORY_MATRIX_H */
