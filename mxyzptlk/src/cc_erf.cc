#include "mxyzptlk.h"
const Complex complex_i = Complex(0.0,1.0);
//const complex complex_1 = complex(1.0,0.0);
//const complex complex_0 = complex(0.0,0.0);

extern "C" {
typedef long flag;
typedef long ftnlen;
typedef long ftnint;

typedef struct
{	flag cierr;
	ftnint ciunit;
	flag ciend;
	char *cifmt;
	ftnint cirec;
} cilist;
    int s_wsfe(cilist*), e_wsfe();
    /* Subroutine */ int s_stop(char* , long);
};

const double SWITCH_POINT = 3.0;
const int SERIES_LOOP = 130;
const int ASYMPTOTIC_LOOP = 5;

#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))

double d1mach_(int);
int i1mach_(int );
double truncatedFactorial(int);

Complex w(const Complex& inputZ) {

  int i = 0;
  int j = 0;
  int imax;
  Complex result;
  Complex series = complex_0;
  Complex tmp1 = -inputZ*inputZ;
  Complex tmp2;
  double tmp;
  double tmp3;
  double sigma;
  Complex sqrtIPI = complex_i * sqrt(M_PI);

  if(abs(inputZ) >= SWITCH_POINT) {
    imax = ASYMPTOTIC_LOOP;
    for (i=1;i<imax; i++) {
      tmp = 2.0*i;
      tmp3 = (double)i + 0.5;
      tmp2 = complex_1/pow(2.0*inputZ,tmp)/tmp3/truncatedFactorial(i);
      series += tmp2;
    }
    if(imag(inputZ) > 0.0)
      sigma = 0.0;
    else if(imag(inputZ) < 0.0)
      sigma = 2.0;
    else
      sigma = 1.0;
    result = sigma*exp(-inputZ*inputZ) - 
      complex_1/sqrtIPI/inputZ - 
      series/sqrtIPI/2.0/inputZ;

    return result;
  } else {
    imax = SERIES_LOOP;
    for (i=1;i<imax; i++) {
      tmp = 2.0*i + 1.0;
      series += pow(tmp1,(double)i)*pow(2.0,tmp)*truncatedFactorial(i);
    }
    result = exp(-inputZ*inputZ) - 
      2.0*inputZ/sqrtIPI - 
      inputZ*series/sqrtIPI;

    return result;
  }
}
JetC w(JetC& inputZ) {

  // Reset default reference point according to input
  static Complex rfp[6];
  JetC dummyJet;
  dummyJet.getReference( rfp );
  JetC::FixReference( inputZ );

  int i = 0;
  int j = 0;
  int imax;
  JetC result;
  JetC series(complex_0);
  JetC tmp1;
  JetC tmp2;
  double xxx;
  double tmp;
  double sigma;
  Complex sqrtIPI = complex_i * sqrt(M_PI);
  if(abs(inputZ.standardPart()) >= SWITCH_POINT) {
    imax = ASYMPTOTIC_LOOP;
    for (i=1;i<imax; i++) {
      tmp = 2.0*i;
      xxx = (double)i + 0.5;
      tmp2 = pow(2.0*inputZ,-tmp)/xxx/truncatedFactorial(i);
      series += tmp2;
    }
    if(imag(inputZ.standardPart()) > 0.0)
      sigma = 0.0;
    else if(imag(inputZ.standardPart()) < 0.0)
      sigma = 2.0;
    else
      sigma = 1.0;
    
    result = sigma*exp(-inputZ*inputZ) - 
      complex_1/sqrtIPI/inputZ -
      series/sqrtIPI/(2.0*inputZ);
    // Reset default reference point to original value
    JetC::FixReference( rfp );

    return result;
  } else {
    tmp1 = -inputZ*inputZ;
    imax = SERIES_LOOP;
    for (i=1;i<imax; i++) {
      tmp = 2.0*i + 1.0;
      series += pow(tmp1,(double)i)*pow(2.0,tmp)*truncatedFactorial(i);
    }
    result = exp(-inputZ*inputZ) - 
      2.0*inputZ/sqrtIPI - 
      inputZ*series/sqrtIPI;

    // Reset default reference point to original value
    JetC::FixReference( rfp );

    return result;
  }
}
// The rest of the file is support routines

