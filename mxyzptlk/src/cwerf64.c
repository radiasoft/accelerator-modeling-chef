/* cwerf64.f -- translated by f2c (version of 3 February 1990  3:36:42).
   You must link the resulting object file with the libraries:
	-lf2c_F77 -lf2c_I77 -lm -lc   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__2 = 2;

/* DECK  ID>, CWERF64. */
/* Double Complex */ int wwerf_(ret_val, z)
doublecomplex *ret_val;
doublecomplex *z;
{
    /* System generated locals */
    integer i_1;
    doublereal d_1, d_2, d_3;
    doublecomplex z_1, z_2, z_3, z_4, z_5, z_6;

    /* Builtin functions */
    double d_imag();
    void d_cnjg();
    double exp();
    void pow_zi(), z_exp();

    /* Local variables */
    static integer n;
    static doublecomplex r[37], s, t, v;

    static doublereal x, y, xa, ya;
    static doublecomplex zh;
    static doublereal xl;

/*    +     Z,ZH,R(37),S,T,V,W,GCMPLX,GCONJG */
/*     GREAL(W)=DREAL(W) */
/*     GIMAG(W)=DIMAG(W) */
/*     GCONJG(W)=DCONJG(W) */
/*     GCMPLX(X,Y)=DCMPLX(X,Y) */
    x = z->r;
    y = d_imag(z);
    xa = abs(x);
    ya = abs(y);
    if (ya < 7.4000000000000004 && xa < 8.3000000000000007) {
	d_1 = ya + 1.6000000000000001;
	z_1.r = d_1, z_1.i = xa;
	zh.r = z_1.r, zh.i = z_1.i;
	r[36].r = 0., r[36].i = 0.;
	for (n = 36; n >= 1; --n) {
	    d_cnjg(&z_3, &r[n]);
	    d_1 = (doublereal) n;
	    z_2.r = d_1 * z_3.r, z_2.i = d_1 * z_3.i;
	    z_1.r = zh.r + z_2.r, z_1.i = zh.i + z_2.i;
	    t.r = z_1.r, t.i = z_1.i;
/* L1: */
	    i_1 = n - 1;
	    z_2.r = t.r * .5, z_2.i = t.i * .5;
/* Computing 2nd power */
	    d_2 = t.r;
/* Computing 2nd power */
	    d_3 = d_imag(&t);
	    d_1 = d_2 * d_2 + d_3 * d_3;
	    z_1.r = z_2.r / d_1, z_1.i = z_2.i / d_1;
	    r[i_1].r = z_1.r, r[i_1].i = z_1.i;
	}
	xl = 46768052394589056.;
	s.r = 0., s.i = 0.;
	for (n = 33; n >= 1; --n) {
	    xl *= .3125;
/* L2: */
	    i_1 = n - 1;
	    z_2.r = s.r + xl, z_2.i = s.i;
	    z_1.r = r[i_1].r * z_2.r - r[i_1].i * z_2.i, z_1.i = r[i_1].r * 
		    z_2.i + r[i_1].i * z_2.r;
	    s.r = z_1.r, s.i = z_1.i;
	}
	z_1.r = s.r * 1.12837916709551257, z_1.i = s.i * 1.12837916709551257;
	v.r = z_1.r, v.i = z_1.i;
    } else {
	z_1.r = ya, z_1.i = xa;
	zh.r = z_1.r, zh.i = z_1.i;
	r[0].r = 0., r[0].i = 0.;
	for (n = 9; n >= 1; --n) {
	    d_cnjg(&z_3, r);
	    d_1 = (doublereal) n;
	    z_2.r = d_1 * z_3.r, z_2.i = d_1 * z_3.i;
	    z_1.r = zh.r + z_2.r, z_1.i = zh.i + z_2.i;
	    t.r = z_1.r, t.i = z_1.i;
/* L3: */
	    z_2.r = t.r * .5, z_2.i = t.i * .5;
/* Computing 2nd power */
	    d_2 = t.r;
/* Computing 2nd power */
	    d_3 = d_imag(&t);
	    d_1 = d_2 * d_2 + d_3 * d_3;
	    z_1.r = z_2.r / d_1, z_1.i = z_2.i / d_1;
	    r[0].r = z_1.r, r[0].i = z_1.i;
	}
	z_1.r = r[0].r * 1.12837916709551257, z_1.i = r[0].i * 
		1.12837916709551257;
	v.r = z_1.r, v.i = z_1.i;
    }
    if (ya == 0.) {
/* Computing 2nd power */
	d_2 = xa;
	d_1 = exp(-(d_2 * d_2));
	d_3 = d_imag(&v);
	z_1.r = d_1, z_1.i = d_3;
	v.r = z_1.r, v.i = z_1.i;
    }
    if (y < 0.) {
	z_6.r = xa, z_6.i = ya;
	pow_zi(&z_5, &z_6, &c__2);
	z_4.r = -z_5.r, z_4.i = -z_5.i;
	z_exp(&z_3, &z_4);
	d_1 = 2.;
	z_2.r = d_1 * z_3.r, z_2.i = d_1 * z_3.i;
	z_1.r = z_2.r - v.r, z_1.i = z_2.i - v.i;
	v.r = z_1.r, v.i = z_1.i;
	if (x > 0.) {
	    d_cnjg(&z_1, &v);
	    v.r = z_1.r, v.i = z_1.i;
	}
    } else {
	if (x < 0.) {
	    d_cnjg(&z_1, &v);
	    v.r = z_1.r, v.i = z_1.i;
	}
    }
    ret_val->r = v.r, ret_val->i = v.i;
    return ;
} /* wwerf_ */

