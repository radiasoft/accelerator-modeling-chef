%{
#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <assert.h>
#include <math.h>  
#include <stdlib.h>  
#include <stdio.h>
#include <string.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /*fb_allocator_h*/

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */
  
#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */
  
#if !defined(comment_arr_h)
#include "comment_arr.h"
#endif /* comment_arr_h */
  
#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */
  
#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(matrix_h)
#include "matrix.h"
#endif /* matrix_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */
  
#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */

#define YYDEBUG 1

void yyerror( char* s );
int  yylex( void );

extern struct madparser_* mp;
 
%}

%union {
  double   dval;
  char*    sval;
  void*    ptr;
}

%token MAD_NEWLINE MAD_IDENTIFIER MAD_STRING_LITERAL MAD_NUMERIC_LITERAL MAD_CONSTANT

%token MAD_HELP MAD_SHOW MAD_TITLE MAD_STOP MAD_OPTION MAD_DEF_PARAM MAD_SET MAD_VALUE

%token MAD_SYSTEM MAD_COMMENT MAD_ENDCOMMENT MAD_EXCOMMENT MAD_RESET 

%token MAD_INTER MAD_ECHO MAD_TRACE MAD_DOUBLE MAD_VERIFY MAD_WARN MAD_INFO MAD_SYMPL MAD_DEBUG

%token MAD_KEYWORD MAD_COMMAND MAD_DEFINE MAD_EXPRESS MAD_LINE MAD_COFACT MAD_TELL

%token MAD_MARKER MAD_DRIFT MAD_SBEND MAD_RBEND MAD_QUADRUPOLE MAD_SEXTUPOLE MAD_OCTUPOLE

%token MAD_MULTIPOLE MAD_SOLENOID MAD_HKICKER MAD_VKICKER MAD_KICKER MAD_RFCAVITY MAD_ELSEPARATOR

%token MAD_HMONITOR MAD_VMONITOR MAD_HMON MAD_VMON MAD_MONITOR MAD_INSTRUMENT MAD_ECOLLIMATOR 

%token MAD_RCOLLIMATOR MAD_YROT MAD_SROT MAD_BEAMBEAM MAD_MATRIX MAD_LUMP

%token MAD_TYPE MAD_L MAD_ANGLE MAD_K1 MAD_E1 MAD_E2 MAD_FINT MAD_HGAP MAD_TILT MAD_K2

%token MAD_H1 MAD_H2 MAD_K3 MAD_LRAD MAD_K0L MAD_K1L MAD_K2L MAD_K3L MAD_K4L MAD_K5L MAD_K6L

%token MAD_K7L MAD_K8L MAD_K9L MAD_T0 MAD_T1 MAD_T2 MAD_T3 MAD_T4 MAD_T5 MAD_T6 MAD_T7

%token MAD_T8 MAD_T9 MAD_KS MAD_KICK MAD_HKICK MAD_VKICK MAD_VOLT MAD_LAG MAD_HARMON

%token MAD_BETRF MAD_PG MAD_SHUNT MAD_TFILL MAD_E MAD_XSIZE MAD_YSIZE MAD_SIGX MAD_SIGY

%token MAD_XMA MAD_YMA MAD_CHARGE MAD_RM MAD_TM MAD_ORDER MAD_SEQUENCE MAD_END_SEQUENCE MAD_LIST

%token MAD_REFER MAD_AT MAD_CENTRE MAD_ENTRY MAD_EXIT MAD_SEQEDIT MAD_ENDEDIT MAD_INSTALL

%token MAD_ELEMENT MAD_FROM MAD_REMOVE MAD_PATTERN MAD_MOVE MAD_BY MAD_TO MAD_REFLECT MAD_CYCLE

%token MAD_START MAD_USE MAD_BEAM MAD_RESBEAM MAD_PRINT MAD_SELECT MAD_SPLIT MAD_SURVEY MAD_TWISS

%token MAD_BETA0 MAD_SAVEBETA MAD_IBS MAD_EMIT MAD_EIGEN MAD_ENVELOPE MAD_TWISS3 MAD_SAVESIGMA 

%token MAD_DYNAMIC MAD_STATIC MAD_OPTICS MAD_PERIOD MAD_RANGE MAD_SYMM MAD_SUPER MAD_PARTICLE

%token MAD_POSITRON MAD_ELECTRON MAD_PROTON MAD_ANTI_PROTON MAD_MASS MAD_ENERGY MAD_PC

%token MAD_GAMMA MAD_EX MAD_EXN MAD_EY MAD_EYN MAD_ET MAD_SIGT MAD_SIGE MAD_KBUNCH MAD_NPART

%token MAD_BCURRENT MAD_BUNCHED MAD_RADIATE MAD_FULL MAD_CLEAR MAD_FLAG MAD_FRACTION MAD_X0

%token MAD_Y0 MAD_Z0 MAD_THETA0 MAD_PHI0 MAD_PSI0 MAD_TAPE MAD_DELTAP MAD_CHROM MAD_COUPLE

%token MAD_SAVE MAD_MUX MAD_MUY MAD_BETX MAD_ALFX MAD_BETY MAD_ALFY MAD_DX MAD_DPX MAD_DY

%token MAD_DPY MAD_X MAD_PX MAD_Y MAD_PY MAD_WX MAD_PHIX MAD_DMUX MAD_WY MAD_PHIY MAD_DMUY

%token MAD_DDX MAD_DDY MAD_DDPX MAD_DDPY MAD_T MAD_PT MAD_LABEL MAD_PLACE MAD_TABLE MAD_SIGMA0

%token MAD_DT MAD_DPT MAD_R21 MAD_R31 MAD_R41 MAD_R51 MAD_R61 MAD_SIGPX MAD_R32 MAD_R42 MAD_R52

%token MAD_R62 MAD_R43 MAD_R53 MAD_R63 MAD_SIGPY MAD_R54 MAD_R64 MAD_R65 MAD_SIGPT MAD_MAP

%token MAD_ORBIT MAD_A MAD_N MAD_RESONANCE MAD_EXPONENT MAD_HAMILTON MAD_INVARIANT MAD_FIXED

%token MAD_BETATRON MAD_NONLINEAR MAD_CONJUGATE MAD_COLUMNS

%token MAD_SQRT MAD_LOG MAD_EXP MAD_SIN MAD_COS MAD_TAN MAD_ASIN MAD_ABS MAD_MAX MAD_MIN

%token MAD_CALL MAD_FILENAME

%start mad_input_list

%left '+' '-'
%left '*' '/'
%left '^'

%%

/* Start */

mad_input_list			: mad_input
				| mad_input_list mad_input
				;

mad_input			: statement MAD_NEWLINE
				| statement excomment
				| excomment
				| MAD_NEWLINE
				;

excomment			: MAD_EXCOMMENT {
                                  if ( !madparser_comment_at_eof_get(mp) ) {
                                    comment_arr_add( $<sval>1, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_comment_arr(mp), madparser_comment_alloc(mp) );
                                  } else {
                                    comment_arr_add( $<sval>1, INT_MAX, madparser_current_filename(mp), madparser_local_linenum(mp), madparser_comment_arr(mp), madparser_comment_alloc(mp) );
                                  }

                                  free( $<sval>1 );
                                }
				;

/* Statement */

statement			: command
                                | comment
                                | definition
				;

