   /* -*- C -*- */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(matrix_h)
#include "matrix.h"
#endif /* matrix_h */

#if !defined(beam_line_h)
#include "beam_line.h"
#endif /* beam_line_h */

#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */

#if !defined(comment_arr_h)
#include "comment_arr.h"
#endif /* comment_arr_h */

#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */

   /*
    *  prototypes
    */

extern int yyparse( void );

   /*
    *  external vars
    */

extern FILE* yyin;
extern FILE* yyout;

   /*
    *  local constants
    */

static const size_t       alignment   = 16;
static const size_t       ident_len   = 4;
static const unsigned int nelem_expr  = 200;
static const unsigned int nelem_ident = 100;
static const unsigned int nelem_const = 100;
static const unsigned int nelem_bel   = 200;
static const unsigned int nelem_mtx   = 20;
static const unsigned int nelem_bml   = 50;
static const unsigned int nelem_cmnt  = 100;
static const unsigned int nelem_yybuf =  50;

typedef struct {
    void* yybuff_;
    char* filename_;
    FILE* yyfile_;
    int   linenum_;
} yybuff;

struct madparser_ {
    char*           filename_in_;
    char*           filename_out_;
    
    fb_allocator*   expr_alloc_;
    fb_allocator*   ident_alloc_;

    fb_allocator*   const_alloc_;
    GHashTable*     const_table_;
    
    fb_allocator*   var_alloc_;
    GHashTable*     var_table_;

    fb_allocator*   bel_alloc_;
    fb_allocator*   matrix_alloc_;
    GHashTable*     bel_table_;

    GHashTable*     bml_table_;
    fb_allocator*   bml_alloc_;

    GPtrArray*      comment_arr_;
    fb_allocator*   comment_alloc_;

    GSList*         yybuff_list_;
    GSList*         filenames_list_;
    fb_allocator*   yybuff_alloc_;

    const char*     current_filename_;
    yybuff*         current_yybuff_;

    int             linenum_;

    int             comment_at_eof_;
    int             comment_mode_;

    beam_element*   current_bel_;
    char            current_bel_type_[BEL_NAME_LENGTH];
};

madparser*
madparser_init( const char* filename_in,
                const char* filename_out ) {
  
  madparser* mp = NULL;
  yybuff*    y = NULL;

  assert( filename_in );
  
  mp = (madparser*)malloc( sizeof(madparser) );

  if ( mp != NULL ) {
    memset( mp, 0, sizeof(madparser) );
      
    mp->filename_in_ = (char*)malloc( strlen(filename_in) + 1 );
    if ( mp->filename_in_ == NULL ) {
      free( mp );
      mp = NULL;
    } else {
      strcpy( mp->filename_in_, filename_in );
      
      mp->filename_out_ = NULL;
      if ( filename_out != NULL ) {
        if ( strcmp( filename_out, filename_in ) != 0 ) { 
          mp->filename_out_ = (char*)malloc( strlen(filename_out) + 1 );
          if ( mp->filename_out_ == NULL ) {
            free( mp->filename_in_ );
            free( mp );
            mp = NULL;
          } else {
            strcpy( mp->filename_out_, filename_out );
          }
        } else {
          fprintf( stderr, "Input and output files are the same, quitting\n" );
          exit( EXIT_FAILURE );
        }
      }

         /*
           allocators initialization
         */
      mp->expr_alloc_ = fb_alloc_init( sizeof(expr_struct), alignment, nelem_expr );
      assert( mp->expr_alloc_ != NULL );
      
      mp->ident_alloc_ = fb_alloc_init( ident_len, alignment, nelem_ident );
      assert( mp->ident_alloc_ != NULL );
      
      mp->const_alloc_ = fb_alloc_init( sizeof(constant), alignment, nelem_const );
      assert( mp->const_alloc_ != NULL );
      
      mp->var_alloc_ = fb_alloc_init( sizeof(variable), alignment, nelem_const );
      assert( mp->var_alloc_ != NULL );
      
      mp->bel_alloc_   = fb_alloc_init( sizeof(beam_element), alignment, nelem_bel );
      assert( mp->bel_alloc_ != NULL );
      
      mp->matrix_alloc_ = fb_alloc_init( sizeof(matrix), alignment, nelem_mtx );
      assert( mp->matrix_alloc_ != NULL );
      
      mp->bml_alloc_ = fb_alloc_init( sizeof(beam_line), alignment, nelem_bml );
      assert( mp->bml_alloc_ != NULL );

      mp->comment_alloc_ = fb_alloc_init( sizeof(comment), alignment, nelem_cmnt );
      assert( mp->comment_alloc_ != NULL );
      
      mp->yybuff_alloc_ = fb_alloc_init( sizeof(yybuff), alignment, nelem_yybuf );
      assert( mp->yybuff_alloc_ != NULL );
      
         /*
           tables and other stuff initialization
         */
      
      expression_init();      
      
      mp->const_table_ = const_table_init( mp->expr_alloc_, mp->const_alloc_ );
      assert( mp->const_table_ != NULL );
      mp->var_table_ = var_table_init();
      assert( mp->var_table_ != NULL );
      mp->bel_table_ = bel_table_init();
      assert( mp->bel_table_ != NULL );
      mp->bml_table_ = bml_table_init();
      assert( mp->bml_table_ != NULL );
      mp->comment_arr_ = comment_arr_init();
      assert( mp->comment_arr_ != NULL );

         /*
           filename list(s) and yybuff list(s) initialization
          */
      
      mp->filenames_list_   = g_slist_prepend( mp->filenames_list_, mp->filename_in_ );
      mp->current_filename_ = mp->filename_in_;

      y = (yybuff*)allocate( mp->yybuff_alloc_ );
      assert( y != NULL );
      
      y->filename_      = mp->filename_in_;
      mp->yybuff_list_ = g_slist_prepend( mp->yybuff_list_, y );

      mp->current_yybuff_ = y;

         /*
           line info initialization
          */
      mp->linenum_ = 0;

         /*
           flags initialization
          */
      mp->comment_at_eof_ = 0;
      mp->comment_mode_   = 0;
         /*
           current beam element initialization          
          */
      mp->current_bel_ = beam_element_init( mp->expr_alloc_, mp->bel_alloc_ );
      assert( mp->current_bel_ != NULL );

      mp->current_bel_type_[0] = '\0';
    }
  }
  return mp;
}

