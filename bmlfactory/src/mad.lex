%{
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */  
  
#include "mad.tab.h"

#define MAX_ERR_MSG_LENGTH 256

static char     errmsg[MAX_ERR_MSG_LENGTH];
static int      eof_encountered = 0;
static GString* comments = NULL;
 
static void copy_and_format( char* dst, char* src );
void        yyerror( char* s );

extern struct madparser_* mp; 

%}

%x Comment Include

%%

<*>^.*\n			{
				  madparser_inc_linenum(mp);
				  REJECT;
				}

(h|H)(e|E)(l|L)(p|P)					return MAD_HELP;
(s|S)(h|H)(o|O)(w|W)					return MAD_SHOW;
(t|T)(i|I)(t|T)(l|L)(e|E)				return MAD_TITLE;
(s|S)(t|T)(o|O)(p|P)					return MAD_STOP;

(o|O)(p|P)(t|T)(i|I)(o|O)(n|N)				return MAD_OPTION;
(r|R)(e|E)(s|S)(e|E)(t|T)		                return MAD_RESET;
(i|I)(n|N)(t|T)(e|E)(r|R)		                return MAD_INTER;
(e|E)(c|C)(h|H)(o|O)			                return MAD_ECHO;
(t|T)(r|R)(a|A)(c|C)(e|E)		                return MAD_TRACE;
(d|D)(o|O)(u|U)(b|B)(l|L)(e|E)		                return MAD_DOUBLE;
(v|V)(e|E)(r|R)(i|I)(f|F)(y|Y)		                return MAD_VERIFY;
(w|W)(a|A)(r|R)(n|N)			                return MAD_WARN;
(i|I)(n|N)(f|F)(o|O)			                return MAD_INFO;
(s|S)(y|Y)(m|M)(p|P)(l|L)		                return MAD_SYMPL;
(d|D)(e|E)(b|B)(u|U)(g|G)                               return MAD_DEBUG;
(k|K)(e|E)(y|Y)(w|W)(o|O)(r|R)(d|D)	                return MAD_KEYWORD;
(c|C)(o|O)(m|M)(m|M)(a|A)(n|N)(d|D)	                return MAD_COMMAND;
(d|D)(e|E)(f|F)(i|I)(n|N)(e|E)		                return MAD_DEFINE;
(e|E)(x|X)(p|P)(r|R)(e|E)(s|S)(s|S)	                return MAD_EXPRESS;
(l|L)(i|I)(n|N)(e|E)			                return MAD_LINE;
(c|C)(o|O)(f|F)(a|A)(c|C)(t|T)		                return MAD_COFACT;
(t|T)(e|E)(l|L)(l|L)			                return MAD_TELL;

:[ \t]*=						return MAD_DEF_PARAM;
(s|S)(e|E)(t|T)						return MAD_SET;
(v|V)(a|A)(l|L)(u|U)(e|E)				return MAD_VALUE;
(s|S)(y|Y)(s|S)(t|T)(e|E)(m|M)				return MAD_SYSTEM;

(m|M)(a|A)(r|R)(k|K)(e|E)(r|R)				return MAD_MARKER;
(t|T)(y|Y)(p|P)(e|E)		                        return MAD_TYPE;

(d|D)(r|R)(i|I)(f|F)(t|T)				return MAD_DRIFT;
(l|L)				                        return MAD_L;

(s|S)(b|B)(e|E)(n|N)(d|D)				return MAD_SBEND;
(r|R)(b|B)(e|E)(n|N)(d|D)				return MAD_RBEND;
(a|A)(n|N)(g|G)(l|L)(e|E)	                        return MAD_ANGLE;
(k|K)1				                        return MAD_K1;
(e|E)1				                        return MAD_E1;
(e|E)2				                        return MAD_E2;
(f|F)(i|I)(n|N)(t|T)		                        return MAD_FINT;
(h|H)(g|G)(a|A)(p|P)		                        return MAD_HGAP;
(t|T)(i|I)(l|L)(t|T)		                        return MAD_TILT;
(k|K)2				                        return MAD_K2;
(h|H)1				                        return MAD_H1;
(h|H)2				                        return MAD_H2;
(k|K)3				                        return MAD_K3;

