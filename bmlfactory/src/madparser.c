/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******
******  File:      madparser.c
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
******                                                            
******                                                                
******  Authors:   Dmitri Mokhov, Oleg Krivosheev                  
******             and Jean-Francois Ostiguy
******                                                   
******  Contacta:  Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
******  Revision History : 
******  
******  - April 2006, ostiguy@fnal.gov
******    Made both lexer and parser reentrant. 
******    Eliminated global variable mp.
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


   /* -*- C -*- */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* windows.h is required for WIN32 specific file io */
#ifdef _WIN32
#include <windows.h>
#endif

#include <glib.h>

#include <fb_allocator.h>
#include <expression.h>
#include <const_table.h>
#include <var_table.h>
#include <beam_element.h>
#include <beamel_table.h>
#include <matrix.h>
#include <beam_line.h>
#include <bml_table.h>
#include <comment_arr.h>
#include <madparser.h>
#include <madparser_types.h>
#include <PhysicsConstants.h>

typedef void * yyscan_t;

/*
   ---- prototypes
*/

void bmlfactory_exit(const char* filename, int lineno, const char* errmessage);

FILE* yyget_in ( yyscan_t scanner);
FILE* yyget_out( yyscan_t scanner);

void  yyset_in (FILE* in_fp , yyscan_t scanner);
void  yyset_out(FILE* out_fp, yyscan_t scanner);
void  yyrestart (FILE *input_file, yyscan_t yyscanner );
int   yyparse( madparser* mp );
void  read_from_string( madparser* mp, const char* );
void  yydebug_on(int);

/*
   --- local constants
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
    char*           filename_;
    FILE*           yyfile_;
    int             linenum_;
} yybuff;

typedef struct {
    char*       particle_type_;  
    double      mass_;           /* mass [GeV/c**2]           */
    double      charge_;         /* charge in units of e      */
    double      energy_;         /* total energy   [GeV]      */
    double      kenergy_;        /* kinetic energy [GeV]      */
    double      pc_;             /* momentum [GeV/c]          */
    double      gamma_;          /* relativistic gamma        */
    double      brho_;
    double      ex_;             /* horizontal emittance [m]  */
    double      ey_;             /* vertical emittance [m]    */
    double      exn_;            /* normalized hor emittance  */
    double      eyn_;            /* normalized ver emittance  */
    double      et_;             /* longitudinal emittance    */
    double      sigt_;           /* rms bunch length          */
    double      sige_;           /* relative energy spread    */
    int         kbunch_;         /* the number of bunches     */
    double      npart_;          /* number of particles/bunch */
    double      bcurrent_;       /* bunch current             */
    char        bunched_;        /* logical, true = the beam is bunched */ 
    char        radiate_;        /* logical, true = the beam is allowed to radiate photons */
} beam_params;


struct madparser_ {
  
    int             inmemory_;
    void*           default_buffer_;

    char*           filename_in_;
    char*           filename_out_;

    char*           working_dir_;  
      
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

    GSList*         yybuff_list_;         /* obsolete */
    GSList*         filenames_list_;

    fb_allocator*   yybuff_alloc_;        /* obsolete */
 
    GSList*         input_buffers_list_;

    const char*     current_filename_;
    yybuff*         current_yybuff_;

    int             linenum_;

    int             comment_at_eof_;
    int             comment_mode_;

    beam_element*   current_bel_;
    char            current_bel_type_[BEL_NAME_LENGTH];

    char*           use_statement_beamline_name_;
    beam_params     beam_params_;

    yyscan_t        scanner_;
  
};