int
madparser_parse( madparser* mp ) {

  int res = 0;
  
  assert( mp != NULL );
  assert( mp->filename_in_ != NULL );

  yyin = fopen( mp->filename_in_, "r" );
  if ( yyin == NULL ) {
    fprintf(stderr, "Can't open input file %s\n", mp->filename_in_ );
    res = 1;
  } else {
    yyout = stdout;
    if ( mp->filename_out_ != NULL ) {
      yyout = fopen( mp->filename_out_, "w" );
      assert( yyout != NULL );      
    }
    while ( !feof( yyin ) ) {
      yyparse();
    }    
  }
  return res;
}

fb_allocator*
madparser_expr_alloc( madparser* mp ) {
  assert( mp != NULL );  
  return ( mp->expr_alloc_ );
}

fb_allocator*
madparser_ident_alloc( madparser* mp ) {
  assert( mp != NULL );  
  return ( mp->ident_alloc_ );
}

GHashTable*
madparser_const_table( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->const_table_ );
}


fb_allocator*
madparser_const_alloc( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->const_alloc_ );
}

GHashTable*
madparser_var_table( madparser* mp ) {
  assert( mp != NULL );
  return mp->var_table_;
}

fb_allocator*
madparser_var_alloc( madparser* mp ) {
  assert( mp != NULL );
  return mp->var_alloc_;
}

GHashTable*
madparser_bel_table( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->bel_table_ );
}

fb_allocator*
madparser_bel_alloc( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->bel_alloc_ );
}

fb_allocator*
madparser_matrix_alloc( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->matrix_alloc_ );
}

GHashTable*
madparser_bml_table( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->bml_table_ );
}

fb_allocator*
madparser_bml_alloc( madparser* mp ) {
  assert( mp != NULL );
  return ( mp->bml_alloc_ );
}

GPtrArray*
madparser_comment_arr( madparser* mp ) {
  assert( mp != NULL );
  return mp->comment_arr_;
}

fb_allocator*
madparser_comment_alloc( madparser* mp ) {
  assert( mp != NULL );
  return mp->comment_alloc_;
}

static gint
filename_compare( gconstpointer a,
                  gconstpointer b ) {
  return (gint)(strcmp( (const char*)a, (const char*)b ));
}

FILE*
madparser_new_yybuff( madparser*  mp,
                      const char* filename ) {

  char*   p;
  GSList* f = NULL;
  yybuff* y = NULL;
  FILE*   r = NULL;

  assert( mp != NULL );
  assert( filename != NULL );

  if ( strcmp( p, mp->filename_in_ ) != 0 ) {
    if ( (mp->filename_in_ == NULL) ||
         ( strcmp( p, mp->filename_out_ ) != 0 ) ) {

      f = g_slist_find_custom( mp->filenames_list_, (gpointer)filename, filename_compare );
      if ( f != NULL ) {
        p = (char*)f->data;
      } else {
        p = (char*)malloc( strlen(filename) + 1 );
        strcpy( p, filename );
        mp->filenames_list_ = g_slist_prepend( mp->filenames_list_, p );
      }
      mp->current_filename_ = p;
      r                     = fopen( p, "r" );

      y = (yybuff*)allocate( mp->yybuff_alloc_ );
      assert( y != NULL );

      y->filename_ = p;
      y->yyfile_   = r;
      y->linenum_  = 0;
      mp->current_yybuff_ = y;

      mp->yybuff_list_ = g_slist_prepend( mp->yybuff_list_, y );
      
    } else {
      fprintf( stderr, "Trying to switch to file with the same name as master out, quitting\n" );
      exit( EXIT_FAILURE );
    }
  } else {
    fprintf( stderr, "Trying to switch to file with the same name as master in, quitting\n" );
    exit( EXIT_FAILURE );
  }
  
  return r;
}

