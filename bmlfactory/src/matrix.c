/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      matrix.c
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
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <assert.h>
#include <string.h>
#include <bmlfactory/matrix.h>
#include <bmlfactory/expression.h>


matrix*
matrix_init( fb_allocator* matrix_alloc ) {

  matrix* mtx = NULL;

  assert( matrix_alloc != NULL );
  
  PRIVATE_ALLOCATE( mtx, matrix_alloc );
  if ( mtx != NULL ) {
    memset( mtx, 0, sizeof( matrix ) );
  }

  return mtx;
}

   /*
     set default values for matrix rm&tm
    */
int
matrix_set_default( matrix*       mtx,
                    fb_allocator* expr_alloc ) {

  int i;
  GNode* expr = NULL;

  for( i = 0; i < MATRIX_NOF_PARAMS; ++i ) {
    expr = NULL;
    expr = expr_node_init( 1.0, "1.", expr_alloc );
    assert( expr != NULL );
    mtx->rm_[i][i] = expr;
  }
  
  return 0;
}

   /*
     delete all expression(s) inside the matrix and then delete the
     matrix itself
   */
int
matrix_delete( matrix*       mtx,
               fb_allocator* expr_alloc,
               fb_allocator* matrix_alloc ) {
  int i, j, k;

  for( i = 0; i < MATRIX_NOF_PARAMS; ++i ) {
    for( j = 0; j < MATRIX_NOF_PARAMS; ++j ) {
      if ( mtx->rm_[i][j] != NULL ) {
        expr_node_delete( mtx->rm_[i][j], expr_alloc );
      }
    }
  }
  
  for( i = 0; i < MATRIX_NOF_PARAMS; ++i ) {
    for( j = 0; j < MATRIX_NOF_PARAMS; ++j ) {
      for( k = 0; k < MATRIX_NOF_PARAMS; ++k ) {
        if ( mtx->tm_[i][j][k] != NULL ) {
          expr_node_delete( mtx->tm_[i][j][k], expr_alloc );
        }
      }
    }
  }

  PRIVATE_DEALLOCATE( mtx, matrix_alloc );
  return 0;
}