double dgamln_(double z)
{
    /* Initialized data */

    static double gln[100] = { 0.,0.,.693147180559945309,
	    1.791759469228055,3.17805383034794562,4.78749174278204599,
	    6.579251212010101,8.5251613610654143,10.6046029027452502,
	    12.8018274800814696,15.1044125730755153,17.5023078458738858,
	    19.9872144956618861,22.5521638531234229,25.1912211827386815,
	    27.8992713838408916,30.6718601060806728,33.5050734501368889,
	    36.3954452080330536,39.339884187199494,42.335616460753485,
	    45.380138898476908,48.4711813518352239,51.6066755677643736,
	    54.7847293981123192,58.0036052229805199,61.261701761002002,
	    64.5575386270063311,67.889743137181535,71.257038967168009,
	    74.6582363488301644,78.0922235533153106,81.5579594561150372,
	    85.0544670175815174,88.5808275421976788,92.1361756036870925,
	    95.7196945421432025,99.3306124547874269,102.968198614513813,
	    106.631760260643459,110.320639714757395,114.034211781461703,
	    117.771881399745072,121.533081515438634,125.317271149356895,
	    129.123933639127215,132.95257503561631,136.802722637326368,
	    140.673923648234259,144.565743946344886,148.477766951773032,
	    152.409592584497358,156.360836303078785,160.331128216630907,
	    164.320112263195181,168.327445448427652,172.352797139162802,
	    176.395848406997352,180.456291417543771,184.533828861449491,
	    188.628173423671591,192.739047287844902,196.866181672889994,
	    201.009316399281527,205.168199482641199,209.342586752536836,
	    213.532241494563261,217.736934113954227,221.956441819130334,
	    226.190548323727593,230.439043565776952,234.701723442818268,
	    238.978389561834323,243.268849002982714,247.572914096186884,
	    251.890402209723194,256.221135550009525,260.564940971863209,
	    264.921649798552801,269.291097651019823,273.673124285693704,
	    278.067573440366143,282.474292687630396,286.893133295426994,
	    291.323950094270308,295.766601350760624,300.220948647014132,
	    304.686856765668715,309.164193580146922,313.652829949879062,
	    318.152639620209327,322.663499126726177,327.185287703775217,
	    331.717887196928473,336.261181979198477,340.815058870799018,
	    345.379407062266854,349.954118040770237,354.539085519440809,
	    359.134205369575399 };
    static double cf[22] = { .0833333333333333333,-.00277777777777777778,
	    7.93650793650793651e-4,-5.95238095238095238e-4,
	    8.41750841750841751e-4,-.00191752691752691753,
	    .00641025641025641026,-.0295506535947712418,.179644372368830573,
	    -1.39243221690590112,13.402864044168392,-156.848284626002017,
	    2193.10333333333333,-36108.7712537249894,691472.268851313067,
	    -15238221.5394074162,382900751.391414141,-10882266035.7843911,
	    347320283765.002252,-12369602142269.2745,488788064793079.335,
	    -21320333960919373.9 };
    static double con = 1.83787706640934548;

    /* System generated locals */
    int i_1;
    int ierr;
    double ret_val;

    /* Local variables */
    static double zinc, zmin, zdmy;
    static int i, k;
    static double s, wdtol;
    static double t1, fz, zm;
    static int mz, nz;
    static double zp;
    static int i1m;
    static double fln, tlg, rln, trm, tst, zsq;

/* ***BEGIN PROLOGUE  DGAMLN */
/* ***DATE WRITTEN   830501   (YYMMDD) */
/* ***REVISION DATE  830501   (YYMMDD) */
/* ***CATEGORY NO.  B5F */
/* ***KEYWORDS  GAMMA FUNCTION,LOGARITHM OF GAMMA FUNCTION */
/* ***AUTHOR  AMOS, DONALD E., SANDIA NATIONAL LABORATORIES */
/* ***PURPOSE  TO COMPUTE THE LOGARITHM OF THE GAMMA FUNCTION */
/* ***DESCRIPTION */

/*               **** A DOUBLE PRECISION ROUTINE **** */
/*         DGAMLN COMPUTES THE NATURAL LOG OF THE GAMMA FUNCTION FOR */
/*         Z.GT.0.  THE ASYMPTOTIC EXPANSION IS USED TO GENERATE VALUES */

/*         GREATER THAN ZMIN WHICH ARE ADJUSTED BY THE RECURSION */
/*         G(Z+1)=Z*G(Z) FOR Z.LE.ZMIN.  THE FUNCTION WAS MADE AS */
/*         PORTABLE AS POSSIBLE BY COMPUTIMG ZMIN FROM THE NUMBER OF BASE 
*/
/*         10 DIGITS IN A WORD, RLN=AMAX1(-ALOG10(R1MACH(4)),0.5E-18) */
/*         LIMITED TO 18 DIGITS OF (RELATIVE) ACCURACY. */

/*         SINCE INT ARGUMENTS ARE COMMON, A TABLE LOOK UP ON 100 */
/*         VALUES IS USED FOR SPEED OF EXECUTION. */

/*     DESCRIPTION OF ARGUMENTS */

/*         INPUT      Z IS D0UBLE PRECISION */
/*           Z      - ARGUMENT, Z.GT.0.0D0 */

/*         OUTPUT      DGAMLN IS DOUBLE PRECISION */
/*           DGAMLN  - NATURAL LOG OF THE GAMMA FUNCTION AT Z.NE.0.0D0 */
/*           IERR    - ERROR FLAG */
/*                     IERR=0, NORMAL RETURN, COMPUTATION COMPLETED */
/*                     IERR=1, Z.LE.0.0D0,    NO COMPUTATION */


/* ***REFERENCES  COMPUTATION OF BESSEL FUNCTIONS OF COMPLEX ARGUMENT */
/*                 BY D. E. AMOS, SAND83-0083, MAY, 1983. */
/* ***ROUTINES CALLED  I1MACH,D1MACH */
/* ***END PROLOGUE  DGAMLN */
/*           LNGAMMA(N), N=1,100 */
/*             COEFFICIENTS OF ASYMPTOTIC EXPANSION */

/*             LN(2*PI) */

/* ***FIRST EXECUTABLE STATEMENT  DGAMLN */
    ierr = 0;
    if (z <= 0.) {
	goto L70;
    }
    if (z > 101.) {
	goto L10;
    }
    nz = (int) (z);
    fz = z - (double) nz;
    if (fz > 0.) {
	goto L10;
    }
    if (nz > 100) {
	goto L10;
    }
    ret_val = gln[nz - 1];
    return ret_val;
L10:
    wdtol = d1mach_(4);
    wdtol = max(wdtol,5e-19);
    i1m = i1mach_(14);
    rln = d1mach_(5) * (double) i1m;
    fln = min(rln,20.);
    fln = max(fln,3.);
    fln += -3.;
    zm = fln * .3875 + 1.8;
    mz = (int) zm + 1;
    zmin = (double) mz;
    zdmy = z;
    zinc = 0.;
    if (z >= zmin) {
	goto L20;
    }
    zinc = zmin - (double) nz;
    zdmy = z + zinc;
L20:
    zp = 1. / zdmy;
    t1 = cf[0] * zp;
    s = t1;
    if (zp < wdtol) {
	goto L40;
    }
    zsq = zp * zp;
    tst = t1 * wdtol;
    for (k = 2; k <= 22; ++k) {
	zp *= zsq;
	trm = cf[k - 1] * zp;
	if (fabs(trm) < tst) {
	    goto L40;
	}
	s += trm;
/* L30: */
    }
L40:
    if (zinc != 0.) {
	goto L50;
    }
    tlg = log(z);
    ret_val = z * (tlg - 1.) + (con - tlg) * .5 + s;
    return ret_val;
L50:
    zp = 1.;
    nz = (int) zinc;
    i_1 = nz;
    for (i = 1; i <= i_1; ++i) {
	zp *= z + (double) (i - 1);
/* L60: */
    }
    tlg = log(zdmy);
    ret_val = zdmy * (tlg - 1.) - log(zp) + (con - tlg) * .5 + s;
    return ret_val;


L70:
    ierr = 1;
    return ret_val;
} /* dgamln_ */