madparser*
madparser_init( const char* filename_in,
                const char* filename_out ) {
  
  madparser* mp = NULL;
  yybuff*    y = NULL;

  assert( filename_in );
  
  mp = (madparser*) malloc( sizeof(madparser) );

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
          
          
          /* fprintf( stderr, "Input and output files are the same, quitting\n" ); */
          bmlfactory_exit(__FILE__, __LINE__, "Input and output files are the same.");
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

      PRIVATE_ALLOCATE( y, mp->yybuff_alloc_ );
      assert( y != NULL );
      
      y->filename_      = mp->filename_in_;
      mp->yybuff_list_ = g_slist_prepend( mp->yybuff_list_, y );

      mp->current_yybuff_ = y;

      mp->input_buffers_list_ = NULL; /* empty list */

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

          /*
          beam parameters 
	  */ 
      
      mp->use_statement_beamline_name_ = NULL;

      mp->beam_params_.particle_type_ = "PROTON";  
      mp->beam_params_.mass_          = 0.93827231;  /* THIS DOES NOT BELONG HERE */
      mp->beam_params_.charge_        = 1.0;         
      mp->beam_params_.energy_        = 1.0;         
      mp->beam_params_.pc_            = 1.0;         
      mp->beam_params_.gamma_         = 1.0;         
      mp->beam_params_.ex_            = 1.0;         
      mp->beam_params_.ey_            = 1.0;         
      mp->beam_params_.exn_           = 1.0;         
      mp->beam_params_.eyn_           = 1.0;         
      mp->beam_params_.et_            = 1.0;         
      mp->beam_params_.sigt_          = 1.0;         
      mp->beam_params_.sige_          = 1.0;         
      mp->beam_params_.kbunch_        = 1;         
      mp->beam_params_.npart_         = 1.0;         
      mp->beam_params_.bcurrent_      = 0;         
      mp->beam_params_.bunched_       = 0;         
      mp->beam_params_.radiate_       = 0;         

    }
  }

   yylex_init( &(mp->scanner_) ); /* user defined scanner state structure allocation               */  
                                  /* this allows the scanner struct ptr to be passed to the parser */
 
   yyset_extra( mp,  mp->scanner_ );  /* the scanner has a handle to mp  */ 
 
   /* yydebug_on(1); */  /****************  turn on tracing ***********************/
  
   return mp;

}


void* 
madparser_get_scanner( void* mp) { return ((madparser *) mp)->scanner_ ; } 