comment                         : MAD_COMMENT { madparser_comment_mode_inc(mp); }
				| MAD_ENDCOMMENT
                                  { if (!madparser_comment_mode(mp) ) {
				    fprintf( stderr, "Error: an unmatched ENDCOMMENT.\n" );
				  } else {
				    madparser_comment_mode_dec(mp);
                                  }
				}
                                ;

command				: help
				| show
				| title
				| MAD_STOP { fprintf(stderr, "Program execution terminated.\n"); }
				| option
				| value
				| system
				| list
				| use
				| beam
				| resbeam
				;

/* Help */

help				: MAD_HELP { printf("List all valid keywords.\n"); }
				| MAD_HELP ',' keyword
				;

keyword				: MAD_HELP { printf("HELP ON HELP.\n"); }
				| MAD_SHOW { printf("HELP ON SHOW.\n"); }
				| MAD_TITLE { printf("HELP ON TITLE.\n"); }
				| MAD_STOP { printf("HELP ON STOP.\n"); }
				| MAD_OPTION { printf("HELP ON OPTION.\n"); }
				| MAD_SET { printf("HELP ON SET.\n"); }
				| MAD_VALUE { printf("HELP ON VALUE.\n"); }
				| MAD_SYSTEM { printf("HELP ON SYSTEM.\n"); }
				| MAD_MARKER { printf("HELP ON MARKER.\n"); }
				| MAD_DRIFT { printf("HELP ON DRIFT.\n"); }
				| MAD_SBEND { printf("HELP ON SBEND.\n"); }
				| MAD_RBEND { printf("HELP ON RBEND.\n"); }
				| MAD_QUADRUPOLE { printf("HELP ON QUADRUPOLE.\n"); }
				| MAD_SEXTUPOLE { printf("HELP ON SEXTUPOLE.\n"); }
				| MAD_OCTUPOLE { printf("HELP ON OCTUPOLE.\n"); }
				| MAD_MULTIPOLE { printf("HELP ON MULTIPOLE.\n"); }
				| MAD_SOLENOID { printf("HELP ON SOLENOID.\n"); }
				| MAD_HKICKER { printf("HELP ON HKICKER.\n"); }
				| MAD_VKICKER { printf("HELP ON VKICKER.\n"); }
				| MAD_KICKER { printf("HELP ON KICKER.\n"); }
				| MAD_RFCAVITY { printf("HELP ON RFCAVITY.\n"); }
				| MAD_ELSEPARATOR { printf("HELP ON ELSEPARATOR.\n"); }
				| MAD_HMONITOR { printf("HELP ON HMONITOR.\n"); }
				| MAD_VMONITOR { printf("HELP ON VMONITOR.\n"); }
				| MAD_MONITOR { printf("HELP ON MONITOR.\n"); }
				| MAD_INSTRUMENT { printf("HELP ON INSTRUMENT.\n"); }
				| MAD_ECOLLIMATOR { printf("HELP ON ECOLLIMATOR.\n"); }
				| MAD_RCOLLIMATOR { printf("HELP ON RCOLLIMATOR.\n"); }
				| MAD_YROT { printf("HELP ON YROT.\n"); }
				| MAD_SROT { printf("HELP ON SROT.\n"); }
				| MAD_BEAMBEAM { printf("HELP ON BEAMBEAM.\n"); }
				| MAD_MATRIX { printf("HELP ON MATRIX.\n"); }
				| MAD_LUMP { printf("HELP ON LUMP.\n"); }
				| MAD_COMMENT { printf("HELP ON COMMENT.\n"); }
				| MAD_ENDCOMMENT { printf("HELP ON ENDCOMMENT.\n"); }
				;

/* Show */

show				: MAD_SHOW { printf("SHOW ALL DEFINED NAMES.\n"); }
				| MAD_SHOW ',' identifier { printf("SHOW THE DEFINITION OF %s.\n",yylval.sval); }
				;

/* Title */

title				: MAD_TITLE ',' MAD_STRING_LITERAL { printf("Title: %s\n", $<sval>3); free( $<sval>3 ); }
				| MAD_TITLE ',' identifier { printf("Title: %s\n", $<sval>3); free( $<sval>3 ); }
				;

/* Option */

option				: MAD_OPTION ',' option_flags
				;

option_flags			: option_flag
				| option_flags ',' option_flag
				;

option_flag			: MAD_RESET { printf("RESET TRUE.\n"); }
				| '-' MAD_RESET { printf("RESET FALSE.\n"); }
				| MAD_INTER { printf("INTER TRUE.\n"); }
				| '-' MAD_INTER { printf("INTER FALSE.\n"); }
				| MAD_ECHO { printf("ECHO TRUE.\n"); }
				| '-' MAD_ECHO { printf("ECHO FALSE.\n"); }
				| MAD_TRACE { printf("TRACE TRUE.\n"); }
				| '-' MAD_TRACE { printf("TRACE FALSE.\n"); }
				| MAD_DOUBLE { printf("DOUBLE TRUE.\n"); }
				| '-' MAD_DOUBLE { printf("DOUBLE FALSE.\n"); }
				| MAD_VERIFY { printf("VERIFY TRUE.\n"); }
				| '-' MAD_VERIFY { printf("VERIFY FALSE.\n"); }
				| MAD_WARN { printf("WARN TRUE.\n"); }
				| '-' MAD_WARN { printf("WARN FALSE.\n"); }
				| MAD_INFO { printf("INFO TRUE.\n"); }
				| '-' MAD_INFO { printf("INFO FALSE.\n"); }
				| MAD_SYMPL { printf("SYMPL TRUE.\n"); }
				| '-' MAD_SYMPL { printf("SYMPL FALSE.\n"); }
				| MAD_DEBUG { printf("DEBUG TRUE.\n"); }
				| '-' MAD_DEBUG { printf("DEBUG FALSE.\n"); }
				| MAD_KEYWORD '=' MAD_NUMERIC_LITERAL { printf("KEYWORD=%s.\n", yylval.sval ); }
				| MAD_COMMAND '=' MAD_NUMERIC_LITERAL { printf("COMMAND=%s.\n", yylval.sval ); }
				| MAD_DEFINE '=' MAD_NUMERIC_LITERAL { printf("DEFINE=%s.\n",yylval.sval); }
				| MAD_EXPRESS '=' MAD_NUMERIC_LITERAL { printf("EXPRESS=%s.\n",yylval.sval); }
				| MAD_LINE '=' MAD_NUMERIC_LITERAL { printf("LINE=%s.\n",yylval.sval); }
				| MAD_COFACT { printf("COFACT IS NOW IGNORED.\n"); }
				| MAD_TELL { printf("LIST THE CURRENT SETTINGS.\n"); }
				;

/* DEFINITION */

definition			: const_definition
				| var_definition
				| marker_definition
				| drift_definition
				| sbend_definition
				| rbend_definition
				| quadrupole_definition
				| sextupole_definition
				| octupole_definition
				| multipole_definition
				| solenoid_definition
				| hkicker_definition
				| vkicker_definition
				| hkick_definition
				| vkick_definition
				| kicker_definition
				| rfcavity_definition
				| elseparator_definition
				| hmonitor_definition
				| vmonitor_definition
				| hmon_definition
				| vmon_definition
				| monitor_definition
				| instrument_definition
				| ecollimator_definition
				| rcollimator_definition
				| yrot_definition
				| srot_definition
				| beambeam_definition
				| matrix_definition
				| lump_definition
				| beam_line_definition
				;

/* Constants */