int
madparser_has_input( const madparser* mp ) {

  assert( mp != NULL );
  
  return ( (mp->current_yybuff_)->filename_ != mp->filename_in_ );
}

void
madparser_save_yybuff( madparser* mp,
                       void*      yyb ) {
  assert( mp  != NULL );
  assert( yyb != NULL );

  (mp->current_yybuff_)->yybuff_ = yyb;
}


const char*
madparser_current_filename( const madparser* mp ) {
  assert( mp != NULL );
  return mp->current_filename_;
}

void*
madparser_current_buffer( const madparser* mp ) {
  assert( mp != NULL );
  return (mp->current_yybuff_)->yybuff_;
}

void*
madparser_restore_yybuff( madparser* mp ) {
  assert( mp != NULL );

  mp->yybuff_list_ = g_slist_remove( mp->yybuff_list_, mp->current_yybuff_ );
  deallocate( mp->current_yybuff_, mp->yybuff_alloc_ );

  mp->current_yybuff_ = (yybuff*)(mp->yybuff_list_)->data;

  return (mp->current_yybuff_)->yybuff_;
}

static void
free_filename( gpointer data,
               gpointer user_data ) {
  assert( data != NULL );
  free( data );
}

static void
free_yybuff( gpointer data,
             gpointer user_data ) {
  assert( data != NULL );
  assert( user_data != NULL );

  deallocate( data, (fb_allocator*)user_data );
}

int
madparser_delete( madparser* mp ) {

  comment_arr_delete( mp->comment_arr_ );
  bml_table_delete( mp->bml_table_, mp->bml_alloc_ );
  bel_table_delete( mp->bel_table_, mp->bel_alloc_ );
  var_table_delete( mp->var_table_, mp->expr_alloc_ );
  const_table_delete( mp->const_table_, mp->expr_alloc_ );
  expression_delete();

  g_slist_foreach( mp->filenames_list_, free_filename, NULL );
  g_slist_free( mp->filenames_list_ );

  g_slist_foreach( mp->yybuff_list_, free_yybuff, mp->yybuff_alloc_ );
  g_slist_free( mp->yybuff_list_ );
  
  fb_alloc_delete( mp->yybuff_alloc_ );
  fb_alloc_delete( mp->comment_alloc_ );
  fb_alloc_delete( mp->bml_alloc_ );
  fb_alloc_delete( mp->matrix_alloc_ );
  fb_alloc_delete( mp->bel_alloc_ );
  fb_alloc_delete( mp->const_alloc_ );
  fb_alloc_delete( mp->expr_alloc_ );
  fb_alloc_delete( mp->ident_alloc_ );

  if ( mp->filename_out_ != NULL ) {
    free( mp->filename_out_ );
  }

  free( mp );

  fclose( yyin );
  fclose( yyout );
  
  return 0;
}

FILE*
madparser_file_out( madparser* mp ) {
  assert( mp != NULL );
  return yyout;
}

int
madparser_linenum( const madparser* mp ) {
  assert( mp != NULL );
  return mp->linenum_;
}

int
madparser_local_linenum( const madparser* mp ) {
  assert( mp != NULL );
  return (mp->current_yybuff_)->linenum_;
}

void
madparser_inc_linenum( madparser* mp ) {
  assert( mp != NULL );
  (mp->linenum_)++;
  ((mp->current_yybuff_)->linenum_)++;
}

void
madparser_comment_at_eof_set( madparser* mp ) {
  assert( mp != NULL );
  mp->comment_at_eof_ = 1;
}

void
madparser_comment_at_eof_clear( madparser* mp ) {
  assert( mp != NULL );
  mp->comment_at_eof_ = 0;
}

int
madparser_comment_at_eof_get( const madparser* mp ) {
  assert( mp != NULL );
  return mp->comment_at_eof_;
}

void
madparser_comment_mode_inc( madparser* mp ) {
  assert( mp != NULL );
  ++(mp->comment_mode_);
}

void
madparser_comment_mode_dec( madparser* mp ) {
  assert( mp != NULL );
  --(mp->comment_mode_);
}

int
madparser_comment_mode( const madparser* mp ) {
  assert( mp != NULL );
  return (mp->comment_mode_);
}

beam_element*
madparser_current_bel( madparser* mp ) {
  assert( mp != NULL );
  return mp->current_bel_;
}

char*
madparser_current_bel_type( madparser* mp ) {
  assert( mp != NULL );
  return mp->current_bel_type_;
}

void
madparser_set_bel_type(madparser* mp, const char* str ) {
  assert(mp!=NULL);
  assert(strlen(str)<BEL_NAME_LENGTH);
  strcpy( mp->current_bel_type_, str );
}

void
madparser_new_bel( madparser* mp ) {
  assert( mp != NULL );
  mp->current_bel_ = beam_element_init( mp->expr_alloc_, mp->bel_alloc_ );
  assert( mp->current_bel_ != NULL );
  mp->current_bel_type_[0] = '\0';
}
