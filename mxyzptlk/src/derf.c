#include <math.h>

#define TRUE  1
#define FALSE 0

#if !defined(derf_h)
#include "derf.h"
#endif /* derf_h */

static double
dSign( double* a, double* b ) {
  double x;
  x = (*a >= 0 ? *a : - *a);
  return( *b >= 0 ? x : -x);
}

static double
derf_internal_( int n__, double* dx ) {
    /* Initialized data */

  static double zero = 0.0;
    
  static double q1[3] = { 215.0588758698612, 91.1649054045149, 15.08279763040779 };
    
  static double p2[8] = { 300.4592610201616, 451.9189537118729,
                          339.3208167343437, 152.9892850469404,
                          43.16222722205674, 7.211758250883094,
                          0.564195517478974, -1.368648573827167e-7 };
    
  static double q2[7] = { 300.4592609569833,790.950925327898,
	                  931.3540948506096,638.9802644656312, 277.5854447439876,
     	                  77.00015293522947,12.78272731962942 };
  
  static double p3[5] = { -0.002996107077035422, -0.04947309106232507,
     	                  -0.2269565935396869,   -0.2786613086096478,
                          -0.02231924597341847 };
  
  static double q3[4] = { 0.01062092305284679, 0.1913089261078298,
	                  1.051675107067932  , 1.987332018171353   };
  
  static double one = 1.;
  static double two = 2.;
  static double four = 4.;
  static double acut = .46875;
  static double const_ = .5641895835477563;
  static double xmax = 8.9;
  static double xunit = 5.9;
  static double p1[4] = { 242.6679552305318,21.97926161829415,
	                  6.996383488619136,-.03560984370181538 };

    /* System generated locals */
  double ret_val, d_1;

    /* Builtin functions */

    /* Local variables */
  int lerf;
  double a, r, t, s, u, y;

  switch(n__) {
    case 1: goto L_derfc;
  }
/*     ****************************************************************** */
/*     ENTRY POINTS ... DERF, DERFC.                                      */
/*     THESE FUNCTIONS ARE COMPUTED FROM THE RATIONAL APPROXIMATIONS OF   */
/*     W.J.CODY, MATHEMATICS OF COMPUTATION, VOLUME 22 (1969), PAGES      */
/*     631-637.                                                           */
/*     FOR ABS(X) LE 0.47 THE BASIC FUNCTION IS ERF.  FOR ABS(X) GT 0.47  */
/*     THE BASIC FUNCTION IS ERFC.  THE FINAL RESULT IS OBTAINED IN TERMS */
/*     OF THE BASIC FUNCTION AS SHOWN IN THE FOLLOWING TABLE, IN WHICH    */
/*     A=ABS(X).                                                          */
/*       FUNCTION     A.LE.(0.47)                   A.GT.(0.47)           */
/*       REQUIRED       (ALL X)          (NEGATIVE X)       (POSITIVE X)  */
/*       ERF(X)         ERF(X)             ERFC(A)-1          1-ERFC(X)   */
/*       ERFC(X)       1-ERF(X)            2-ERFC(A)           ERFC(X)    */
/*     ****************************************************************** */
/*     ( ACUT AND 4.0 ARE CHANGE-OVER POINTS FOR THE RATIONAL APPROXIM-   */
/*     ATIONS. )                                                          */
/*     ( CONST=SQRT(1/PI). )                                              */
/*     ( XMAX=SQRT(-ALOG(RMIN)-10.0), WHERE RMIN IS THE SMALLEST NORMAL-  */
/*     IZED REPRESENTABLE NUMBER.  ERFC(XMAX) IS CLOSE TO THE UNDERFLOW   */
/*     THRESHOLD. )                                                       */
/*     ( XUNIT=SQRT(-ALOG(RELPR)+1.0), WHERE RELPR IS THE SMALLEST NUMBER */
/*     FOR WHICH 1.0+RELPR DIFFERS FROM 1.0.  ERF(XUNIT) IS INDISTIN-     */
/*     GUISHABLE FROM 1.0. )                                              */
/*     ****************************************************************** */
  
/*  ENTRY DERF. */
  
  lerf = TRUE;
  t = *dx;
  a = fabs(t);
  if (a > xunit) {
    ret_val = dSign( &one, &t );
    return ret_val;
  }
  goto L1;

/*  ENTRY DERFC. */

L_derfc:
  
  lerf = FALSE;
  t = *dx;
  a = fabs(t);
  if ( t < -xunit ) {
    ret_val = two;
    return ret_val;
  } else if (t > xmax) {
    ret_val = zero;
    return ret_val;
  }

/*  COMMON CODE. */

L1:
  
/* Computing 2nd power */
  d_1 = t;
  s = d_1 * d_1;
  if (a <= acut) {

/*  SET Y=DERF(X) AND TERMINATE. */

    y = t * (p1[0] + s * (p1[1] + s * (p1[2] + s * p1[3]))) / (q1[0] + s *
	    (q1[1] + s * (q1[2] + s)));
    if (lerf) {
      ret_val = y;
    } else {
      ret_val = one - y;
    }

  } else {

/*  SET Y=DERFC(A) AND TERMINATE. */

    if (a <= four) {
      y = exp(-s) * (p2[0] + a * (p2[1] + a * (p2[2] + a * (p2[3] + a * 
		    (p2[4] + a * (p2[5] + a * (p2[6] + a * p2[7]))))))) /
                    (q2[0] + a * (q2[1] + a * (q2[2] + a * (q2[3] + a *
                    (q2[4] + a * (q2[5] + a * (q2[6] + a)))))));
    } else {
      r = one / a;
      
/* Computing 2nd power */
      
      d_1 = r;
      u = d_1 * d_1;
      y = r * exp(-s) * (const_ + u * (p3[0] + u * (p3[1] + u * (p3[2] + u *
                        (p3[3] + u * p3[4])))) / (q3[0] + u * (q3[1] + u * (
		    q3[2] + u * (q3[3] + u)))));
    }
    if ( lerf ) {
      d_1 = one - y;
      ret_val = dSign( &d_1, &t );
    } else {
      if (t >= zero) {
        ret_val = y;
      } else {
        ret_val = two - y;
      }
    }

  }
  return ret_val;
} /* derf_ */

#ifdef __cplusplus
extern "C"
#endif
double
derf( double dX ) {
  return derf_internal_( 0, &dX );
}

#ifdef __cplusplus
extern "C"
#endif
double
derfc( double dX ) {
  return derf_internal_( 1, &dX );
}