const_definition		: identifier ':' MAD_CONSTANT '=' identifier {
                                  if ( const_table_lookup( $<sval>1, madparser_const_table( mp ) ) == 0 ) {
                                    if ( var_table_lookup( $<sval>1, madparser_var_table( mp ) ) == 0 ) {
                                      constant* dst; constant* src;
                                      PRIVATE_ALLOCATE( dst, madparser_const_alloc( mp ) );
                                      src = (constant*)const_table_lookup( $<sval>5, madparser_const_table(mp) );
                                      
                                      if ( src != NULL ) {
                                        const_init_e( dst, $<sval>1, src, $<sval>5, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_expr_alloc(mp) );
                                      } else {
                                        fprintf( stderr, "parser error ! constant %s is undefined\n", $<sval>5 );
                                        const_init_a( dst, $<sval>1, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_expr_alloc(mp) );
                                        free( $<sval>5 );
                                      }
                                      
                                      const_table_add( $<sval>1, dst, madparser_const_table(mp) );
                                    } else {
                                      fprintf( stderr, "parser error ! %s is already defined as a variable, so can't define such constant\n", $<sval>1 );
                                    }
                                  } else {
                                    fprintf( stderr, "parser error ! constant %s is already defined\n", $<sval>1 );
                                  }
                                }
				| identifier ':' MAD_CONSTANT '=' const_value {
                                  if ( const_table_lookup( $<sval>1, madparser_const_table(mp) ) == 0 ) {
                                    if ( var_table_lookup( $<sval>1, madparser_var_table(mp) ) == 0 ) {
                                      constant* ptr;
                                      PRIVATE_ALLOCATE( ptr, madparser_const_alloc(mp) );
                                      
                                      if ( current_constant == CONSTANT_DOUBLE ) {
                                        const_init_d( ptr, $<sval>1, (GNode*)$<ptr>5, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                      } else {
                                        const_init_c( ptr, $<sval>1, $<sval>5, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_expr_alloc(mp) );
                                        free( $<sval>5 );
                                      }
                                      
                                      const_table_add( $<sval>1, ptr, madparser_const_table(mp) );
                                    } else {
                                      fprintf(stderr, "parser error ! %s is already defined as a variable, so can't define such constant\n", $<sval>1);
                                    }
                                  } else {
                                    fprintf(stderr, "parser error ! constant %s is already defined\n", $<sval>1);
                                  }
                                }
				;

const_value			: MAD_STRING_LITERAL
				| const_expression
				;

/* Variables */

var_definition			: identifier MAD_DEF_PARAM var_expression {
                                  if ( const_table_lookup( $<sval>1, madparser_const_table(mp) ) == 0 ) {
                                    variable* dst;
                                    
                                    variable* ptr = (variable*)var_table_lookup( $<sval>1, madparser_var_table(mp) );
                                    if ( ptr != NULL ) {
                                      var_delete( ptr, madparser_expr_alloc(mp) );
                                      PRIVATE_DEALLOCATE( ptr, madparser_var_alloc(mp) );
                                    }
                                    
                                    dst = var_alloc_init( $<sval>1, (GNode*)$<ptr>3, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_var_alloc(mp) );
                                    assert( dst != NULL );
                                    var_table_add( $<sval>1, dst, madparser_var_table(mp) );
                                  } else {
                                    fprintf(stderr, "parser error ! %s is already defined as a constant\n", $<sval>1);
                                  }
                                }
				| identifier '=' var_expression {
                                  if ( const_table_lookup( $<sval>1, madparser_const_table(mp) ) == 0 ) {
                                    variable* dst;
                                    variable* ptr = (variable*)var_table_lookup( $<sval>1, madparser_var_table(mp) );
                                    if ( ptr != NULL ) {
                                      var_delete( ptr, madparser_expr_alloc(mp) );
                                      PRIVATE_DEALLOCATE( ptr, madparser_var_alloc(mp) );
                                    }
                                    
                                    dst = var_alloc_init( $<sval>1, (GNode*)$<ptr>3, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_var_alloc(mp) );
                                    assert( dst != NULL );
                                    var_table_add( $<sval>1, dst, madparser_var_table(mp) );
                                  } else {
                                    fprintf(stderr, "parser error ! %s is already defined as a constant\n", $<sval>1);
                                  }
                                }
                                | MAD_SET ',' identifier ',' var_expression {
                                  if ( const_table_lookup( $<sval>3, madparser_const_table(mp) ) == 0 ) {
                                    variable* dst;
                                    variable* ptr = (variable*)var_table_lookup( $<sval>3, madparser_var_table(mp) );
                                    double val = expr_evaluate( (GNode*)$<ptr>5, madparser_var_table(mp), madparser_bel_table(mp) );
                                    
                                    expr_struct* data;
                                    PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                    data->kind_ = NUMBER_EXPR;
                                    data->dvalue_ = val;
                                    data->svalue_ = (char*)malloc( 16 );
                                    sprintf(data->svalue_, "%e", val);
                                    expr_node_delete( (GNode*)$<ptr>5, madparser_expr_alloc(mp) );
                                    
                                    if ( ptr != 0 ) {
                                      var_delete( ptr, madparser_expr_alloc(mp) );
                                      PRIVATE_DEALLOCATE( ptr, madparser_var_alloc(mp) );
                                    }

                                    dst = var_alloc_init( $<sval>3, g_node_new( data), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp), madparser_var_alloc(mp) );
                                    assert( dst != NULL );                                    
                                    var_table_add( $<sval>3, dst, madparser_var_table(mp) );
                                  } else {
                                    fprintf(stderr, "parser error ! %s is already defined as a constant\n", $<sval>3);
                                  }
                                }
				;

/* Beam Elements */

marker_definition		: identifier ':' MAD_MARKER marker_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_MARKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                }
				;

marker_attrs			: /* Empty */
				| marker_attrs ',' marker_attr
				;

marker_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3); /* strcpy( curr_bel_type, $<sval>3 ); */
                                  free( $<sval>3 );
                                }
				;

drift_definition		: identifier ':' MAD_DRIFT drift_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_DRIFT );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                  madparser_new_bel(mp);
                                }
				;

drift_attrs			: /* Empty */
				| drift_attrs ',' drift_attr
				;

drift_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type( mp, $<sval>3 );
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				;

sbend_definition		: identifier ':' MAD_SBEND sbend_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_SBEND );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                  madparser_new_bel(mp);
                                }
				;

sbend_attrs			: /* Empty */
				| sbend_attrs ',' sbend_attr
				;

sbend_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_ANGLE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_ANGLE, (GNode*)$<ptr>3 );
                                }
				| MAD_K1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_K1, (GNode*)$<ptr>3 );
                                }
				| MAD_E1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_E1, (GNode*)$<ptr>3 );
                                }
				| MAD_E2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_E2, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT {
                                  GNode* expr = expr_node_init( M_PI_2, "1.57079632679489661923", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_TILT, expr );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_TILT, (GNode*)$<ptr>3 );
                                }
				| MAD_K2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_K2, (GNode*)$<ptr>3 );
                                }
				| MAD_H1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_H1, (GNode*)$<ptr>3 );
                                }
				| MAD_H2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_H2, (GNode*)$<ptr>3 );
                                }
				| MAD_HGAP '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_HGAP, (GNode*)$<ptr>3 );
                                }
				| MAD_FINT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_FINT, (GNode*)$<ptr>3 );
                                }
				| MAD_K3 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SBEND_K3, (GNode*)$<ptr>3 );
                                }
				;