(q|Q)(u|U)(a|A)(d|D)(r|R)(u|U)(p|P)(o|O)(l|L)(e|E)	return MAD_QUADRUPOLE;
(s|S)(e|E)(x|X)(t|T)(u|U)(p|P)(o|O)(l|L)(e|E)		return MAD_SEXTUPOLE;
(o|O)(c|C)(t|T)(u|U)(p|P)(o|O)(l|L)(e|E)		return MAD_OCTUPOLE;

(m|M)(u|U)(l|L)(t|T)(i|I)(p|P)(o|O)(l|L)(e|E)		return MAD_MULTIPOLE;
(l|L)(r|R)(a|A)(d|D)		                        return MAD_LRAD;
(k|K)0(l|L)			                        return MAD_K0L;
(k|K)1(l|L)			                        return MAD_K1L;
(k|K)2(l|L)			                        return MAD_K2L;
(k|K)3(l|L)			                        return MAD_K3L;
(k|K)4(l|L)			                        return MAD_K4L;
(k|K)5(l|L)			                        return MAD_K5L;
(k|K)6(l|L)			                        return MAD_K6L;
(k|K)7(l|L)			                        return MAD_K7L;
(k|K)8(l|L)			                        return MAD_K8L;
(k|K)9(l|L)			                        return MAD_K9L;
(t|T)0				                        return MAD_T0;
(t|T)1				                        return MAD_T1;
(t|T)2				                        return MAD_T2;
(t|T)3				                        return MAD_T3;
(t|T)4				                        return MAD_T4;
(t|T)5				                        return MAD_T5;
(t|T)6				                        return MAD_T6;
(t|T)7				                        return MAD_T7;
(t|T)8				                        return MAD_T8;
(t|T)9				                        return MAD_T9;

(s|S)(o|O)(l|L)(e|E)(n|N)(o|O)(i|I)(d|D)		return MAD_SOLENOID;
(k|K)(s|S)			                        return MAD_KS;

(h|H)(k|K)(i|I)(c|C)(k|K)(e|E)(r|R)			return MAD_HKICKER;
(v|V)(k|K)(i|I)(c|C)(k|K)(e|E)(r|R)			return MAD_VKICKER;
(k|K)(i|I)(c|C)(k|K)(e|E)(r|R)				return MAD_KICKER;
(k|K)(i|I)(c|C)(k|K)		                        return MAD_KICK;
(h|H)(k|K)(i|I)(c|C)(k|K)	                        return MAD_HKICK;
(v|V)(k|K)(i|I)(c|C)(k|K)	                        return MAD_VKICK;

(r|R)(f|F)(c|C)(a|A)(v|V)(i|I)(t|T)(y|Y)		return MAD_RFCAVITY;
(v|V)(o|O)(l|L)(t|T)		                        return MAD_VOLT;
(l|L)(a|A)(g|G)			                        return MAD_LAG;
(h|H)(a|A)(r|R)(m|M)(o|O)(n|N)	                        return MAD_HARMON;
(b|B)(e|E)(t|T)(r|R)(f|F)	                        return MAD_BETRF;
(p|P)(g|G)			                        return MAD_PG;
(s|S)(h|H)(u|U)(n|N)(t|T)	                        return MAD_SHUNT;
(t|T)(f|F)(i|I)(l|L)(l|L)	                        return MAD_TFILL;

(e|E)(l|L)(s|S)(e|E)(p|P)(a|A)(r|R)(a|A)(t|T)(o|O)(r|R)	return MAD_ELSEPARATOR;
(e|E)				                        return MAD_E;

(h|H)(m|M)(o|O)(n|N)(i|I)(t|T)(o|O)(r|R)		return MAD_HMONITOR;
(v|V)(m|M)(o|O)(n|N)(i|I)(t|T)(o|O)(r|R)		return MAD_VMONITOR;
(h|H)(m|M)(o|O)(n|N)                    		return MAD_HMON;
(v|V)(m|M)(o|O)(n|N)                    		return MAD_VMON;
(m|M)(o|O)(n|N)(i|I)(t|T)(o|O)(r|R)			return MAD_MONITOR;
(i|I)(n|N)(s|S)(t|T)(r|R)(u|U)(m|M)(e|E)(n|N)(t|T)	return MAD_INSTRUMENT;

(e|E)(c|C)(o|O)(l|L)(l|L)(i|I)(m|M)(a|A)(t|T)(o|O)(r|R)	return MAD_ECOLLIMATOR;
(r|R)(c|C)(o|O)(l|L)(l|L)(i|I)(m|M)(a|A)(t|T)(o|O)(r|R)	return MAD_RCOLLIMATOR;
(x|X)(s|S)(i|I)(z|Z)(e|E)	                        return MAD_XSIZE;
(y|Y)(s|S)(i|I)(z|Z)(e|E)	                        return MAD_YSIZE;