int
madparser_parse( madparser* mp, const char* stringbuffer) {

  FILE* fp;

  char*    tmpfilename;
  int      res = 0;
  int      i   = 0;

  assert( mp != NULL );
  assert( mp->filename_in_ != NULL );

  if (stringbuffer) 
     mp->inmemory_ = 1;
  else
     mp->inmemory_ = 0;

#if 0
  tmpfilename = (char*) malloc(strlen(mp->filename_in_) + 1);
  mp->working_dir_ = (char*) malloc(strlen(mp->filenaname_in+1);
#endif

  tmpfilename = (char*) malloc(512);
  mp->working_dir_ = (char*) malloc(512);


  strcpy(tmpfilename, mp->filename_in_ );

#ifdef _WIN32
    char fnametmp[512];
    GetFullPathName(tmpfilename, 512, mp->working_dir_, fnametmp);

    for (i = strlen(mp->working_dir_)-1; i >=0; i--)
       {
         if (mp->working_dir_[i] == '\\')
         {
             mp->working_dir_[i] = 0;
             break;
         }
    }

#else
    strcpy( mp->working_dir_, (const char*) dirname(tmpfilename) ); 
#endif

  if (mp->inmemory_ != 0)
  {
    read_from_string(mp, stringbuffer );
    yyset_out( stdout, mp->scanner_); 
    mp->current_yybuff_->yyfile_ = yyget_in( mp->scanner_ ); /* important ! */
    yyparse( mp );
  } 
  else 
  {
    fp = fopen( mp->filename_in_, "r" );
    yyset_in( fp , mp->scanner_) ;

    if (  yyget_in( mp->scanner_) == NULL ) 
    {
      /* fprintf(stderr, "Can't open input file %s\n", mp->filename_in_ ); */
      send_to_stderr_stream(stderr, "Can't open input file %s\n", mp->filename_in_ );
      res = 1;
    } 
    else 
    {

      yyset_out( stdout, mp->scanner_); 

      mp->current_yybuff_->yyfile_ =  yyget_in( mp->scanner_); /* important ! */

      if (mp->filename_out_ != NULL) {
        yyset_out( fopen( mp->filename_out_, "w" ), mp->scanner_ );
        assert( yyget_out( mp->scanner_) != NULL );      
      }

      yyrestart( yyget_in( mp->scanner_),  mp->scanner_ );
      
      while ( !feof( yyget_in( mp->scanner_) ) ) {
        yyparse( mp );
      }    
    }
  }
  free(tmpfilename); 

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

void
madparser_new_yybuff( madparser*  mp,
                      const char* filename ) {

  char*   p = NULL;
  GSList* f = NULL;
  yybuff* y = NULL;
  FILE*   r = NULL;


  assert( mp != NULL );
  assert( filename != NULL );

  if ( strcmp( filename, mp->filename_in_ ) != 0 ) {
    if ( (mp->filename_out_ == NULL) ||
         ( strcmp( filename, mp->filename_out_ ) != 0 ) ) {

      f = g_slist_find_custom( mp->filenames_list_, (gpointer)filename, filename_compare );
      if ( f != NULL ) {
        p = (char*)f->data;
      } else {
        p = (char*)malloc( strlen(filename) + 1 );
        strcpy( p, filename );
        mp->filenames_list_ = g_slist_prepend( mp->filenames_list_, p );
      }
      mp->current_filename_ = p;


      PRIVATE_ALLOCATE( y, mp->yybuff_alloc_ );
      assert( y != NULL );

      y->filename_ = p;
      y->yyfile_   = r;
      y->linenum_  = 0;

      mp->current_yybuff_ = y;
      
      mp->yybuff_list_ = g_slist_prepend( mp->yybuff_list_, y );
      
    } else {
      /* fprintf( stderr, "Trying to switch to file with the same name as master out, quitting\n" ); */
      bmlfactory_exit( __FILE__, __LINE__, "Trying to switch to file with the same name as master out, quitting." );
    }
  } else {

    /*fprintf( stderr, "Trying to switch to file with the same name as master in, quitting\n" ); */
    bmlfactory_exit( __FILE__, __LINE__, "Trying to switch to file with the same name as master in, quitting.");
  }
  
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

  /*** (mp->current_yybuff_)->yybuff_ = yyb; ***/
}


const char*
madparser_current_filename( const madparser* mp ) {
  assert( mp != NULL );
  return mp->current_filename_;
}

void*
madparser_current_buffer( const madparser* mp ) {
  /*** REMOVE ***/

  assert( mp != NULL );
  /*** return (mp->current_yybuff_)->yybuff_; ***/
}


void
madparser_restore_yybuff( madparser* mp ) {
  assert( mp != NULL );
  
  if  ( g_slist_length( mp->yybuff_list_) == 1 ) return; /*  this means the list only contains the current buffer */
                                                         /*  note: yybuff_list_ needs to go away ! */

  mp->yybuff_list_ = g_slist_remove( mp->yybuff_list_, mp->current_yybuff_ );
  PRIVATE_DEALLOCATE( mp->current_yybuff_, mp->yybuff_alloc_ );

  mp->current_yybuff_ = (yybuff*)(mp->yybuff_list_)->data;

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

  PRIVATE_DEALLOCATE( data, (fb_allocator*)user_data );
}

int
madparser_delete( madparser* mp ) {

  reset_input_buffer( mp );

  comment_arr_delete( mp->comment_arr_ );
  bml_table_delete( mp->bml_table_, mp->bml_alloc_ );
  bel_table_delete( mp->bel_table_, mp->bel_alloc_ , mp->expr_alloc_);
  var_table_delete( mp->var_table_, mp->expr_alloc_ );
  const_table_delete( mp->const_table_, mp->expr_alloc_ );
  expression_delete();

  g_slist_foreach( mp->filenames_list_, free_filename, NULL );
  g_slist_free( mp->filenames_list_ ); 

  g_slist_foreach( mp->yybuff_list_, free_yybuff, mp->yybuff_alloc_ );
  g_slist_free( mp->yybuff_list_ );
  
  g_slist_free( mp->input_buffers_list_);

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

  if ( mp->working_dir_ != NULL ) {
    free( mp->working_dir_ );
  }

  if ( mp->use_statement_beamline_name_ != NULL ) { 
     free(mp->use_statement_beamline_name_);
  }

 
  if ( mp->inmemory_ == 0) 
  { 
    if ( yyget_in( mp->scanner_) != stdin)  
        fclose( yyget_in( mp->scanner_)  ); 
    if ((yyget_out( mp->scanner_) != stdout ) && ( yyget_out( mp->scanner_) != stderr) ) 
        fclose(  yyget_out( mp->scanner_) );
  }
   
   
  yylex_destroy( mp->scanner_ );

  free( mp );

  return 0;

}

FILE*
madparser_file_out( madparser* mp ) {
  assert( mp != NULL );
  return yyget_out( mp->scanner_);
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
  mp->current_bel_->reference_energy_ = mp->beam_params_.energy_; /* set reference to current energy (set by BEAM statement) */
}


FILE*  
madparser_call_include( madparser* mp, char* newfile, void* yybuffer) {

  /*  Save the (f)lex input buffer                                                 */
  /*  Returns the full (canonical) path for the included file specified by newfile.*/
 

  char* fullpathnewfile;
  char* errmsg;

  madparser_push_input_buffer(mp, yybuffer); 
  fullpathnewfile = (char*) malloc( strlen(newfile) + strlen(mp->working_dir_)+2 );

#ifdef _WIN32
  if ( newfile[1] != ':' ) /* if the leading string is of the form "a:", then this is an absolute path. */ 
  {
    strcpy(fullpathnewfile, mp->working_dir_);
    strcat(fullpathnewfile,"\\");
    strcat(fullpathnewfile,newfile);
  } 
  else
  {
    strcpy(fullpathnewfile,newfile);
  };

#else
  if ( newfile[0] != '/' ) /* if the leading char is a '/', then this is an absolute path. */ 
  {
    strcpy(fullpathnewfile, mp->working_dir_);
    strcat(fullpathnewfile,"/");
    strcat(fullpathnewfile,newfile);
  } 
  else
  {
    strcpy(fullpathnewfile,newfile);
  };

#endif
  
  madparser_new_yybuff(mp, fullpathnewfile); /**** need to eliminate this ***/

  yyset_in( fopen( mp->current_filename_, "r"), mp->scanner_);

  if ( yyget_in(mp->scanner_)  == NULL ) {
    errmsg = (char*) malloc( strlen(fullpathnewfile)+ strlen("File ")+strlen( " not found.") + 1 );
    strcpy(errmsg, "File ");   
    strcat(errmsg,  fullpathnewfile);
    strcat(errmsg,  " not found.");
    bmlfactory_exit(__FILE__, __LINE__, errmsg);
  }

  free(fullpathnewfile);
  return yyget_in( mp->scanner_);

}

void* 
madparser_return_from_include( madparser* mp ) {

  madparser_restore_yybuff( mp ); /*** need to eliminate this ***/

  mp->current_filename_ =  mp->current_yybuff_->filename_;
  return madparser_pop_input_buffer( mp );
 
}

int 
madparser_includestack_empty( madparser* mp ) {

  if ( mp->input_buffers_list_) { 
    return 0;
  }
  else {
    return 1;
  }
}


void
madparser_push_input_buffer(madparser* mp, void* yybuffer) 
{

 mp->input_buffers_list_ = g_slist_prepend( mp->input_buffers_list_, yybuffer );


}

void*
madparser_pop_input_buffer( madparser* mp )
{
  void* p;

  /* get the first element and return it after removing it from the list */

  p = mp->input_buffers_list_->data;
  mp->input_buffers_list_ = g_slist_remove( mp->input_buffers_list_, p); 

  return p;

}

int 
madparser_is_reading_from_memory(madparser* mp)
{

  return mp->inmemory_;

}

void 
madparser_set_inmemory_flag(madparser* mp, int flag)
{

  mp->inmemory_ = flag;

}

void* 
madparser_get_default_buffer(madparser* mp)
{

  return mp->default_buffer_;

}


void
madparser_use_parameters ( madparser* mp, const char* s) {

  if ( mp->use_statement_beamline_name_ != NULL ) free(mp->use_statement_beamline_name_);

  mp->use_statement_beamline_name_  = (char*) malloc( (strlen(s)+1)*sizeof(char) );
  strcpy(  mp->use_statement_beamline_name_ ,s );  
}


const char* 
madparser_get_use_statement_beamline_name(  madparser* mp ) { 

   return mp->use_statement_beamline_name_;

}


const char* 
madparser_get_particle_type( madparser* mp ) {
  return mp->beam_params_.particle_type_;
}


void  
madparser_set_beam_particle_mass( madparser* mp, double mass) {
  if( fabs( mass - PH_NORM_mp) < 1.0e-6*fabs(mass + PH_NORM_mp) ) {
    if( 0 == strcasecmp( "PROTON", mp->beam_params_.particle_type_  ) ) {
      mp->beam_params_.mass_ = mass;
    }
    else {
      madparser_set_beam_particle_type( mp, "PROTON" );
    }
  }

  else if( fabs( mass - PH_NORM_me) < 1.0e-6*fabs(mass + PH_NORM_me) ) {
    if( 0 == strcasecmp( "POSITRON", mp->beam_params_.particle_type_  ) ) {
      mp->beam_params_.mass_ = mass;
    }
    else {
      madparser_set_beam_particle_type( mp, "POSITRON" );
    }
  }

  else {
    send_to_stderr_stream( stderr, "error ! Incorrect mass assignment %f for %s\n"
                                   , mass, mp->beam_params_.particle_type_ );
    char error_msg[132];
    sprintf( error_msg, "*** ERROR *** Incorrect mass assignment %f for %s"
                      , mass, mp->beam_params_.particle_type_ );
    bmlfactory_exit(__FILE__, __LINE__, error_msg);
  }
}


void  
madparser_set_beam_particle_charge( madparser* mp, double charge){
  if( fabs( charge - 1.0) < 1.0e-6*fabs(charge + 1.0) ) {
    if(    (0 == strcasecmp( "PROTON"  , mp->beam_params_.particle_type_  ))
        || (0 == strcasecmp( "POSITRON", mp->beam_params_.particle_type_  )) ) {
      mp->beam_params_.charge_ = charge;
    }
  }
  else {
    send_to_stderr_stream( stderr, "error ! Incorrect charge assignment %f for %s\n"
                                   , charge, mp->beam_params_.particle_type_ );
    char error_msg[132];
    sprintf( error_msg, "*** ERROR *** Incorrect charge assignment %f for %s"
                      , charge, mp->beam_params_.particle_type_ );
    bmlfactory_exit(__FILE__, __LINE__, error_msg);
  }
}


void  
madparser_set_beam_energy( madparser* mp, double value){

  double pc, m_p, et, ek, brho, gamma; 

  m_p      = mp->beam_params_.mass_;
  et        = value;

  if( et < m_p ) {
    send_to_stderr_stream( stderr, "error ! Energy %f smaller than rest mass %f\n"
                                   , et, m_p                                       );
    char error_msg[132];
    sprintf( error_msg, "*** ERROR *** Energy %f smaller than rest mass %f"
                      , et, m_p                                                    );
    bmlfactory_exit(__FILE__, __LINE__, error_msg);
  }

  else {
    ek        = et - m_p;
    gamma     = et/m_p;
    pc        = sqrt((et*et)-(m_p*m_p));
    brho      = pc/PH_CNV_brho_to_p;

    mp->beam_params_.pc_      = pc;
    mp->beam_params_.energy_  = et;
    mp->beam_params_.kenergy_ = ek;
    mp->beam_params_.brho_    = brho;
    mp->beam_params_.gamma_   = gamma;
  }
} 


void  
madparser_set_beam_momentum( madparser* mp, double value){

  double pc, m_p, et, ek, brho, gamma; 

  m_p      = mp->beam_params_.mass_;

  pc      = value;  
  et      = sqrt( (pc*pc)+(m_p*m_p) );
  ek      = et - m_p;
  gamma   = et/m_p;
  brho    = pc/PH_CNV_brho_to_p;

  mp->beam_params_.pc_      = pc;
  mp->beam_params_.energy_  = et;
  mp->beam_params_.kenergy_ = ek;
  mp->beam_params_.brho_    = brho;
  mp->beam_params_.gamma_   = gamma;
}


void  
madparser_set_beam_gamma( madparser* mp, double value){

  double pc, m_p, et, ek, brho, gamma; 

  if( value < 1.0 ) {
    send_to_stderr_stream( stderr, "error ! Assigned gamma %f is smaller than 1.\n"
                                   , value                                          );
    char error_msg[132];
    sprintf( error_msg, "*** ERROR *** Assigned gamma %f is smaller than 1."
                      , value                                                       );
    bmlfactory_exit(__FILE__, __LINE__, error_msg);
  }

  else {
    m_p   = mp->beam_params_.mass_;

    gamma = value;
    et    = gamma*m_p;
    ek    = et-m_p;
    pc    = sqrt((et*et)-(m_p*m_p));
    brho  = pc/PH_CNV_brho_to_p;

    mp->beam_params_.pc_      = pc;
    mp->beam_params_.energy_  = et;
    mp->beam_params_.kenergy_ = ek;
    mp->beam_params_.brho_    = brho;
    mp->beam_params_.gamma_   = gamma;
  }
}


void  
madparser_set_beam_particle_type( madparser* mp, char* type){

  char* s;
  s = (char*) malloc( ( strlen(type)+1 )*sizeof(char) ); 
  strcpy( s, type);

  mp->beam_params_.particle_type_ = s;

  if( 0 == strcasecmp( type, "POSITRON" ) ) {
    madparser_set_beam_particle_charge( mp, 1.0 );
    madparser_set_beam_particle_mass( mp, PH_NORM_me );
  }
  else if( 0 == strcasecmp( type, "PROTON" ) ) {
    madparser_set_beam_particle_charge( mp, 1.0 );
    madparser_set_beam_particle_mass( mp, PH_NORM_mp );
  }
  else {
    send_to_stderr_stream(stderr, "error ! Unrecognized particle type %s\n", type );
    char error_msg[132];
    sprintf(error_msg, "*** ERROR *** Unrecognized particle type %s", type  );
    bmlfactory_exit(__FILE__, __LINE__, error_msg);
  }
}

double
madparser_get_brho(  madparser* mp ) { 

  return mp->beam_params_.brho_;

}


void
madparser_set_bml_ref_energy(madparser* mp, beam_line* bml) {

  bml->reference_energy_ = mp->beam_params_.energy_;
 
}


#if 0
   case EK: 
 
   _ek        = value;
   _et        = m_p + _ek;
   _gamma     = _et/m_p;
   _momentum  = sqrt((_et*_et)-(m_p*m_p));
   _brho      = _momentum/PH_CNV_brho_to_p;
  
   break;

 case BRHO:

   _brho      =  value;
   _momentum  =  _brho*PH_CNV_brho_to_p;
   _et        =  sqrt( (_momentum*_momentum)+(m_p*m_p) );
   _ek        =  _et - m_p;
   _gamma     =  _et/m_p;

   break;

 }
#endif