/* DECK D1MACH */
double d1mach_(int i)
{
    /* System generated locals */
    double ret_val;
    static double equiv_4[6];

    /* Local variables */
#define plog10 ((int *)equiv_4 + 8)
#define dmach (equiv_4)
#define large ((int *)equiv_4 + 2)
#define small ((int *)equiv_4)
#define diver ((int *)equiv_4 + 6)
#define right ((int *)equiv_4 + 4)


/*     from SLATEC CML committee - work for Sun3, Sun4, and Sparc */
    small[0] = 0x00100000;
    small[1] = 0x0;
    large[0] = 0x7FeFFFFF;
    large[1] = 0xFFFFFFFF;
    right[0] = 0x3CA00000;
    right[1] = 0x0;
    diver[0] = 0x3CB00000;
    diver[1] = 0x0;
    plog10[0] = 0x3FD34413;
    plog10[1] = 0x509F79FF;

/*     DATA SMALL(1), SMALL(2) / Z'00100000', Z'00000000' / */
/*     DATA LARGE(1), LARGE(2) / Z'7FEFFFFF', Z'FFFFFFFF' / */
/*     DATA RIGHT(1), RIGHT(2) / Z'3CA00000', Z'00000000' / */
/*     DATA DIVER(1), DIVER(2) / Z'3CB00000', Z'00000000' / */
/*     DATA LOG10(1), LOG10(2) / Z'3FD34413', Z'509F79FF' / */




/* ***BEGIN PROLOGUE  D1MACH */
/* ***DATE WRITTEN   750101   (YYMMDD) */
/* ***REVISION DATE  890213   (YYMMDD) */
/* ***CATEGORY NO.  R1 */
/* ***KEYWORDS  LIBRARY=SLATEC,TYPE=DOUBLE PRECISION(R1MACH-S D1MACH-D), 
*/
/*             MACHINE CONSTANTS */
/* ***AUTHOR  FOX, P. A., (BELL LABS) */
/*           HALL, A. D., (BELL LABS) */
/*           SCHRYER, N. L., (BELL LABS) */
/* ***PURPOSE  Returns double precision machine dependent constants */
/* ***DESCRIPTION */

/*   D1MACH can be used to obtain machine-dependent parameters */
/*   for the local machine environment.  It is a function */
/*   subprogram with one (input) argument, and can be called */
/*   as follows, for example */

/*        D = D1MACH(I) */

/*   where I=1,...,5.  The (output) value of D above is */
/*   determined by the (input) value of I.  The results for */
/*   various values of I are discussed below. */

/*   D1MACH( 1) = B**(EMIN-1), the smallest positive magnitude. */
/*   D1MACH( 2) = B**EMAX*(1 - B**(-T)), the largest magnitude. */
/*   D1MACH( 3) = B**(-T), the smallest relative spacing. */
/*   D1MACH( 4) = B**(1-T), the largest relative spacing. */
/*   D1MACH( 5) = LOG10(B) */

/*   Assume double precision numbers are represented in the T-digit, */
/*   base-B form */

/*              sign (B**E)*( (X(1)/B) + ... + (X(T)/B**T) ) */

/*   where 0 .LE. X(I) .LT. B for I=1,...,T, 0 .LT. X(1), and */
/*   EMIN .LE. E .LE. EMAX. */

/*   The values of B, T, EMIN and EMAX are provided in I1MACH as */
/*   follows: */
/*   I1MACH(10) = B, the base. */
/*   I1MACH(14) = T, the number of base-B digits. */
/*   I1MACH(15) = EMIN, the smallest exponent E. */
/*   I1MACH(16) = EMAX, the largest exponent E. */

/*   To alter this function for a particular environment, */
/*   the desired set of DATA statements should be activated by */
/*   removing the C from column 1.  Also, the values of */
/*   D1MACH(1) - D1MACH(4) should be checked for consistency */
/*   with the local operating system. */

/* ***REFERENCES  FOX P.A., HALL A.D., SCHRYER N.L.,*FRAMEWORK FOR A */
/*                 PORTABLE LIBRARY*, ACM TRANSACTIONS ON MATHEMATICAL */
/*                 SOFTWARE, VOL. 4, NO. 2, JUNE 1978, PP. 177-188. */
/* ***ROUTINES CALLED  XERROR */
/* ***END PROLOGUE  D1MACH */




/*     MACHINE CONSTANTS FOR THE IBM PC */
/*     ASSUMES THAT ALL ARITHMETIC IS DONE IN DOUBLE PRECISION */
/*     ON 8088, I.E., NOT IN 80 BIT FORM FOR THE 8087. */

/*     DATA SMALL(1),SMALL(2) /  2002288515,    1050897 / */
/*     DATA LARGE(1),LARGE(2) /  1487780761, 2146426097 / */
/*     DATA RIGHT(1),RIGHT(2) / -1209488034, 1017118298 / */
/*     DATA DIVER(1),DIVER(2) / -1209488034, 1018166874 / */
/*     DATA LOG10(1),LOG10(2) /  1352628735, 1070810131 / */

/*     MACHINE CONSTANTS FOR THE SILICON GRAPHICS IRIS */

/*     data dmach(1) / 2.22507 38585 072012 d-308 / */
/*     data dmach(2) / 1.79769 31348 623158 d+308 / */
/*     data dmach(3) / 2.22044 60492 503131 d-16  / */
/*     data dmach(4) / 4.44089 20985 006262 d-16  / */
/*     data dmach(5) / 0.30102 99956 639812       / */

/*     DATA SMALL(1), SMALL(2) / Z'00100000',Z'00000000' / */
/*     DATA LARGE(1), LARGE(2) / Z'7FEFFFFF',Z'FFFFFFFF' / */
/*     DATA RIGHT(1), RIGHT(2) / Z'3CB00000',Z'00000000' / */
/*     DATA DIVER(1), DIVER(2) / Z'3CC00000',Z'00000000' / */
/*     DATA LOG10(1), LOG10(2) / Z'3FD34413',Z'509F79FF' / */

/*     MACHINE CONSTANTS FOR THE SUN */

/*     from Sun Microsystems - work for Sun 386i */

/*     DATA SMALL(1), SMALL(2) / Z'00000000', Z'00100000' / */
/*     DATA LARGE(1), LARGE(2) / Z'FFFFFFFF', Z'7FEFFFFF' / */
/*     DATA RIGHT(1), RIGHT(2) / Z'00000000', Z'3CA00000' / */
/*     DATA DIVER(1), DIVER(2) / Z'00000000', Z'3CB00000' / */
/*     DATA LOG10(1), LOG10(2) / Z'509F79FF', Z'3FD34413' / */

/*     MACHINE CONSTANTS FOR VAX 11/780 */
/*     (EXPRESSED IN INT AND HEXADECIMAL) */
/*     THE HEX FORMAT BELOW MAY NOT BE SUITABLE FOR UNIX SYSYEMS */
/*     THE INT FORMAT SHOULD BE OK FOR UNIX SYSTEMS */

/*     DATA SMALL(1), SMALL(2) /        128,           0 / */
/*     DATA LARGE(1), LARGE(2) /     -32769,          -1 / */
/*     DATA RIGHT(1), RIGHT(2) /       9344,           0 / */
/*     DATA DIVER(1), DIVER(2) /       9472,           0 / */
/*     DATA LOG10(1), LOG10(2) /  546979738,  -805796613 / */

/*     DATA SMALL(1), SMALL(2) / Z00000080, Z00000000 / */
/*     DATA LARGE(1), LARGE(2) / ZFFFF7FFF, ZFFFFFFFF / */
/*     DATA RIGHT(1), RIGHT(2) / Z00002480, Z00000000 / */
/*     DATA DIVER(1), DIVER(2) / Z00002500, Z00000000 / */
/*     DATA LOG10(1), LOG10(2) / Z209A3F9A, ZCFF884FB / */

/*     MACHINE CONSTANTS FOR VAX 11/780 (G-FLOATING) */
/*     (EXPRESSED IN INT AND HEXADECIMAL) */
/*     THE HEX FORMAT BELOW MAY NOT BE SUITABLE FOR UNIX SYSYEMS */
/*     THE INT FORMAT SHOULD BE OK FOR UNIX SYSTEMS */

/*     DATA SMALL(1), SMALL(2) /         16,           0 / */
/*     DATA LARGE(1), LARGE(2) /     -32769,          -1 / */
/*     DATA RIGHT(1), RIGHT(2) /      15552,           0 / */
/*     DATA DIVER(1), DIVER(2) /      15568,           0 / */
/*     DATA LOG10(1), LOG10(2) /  1142112243, 2046775455 / */

/*     DATA SMALL(1), SMALL(2) / Z00000010, Z00000000 / */
/*     DATA LARGE(1), LARGE(2) / ZFFFF7FFF, ZFFFFFFFF / */
/*     DATA RIGHT(1), RIGHT(2) / Z00003CC0, Z00000000 / */
/*     DATA DIVER(1), DIVER(2) / Z00003CD0, Z00000000 / */
/*     DATA LOG10(1), LOG10(2) / Z44133FF3, Z79FF509F / */


/* ***FIRST EXECUTABLE STATEMENT  D1MACH */
    if (i < 1 || i > 5) {
      cerr << "D1MACH -- I OUT OF BOUNDS" << endl;
    }

    ret_val = dmach[i - 1];
    return ret_val;

} /* d1mach_ */