(y|Y)(r|R)(o|O)(t|T)					return MAD_YROT;
(s|S)(r|R)(o|O)(t|T)					return MAD_SROT;

(b|B)(e|E)(a|A)(m|M)(b|B)(e|E)(a|A)(m|M)		return MAD_BEAMBEAM;
(s|S)(i|I)(g|G)(x|X)		                        return MAD_SIGX;
(s|S)(i|I)(g|G)(y|Y)		                        return MAD_SIGY;
(x|X)(m|M)(a|A)			                        return MAD_XMA;
(y|Y)(m|M)(a|A)			                        return MAD_YMA;
(c|C)(h|H)(a|A)(r|R)(g|G)(e|E)	                        return MAD_CHARGE;

(m|M)(a|A)(t|T)(r|R)(i|I)(x|X)				return MAD_MATRIX;
(r|R)(m|M)			                        return MAD_RM;
(t|T)(m|M)			                        return MAD_TM;

(l|L)(u|U)(m|M)(p|P)					return MAD_LUMP;
(o|O)(r|R)(d|D)(e|E)(r|R)	                        return MAD_ORDER;

(s|S)(e|E)(q|Q)(u|U)(e|E)(n|N)(c|C)(e|E)		return MAD_SEQUENCE;
(e|E)(n|N)(d|D)(s|S)(e|E)(q|Q)(u|U)(e|E)(n|N)(c|C)(e|E)	return MAD_END_SEQUENCE;
(r|R)(e|E)(f|F)(e|E)(r|R)	                        return MAD_REFER;
(a|A)(t|T)			                        return MAD_AT;
(c|C)(e|E)(n|N)(t|T)(r|R)(e|E)	                        return MAD_CENTRE;
(e|E)(n|N)(t|T)(r|R)(y|Y)	                        return MAD_ENTRY;
(e|E)(x|X)(i|I)(t|T)		                        return MAD_EXIT;
(s|S)(e|E)(q|Q)(e|E)(d|D)(i|I)(t|T)	                return MAD_SEQEDIT;
(e|E)(n|N)(d|D)(e|E)(d|D)(i|I)(t|T)	                return MAD_ENDEDIT;
(i|I)(n|N)(s|S)(t|T)(a|A)(l|L)(l|L)	                return MAD_INSTALL;
(e|E)(l|L)(e|E)(m|M)(e|E)(n|N)(t|T)	                return MAD_ELEMENT;
(f|F)(r|R)(o|O)(m|M)			                return MAD_FROM;
(r|R)(e|E)(m|M)(o|O)(v|V)(e|E)		                return MAD_REMOVE;
(p|P)(a|A)(t|T)(t|T)(e|E)(r|R)(n|N)	return MAD_PATTERN;
(m|M)(o|O)(v|V)(e|E)			return MAD_MOVE;
(b|B)(y|Y)				return MAD_BY;
(t|T)(o|O)				return MAD_TO;
(r|R)(e|E)(f|F)(l|L)(e|E)(c|C)(t|T)	return MAD_REFLECT;
(c|C)(y|Y)(c|C)(l|L)(e|E)		return MAD_CYCLE;
(s|S)(t|T)(a|A)(r|R)(t|T)		return MAD_START;

(l|L)(i|I)(s|S)(t|T)			return MAD_LIST;

(u|U)(s|S)(e|E)				return MAD_USE;
(p|P)(e|E)(r|R)(i|I)(o|O)(d|D)	        return MAD_PERIOD;
(r|R)(a|A)(n|N)(g|G)(e|E)	        return MAD_RANGE;
(s|S)(y|Y)(m|M)(m|M)		        return MAD_SYMM;
(s|S)(u|U)(p|P)(e|E)(r|R)	        return MAD_SUPER;