rbend_definition		: identifier ':' MAD_RBEND rbend_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_RBEND );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                  madparser_new_bel(mp);
                                }
				;

rbend_attrs			: /* Empty */
				| rbend_attrs ',' rbend_attr
				;

rbend_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3 );
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_ANGLE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_ANGLE, (GNode*)$<ptr>3 );
                                }
				| MAD_K1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_K1, (GNode*)$<ptr>3 );
                                }
				| MAD_E1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_E1, (GNode*)$<ptr>3 );
                                }
				| MAD_E2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_E2, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT {
                                  GNode* expr = expr_node_init( M_PI_2, "1.57079632679489661923", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_TILT, expr );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_TILT, (GNode*)$<ptr>3 );
                                }
				| MAD_K2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_K2, (GNode*)$<ptr>3 );
                                }
				| MAD_H1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_H1, (GNode*)$<ptr>3 );
                                }
				| MAD_H2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_H2, (GNode*)$<ptr>3 );
                                }
				| MAD_HGAP '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_HGAP, (GNode*)$<ptr>3 );
                                }
				| MAD_FINT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_FINT, (GNode*)$<ptr>3 );
                                }
				| MAD_K3 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RBEND_K3, (GNode*)$<ptr>3 );
                                }
				;

quadrupole_definition		: identifier ':' MAD_QUADRUPOLE quadrupole_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_QUADRUPOLE );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                  madparser_new_bel(mp);
                                }
                                ;

quadrupole_attrs		: /* Empty */
				| quadrupole_attrs ',' quadrupole_attr
				;

quadrupole_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_K1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_QUADRUPOLE_K1, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT {
                                  GNode* expr = expr_node_init( M_PI_4, "0.78539816339744830962", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_QUADRUPOLE_TILT, expr );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_QUADRUPOLE_TILT, (GNode*)$<ptr>3 );
                                }
				;

sextupole_definition		: identifier ':' MAD_SEXTUPOLE sextupole_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_SEXTUPOLE );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                }
				;

sextupole_attrs			: /* Empty */
				| sextupole_attrs ',' sextupole_attr
				;

sextupole_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_K2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SEXTUPOLE_K2, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT {
                                  GNode* expr = expr_node_init( M_PI/6.0, "0.52359877559829887308", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SEXTUPOLE_TILT, expr );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SEXTUPOLE_TILT, (GNode*)$<ptr>3 );
                                }
				;

octupole_definition		: identifier ':' MAD_OCTUPOLE octupole_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_OCTUPOLE );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

octupole_attrs			: /* Empty */
				| octupole_attrs ',' octupole_attr
				;

octupole_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_K3 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_OCTUPOLE_K3, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT {
                                  GNode* expr = expr_node_init( M_PI/8.0, "0.39269908169872415481", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_OCTUPOLE_TILT, expr );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_OCTUPOLE_TILT, (GNode*)$<ptr>3 );
                                }
				;

multipole_definition		: identifier ':' MAD_MULTIPOLE multipole_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_MULTIPOLE );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

multipole_attrs			: /* Empty */
				| multipole_attrs ',' multipole_attr
				;

multipole_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_LRAD '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_K0L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K0L, (GNode*)$<ptr>3 );
                                }
				| MAD_K1L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K1L, (GNode*)$<ptr>3 );
                                }
				| MAD_K2L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K2L, (GNode*)$<ptr>3 );
                                }
				| MAD_K3L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K3L, (GNode*)$<ptr>3 );
                                }
				| MAD_K4L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K4L, (GNode*)$<ptr>3 );
                                }
				| MAD_K5L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K5L, (GNode*)$<ptr>3 );
                                }
				| MAD_K6L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K6L, (GNode*)$<ptr>3 );
                                }
				| MAD_K7L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K7L, (GNode*)$<ptr>3 );
                                }
				| MAD_K8L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K8L, (GNode*)$<ptr>3 );
                                }
				| MAD_K9L '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_K9L, (GNode*)$<ptr>3 );
                                }
				| MAD_T0 {
                                  GNode* expr = expr_node_init( M_PI/2.0, "1.57079632679489661923", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T0, expr );
                                }
				| MAD_T1 {
                                  GNode* expr = expr_node_init( M_PI/4.0, "0.78539816339744830962", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T1, expr );
                                }
				| MAD_T2 {
                                  GNode* expr = expr_node_init( M_PI/6.0, "0.52359877559829887308", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T2, expr );
                                }
				| MAD_T3 {
                                  GNode* expr = expr_node_init( M_PI/8.0, "0.39269908169872415481", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T3, expr );
                                }
				| MAD_T4 {
                                  GNode* expr = expr_node_init( M_PI/10.0, "0.31415926535897932385", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T4, expr );
                                }
				| MAD_T5 {
                                  GNode* expr = expr_node_init( M_PI/12.0, "0.26179938779914943654", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T5, expr );
                                }
				| MAD_T6 {
                                  GNode* expr = expr_node_init( M_PI/14.0, "0.22439947525641380275", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T6, expr );
                                }
				| MAD_T7 {
                                  GNode* expr = expr_node_init( M_PI/16.0, "0.19634954084936207740", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T7, expr );
                                }
				| MAD_T8 {
                                  GNode* expr = expr_node_init( M_PI/18.0, "0.17453292519943295769", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T8, expr );
                                }
				| MAD_T9 {
                                  GNode* expr = expr_node_init( M_PI/20.0, "0.15707963267948966192", madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T9, expr );
                                }
				| MAD_T0 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T0, (GNode*)$<ptr>3 );
                                }
				| MAD_T1 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T1, (GNode*)$<ptr>3 );
                                }
				| MAD_T2 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T2, (GNode*)$<ptr>3 );
                                }
                                | MAD_T3 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T3, (GNode*)$<ptr>3 );
                                }
				| MAD_T4 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T4, (GNode*)$<ptr>3 );
                                }
				| MAD_T5 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T5, (GNode*)$<ptr>3 );
                                }
				| MAD_T6 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T6, (GNode*)$<ptr>3 );
                                }
				| MAD_T7 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T7, (GNode*)$<ptr>3 );
                                }
				| MAD_T8 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T8, (GNode*)$<ptr>3 );
                                }
				| MAD_T9 '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_MULTIPOLE_T9, (GNode*)$<ptr>3 );
                                }
				;

solenoid_definition		: identifier ':' MAD_SOLENOID solenoid_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_SOLENOID );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                     /*
                                  curr_bel = beam_element_init( madparser_expr_alloc(mp), madparser_bel_alloc(mp) );
                                  assert( curr_bel != NULL );
                                  curr_bel_type[0] = '\0';
                                     */
                                  madparser_new_bel(mp);
                                }
				;

solenoid_attrs			: /* Empty */
                                | solenoid_attrs ',' solenoid_attr
				;

solenoid_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_KS '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SOLENOID_KS, (GNode*)$<ptr>3 );
                                }
				;

hkicker_definition		: identifier ':' MAD_HKICKER hkicker_attrs { 
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_HKICKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
                                ;

hkick_definition		: identifier ':' MAD_HKICK hkicker_attrs { 
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_HKICKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;


hkicker_attrs			: /* Empty */
				| hkicker_attrs ',' hkicker_attr
				;

hkicker_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp, $<sval>3 );
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_KICK '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_HKICKER_KICK, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_HKICKER_TILT, (GNode*)$<ptr>3 );
                                }
				;

vkicker_definition		: identifier ':' MAD_VKICKER vkicker_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_VKICKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

