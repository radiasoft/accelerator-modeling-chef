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

int          comment_arr_lookup( int linenum, GPtrArray* comment_arr );
int          comment_arr_add( const char* sptr, int linenum, const char* filename, int local_linenum, GPtrArray* comment_arr, fb_allocator* comment_alloc );

#endif /* comment_arr_h */