(b|B)(e|E)(a|A)(m|M)                                 return MAD_BEAM;
(p|P)(a|A)(r|R)(t|T)(i|I)(c|C)(l|L)(e|E)	     return MAD_PARTICLE;
(p|P)(o|O)(s|S)(i|I)(t|T)(r|R)(o|O)(n|N)	     return MAD_POSITRON;
(e|E)(l|L)(e|E)(c|C)(t|T)(r|R)(o|O)(n|N)	     return MAD_ELECTRON;
(p|P)(r|R)(o|O)(t|T)(o|O)(n|N)			     return MAD_PROTON;
(a|A)(n|N)(t|T)(i|I)-(p|P)(r|R)(o|O)(t|T)(o|O)(n|N)  return MAD_ANTI_PROTON;
(m|M)(a|A)(s|S)(s|S)				return MAD_MASS;
(e|E)(n|N)(e|E)(r|R)(g|G)(y|Y)			return MAD_ENERGY;
(p|P)(c|C)					return MAD_PC;
(g|G)(a|A)(m|M)(m|M)(a|A)			return MAD_GAMMA;
(e|E)(x|X)					return MAD_EX;
(e|E)(x|X)(n|N)					return MAD_EXN;
(e|E)(y|Y)					return MAD_EY;
(e|E)(y|Y)(n|N)					return MAD_EYN;
(e|E)(t|T)					return MAD_ET;
(s|S)(i|I)(g|G)(t|T)				return MAD_SIGT;
(s|S)(i|I)(g|G)(e|E)				return MAD_SIGE;
(k|K)(b|B)(u|U)(n|N)(c|C)(h|H)			return MAD_KBUNCH;
(n|N)(p|P)(a|A)(r|R)(t|T)			return MAD_NPART;
(b|B)(c|C)(u|U)(r|R)(r|R)(e|E)(n|N)(t|T)	return MAD_BCURRENT;
(b|B)(u|U)(n|N)(c|C)(h|H)(e|E)(d|D)		return MAD_BUNCHED;
(r|R)(a|A)(d|D)(i|I)(a|A)(t|T)(e|E)		return MAD_RADIATE;

(r|R)(e|E)(s|S)(b|B)(e|E)(a|A)(m|M)		return MAD_RESBEAM;

(p|P)(r|R)(i|I)(n|N)(t|T)			return MAD_PRINT;
(f|F)(u|U)(l|L)(l|L)		                return MAD_FULL;
(c|C)(l|L)(e|E)(a|A)(r|R)	                return MAD_CLEAR;

(s|S)(e|E)(l|L)(e|E)(c|C)(t|T)			return MAD_SELECT;
(f|F)(l|L)(a|A)(g|G)		                return MAD_FLAG;

(s|S)(p|P)(l|L)(i|I)(t|T)			return MAD_SPLIT;
(f|F)(r|R)(a|A)(c|C)(t|T)(i|I)(o|O)(n|N)	return MAD_FRACTION;

(s|S)(u|U)(r|R)(v|V)(e|E)(y|Y)			return MAD_SURVEY;
(x|X)0				return MAD_X0;
(y|Y)0				return MAD_Y0;
(z|Z)0				return MAD_Z0;
(t|T)(h|H)(e|E)(t|T)(a|A)0	return MAD_THETA0;
(p|P)(h|H)(i|I)0		return MAD_PHI0;
(p|P)(s|S)(i|I)0		return MAD_PSI0;
(t|T)(a|A)(p|P)(e|E)		return MAD_TAPE;

(t|T)(w|W)(i|I)(s|S)(s|S)       return MAD_TWISS;
(d|D)(e|E)(l|L)(t|T)(a|A)(p|P)	return MAD_DELTAP;
(c|C)(h|H)(r|R)(o|O)(m|M)	return MAD_CHROM;
(c|C)(o|O)(u|U)(p|P)(l|L)(e|E)	return MAD_COUPLE;
(s|S)(a|A)(v|V)(e|E)		return MAD_SAVE;
(m|M)(u|U)(x|X)			return MAD_MUX;
(m|M)(u|U)(y|Y)			return MAD_MUY;
(b|B)(e|E)(t|T)(x|X)		return MAD_BETX;
(a|A)(l|L)(f|F)(x|X)		return MAD_ALFX;
(b|B)(e|E)(t|T)(y|Y)		return MAD_BETY;
(a|A)(l|L)(f|F)(y|Y)		return MAD_ALFY;
(d|D)(x|X)			return MAD_DX;
(d|D)(p|P)(x|X)			return MAD_DPX;
(d|D)(y|Y)			return MAD_DY;
(d|D)(p|P)(y|Y)			return MAD_DPY;
(x|X)				return MAD_X;
(p|P)(x|X)			return MAD_PX;
(y|Y)				return MAD_Y;
(p|P)(y|Y)			return MAD_PY;
(w|W)(x|X)			return MAD_WX;
(p|P)(h|H)(i|I)(x|X)		return MAD_PHIX;
(d|D)(m|M)(u|U)(x|X)		return MAD_DMUX;
(w|W)(y|Y)			return MAD_WY;
(p|P)(h|H)(i|I)(y|Y)		return MAD_PHIY;
(d|D)(m|M)(u|U)(y|Y)		return MAD_DMUY;
(d|D)(d|D)(x|X)			return MAD_DDX;
(d|D)(d|D)(y|Y)			return MAD_DDY;
(d|D)(d|D)(p|P)(x|X)		return MAD_DDPX;
(d|D)(d|D)(p|P)(y|Y)		return MAD_DDPY;