vkick_definition		: identifier ':' MAD_VKICK vkicker_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_VKICKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

vkicker_attrs			: /* Empty */
				| vkicker_attrs ',' vkicker_attr
				;

vkicker_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_KICK '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_VKICKER_KICK, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_VKICKER_TILT, (GNode*)$<ptr>3 );
                                }
				;

kicker_definition		: identifier ':' MAD_KICKER kicker_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL )
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_KICKER );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

kicker_attrs			: /* Empty */
				| kicker_attrs ',' kicker_attr
				;

kicker_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_HKICK '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_KICKER_HKICK, (GNode*)$<ptr>3 );
                                }
				| MAD_VKICK '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_KICKER_VKICK, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_KICKER_TILT, (GNode*)$<ptr>3 );
                                }
				;

rfcavity_definition		: identifier ':' MAD_RFCAVITY rfcavity_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_RFCAVITY );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

rfcavity_attrs			: /* Empty */
				| rfcavity_attrs ',' rfcavity_attr
				;

rfcavity_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_VOLT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_VOLT, (GNode*)$<ptr>3 );
                                }
				| MAD_LAG '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_LAG, (GNode*)$<ptr>3 );
                                }
				| MAD_HARMON '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_HARMON, (GNode*)$<ptr>3 );
                                }
				| MAD_BETRF '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_BETRF, (GNode*)$<ptr>3 );
                                }
				| MAD_PG '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_PG, (GNode*)$<ptr>3 );
                                }
				| MAD_SHUNT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_SHUNT, (GNode*)$<ptr>3 );
                                }
				| MAD_TFILL '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RFCAVITY_TFILL, (GNode*)$<ptr>3 );
                                }
				;

elseparator_definition		: identifier ':' MAD_ELSEPARATOR elseparator_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_ELSEPARATOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

elseparator_attrs		: /* Empty */
				| elseparator_attrs ',' elseparator_attr
				;

elseparator_attr		: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_E '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_ELSEPARATOR_E, (GNode*)$<ptr>3 );
                                }
				| MAD_TILT '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_ELSEPARATOR_TILT, (GNode*)$<ptr>3 );
                                }
				;

hmonitor_definition		: identifier ':' MAD_HMONITOR hmonitor_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_HMONITOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

hmon_definition		        : identifier ':' MAD_HMON hmonitor_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_HMONITOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
                                ;

hmonitor_attrs			: /* Empty */
				| hmonitor_attrs ',' hmonitor_attr
				;

hmonitor_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				;

vmonitor_definition		: identifier ':' MAD_VMONITOR vmonitor_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_VMONITOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

vmon_definition		: identifier ':' MAD_VMON vmonitor_attrs {
                          beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                          if ( ptr != NULL ) {
                            beam_element_delete( ptr, madparser_bel_alloc(mp) );
                          }
                          
                          beam_element_set_kind( madparser_current_bel(mp), BEL_VMONITOR );
                          beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                          beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                          bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                          madparser_new_bel(mp);
                        }
                        ;

vmonitor_attrs			: /* Empty */
				| vmonitor_attrs ',' vmonitor_attr
				;

vmonitor_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				;

monitor_definition		: identifier ':' MAD_MONITOR monitor_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_MONITOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

monitor_attrs			: /* Empty */
				| monitor_attrs ',' monitor_attr
				;

monitor_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				;

instrument_definition		: identifier ':' MAD_INSTRUMENT instrument_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_INSTRUMENT );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

instrument_attrs		: /* Empty */
				| instrument_attrs ',' instrument_attr
				;

instrument_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				;

ecollimator_definition		: identifier ':' MAD_ECOLLIMATOR ecollimator_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_ECOLLIMATOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

ecollimator_attrs		: /* Empty */
				| ecollimator_attrs ',' ecollimator_attr
				;

ecollimator_attr		: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_XSIZE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_ECOLLIMATOR_XSIZE, (GNode*)$<ptr>3 );
                                }
				| MAD_YSIZE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_ECOLLIMATOR_YSIZE, (GNode*)$<ptr>3 );
                                }
				;

rcollimator_definition		: identifier ':' MAD_RCOLLIMATOR rcollimator_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }

                                  beam_element_set_kind( madparser_current_bel(mp), BEL_RCOLLIMATOR );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

rcollimator_attrs		: /* Empty */
				| rcollimator_attrs ',' rcollimator_attr
				;

rcollimator_attr		: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_L '=' var_expression {
                                  beam_element_set_length( madparser_current_bel(mp), (GNode*)$<ptr>3, madparser_expr_alloc(mp) );
                                }
				| MAD_XSIZE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RCOLLIMATOR_XSIZE, (GNode*)$<ptr>3 );
                                }
				| MAD_YSIZE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_RCOLLIMATOR_YSIZE, (GNode*)$<ptr>3 );
                                }
				;

yrot_definition			: identifier ':' MAD_YROT yrot_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_YROT );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

yrot_attrs			: /* Empty */
				| yrot_attrs ',' yrot_attr
				;

yrot_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_ANGLE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_YROT_ANGLE, (GNode*)$<ptr>3 );
                                }
				;

srot_definition			: identifier ':' MAD_SROT srot_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_SROT );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

srot_attrs			: /* Empty */
				| srot_attrs ',' srot_attr
				;

srot_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_ANGLE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_SROT_ANGLE, (GNode*)$<ptr>3 );
                                }
				;

beambeam_definition		: identifier ':' MAD_BEAMBEAM beambeam_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_BEAMBEAM );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

beambeam_attrs			: /* Empty */
				| beambeam_attrs ',' beambeam_attr
				;

beambeam_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 ); 
                                }
				| MAD_SIGX '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_SIGX, (GNode*)$<ptr>3 );
                                }
				| MAD_SIGY '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_SIGY, (GNode*)$<ptr>3 );
                                }
				| MAD_XMA '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_XMA, (GNode*)$<ptr>3 );
                                }
				| MAD_YMA '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_YMA, (GNode*)$<ptr>3 );
                                }
				| MAD_CHARGE {
  /* beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_CHARGE, -1.0 ); */
                                }
				| MAD_CHARGE '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_BEAMBEAM_CHARGE, (GNode*)$<ptr>3 );
                                }
				;

matrix_definition		: identifier ':' MAD_MATRIX matrix_attrs {
  
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );

                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_MATRIX );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );

                                  madparser_new_bel(mp);

                                  if ( (madparser_current_bel(mp))->udata_ == NULL ) {
                                    matrix* mtx = matrix_init( madparser_matrix_alloc(mp) );
                                    assert( mtx != NULL );
                                    matrix_set_default( mtx, madparser_expr_alloc(mp) );
                                    
                                    beam_element_set_udata( madparser_current_bel(mp), mtx );
                                  }
                                }
				;

matrix_attrs			: /* Empty */
				| matrix_attrs ',' matrix_attr
				;