#undef right
#undef diver
#undef small
#undef large
#undef dmach
#undef log10


/* DECK I1MACH */
int i1mach_(int i)
{
    /* Format strings */
    static char fmt_9000[] = "(\0021ERROR    1 IN I1MACH - I OUT OF BOUND\
S\002)";

    /* System generated locals */
    int ret_val;
    static int equiv_0[16];

    equiv_0[0] = 5;
    equiv_0[1] = 6;
    equiv_0[2] = 6;
    equiv_0[3] = 6;
    equiv_0[4] = 32;
    equiv_0[5] = 4;
    equiv_0[6] = 2;
    equiv_0[7] = 31;
    equiv_0[8] = 2147483647;
    equiv_0[9] = 2;
    equiv_0[10] =24;
    equiv_0[11] = -125;
    equiv_0[12] = 128;
    equiv_0[13] = 53;
    equiv_0[14] = -1021;
    equiv_0[15] = 1024;

    /* Local variables */
#define imach (equiv_0)
#define output (equiv_0 + 3)

    /* Fortran I/O blocks */
    static cilist io__32 = { 0, 0, 0, fmt_9000, 0 };


/* ***BEGIN PROLOGUE  I1MACH */
/* ***DATE WRITTEN   750101   (YYMMDD) */
/* ***REVISION DATE  890213   (YYMMDD) */
/* ***CATEGORY NO.  R1 */
/* ***KEYWORDS  LIBRARY=SLATEC,TYPE=INT(I1MACH-I),MACHINE CONSTANTS */

/* ***AUTHOR  FOX, P. A., (BELL LABS) */
/*           HALL, A. D., (BELL LABS) */
/*           SCHRYER, N. L., (BELL LABS) */
/* ***PURPOSE  Returns int machine dependent constants */
/* ***DESCRIPTION */

/*     I1MACH can be used to obtain machine-dependent parameters */
/*     for the local machine environment.  It is a function */
/*     subroutine with one (input) argument, and can be called */
/*     as follows, for example */

/*          K = I1MACH(I) */

/*     where I=1,...,16.  The (output) value of K above is */
/*     determined by the (input) value of I.  The results for */
/*     various values of I are discussed below. */

/*  I/O unit numbers. */
/*    I1MACH( 1) = the standard input unit. */
/*    I1MACH( 2) = the standard output unit. */
/*    I1MACH( 3) = the standard punch unit. */
/*    I1MACH( 4) = the standard error message unit. */

/*  Words. */
/*    I1MACH( 5) = the number of bits per int storage unit. */
/*    I1MACH( 6) = the number of characters per int storage unit. */

/*  Ints. */
/*    assume ints are represented in the S-digit, base-A form */

/*               sign ( X(S-1)*A**(S-1) + ... + X(1)*A + X(0) ) */

/*               where 0 .LE. X(I) .LT. A for I=0,...,S-1. */
/*    I1MACH( 7) = A, the base. */
/*    I1MACH( 8) = S, the number of base-A digits. */
/*    I1MACH( 9) = A**S - 1, the largest magnitude. */

/*  Floating-Point Numbers. */
/*    Assume floating-point numbers are represented in the T-digit, */
/*    base-B form */
/*               sign (B**E)*( (X(1)/B) + ... + (X(T)/B**T) ) */

/*               where 0 .LE. X(I) .LT. B for I=1,...,T, */
/*               0 .LT. X(1), and EMIN .LE. E .LE. EMAX. */
/*    I1MACH(10) = B, the base. */

/*  Single-Precision */
/*    I1MACH(11) = T, the number of base-B digits. */
/*    I1MACH(12) = EMIN, the smallest exponent E. */
/*    I1MACH(13) = EMAX, the largest exponent E. */

/*  Double-Precision */
/*    I1MACH(14) = T, the number of base-B digits. */
/*    I1MACH(15) = EMIN, the smallest exponent E. */
/*    I1MACH(16) = EMAX, the largest exponent E. */

/*  To alter this function for a particular environment, */
/*  the desired set of DATA statements should be activated by */
/*  removing the C from column 1.  Also, the values of */
/*  I1MACH(1) - I1MACH(4) should be checked for consistency */
/*  with the local operating system. */

/* ***REFERENCES  FOX P.A., HALL A.D., SCHRYER N.L.,*FRAMEWORK FOR A */
/*                 PORTABLE LIBRARY*, ACM TRANSACTIONS ON MATHEMATICAL */
/*                 SOFTWARE, VOL. 4, NO. 2, JUNE 1978, PP. 177-188. */
/* ***ROUTINES CALLED  (NONE) */
/* ***END PROLOGUE  I1MACH */


/*     MACHINE CONSTANTS FOR THE HP 9000 */

/*     DATA IMACH(1)  /    5 / */
/*     DATA IMACH(2)  /    6 / */
/*     DATA IMACH(3)  /    6 / */
/*     DATA IMACH(3)  /    7 / */
/*     DATA IMACH(5)  /   32 / */
/*     DATA IMACH(6)  /    4 / */
/*     TA IMACH(13) /  127 / */
/*     DATA IMACH(14) /   62 / */
/*     DATA IMACH(15) / -128 / */
/*     DATA IMACH(16) /  127 / */

/*     MACHINE CONSTANTS FOR THE SILICON GRAPHICS IRIS */

/*     DATA IMACH( 1) /     5 / */
/*     DATA IMACH( 2) /     6 / */
/*     DATA IMACH( 3) /     6 / */
/*     DATA IMACH( 4) /     0 / */
/*     DATA IMACH( 5) /    32 / */
/*     DATA IMACH( 6) /     4 / */
/*     DATA IMACH( 7) /     2 / */
/*     DATA IMACH( 8) /    31 / */
/*     DATA IMACH( 9) / 2147483647 / */
/*     DATA IMACH(10) /     2 / */
/*     DATA IMACH(11) /    23 / */
/*     DATA IMACH(12) /  -126 / */
/*     DATA IMACH(13) /   127 / */
/*     DATA IMACH(14) /    52 / */
/*     DATA IMACH(15) / -1022 / */
/*     DATA IMACH(16) /  1023 / */

/*     MACHINE CONSTANTS FOR THE SUN */

/*     DATA IMACH(1) /    5 / */
/*     DATA IMACH(2) /    6 / */
/*     DATA IMACH(3) /    6 / */
/*     DATA IMACH(4) /    6 / */
/*     DATA IMACH(5) /   32 / */
/*     DATA IMACH(6) /    4 / */
/*     DATA IMACH(7) /    2 / */
/*     DATA IMACH(8) /   31 / */
/*     DATA IMACH(9) /2147483647 / */
/*     DATA IMACH(10)/    2 / */
/*     DATA IMACH(11)/   24 / */
/*     DATA IMACH(12)/ -125 / */
/*     DATA IMACH(13)/  128 / */
/*     DATA IMACH(14)/   53 / */
/*     DATA IMACH(15)/ -1021 / */
/*     DATA IMACH(16)/  1024 / */

/*     MACHINE CONSTANTS FOR THE VAX 11/780 */

/*     DATA IMACH(1) /    5 / */
/*     DATA IMACH(2) /    6 / */
/*     DATA IMACH(3) /    5 / */
/*     DATA IMACH(4) /    6 / */
/*     DATA IMACH(5) /   32 / */
/*     DATA IMACH(6) /    4 / */
/*     DATA IMACH(7) /    2 / */
/*     DATA IMACH(8) /   31 / */
/*     DATA IMACH(9) /2147483647 / */
/*     DATA IMACH(10)/    2 / */
/*     DATA IMACH(11)/   24 / */
/*     DATA IMACH(12)/ -127 / */
/*     DATA IMACH(13)/  127 / */
/*     DATA IMACH(14)/   56 / */
/*     DATA IMACH(15)/ -127 / */
/*     DATA IMACH(16)/  127 / */

/*     MACHINE CONSTANTS FOR THE VAX 11/780, G-FLOAT OPTION */

/*     DATA IMACH(1) /    5 / */
/*     DATA IMACH(2) /    6 / */
/*     DATA IMACH(3) /    5 / */
/*     DATA IMACH(4) /    6 / */
/*     DATA IMACH(5) /   32 / */
/*     DATA IMACH(6) /    4 / */
/*     DATA IMACH(7) /    2 / */
/*     DATA IMACH(8) /   31 / */
/*     DATA IMACH(9) /2147483647 / */
/*     DATA IMACH(10)/    2 / */
/*     DATA IMACH(11)/   24 / */
/*     DATA IMACH(12)/ -127 / */
/*     DATA IMACH(13)/  127 / */
/*     DATA IMACH(14)/   53 / */
/*     DATA IMACH(15)/ -1022 / */
/*     DATA IMACH(16)/  1023 / */

/* ***FIRST EXECUTABLE STATEMENT  I1MACH */
    if (i < 1 || i > 16) {
	goto L10;
    }

    ret_val = imach[i - 1];
    return ret_val;

L10:
    io__32.ciunit = *output;
    s_wsfe(&io__32);
    e_wsfe();

/*     CALL FDUMP */


    s_stop("", 0L);
} /* i1mach_ */

#undef output
#undef imach





double truncatedFactorial(int n)
{

  double tmp = n + 1.0;
  double tmp1 = 2*n + 2.0;
  double g = dgamln_(tmp) - dgamln_(tmp1);
  return exp(g);
}