(b|B)(e|E)(t|T)(a|A)0           return MAD_BETA0;
(t|T)				return MAD_T;
(p|P)(t|T)			return MAD_PT;

(s|S)(a|A)(v|V)(e|E)(b|B)(e|E)(t|T)(a|A)		return MAD_SAVEBETA;
(l|L)(a|A)(b|B)(e|E)(l|L)	                        return MAD_LABEL;
(p|P)(l|L)(a|A)(c|C)(e|E)	                        return MAD_PLACE;

(i|I)(b|B)(s|S)                                         return MAD_IBS;
(t|T)(a|A)(b|B)(l|L)(e|E)	                        return MAD_TABLE;

(e|E)(m|M)(i|I)(t|T)					return MAD_EMIT;
(e|E)(i|I)(g|G)(e|E)(n|N)				return MAD_EIGEN;

(e|E)(n|N)(v|V)(e|E)(l|L)(o|O)(p|P)(e|E)		return MAD_ENVELOPE;
(s|S)(i|I)(g|G)(m|M)(a|A)0	                        return MAD_SIGMA0;

(t|T)(w|W)(i|I)(s|S)(s|S)3				return MAD_TWISS3;

(d|D)(t|T)			return MAD_DT;
(d|D)(p|P)(t|T)			return MAD_DPT;
(r|R)21				return MAD_R21;
(r|R)31				return MAD_R31;
(r|R)41				return MAD_R41;
(r|R)51				return MAD_R51;
(r|R)61				return MAD_R61;
(s|S)(i|I)(p|P)(x|X)		return MAD_SIGPX;
(r|R)32				return MAD_R32;
(r|R)42				return MAD_R42;
(r|R)52				return MAD_R52;
(r|R)62				return MAD_R62;
(r|R)43				return MAD_R43;
(r|R)53				return MAD_R53;
(r|R)63				return MAD_R63;
(s|S)(i|I)(p|P)(y|Y)		return MAD_SIGPY;
(r|R)54				return MAD_R54;
(r|R)64				return MAD_R64;
(r|R)65				return MAD_R65;
(s|S)(i|I)(p|P)(t|T)		return MAD_SIGPT;

(s|S)(a|A)(v|V)(e|E)(s|S)(i|I)(g|G)(m|M)(a|A)		return MAD_SAVESIGMA;

(d|D)(y|Y)(n|N)(a|A)(m|M)(i|I)(c|C)			return MAD_DYNAMIC;
(m|M)(a|A)(p|P)					return MAD_MAP;
(o|O)(r|R)(b|B)(i|I)(t|T)			return MAD_ORBIT;
(a|A)						return MAD_A;
(n|N)						return MAD_N;
(r|R)(e|E)(s|S)(o|O)(n|N)(a|A)(n|N)(c|C)(e|E)	return MAD_RESONANCE;
(e|E)(x|X)(p|P)(o|O)(n|N)(e|E)(n|N)(t|T)	return MAD_EXPONENT;
(h|H)(a|A)(m|M)(i|I)(l|L)(t|T)(o|O)(n|N)	return MAD_HAMILTON;
(i|I)(n|N)(v|V)(a|A)(r|R)(i|I)(a|A)(n|N)(t|T)	return MAD_INVARIANT;

(s|S)(t|T)(a|A)(t|T)(i|I)(c|C)				return MAD_STATIC;
(f|F)(i|I)(x|X)(e|E)(d|D)			        return MAD_FIXED;
(b|B)(e|E)(t|T)(a|A)(t|T)(r|R)(o|O)(n|N)	        return MAD_BETATRON;
(n|N)(o|O)(n|N)(l|L)(i|I)(n|N)(e|E)(a|A)(r|R)	        return MAD_NONLINEAR;
(c|C)(o|O)(n|N)(j|J)(u|U)(g|G)(a|A)(t|T)(e|E)	        return MAD_CONJUGATE;