matrix_attr			: MAD_TYPE '=' identifier {
                                    madparser_set_bel_type(mp,$<sval>3);
                                    free( $<sval>3 );
                                }
				| MAD_RM '(' MAD_NUMERIC_LITERAL ',' MAD_NUMERIC_LITERAL ')' '=' var_expression {
                                  int i, j;
                                  matrix* mtx = NULL;
                                  if ( madparser_current_bel(mp)->udata_ == NULL ) {
                                    mtx = matrix_init( madparser_matrix_alloc(mp) );
                                    assert( mtx != NULL );
                                    matrix_set_default( mtx, madparser_expr_alloc(mp) );
                                    
                                    beam_element_set_udata( madparser_current_bel(mp), mtx );
                                  } else {
                                    mtx = ((matrix*)((madparser_current_bel(mp))->udata_));
                                  }                                  
                                  i = (int)($<dval>3+1.0e-6);
                                  j = (int)($<dval>5+1.0e-6);
                                  mtx->rm_[i][j]=(GNode*)$<ptr>8;
                                }
				| MAD_TM '(' MAD_NUMERIC_LITERAL ',' MAD_NUMERIC_LITERAL ',' MAD_NUMERIC_LITERAL ')' '=' var_expression {
                                  int i, j, k;
                                  matrix* mtx = NULL;
                                  if ( (madparser_current_bel(mp))->udata_ == NULL ) {
                                    mtx = matrix_init( madparser_matrix_alloc(mp) );
                                    assert( mtx != NULL );
                                    matrix_set_default( mtx, madparser_expr_alloc(mp) );
                                    
                                    beam_element_set_udata( madparser_current_bel(mp), mtx );
                                  } else {
                                    mtx = ((matrix*)((madparser_current_bel(mp))->udata_));
                                    }
                                  
                                  i = (int)($<dval>3+1.0e-6);
                                  j = (int)($<dval>5+1.0e-6);
                                  k = (int)($<dval>7+1.0e-6);
                                  mtx->tm_[i][j][k]=(GNode*)$<ptr>10;
                                }
				;

lump_definition			: identifier ':' MAD_LUMP lump_attrs {
                                  beam_element* ptr = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_element_delete( ptr, madparser_bel_alloc(mp) );
                                  }
                                  
                                  beam_element_set_kind( madparser_current_bel(mp), BEL_LUMP );
                                  beam_element_set_name( madparser_current_bel(mp), $<sval>1, madparser_current_bel_type(mp) );
                                  beam_element_set_linenum( madparser_current_bel(mp), madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bel_table_add( $<sval>1, madparser_current_bel(mp), madparser_bel_table(mp) );
                                  madparser_new_bel(mp);
                                }
				;

lump_attrs			: /* Empty */
				| lump_attrs ',' lump_attr
				;

lump_attr			: MAD_TYPE '=' identifier {
                                  madparser_set_bel_type(mp,$<sval>3);
                                  free( $<sval>3 );
                                }
				| MAD_ORDER '=' var_expression {
                                  beam_element_set_param( madparser_current_bel(mp), BEL_LUMP_ORDER, (GNode*)$<ptr>3 );
                                }
				| MAD_LINE '=' identifier {
                                }
				| MAD_LINE '=' '(' beam_line_elements ')' {
                                }
				;

/* Beam Line */

beam_line_definition		: identifier ':' MAD_LINE '=' '(' beam_line_elements ')' {
                                  beam_line* ptr = (beam_line*)bml_table_lookup( $<sval>1, madparser_bml_table(mp) );
                                  if ( ptr != NULL ) {
                                    beam_line_delete( ptr, madparser_bml_alloc(mp) );
                                  }
                                  beam_line_set_name( (beam_line*)$<ptr>6, $<sval>1 );
                                  beam_line_set_linenum( (beam_line*)$<ptr>6, madparser_linenum(mp), madparser_current_filename(mp), madparser_local_linenum(mp) );
                                  bml_table_add( $<sval>1, (beam_line*)$<ptr>6, madparser_bml_table(mp) );
                                }
                                ;

beam_line_elements		: beam_line_element
				| beam_line_elements ',' beam_line_element {
                                  beam_line* bml = (beam_line*)$<ptr>1;
                                  beam_line_add_bml( bml, (beam_line*)$<ptr>3 );
                                  beam_line_delete( (beam_line*)$<ptr>3, madparser_bml_alloc(mp) );
                                  $<ptr>$ = bml;
                                }
				| beam_line_elements beam_line_element {
                                  beam_line* bml = (beam_line*)$<ptr>1;
                                  beam_line_add_bml( bml, (beam_line*)$<ptr>3 );
                                  beam_line_delete( (beam_line*)$<ptr>3, madparser_bml_alloc(mp) );
                                  $<ptr>$ = bml;
                                }
				;

beam_line_element		: identifier {
                                  beam_line* bml = beam_line_init( madparser_bml_alloc(mp) );
                                  assert( bml != NULL );
                                  beam_line_add_bel( bml, $<sval>1, madparser_bel_table(mp), madparser_bml_table(mp) );
                                  free( $<sval>1 );
                                  $<ptr>$ = bml;
                                }
				| '(' beam_line_elements ')' {
                                  $<ptr>$ = $<ptr>2;
                                }
				| MAD_NUMERIC_LITERAL '*' beam_line_element {
                                  int i;
                                  int n = (int)atof( $<sval>1 );
                                  beam_line* bml = beam_line_init( madparser_bml_alloc(mp) );
                                  assert( bml != NULL );
                                  for ( i = 0; i < n; ++i ) {
                                    beam_line_add_bml( bml, (beam_line*)$<ptr>3 );
                                  }
                                  beam_line_delete( (beam_line*)$<ptr>3, madparser_bml_alloc(mp) );
                                  free( $<sval>1 );
                                  $<ptr>$ = bml;
                                }
				| '-' beam_line_element {
                                  GList* list = ((beam_line*)$<ptr>2)->bel_list_;
                                  GList* last = NULL;
                                  beam_line* bml = beam_line_init( madparser_bml_alloc(mp) );
                                  assert( bml != NULL );

                                  while (list) {
                                    if ( *((char*)(list->data)) == '+' ) {
                                      *((char*)(list->data)) = '-';
                                    } else {
                                      *((char*)(list->data)) = '+';
                                    }
                                    last = list;
                                    list = last->next;
                                    last->next = last->prev;
                                    last->prev = list;
                                  }
                                  bml->bel_list_ = last;
                                  $<ptr>$ = bml;
                                }
				;

/* Expressions */

