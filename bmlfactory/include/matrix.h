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