(o|O)(p|P)(t|T)(i|I)(c|C)(s|S)				return MAD_OPTICS;
(f|F)(i|I)(l|L)(e|E)(n|N)(a|A)(m|M)(e|E)	        return MAD_FILENAME;
(c|C)(o|O)(l|L)(u|U)(m|M)(n|N)(s|S)		        return MAD_COLUMNS;

(s|S)(q|Q)(r|R)(t|T)		return MAD_SQRT;
(l|L)(o|O)(g|G)			return MAD_LOG;
(e|E)(x|X)(p|P)			return MAD_EXP;
(s|S)(i|I)(n|N)			return MAD_SIN;
(c|C)(o|O)(s|S)			return MAD_COS;
(t|T)(a|A)(n|N)			return MAD_TAN;
(a|A)(s|S)(i|I)(n|N)		return MAD_ASIN;
(a|A)(b|B)(s|S)			return MAD_ABS;
(m|M)(a|A)(x|X)			return MAD_MAX;
(m|M)(i|I)(n|N)			return MAD_MIN;

(C|c)(A|a)(L|l)(L|l)[ \t]*(F|f)(I|i)(L|l)(E|e)(N|n)(A|a)(M|m)(E|e)[ \t]*= { BEGIN(Include); }

^[ \t]*\!.*\n			{
				  char* ptr;
				  for ( ptr = yytext; *ptr != '!'; ++ptr ) {}
				  comments = g_string_new( ++ptr );
				  BEGIN(Comment);
				}

<Comment>^[ \t]*\n |
         ^[ \t]*		;

<Comment>\!.*\n			{
				  g_string_append( comments, "// " );
				  g_string_append( comments, yytext+1 );
				}

<Comment><<EOF>>		{
				  /* Store the comments without last '\n' */
				  yylval.sval = (char*)malloc( comments->len );
				  if ( yylval.sval != NULL ) {
				      strncpy( yylval.sval, comments->str, comments->len - 1 );
				      *(yylval.sval+ comments->len - 1) = '\0';

                                      g_string_free( comments, TRUE ); /* !!! */
				  } else {
				      yyerror( "Memory allocation problem\n" );
				  }
				  madparser_comment_at_eof_set(mp);
				  BEGIN(INITIAL);
				  return MAD_EXCOMMENT;
				}

<Comment>.			{
				  /* Store the comments without last '\n' */
				  yylval.sval = (char*)malloc( comments->len );
				  if ( yylval.sval != NULL ) {
				      strncpy( yylval.sval, comments->str, comments->len - 1 );
				      *(yylval.sval+ comments->len - 1) = '\0';
                                      g_string_free( comments, TRUE ); /* !!! */
				  } else {
				      yyerror( "Memory allocation problem\n" );
				  }
				  yyless(0);
				  BEGIN(INITIAL);
				  return MAD_EXCOMMENT;
				}

\!.*\n				{
				  /* Store the comment without '!' and '\n' */
				  yylval.sval = (char*)malloc( yyleng-1 );
				  if ( yylval.sval != NULL ) {
				      strncpy( yylval.sval, yytext+1, yyleng-2 );
				      *(yylval.sval+yyleng-2) = '\0';
				  } else {
				      yyerror( "Memory allocation problem\n" );
				  }
				  return MAD_EXCOMMENT;
				}

(c|C)(o|O)(m|M)(m|M)(e|E)(n|N)(t|T)			return MAD_COMMENT;
(e|E)(n|N)(d|D)(c|C)(o|O)(m|M)(m|M)(e|E)(n|N)(t|T)	return MAD_ENDCOMMENT;

(c|C)(o|O)(n|N)(s|S)(t|T)(a|A)(n|N)(t|T)		return MAD_CONSTANT;

[A-Za-z][A-Za-z0-9.'_]*		{
				  yylval.sval=(char*)malloc( yyleng+1 );
				  if ( yylval.sval != NULL ) {
				      strcpy( yylval.sval, yytext );
				  } else {
				      yyerror( "Memory allocation problem\n" );
				  }
				  return MAD_IDENTIFIER;
				}