const_expression		: const_expr_unit
				| '(' const_expression ')' {
  #if 0
                                  $<ptr>$ = $<ptr>2;
  #else
                                  expr_struct* data;
                                  GNode*       parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_   = BRACKETS_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>2))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;                                  
  #endif                                 
                                }
				| '+' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_   = UN_PLUS_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>2))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;
                                }
				| '-' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_   = UN_MINUS_EXPR;
                                  data->dvalue_ = -((expr_struct*)(((GNode*)($<ptr>2))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;
                                }
				| const_expression '+' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ADD_OP_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>1))->data))->dvalue_ + ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| const_expression '-' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SUB_OP_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>1))->data))->dvalue_ - ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| const_expression '*' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MUL_OP_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>1))->data))->dvalue_ * ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| const_expression '/' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = DIV_OP_EXPR;
                                  data->dvalue_ = ((expr_struct*)(((GNode*)($<ptr>1))->data))->dvalue_ / ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| const_expression '^' const_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = POW_OP_EXPR;
                                  data->dvalue_ = pow( ((expr_struct*)(((GNode*)($<ptr>1))->data))->dvalue_, ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| MAD_SQRT '(' const_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SQRT_EXPR;
                                  data->dvalue_ = sqrt( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_LOG '(' const_expression ')' {
                                  expr_struct* data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = LOG_EXPR;
                                  data->dvalue_ = log( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_EXP '(' const_expression ')' {
                                  expr_struct* data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = EXP_EXPR;
                                  data->dvalue_ = exp( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_SIN '(' const_expression ')' {
                                  expr_struct* data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SIN_EXPR;
                                  data->dvalue_ = sin( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_COS '(' const_expression ')' {
                                  expr_struct* data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = COS_EXPR;
                                  data->dvalue_ = cos( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_TAN '(' const_expression ')' {
                                  expr_struct* data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = TAN_EXPR;
                                  data->dvalue_ = tan( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
                                | MAD_ASIN '(' const_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ASIN_EXPR;
                                  data->dvalue_ = asin( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_ABS '(' const_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;                                  
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ABS_EXPR;
                                  data->dvalue_ = fabs( ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_ );
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_MAX '(' const_expression ',' const_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  double val1 = ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  double val2 = ((expr_struct*)(((GNode*)($<ptr>5))->data))->dvalue_;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MAX_EXPR;
                                  data->dvalue_ = ( val1 > val2 ) ? val1 : val2;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>5 );
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_MIN '(' const_expression ',' const_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  double val1 = ((expr_struct*)(((GNode*)($<ptr>3))->data))->dvalue_;
                                  double val2 = ((expr_struct*)(((GNode*)($<ptr>5))->data))->dvalue_;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MIN_EXPR;
                                  data->dvalue_ = ( val1 < val2 ) ? val1 : val2;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>5 );
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				;

const_expr_unit			: MAD_NUMERIC_LITERAL {
                                  expr_struct* data;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  data->kind_ = NUMBER_EXPR;
                                  data->dvalue_ = atof( $<sval>1 );
                                  data->svalue_ = $<sval>1;
                                  $<ptr>$ = g_node_new( data );
                                }
                                | identifier {
                                  expr_struct *data;
                                  constant *ptr;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  ptr = (constant*)const_table_lookup( $<sval>1, madparser_const_table(mp) );
                                  if ( ptr != 0 ) {
                                    if ( ptr->svalue_ != NULL ) {
                                      fprintf(stderr, "fatal parser error ! string constant %s used in an algebraic expression\n", $<sval>1);
                                      bmlfactory_exit();
                                    }
                                    
                                    data->kind_ = NUM_IDENT_EXPR;
                                    data->dvalue_ = ((expr_struct*)(((GNode*)(ptr->expr_))->data))->dvalue_;
                                    data->svalue_ = $<sval>1;
                                  } else {
                                    fprintf(stderr, "parser error ! constant %s is undefined\n", $<sval>1);
                                    data->kind_ = NUMBER_EXPR;
                                    data->dvalue_ = 0.0;
                                    data->svalue_ = (char*)malloc( 3 );
                                    strcpy( data->svalue_, "0." );
                                    free( $<sval>1 );
                                  }
                                  
                                  current_constant = CONSTANT_DOUBLE;
                                  $<ptr>$ = g_node_new( data );
                                }
				;

var_expression			: var_expr_unit
				| '(' var_expression ')' {
     #if 0
                                  $<ptr>$ = $<ptr>2;
     #else
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = BRACKETS_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;
     #endif
                                }
				| '+' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = UN_PLUS_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;
                                }
				| '-' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = UN_MINUS_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>2 );
                                  $<ptr>$ = parent;
                                }
                                | var_expression '+' var_expression {
                                  expr_struct* data;
                                  GNode* parent;                                  
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ADD_OP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| var_expression '-' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SUB_OP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| var_expression '*' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MUL_OP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| var_expression '/' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = DIV_OP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| var_expression '^' var_expression {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = POW_OP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  g_node_prepend( parent, (GNode*)$<ptr>1 );
                                  $<ptr>$ = parent;
                                }
				| MAD_SQRT '(' var_expression ')' {
                                  expr_struct *data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SQRT_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_LOG '(' var_expression ')' {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = LOG_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_EXP '(' var_expression ')' {
                                  expr_struct* data;
                                  GNode* parent;                                  
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = EXP_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_SIN '(' var_expression ')' {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = SIN_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_COS '(' var_expression ')' {
                                  expr_struct* data;
                                  GNode* parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = COS_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_TAN '(' var_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = TAN_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_ASIN '(' var_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ASIN_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_ABS '(' var_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = ABS_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_MAX '(' var_expression ',' var_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MAX_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>5 );
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				| MAD_MIN '(' var_expression ',' var_expression ')' {
                                  expr_struct *data;
                                  GNode *parent;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  parent = g_node_new( data );
                                  data->kind_ = MIN_EXPR;
                                  data->svalue_ = NULL;
                                  g_node_prepend( parent, (GNode*)$<ptr>5 );
                                  g_node_prepend( parent, (GNode*)$<ptr>3 );
                                  $<ptr>$ = parent;
                                }
				;

var_expr_unit			: MAD_NUMERIC_LITERAL {
                                  GNode* expr = expr_node_init( atof( $<sval>1 ), $<sval>1, madparser_expr_alloc(mp) );
                                  assert( expr != NULL );
                                  free( $<sval>1 );
                                  $<ptr>$ = expr;
                                }
				| MAD_STRING_LITERAL {
                                  expr_struct* data;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  data->kind_   = STRING_EXPR;
                                  data->svalue_ = $<sval>1;
                                  $<ptr>$ = g_node_new( data );
                                }
				| identifier {
                                  expr_struct* data;
                                  constant* src1;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  src1 = (constant*)const_table_lookup( $<sval>1, madparser_const_table(mp) );
                                  
                                  if ( src1 != 0 ) {
                                    if ( src1->svalue_ != NULL ) {
                                      data->kind_ = STR_IDENT_EXPR;
                                      data->svalue_ = $<sval>1;
                                    } else {
                                      data->kind_ = NUM_IDENT_EXPR;
                                      data->dvalue_ = ((expr_struct*)(((GNode*)(src1->expr_))->data))->dvalue_;
                                      data->svalue_ = $<sval>1;
                                    }
                                  } else {
                                    variable* src2 = (variable*)var_table_lookup( $<sval>1, madparser_var_table(mp) );
                                    if ( src2 == 0 ) {
                                      fprintf( stderr, "warning ! %s is not yet defined\n", $<sval>1);
                                    }
                                    data->kind_ = VAR_IDENT_EXPR;
                                    data->svalue_ = $<sval>1;
                                  }
                                  $<ptr>$ = g_node_new( data );
                                }
				| identifier '[' MAD_L ']' {
                                  expr_struct* data;
                                  beam_element* bel;
                                  PRIVATE_ALLOCATE( data, madparser_expr_alloc(mp) );
                                  bel = (beam_element*)bel_table_lookup( $<sval>1, madparser_bel_table(mp) );
                                  if ( bel == 0 ) {
                                    fprintf( stderr, "warning ! %s is not yet defined\n", $<sval>1);
                                  }
                                  data->kind_ = BEAMEL_LEN_EXPR;
                                  data->svalue_ = $<sval>1;
                                  
                                  $<ptr>$ = g_node_new( data );
                                }
				;

/* Value */

value				: MAD_VALUE ',' var_expression { printf("An expression has been evaluated with VALUE.\n"); }
				;

/* System */

system				: MAD_SYSTEM ',' MAD_STRING_LITERAL { printf("Execute a system command %s.\n",yylval.sval); }
				;

/* List */

list				: MAD_LIST '=' list_expression
				;

list_expression			: '(' list_members ')'
				;

list_members			: list_member
				| list_members ',' list_member
				;

list_member			: identifier { printf("LIST with member %s.\n",yylval.sval); }
				| list_expression
				;

/* Use */

use				: MAD_USE ',' use_attrs
				;

use_attrs			: use_attr
				| use_attrs ',' use_attr
				;

use_attr			: identifier { printf("USE %s.\n",yylval.sval); }
				| MAD_PERIOD '=' identifier { printf("USE with period line=%s.\n",yylval.sval); }
				| MAD_PERIOD '=' '(' beam_line_elements ')' { printf("USE with period.\n"); }
				| MAD_RANGE '=' range_expr { printf("USE with range.\n"); }
				| MAD_SYMM { printf("USE with SYMM true.\n"); }
				| '-' MAD_SYMM { printf("USE with SYMM false.\n"); }
				| MAD_SUPER '=' MAD_NUMERIC_LITERAL { printf("USE with super=%s.\n",yylval.sval); }
				;

range_expr			: element_occurence '/' element_occurence
				| element_occurence
				;

element_occurence		: '#' identifier
				| identifier '[' MAD_NUMERIC_LITERAL ']'
				| identifier
				;

/* Beam */

beam				: MAD_BEAM ',' beam_attrs
				;

beam_attrs			: beam_attr
				| beam_attrs ',' beam_attr
				;

beam_attr			: MAD_PARTICLE '=' particle_name
				| MAD_MASS '=' const_expression { printf("BEAM with particle mass=expr.\n"); }
				| MAD_CHARGE '=' const_expression { printf("BEAM with particle charge=expr.\n"); }
				| MAD_ENERGY '=' const_expression { printf("BEAM with energy=expr.\n"); }
				| MAD_PC '=' const_expression { printf("BEAM with pc=expr.\n"); }
				| MAD_GAMMA '=' const_expression { printf("BEAM with gamma=expr.\n"); }
				| MAD_EX '=' const_expression { printf("BEAM with ex=expr.\n"); }
				| MAD_EXN '=' const_expression { printf("BEAM with exn=expr.\n"); }
				| MAD_EY '=' const_expression { printf("BEAM with ey=expr.\n"); }
				| MAD_EYN '=' const_expression { printf("BEAM with eyn=expr.\n"); }
				| MAD_ET '=' const_expression { printf("BEAM with et=expr.\n"); }
				| MAD_SIGT '=' const_expression { printf("BEAM with sigt=expr.\n"); }
				| MAD_SIGE '=' const_expression { printf("BEAM with sige=expr.\n"); }
				| MAD_KBUNCH '=' const_expression { printf("BEAM with kbunch=expr.\n"); }
				| MAD_NPART '=' const_expression { printf("BEAM with npart=expr.\n"); }
				| MAD_BCURRENT '=' const_expression { printf("BEAM with bcurrent=expr.\n"); }
				| MAD_BUNCHED { printf("BEAM with BUNCHED true.\n"); }
				| '-' MAD_BUNCHED { printf("BEAM with BUNCHED false.\n"); }
				| MAD_RADIATE { printf("BEAM with RADIATE true.\n"); }
				| '-' MAD_RADIATE { printf("BEAM with RADIATE false.\n"); }
				;

particle_name			: MAD_POSITRON { printf("BEAM with positrons.\n"); }
				| MAD_ELECTRON { printf("BEAM with electrons.\n"); }
				| MAD_PROTON { printf("BEAM with protons.\n"); }
				| MAD_ANTI_PROTON { printf("BEAM with anti-protons.\n"); }
				;
/* Beam */

resbeam				: MAD_RESBEAM { printf("Reset all beam data.\n"); }
				;

/* Identifiers */

identifier			: MAD_IDENTIFIER
				| MAD_A {
				  char* str = malloc( 4 );
				  str[0] = 'A';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_AT {
                                  char* str = malloc( 4 );
				  str[0] = 'A';
				  str[1] = 'T';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_BY {
				  char* str = malloc( 4 );
				  str[0] = 'B';
				  str[1] = 'Y';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_DT {
				  char* str = malloc( 4 );
				  str[0] = 'D';
				  str[1] = 'T';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_DX {
				  char* str = malloc( 4 );
				  str[0] = 'D';
				  str[1] = 'X';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_DY {
				  char* str = malloc( 4 );
				  str[0] = 'D';
				  str[1] = 'Y';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_E {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_E1 {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = '1';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_E2 {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = '2';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_ET {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = 'T';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_EX {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = 'X';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_EY {
				  char* str = malloc( 4 );
				  str[0] = 'E';
				  str[1] = 'Y';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_H1 {
				  char* str = malloc( 4 );
				  str[0] = 'H';
				  str[1] = '1';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_H2 {
				  char* str = malloc( 4 );
				  str[0] = 'H';
				  str[1] = '2';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_K1 {
				  char* str = malloc( 4 );
				  str[0] = 'K';
				  str[1] = '1';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_K2 {
				  char* str = malloc( 4 );
				  str[0] = 'K';
				  str[1] = '2';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_K3 {
				  char* str = malloc( 4 );
				  str[0] = 'K';
				  str[1] = '3';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_KS {
				  char* str = malloc( 4 );
				  str[0] = 'K';
				  str[1] = 'S';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_L {
				  char* str = malloc( 4 );
				  str[0] = 'L';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_N {
				  char* str = malloc( 4 );
				  str[0] = 'N';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_PC {
				  char* str = malloc( 4 );
				  str[0] = 'P';
				  str[1] = 'C';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_PG {
				  char* str = malloc( 4 );
				  str[0] = 'P';
				  str[1] = 'G';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_PT {
				  char* str = malloc( 4 );
				  str[0] = 'P';
				  str[1] = 'T';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_PX {
				  char* str = malloc( 4 );
				  str[0] = 'P';
				  str[1] = 'X';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_PY {
				  char* str = malloc( 4 );
				  str[0] = 'P';
				  str[1] = 'Y';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_RM {
				  char* str = malloc( 4 );
				  str[0] = 'R';
				  str[1] = 'M';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T0 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '0';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T1 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '1';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T2 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '2';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T3 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '3';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T4 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '4';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T5 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '5';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T6 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '6';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T7 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '7';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T8 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '8';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_T9 {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = '9';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_TM {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = 'M';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_TO {
				  char* str = malloc( 4 );
				  str[0] = 'T';
				  str[1] = 'O';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_WX {
				  char* str = malloc( 4 );
				  str[0] = 'W';
				  str[1] = 'X';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_WY {
				  char* str = malloc( 4 );
				  str[0] = 'W';
				  str[1] = 'Y';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_X {
				  char* str = malloc( 4 );
				  str[0] = 'X';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_X0 {
				  char* str = malloc( 4 );
				  str[0] = 'X';
				  str[1] = '0';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_Y {
				  char* str = malloc( 4 );
				  str[0] = 'Y';
				  str[1] = '\0';
				  $<sval>$ = str;
				}
				| MAD_Y0 {
				  char* str = malloc( 4 );
				  str[0] = 'Y';
				  str[1] = '0';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_Z0 {
				  char* str = malloc( 4 );
				  str[0] = 'Z';
				  str[1] = '0';
				  str[2] = '\0';
				  $<sval>$ = str;
				}
				| MAD_GAMMA {
				  char* str = malloc( 8 );
				  str[0] = 'G';
				  str[1] = 'A';
				  str[2] = 'M';
				  str[3] = 'M';
				  str[4] = 'A';                                  
				  str[5] = '\0';
				  $<sval>$ = str;
				}
				;

%%

void
yyerror( char* s ) {
  fprintf( stderr, "Line %d: %s\n" , madparser_linenum(mp), s );
}