"."[0-9]+([DEde][+-]?[0-9]+)? |
[0-9]+"."[0-9]*([DEde][+-]?[0-9]+)? |
[0-9]+[DEde][+-]?[0-9]+ |
[1-9][0-9]+ |
[0-9]				{
				  yylval.sval=(char*)malloc( yyleng+2 );
				  if ( yylval.sval == NULL ) {
				      yyerror( "Memory allocation problem\n" );
				  }
				  copy_and_format( yylval.sval, yytext );
				  strtod( yylval.sval, (char**)NULL );
				  if ( errno == ERANGE ) {
				      strcpy( errmsg, "fatal error ! number out of range: " );
				      assert( strlen( errmsg )+yyleng+1 < MAX_ERR_MSG_LENGTH );
				      yyerror( strcat( errmsg, (char*)yylval.sval ) );
				      exit( EXIT_FAILURE );
				  }
				  current_constant = CONSTANT_DOUBLE;
				  return MAD_NUMERIC_LITERAL;
				}

\"[^\"\n]*\" |
\'[^\'\n]*\'			{
				  /* Store the string without quotes */
				  yylval.sval=(char*)malloc( yyleng-1 );
				  if ( yylval.sval != NULL ) {
				      strncpy( yylval.sval, yytext+1, yyleng-2 );
				      *(yylval.sval+yyleng-2) = '\0';
				  } else {
				      yyerror( "Memory allocation problem\n" );
				  }

				  current_constant = CONSTANT_STRING;
				  return MAD_STRING_LITERAL;
				}

("*"|"/")[ \t]*("+"|"-")	{
				  strcpy( errmsg, "Illegal *,/,+,- combination: " );
				  assert( strlen( errmsg )+yyleng < MAX_ERR_MSG_LENGTH );
				  yyerror( strcat( errmsg, (char*)yytext ) );
				  exit( EXIT_FAILURE );
				}

[ \t]* |
^[ \t]*\n |
"&"[ \t]*\n			;

\n |
";"				{ return MAD_NEWLINE; }

<<EOF>>				{
				  if ( eof_encountered == 0) {
			            eof_encountered = 1;
				    return MAD_NEWLINE;
				  } else {
   /*
                                           if ( --include_stack_ptr < 0 ) {
                                             yyterminate();
                                           } else {
                                             yy_delete_buffer( YY_CURRENT_BUFFER );
                                             yy_switch_to_buffer( include_stack[include_stack_ptr] );
                                           }
  */
                                    eof_encountered = 0; /* !!! */

                                    if ( madparser_has_input(mp) ) {
                                      yy_delete_buffer( (YY_BUFFER_STATE)madparser_current_buffer(mp) );
                                      yy_switch_to_buffer( (YY_BUFFER_STATE)madparser_restore_yybuff(mp) );
                                    } else {
                                      yyterminate();
                                    }
                                  }
				}

.				return yytext[0];

<Include>[ \t]*      /* eat the whitespace */

<Include>[^ \t\n]+              { /* got the include file name */
                                     /*
                                     if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
                                       fprintf( stderr, "Includes nested too deeply: %d\n", MAX_INCLUDE_DEPTH );
                                       exit( EXIT_FAILURE );
                                     }     
                                     include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
                                    */
                                  assert( sizeof(YY_CURRENT_BUFFER) <= sizeof(void*) );
                                  madparser_save_yybuff( mp, YY_CURRENT_BUFFER );
                                  yyin = madparser_new_yybuff( mp, yytext );
                                  assert( yyin != NULL );
                                     /*
                                  yyin = fopen( yytext, "r" );
                                     */                                  
                                  if ( ! yyin ) {
                                    fprintf( stderr, "Unable to open file %s\n", yytext );
                                    exit( EXIT_FAILURE );
                                  }
                                  yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );     
                                  BEGIN(INITIAL);
                                }

%%

static void
copy_and_format( char* dst, char* src ) {
  int dec_pt_found = 0;
  char* p;
  char* q;
  for ( p = src, q = dst; *p != '\0'; ++p, ++q ) {
    if ( *p == '.' ) {
      dec_pt_found = 1;
      *q = *p;
    } else if ( *p == 'D' || *p == 'd' || *p == 'E' || *p == 'e' ) {
      if ( dec_pt_found == 0 ) {
        *(q++) = '.';
        dec_pt_found = 1;
      }
      *q = 'e';
    } else {
      *q = *p;
    }
  }
  if ( dec_pt_found == 0 ) {
    *(q++) = '.';
  }

  *q = '\0';
}
