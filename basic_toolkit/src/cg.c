/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  File:      cg.c
******
******
******  translated by f2c (version of 3 February 1990  3:36:42).
******  from the eispack library
******
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <math.h>
#include "f2c.h"

/* Subroutine */ int cg_(nm, n, ar, ai, wr, wi, matz, zr, zi, fv1, fv2, fv3, 
	ierr)
integer *nm, *n;
doublereal *ar, *ai, *wr, *wi;
integer *matz;
doublereal *zr, *zi, *fv1, *fv2, *fv3;
integer *ierr;
{
    /* System generated locals */
    integer ar_dim1, ar_offset, ai_dim1, ai_offset, zr_dim1, zr_offset, 
	    zi_dim1, zi_offset;

    /* Local variables */
    extern /* Subroutine */ int cbal_(), corth_(), comqr_(), cbabk2_(), 
	    comqr2_();
    static integer is1, is2;

    /* Parameter adjustments */
    ar_dim1 = *nm;
    ar_offset = ar_dim1 + 1;
    ar -= ar_offset;
    ai_dim1 = *nm;
    ai_offset = ai_dim1 + 1;
    ai -= ai_offset;
    --wr;
    --wi;
    zr_dim1 = *nm;
    zr_offset = zr_dim1 + 1;
    zr -= zr_offset;
    zi_dim1 = *nm;
    zi_offset = zi_dim1 + 1;
    zi -= zi_offset;
    --fv1;
    --fv2;
    --fv3;

    /* Function Body */


/*     this subroutine calls the recommended sequence of */
/*     subroutines from the eigensystem subroutine package (eispack) */
/*     to find the eigenvalues and eigenvectors (if desired) */
/*     of a complex general matrix. */

/*     on input */

/*        nm  must be set to the row dimension of the two-dimensional */
/*        array parameters as declared in the calling program */
/*        dimension statement. */

/*        n  is the order of the matrix  a=(ar,ai). */

/*        ar  and  ai  contain the real and imaginary parts, */
/*        respectively, of the complex general matrix. */

/*        matz  is an integer variable set equal to zero if */
/*        only eigenvalues are desired.  otherwise it is set to */
/*        any non-zero integer for both eigenvalues and eigenvectors. */

/*     on output */

/*        wr  and  wi  contain the real and imaginary parts, */
/*        respectively, of the eigenvalues. */

/*        zr  and  zi  contain the real and imaginary parts, */
/*        respectively, of the eigenvectors if matz is not zero. */

/*        ierr  is an integer output variable set equal to an error */
/*           completion code described in the documentation for comqr */
/*           and comqr2.  the normal completion code is zero. */

/*        fv1, fv2, and  fv3  are temporary storage arrays. */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated august 1983. */

/*     ------------------------------------------------------------------ 
*/

    if (*n <= *nm) {
	goto L10;
    }
    *ierr = *n * 10;
    goto L50;

L10:
    cbal_(nm, n, &ar[ar_offset], &ai[ai_offset], &is1, &is2, &fv1[1]);
    corth_(nm, n, &is1, &is2, &ar[ar_offset], &ai[ai_offset], &fv2[1], &fv3[1]
	    );
    if (*matz != 0) {
	goto L20;
    }
/*     .......... find eigenvalues only .......... */
    comqr_(nm, n, &is1, &is2, &ar[ar_offset], &ai[ai_offset], &wr[1], &wi[1], 
	    ierr);
    goto L50;
/*     .......... find both eigenvalues and eigenvectors .......... */
L20:
    comqr2_(nm, n, &is1, &is2, &fv2[1], &fv3[1], &ar[ar_offset], &ai[
	    ai_offset], &wr[1], &wi[1], &zr[zr_offset], &zi[zi_offset], ierr);

    if (*ierr != 0) {
	goto L50;
    }
    cbabk2_(nm, n, &is1, &is2, &fv1[1], n, &zr[zr_offset], &zi[zi_offset]);
L50:
    return 0;
} /* cg_ */

/* Subroutine */ int cbabk2_(nm, n, low, igh, scale, m, zr, zi)
integer *nm, *n, *low, *igh;
doublereal *scale;
integer *m;
doublereal *zr, *zi;
{
    /* System generated locals */
    integer zr_dim1, zr_offset, zi_dim1, zi_offset, i_1, i_2;

    /* Local variables */
    static integer i, j, k;
    static doublereal s;
    static integer ii;

    /* Parameter adjustments */
    --scale;
    zr_dim1 = *nm;
    zr_offset = zr_dim1 + 1;
    zr -= zr_offset;
    zi_dim1 = *nm;
    zi_offset = zi_dim1 + 1;
    zi -= zi_offset;

    /* Function Body */


/*     this subroutine is a translation of the algol procedure */
/*     cbabk2, which is a complex version of balbak, */
/*     num. math. 13, 293-304(1969) by parlett and reinsch. */
/*     handbook for auto. comp., vol.ii-linear algebra, 315-326(1971). */

/*     this subroutine forms the eigenvectors of a complex general */
/*     matrix by back transforming those of the corresponding */
/*     balanced matrix determined by  cbal. */

/*     on input */

/*        nm must be set to the row dimension of two-dimensional */
/*          array parameters as declared in the calling program */
/*          dimension statement. */

/*        n is the order of the matrix. */

/*        low and igh are integers determined by  cbal. */

/*        scale contains information determining the permutations */
/*          and scaling factors used by  cbal. */

/*        m is the number of eigenvectors to be back transformed. */

/*        zr and zi contain the real and imaginary parts, */
/*          respectively, of the eigenvectors to be */
/*          back transformed in their first m columns. */

/*     on output */

/*        zr and zi contain the real and imaginary parts, */
/*          respectively, of the transformed eigenvectors */
/*          in their first m columns. */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated august 1983. */

/*     ------------------------------------------------------------------ 
*/

    if (*m == 0) {
	goto L200;
    }
    if (*igh == *low) {
	goto L120;
    }

    i_1 = *igh;
    for (i = *low; i <= i_1; ++i) {
	s = scale[i];
/*     .......... left hand eigenvectors are back transformed */
/*                if the foregoing statement is replaced by */
/*                s=1.0d0/scale(i). .......... */
	i_2 = *m;
	for (j = 1; j <= i_2; ++j) {
	    zr[i + j * zr_dim1] *= s;
	    zi[i + j * zi_dim1] *= s;
/* L100: */
	}

/* L110: */
    }
/*     .......... for i=low-1 step -1 until 1, */
/*                igh+1 step 1 until n do -- .......... */
L120:
    i_1 = *n;
    for (ii = 1; ii <= i_1; ++ii) {
	i = ii;
	if (i >= *low && i <= *igh) {
	    goto L140;
	}
	if (i < *low) {
	    i = *low - ii;
	}
	k = (integer) scale[i];
	if (k == i) {
	    goto L140;
	}

	i_2 = *m;
	for (j = 1; j <= i_2; ++j) {
	    s = zr[i + j * zr_dim1];
	    zr[i + j * zr_dim1] = zr[k + j * zr_dim1];
	    zr[k + j * zr_dim1] = s;
	    s = zi[i + j * zi_dim1];
	    zi[i + j * zi_dim1] = zi[k + j * zi_dim1];
	    zi[k + j * zi_dim1] = s;
/* L130: */
	}

L140:
    ;}

L200:
    return 0;
} /* cbabk2_ */

/* Subroutine */ int comqr2_(nm, n, low, igh, ortr, orti, hr, hi, wr, wi, zr, 
	zi, ierr)
integer *nm, *n, *low, *igh;
doublereal *ortr, *orti, *hr, *hi, *wr, *wi, *zr, *zi;
integer *ierr;
{
    /* System generated locals */
    integer hr_dim1, hr_offset, hi_dim1, hi_offset, zr_dim1, zr_offset, 
	    zi_dim1, zi_offset, i_1, i_2, i_3;
    doublereal d_1, d_2, d_3, d_4;

    /* Local variables */
    static integer iend;
    extern /* Subroutine */ int cdiv_();
    static doublereal norm;
    static integer i, j, k, l, m, ii, en, jj, ll, nn;
    static doublereal si, ti, xi, yi, sr, tr, xr, yr;
    extern doublereal pythag_();
    extern /* Subroutine */ int csroot_();
    static integer ip1, lp1, itn, its;
    static doublereal zzi, zzr;
    static integer enm1;
    static doublereal tst1, tst2;

    /* Parameter adjustments */
    --ortr;
    --orti;
    hr_dim1 = *nm;
    hr_offset = hr_dim1 + 1;
    hr -= hr_offset;
    hi_dim1 = *nm;
    hi_offset = hi_dim1 + 1;
    hi -= hi_offset;
    --wr;
    --wi;
    zr_dim1 = *nm;
    zr_offset = zr_dim1 + 1;
    zr -= zr_offset;
    zi_dim1 = *nm;
    zi_offset = zi_dim1 + 1;
    zi -= zi_offset;

    /* Function Body */
/*  MESHED overflow control WITH vectors of isolated roots (10/19/89 BSG) 
*/
/*  MESHED overflow control WITH triangular multiply (10/30/89 BSG) */


/*     this subroutine is a translation of a unitary analogue of the */
/*     algol procedure  comlr2, num. math. 16, 181-204(1970) by peters */
/*     and wilkinson. */
/*     handbook for auto. comp., vol.ii-linear algebra, 372-395(1971). */
/*     the unitary analogue substitutes the qr algorithm of francis */
/*     (comp. jour. 4, 332-345(1962)) for the lr algorithm. */

/*     this subroutine finds the eigenvalues and eigenvectors */
/*     of a complex upper hessenberg matrix by the qr */
/*     method.  the eigenvectors of a complex general matrix */
/*     can also be found if  corth  has been used to reduce */
/*     this general matrix to hessenberg form. */

/*     on input */

/*        nm must be set to the row dimension of two-dimensional */
/*          array parameters as declared in the calling program */
/*          dimension statement. */

/*        n is the order of the matrix. */

/*        low and igh are integers determined by the balancing */
/*          subroutine  cbal.  if  cbal  has not been used, */
/*          set low=1, igh=n. */

/*        ortr and orti contain information about the unitary trans- */
/*          formations used in the reduction by  corth, if performed. */
/*          only elements low through igh are used.  if the eigenvectors 
*/
/*          of the hessenberg matrix are desired, set ortr(j) and */
/*          orti(j) to 0.0d0 for these elements. */

/*        hr and hi contain the real and imaginary parts, */
/*          respectively, of the complex upper hessenberg matrix. */
/*          their lower triangles below the subdiagonal contain further */

/*          information about the transformations which were used in the 
*/
/*          reduction by  corth, if performed.  if the eigenvectors of */
/*          the hessenberg matrix are desired, these elements may be */
/*          arbitrary. */

/*     on output */

/*        ortr, orti, and the upper hessenberg portions of hr and hi */
/*          have been destroyed. */

/*        wr and wi contain the real and imaginary parts, */
/*          respectively, of the eigenvalues.  if an error */
/*          exit is made, the eigenvalues should be correct */
/*          for indices ierr+1,...,n. */

/*        zr and zi contain the real and imaginary parts, */
/*          respectively, of the eigenvectors.  the eigenvectors */
/*          are unnormalized.  if an error exit is made, none of */
/*          the eigenvectors has been found. */

/*        ierr is set to */
/*          zero       for normal return, */
/*          j          if the limit of 30*n iterations is exhausted */
/*                     while the j-th eigenvalue is being sought. */

/*     calls cdiv for complex division. */
/*     calls csroot for complex square root. */
/*     calls pythag for  dsqrt(a*a + b*b) . */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated october 1989. */

/*     ------------------------------------------------------------------ 
*/

    *ierr = 0;
/*     .......... initialize eigenvector matrix .......... */
    i_1 = *n;
    for (j = 1; j <= i_1; ++j) {

	i_2 = *n;
	for (i = 1; i <= i_2; ++i) {
	    zr[i + j * zr_dim1] = 0.;
	    zi[i + j * zi_dim1] = 0.;
/* L100: */
	}
	zr[j + j * zr_dim1] = 1.;
/* L101: */
    }
/*     .......... form the matrix of accumulated transformations */
/*                from the information left by corth .......... */
    iend = *igh - *low - 1;
    if (iend < 0) {
	goto L180;
    } else if (iend == 0) {
	goto L150;
    } else {
	goto L105;
    }
/*     .......... for i=igh-1 step -1 until low+1 do -- .......... */
L105:
    i_1 = iend;
    for (ii = 1; ii <= i_1; ++ii) {
	i = *igh - ii;
	if (ortr[i] == 0. && orti[i] == 0.) {
	    goto L140;
	}
	if (hr[i + (i - 1) * hr_dim1] == 0. && hi[i + (i - 1) * hi_dim1] == 
		0.) {
	    goto L140;
	}
/*     .......... norm below is negative of h formed in corth 
.......... */
	norm = hr[i + (i - 1) * hr_dim1] * ortr[i] + hi[i + (i - 1) * hi_dim1]
		 * orti[i];
	ip1 = i + 1;

	i_2 = *igh;
	for (k = ip1; k <= i_2; ++k) {
	    ortr[k] = hr[k + (i - 1) * hr_dim1];
	    orti[k] = hi[k + (i - 1) * hi_dim1];
/* L110: */
	}

	i_2 = *igh;
	for (j = i; j <= i_2; ++j) {
	    sr = 0.;
	    si = 0.;

	    i_3 = *igh;
	    for (k = i; k <= i_3; ++k) {
		sr = sr + ortr[k] * zr[k + j * zr_dim1] + orti[k] * zi[k + j *
			 zi_dim1];
		si = si + ortr[k] * zi[k + j * zi_dim1] - orti[k] * zr[k + j *
			 zr_dim1];
/* L115: */
	    }

	    sr /= norm;
	    si /= norm;

	    i_3 = *igh;
	    for (k = i; k <= i_3; ++k) {
		zr[k + j * zr_dim1] = zr[k + j * zr_dim1] + sr * ortr[k] - si 
			* orti[k];
		zi[k + j * zi_dim1] = zi[k + j * zi_dim1] + sr * orti[k] + si 
			* ortr[k];
/* L120: */
	    }

/* L130: */
	}

L140:
    ;}
/*     .......... create real subdiagonal elements .......... */
L150:
    l = *low + 1;

    i_1 = *igh;
    for (i = l; i <= i_1; ++i) {
/* Computing MAX */
	i_2 = i + 1;
	ll = min(*igh,i_2);
	if (hi[i + (i - 1) * hi_dim1] == 0.) {
	    goto L170;
	}
	norm = pythag_(&hr[i + (i - 1) * hr_dim1], &hi[i + (i - 1) * hi_dim1])
		;
	yr = hr[i + (i - 1) * hr_dim1] / norm;
	yi = hi[i + (i - 1) * hi_dim1] / norm;
	hr[i + (i - 1) * hr_dim1] = norm;
	hi[i + (i - 1) * hi_dim1] = 0.;

	i_2 = *n;
	for (j = i; j <= i_2; ++j) {
	    si = yr * hi[i + j * hi_dim1] - yi * hr[i + j * hr_dim1];
	    hr[i + j * hr_dim1] = yr * hr[i + j * hr_dim1] + yi * hi[i + j * 
		    hi_dim1];
	    hi[i + j * hi_dim1] = si;
/* L155: */
	}

	i_2 = ll;
	for (j = 1; j <= i_2; ++j) {
	    si = yr * hi[j + i * hi_dim1] + yi * hr[j + i * hr_dim1];
	    hr[j + i * hr_dim1] = yr * hr[j + i * hr_dim1] - yi * hi[j + i * 
		    hi_dim1];
	    hi[j + i * hi_dim1] = si;
/* L160: */
	}

	i_2 = *igh;
	for (j = *low; j <= i_2; ++j) {
	    si = yr * zi[j + i * zi_dim1] + yi * zr[j + i * zr_dim1];
	    zr[j + i * zr_dim1] = yr * zr[j + i * zr_dim1] - yi * zi[j + i * 
		    zi_dim1];
	    zi[j + i * zi_dim1] = si;
/* L165: */
	}

L170:
    ;}
/*     .......... store roots isolated by cbal .......... */
L180:
    i_1 = *n;
    for (i = 1; i <= i_1; ++i) {
	if (i >= *low && i <= *igh) {
	    goto L200;
	}
	wr[i] = hr[i + i * hr_dim1];
	wi[i] = hi[i + i * hi_dim1];
L200:
    ;}

    en = *igh;
    tr = 0.;
    ti = 0.;
    itn = *n * 30;
/*     .......... search for next eigenvalue .......... */
L220:
    if (en < *low) {
	goto L680;
    }
    its = 0;
    enm1 = en - 1;
/*     .......... look for single small sub-diagonal element */
/*                for l=en step -1 until low do -- .......... */
L240:
    i_1 = en;
    for (ll = *low; ll <= i_1; ++ll) {
	l = en + *low - ll;
	if (l == *low) {
	    goto L300;
	}
	tst1 = (d_1 = hr[l - 1 + (l - 1) * hr_dim1], abs(d_1)) + (d_2 = hi[l 
		- 1 + (l - 1) * hi_dim1], abs(d_2)) + (d_3 = hr[l + l * 
		hr_dim1], abs(d_3)) + (d_4 = hi[l + l * hi_dim1], abs(d_4));
	tst2 = tst1 + (d_1 = hr[l + (l - 1) * hr_dim1], abs(d_1));
	if (tst2 == tst1) {
	    goto L300;
	}
/* L260: */
    }
/*     .......... form shift .......... */
L300:
    if (l == en) {
	goto L660;
    }
    if (itn == 0) {
	goto L1000;
    }
    if (its == 10 || its == 20) {
	goto L320;
    }
    sr = hr[en + en * hr_dim1];
    si = hi[en + en * hi_dim1];
    xr = hr[enm1 + en * hr_dim1] * hr[en + enm1 * hr_dim1];
    xi = hi[enm1 + en * hi_dim1] * hr[en + enm1 * hr_dim1];
    if (xr == 0. && xi == 0.) {
	goto L340;
    }
    yr = (hr[enm1 + enm1 * hr_dim1] - sr) / 2.;
    yi = (hi[enm1 + enm1 * hi_dim1] - si) / 2.;
/* Computing 2nd power */
    d_2 = yr;
/* Computing 2nd power */
    d_3 = yi;
    d_1 = d_2 * d_2 - d_3 * d_3 + xr;
    d_4 = yr * 2. * yi + xi;
    csroot_(&d_1, &d_4, &zzr, &zzi);
    if (yr * zzr + yi * zzi >= 0.) {
	goto L310;
    }
    zzr = -zzr;
    zzi = -zzi;
L310:
    d_1 = yr + zzr;
    d_2 = yi + zzi;
    cdiv_(&xr, &xi, &d_1, &d_2, &xr, &xi);
    sr -= xr;
    si -= xi;
    goto L340;
/*     .......... form exceptional shift .......... */
L320:
    sr = (d_1 = hr[en + enm1 * hr_dim1], abs(d_1)) + (d_2 = hr[enm1 + (en - 2)
	     * hr_dim1], abs(d_2));
    si = 0.;

L340:
    i_1 = en;
    for (i = *low; i <= i_1; ++i) {
	hr[i + i * hr_dim1] -= sr;
	hi[i + i * hi_dim1] -= si;
/* L360: */
    }

    tr += sr;
    ti += si;
    ++its;
    --itn;
/*     .......... reduce to triangle (rows) .......... */
    lp1 = l + 1;

    i_1 = en;
    for (i = lp1; i <= i_1; ++i) {
	sr = hr[i + (i - 1) * hr_dim1];
	hr[i + (i - 1) * hr_dim1] = 0.;
	d_1 = pythag_(&hr[i - 1 + (i - 1) * hr_dim1], &hi[i - 1 + (i - 1) * 
		hi_dim1]);
	norm = pythag_(&d_1, &sr);
	xr = hr[i - 1 + (i - 1) * hr_dim1] / norm;
	wr[i - 1] = xr;
	xi = hi[i - 1 + (i - 1) * hi_dim1] / norm;
	wi[i - 1] = xi;
	hr[i - 1 + (i - 1) * hr_dim1] = norm;
	hi[i - 1 + (i - 1) * hi_dim1] = 0.;
	hi[i + (i - 1) * hi_dim1] = sr / norm;

	i_2 = *n;
	for (j = i; j <= i_2; ++j) {
	    yr = hr[i - 1 + j * hr_dim1];
	    yi = hi[i - 1 + j * hi_dim1];
	    zzr = hr[i + j * hr_dim1];
	    zzi = hi[i + j * hi_dim1];
	    hr[i - 1 + j * hr_dim1] = xr * yr + xi * yi + hi[i + (i - 1) * 
		    hi_dim1] * zzr;
	    hi[i - 1 + j * hi_dim1] = xr * yi - xi * yr + hi[i + (i - 1) * 
		    hi_dim1] * zzi;
	    hr[i + j * hr_dim1] = xr * zzr - xi * zzi - hi[i + (i - 1) * 
		    hi_dim1] * yr;
	    hi[i + j * hi_dim1] = xr * zzi + xi * zzr - hi[i + (i - 1) * 
		    hi_dim1] * yi;
/* L490: */
	}

/* L500: */
    }

    si = hi[en + en * hi_dim1];
    if (si == 0.) {
	goto L540;
    }
    norm = pythag_(&hr[en + en * hr_dim1], &si);
    sr = hr[en + en * hr_dim1] / norm;
    si /= norm;
    hr[en + en * hr_dim1] = norm;
    hi[en + en * hi_dim1] = 0.;
    if (en == *n) {
	goto L540;
    }
    ip1 = en + 1;

    i_1 = *n;
    for (j = ip1; j <= i_1; ++j) {
	yr = hr[en + j * hr_dim1];
	yi = hi[en + j * hi_dim1];
	hr[en + j * hr_dim1] = sr * yr + si * yi;
	hi[en + j * hi_dim1] = sr * yi - si * yr;
/* L520: */
    }
/*     .......... inverse operation (columns) .......... */
L540:
    i_1 = en;
    for (j = lp1; j <= i_1; ++j) {
	xr = wr[j - 1];
	xi = wi[j - 1];

	i_2 = j;
	for (i = 1; i <= i_2; ++i) {
	    yr = hr[i + (j - 1) * hr_dim1];
	    yi = 0.;
	    zzr = hr[i + j * hr_dim1];
	    zzi = hi[i + j * hi_dim1];
	    if (i == j) {
		goto L560;
	    }
	    yi = hi[i + (j - 1) * hi_dim1];
	    hi[i + (j - 1) * hi_dim1] = xr * yi + xi * yr + hi[j + (j - 1) * 
		    hi_dim1] * zzi;
L560:
	    hr[i + (j - 1) * hr_dim1] = xr * yr - xi * yi + hi[j + (j - 1) * 
		    hi_dim1] * zzr;
	    hr[i + j * hr_dim1] = xr * zzr + xi * zzi - hi[j + (j - 1) * 
		    hi_dim1] * yr;
	    hi[i + j * hi_dim1] = xr * zzi - xi * zzr - hi[j + (j - 1) * 
		    hi_dim1] * yi;
/* L580: */
	}

	i_2 = *igh;
	for (i = *low; i <= i_2; ++i) {
	    yr = zr[i + (j - 1) * zr_dim1];
	    yi = zi[i + (j - 1) * zi_dim1];
	    zzr = zr[i + j * zr_dim1];
	    zzi = zi[i + j * zi_dim1];
	    zr[i + (j - 1) * zr_dim1] = xr * yr - xi * yi + hi[j + (j - 1) * 
		    hi_dim1] * zzr;
	    zi[i + (j - 1) * zi_dim1] = xr * yi + xi * yr + hi[j + (j - 1) * 
		    hi_dim1] * zzi;
	    zr[i + j * zr_dim1] = xr * zzr + xi * zzi - hi[j + (j - 1) * 
		    hi_dim1] * yr;
	    zi[i + j * zi_dim1] = xr * zzi - xi * zzr - hi[j + (j - 1) * 
		    hi_dim1] * yi;
/* L590: */
	}

/* L600: */
    }

    if (si == 0.) {
	goto L240;
    }

    i_1 = en;
    for (i = 1; i <= i_1; ++i) {
	yr = hr[i + en * hr_dim1];
	yi = hi[i + en * hi_dim1];
	hr[i + en * hr_dim1] = sr * yr - si * yi;
	hi[i + en * hi_dim1] = sr * yi + si * yr;
/* L630: */
    }

    i_1 = *igh;
    for (i = *low; i <= i_1; ++i) {
	yr = zr[i + en * zr_dim1];
	yi = zi[i + en * zi_dim1];
	zr[i + en * zr_dim1] = sr * yr - si * yi;
	zi[i + en * zi_dim1] = sr * yi + si * yr;
/* L640: */
    }

    goto L240;
/*     .......... a root found .......... */
L660:
    hr[en + en * hr_dim1] += tr;
    wr[en] = hr[en + en * hr_dim1];
    hi[en + en * hi_dim1] += ti;
    wi[en] = hi[en + en * hi_dim1];
    en = enm1;
    goto L220;
/*     .......... all roots found.  backsubstitute to find */
/*                vectors of upper triangular form .......... */
L680:
    norm = 0.;

    i_1 = *n;
    for (i = 1; i <= i_1; ++i) {

	i_2 = *n;
	for (j = i; j <= i_2; ++j) {
	    tr = (d_1 = hr[i + j * hr_dim1], abs(d_1)) + (d_2 = hi[i + j * 
		    hi_dim1], abs(d_2));
	    if (tr > norm) {
		norm = tr;
	    }
/* L720: */
	}
    }

    if (*n == 1 || norm == 0.) {
	goto L1001;
    }
/*     .......... for en=n step -1 until 2 do -- .......... */
    i_2 = *n;
    for (nn = 2; nn <= i_2; ++nn) {
	en = *n + 2 - nn;
	xr = wr[en];
	xi = wi[en];
	hr[en + en * hr_dim1] = 1.;
	hi[en + en * hi_dim1] = 0.;
	enm1 = en - 1;
/*     .......... for i=en-1 step -1 until 1 do -- .......... */
	i_1 = enm1;
	for (ii = 1; ii <= i_1; ++ii) {
	    i = en - ii;
	    zzr = 0.;
	    zzi = 0.;
	    ip1 = i + 1;

	    i_3 = en;
	    for (j = ip1; j <= i_3; ++j) {
		zzr = zzr + hr[i + j * hr_dim1] * hr[j + en * hr_dim1] - hi[i 
			+ j * hi_dim1] * hi[j + en * hi_dim1];
		zzi = zzi + hr[i + j * hr_dim1] * hi[j + en * hi_dim1] + hi[i 
			+ j * hi_dim1] * hr[j + en * hr_dim1];
/* L740: */
	    }

	    yr = xr - wr[i];
	    yi = xi - wi[i];
	    if (yr != 0. || yi != 0.) {
		goto L765;
	    }
	    tst1 = norm;
	    yr = tst1;
L760:
	    yr *= .01;
	    tst2 = norm + yr;
	    if (tst2 > tst1) {
		goto L760;
	    }
L765:
	    cdiv_(&zzr, &zzi, &yr, &yi, &hr[i + en * hr_dim1], &hi[i + en * 
		    hi_dim1]);
/*     .......... overflow control .......... */
	    tr = (d_1 = hr[i + en * hr_dim1], abs(d_1)) + (d_2 = hi[i + en * 
		    hi_dim1], abs(d_2));
	    if (tr == 0.) {
		goto L780;
	    }
	    tst1 = tr;
	    tst2 = tst1 + 1. / tst1;
	    if (tst2 > tst1) {
		goto L780;
	    }
	    i_3 = en;
	    for (j = i; j <= i_3; ++j) {
		hr[j + en * hr_dim1] /= tr;
		hi[j + en * hi_dim1] /= tr;
/* L770: */
	    }

L780:
	;}

/* L800: */
    }
/*     .......... end backsubstitution .......... */
/*     .......... vectors of isolated roots .......... */
    i_2 = *n;
    for (i = 1; i <= i_2; ++i) {
	if (i >= *low && i <= *igh) {
	    goto L840;
	}

	i_1 = *n;
	for (j = i; j <= i_1; ++j) {
	    zr[i + j * zr_dim1] = hr[i + j * hr_dim1];
	    zi[i + j * zi_dim1] = hi[i + j * hi_dim1];
/* L820: */
	}

L840:
    ;}
/*     .......... multiply by transformation matrix to give */
/*                vectors of original full matrix. */
/*                for j=n step -1 until low do -- .......... */
    i_2 = *n;
    for (jj = *low; jj <= i_2; ++jj) {
	j = *n + *low - jj;
	m = min(j,*igh);

	i_1 = *igh;
	for (i = *low; i <= i_1; ++i) {
	    zzr = 0.;
	    zzi = 0.;

	    i_3 = m;
	    for (k = *low; k <= i_3; ++k) {
		zzr = zzr + zr[i + k * zr_dim1] * hr[k + j * hr_dim1] - zi[i 
			+ k * zi_dim1] * hi[k + j * hi_dim1];
		zzi = zzi + zr[i + k * zr_dim1] * hi[k + j * hi_dim1] + zi[i 
			+ k * zi_dim1] * hr[k + j * hr_dim1];
/* L860: */
	    }

	    zr[i + j * zr_dim1] = zzr;
	    zi[i + j * zi_dim1] = zzi;
/* L880: */
	}
    }

    goto L1001;
/*     .......... set error -- all eigenvalues have not */
/*                converged after 30*n iterations .......... */
L1000:
    *ierr = en;
L1001:
    return 0;
} /* comqr2_ */

/* Subroutine */ int comqr_(nm, n, low, igh, hr, hi, wr, wi, ierr)
integer *nm, *n, *low, *igh;
doublereal *hr, *hi, *wr, *wi;
integer *ierr;
{
    /* System generated locals */
    integer hr_dim1, hr_offset, hi_dim1, hi_offset, i_1, i_2;
    doublereal d_1, d_2, d_3, d_4;

    /* Local variables */
    extern /* Subroutine */ int cdiv_();
    static doublereal norm;
    static integer i, j, l, en, ll;
    static doublereal si, ti, xi, yi, sr, tr, xr, yr;
    extern doublereal pythag_();
    extern /* Subroutine */ int csroot_();
    static integer lp1, itn, its;
    static doublereal zzi, zzr;
    static integer enm1;
    static doublereal tst1, tst2;

    /* Parameter adjustments */
    hr_dim1 = *nm;
    hr_offset = hr_dim1 + 1;
    hr -= hr_offset;
    hi_dim1 = *nm;
    hi_offset = hi_dim1 + 1;
    hi -= hi_offset;
    --wr;
    --wi;

    /* Function Body */


/*     this subroutine is a translation of a unitary analogue of the */
/*     algol procedure  comlr, num. math. 12, 369-376(1968) by martin */
/*     and wilkinson. */
/*     handbook for auto. comp., vol.ii-linear algebra, 396-403(1971). */
/*     the unitary analogue substitutes the qr algorithm of francis */
/*     (comp. jour. 4, 332-345(1962)) for the lr algorithm. */

/*     this subroutine finds the eigenvalues of a complex */
/*     upper hessenberg matrix by the qr method. */

/*     on input */

/*        nm must be set to the row dimension of two-dimensional */
/*          array parameters as declared in the calling program */
/*          dimension statement. */

/*        n is the order of the matrix. */

/*        low and igh are integers determined by the balancing */
/*          subroutine  cbal.  if  cbal  has not been used, */
/*          set low=1, igh=n. */

/*        hr and hi contain the real and imaginary parts, */
/*          respectively, of the complex upper hessenberg matrix. */
/*          their lower triangles below the subdiagonal contain */
/*          information about the unitary transformations used in */
/*          the reduction by  corth, if performed. */

/*     on output */

/*        the upper hessenberg portions of hr and hi have been */
/*          destroyed.  therefore, they must be saved before */
/*          calling  comqr  if subsequent calculation of */
/*          eigenvectors is to be performed. */

/*        wr and wi contain the real and imaginary parts, */
/*          respectively, of the eigenvalues.  if an error */
/*          exit is made, the eigenvalues should be correct */
/*          for indices ierr+1,...,n. */

/*        ierr is set to */
/*          zero       for normal return, */
/*          j          if the limit of 30*n iterations is exhausted */
/*                     while the j-th eigenvalue is being sought. */

/*     calls cdiv for complex division. */
/*     calls csroot for complex square root. */
/*     calls pythag for  dsqrt(a*a + b*b) . */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated august 1983. */

/*     ------------------------------------------------------------------ 
*/

    *ierr = 0;
    if (*low == *igh) {
	goto L180;
    }
/*     .......... create real subdiagonal elements .......... */
    l = *low + 1;

    i_1 = *igh;
    for (i = l; i <= i_1; ++i) {
/* Computing MAX */
	i_2 = i + 1;
	ll = min(*igh,i_2);
	if (hi[i + (i - 1) * hi_dim1] == 0.) {
	    goto L170;
	}
	norm = pythag_(&hr[i + (i - 1) * hr_dim1], &hi[i + (i - 1) * hi_dim1])
		;
	yr = hr[i + (i - 1) * hr_dim1] / norm;
	yi = hi[i + (i - 1) * hi_dim1] / norm;
	hr[i + (i - 1) * hr_dim1] = norm;
	hi[i + (i - 1) * hi_dim1] = 0.;

	i_2 = *igh;
	for (j = i; j <= i_2; ++j) {
	    si = yr * hi[i + j * hi_dim1] - yi * hr[i + j * hr_dim1];
	    hr[i + j * hr_dim1] = yr * hr[i + j * hr_dim1] + yi * hi[i + j * 
		    hi_dim1];
	    hi[i + j * hi_dim1] = si;
/* L155: */
	}

	i_2 = ll;
	for (j = *low; j <= i_2; ++j) {
	    si = yr * hi[j + i * hi_dim1] + yi * hr[j + i * hr_dim1];
	    hr[j + i * hr_dim1] = yr * hr[j + i * hr_dim1] - yi * hi[j + i * 
		    hi_dim1];
	    hi[j + i * hi_dim1] = si;
/* L160: */
	}

L170:
    ;}
/*     .......... store roots isolated by cbal .......... */
L180:
    i_1 = *n;
    for (i = 1; i <= i_1; ++i) {
	if (i >= *low && i <= *igh) {
	    goto L200;
	}
	wr[i] = hr[i + i * hr_dim1];
	wi[i] = hi[i + i * hi_dim1];
L200:
    ;}

    en = *igh;
    tr = 0.;
    ti = 0.;
    itn = *n * 30;
/*     .......... search for next eigenvalue .......... */
L220:
    if (en < *low) {
	goto L1001;
    }
    its = 0;
    enm1 = en - 1;
/*     .......... look for single small sub-diagonal element */
/*                for l=en step -1 until low d0 -- .......... */
L240:
    i_1 = en;
    for (ll = *low; ll <= i_1; ++ll) {
	l = en + *low - ll;
	if (l == *low) {
	    goto L300;
	}
	tst1 = (d_1 = hr[l - 1 + (l - 1) * hr_dim1], abs(d_1)) + (d_2 = hi[l 
		- 1 + (l - 1) * hi_dim1], abs(d_2)) + (d_3 = hr[l + l * 
		hr_dim1], abs(d_3)) + (d_4 = hi[l + l * hi_dim1], abs(d_4));
	tst2 = tst1 + (d_1 = hr[l + (l - 1) * hr_dim1], abs(d_1));
	if (tst2 == tst1) {
	    goto L300;
	}
/* L260: */
    }
/*     .......... form shift .......... */
L300:
    if (l == en) {
	goto L660;
    }
    if (itn == 0) {
	goto L1000;
    }
    if (its == 10 || its == 20) {
	goto L320;
    }
    sr = hr[en + en * hr_dim1];
    si = hi[en + en * hi_dim1];
    xr = hr[enm1 + en * hr_dim1] * hr[en + enm1 * hr_dim1];
    xi = hi[enm1 + en * hi_dim1] * hr[en + enm1 * hr_dim1];
    if (xr == 0. && xi == 0.) {
	goto L340;
    }
    yr = (hr[enm1 + enm1 * hr_dim1] - sr) / 2.;
    yi = (hi[enm1 + enm1 * hi_dim1] - si) / 2.;
/* Computing 2nd power */
    d_2 = yr;
/* Computing 2nd power */
    d_3 = yi;
    d_1 = d_2 * d_2 - d_3 * d_3 + xr;
    d_4 = yr * 2. * yi + xi;
    csroot_(&d_1, &d_4, &zzr, &zzi);
    if (yr * zzr + yi * zzi >= 0.) {
	goto L310;
    }
    zzr = -zzr;
    zzi = -zzi;
L310:
    d_1 = yr + zzr;
    d_2 = yi + zzi;
    cdiv_(&xr, &xi, &d_1, &d_2, &xr, &xi);
    sr -= xr;
    si -= xi;
    goto L340;
/*     .......... form exceptional shift .......... */
L320:
    sr = (d_1 = hr[en + enm1 * hr_dim1], abs(d_1)) + (d_2 = hr[enm1 + (en - 2)
	     * hr_dim1], abs(d_2));
    si = 0.;

L340:
    i_1 = en;
    for (i = *low; i <= i_1; ++i) {
	hr[i + i * hr_dim1] -= sr;
	hi[i + i * hi_dim1] -= si;
/* L360: */
    }

    tr += sr;
    ti += si;
    ++its;
    --itn;
/*     .......... reduce to triangle (rows) .......... */
    lp1 = l + 1;

    i_1 = en;
    for (i = lp1; i <= i_1; ++i) {
	sr = hr[i + (i - 1) * hr_dim1];
	hr[i + (i - 1) * hr_dim1] = 0.;
	d_1 = pythag_(&hr[i - 1 + (i - 1) * hr_dim1], &hi[i - 1 + (i - 1) * 
		hi_dim1]);
	norm = pythag_(&d_1, &sr);
	xr = hr[i - 1 + (i - 1) * hr_dim1] / norm;
	wr[i - 1] = xr;
	xi = hi[i - 1 + (i - 1) * hi_dim1] / norm;
	wi[i - 1] = xi;
	hr[i - 1 + (i - 1) * hr_dim1] = norm;
	hi[i - 1 + (i - 1) * hi_dim1] = 0.;
	hi[i + (i - 1) * hi_dim1] = sr / norm;

	i_2 = en;
	for (j = i; j <= i_2; ++j) {
	    yr = hr[i - 1 + j * hr_dim1];
	    yi = hi[i - 1 + j * hi_dim1];
	    zzr = hr[i + j * hr_dim1];
	    zzi = hi[i + j * hi_dim1];
	    hr[i - 1 + j * hr_dim1] = xr * yr + xi * yi + hi[i + (i - 1) * 
		    hi_dim1] * zzr;
	    hi[i - 1 + j * hi_dim1] = xr * yi - xi * yr + hi[i + (i - 1) * 
		    hi_dim1] * zzi;
	    hr[i + j * hr_dim1] = xr * zzr - xi * zzi - hi[i + (i - 1) * 
		    hi_dim1] * yr;
	    hi[i + j * hi_dim1] = xr * zzi + xi * zzr - hi[i + (i - 1) * 
		    hi_dim1] * yi;
/* L490: */
	}

/* L500: */
    }

    si = hi[en + en * hi_dim1];
    if (si == 0.) {
	goto L540;
    }
    norm = pythag_(&hr[en + en * hr_dim1], &si);
    sr = hr[en + en * hr_dim1] / norm;
    si /= norm;
    hr[en + en * hr_dim1] = norm;
    hi[en + en * hi_dim1] = 0.;
/*     .......... inverse operation (columns) .......... */
L540:
    i_1 = en;
    for (j = lp1; j <= i_1; ++j) {
	xr = wr[j - 1];
	xi = wi[j - 1];

	i_2 = j;
	for (i = l; i <= i_2; ++i) {
	    yr = hr[i + (j - 1) * hr_dim1];
	    yi = 0.;
	    zzr = hr[i + j * hr_dim1];
	    zzi = hi[i + j * hi_dim1];
	    if (i == j) {
		goto L560;
	    }
	    yi = hi[i + (j - 1) * hi_dim1];
	    hi[i + (j - 1) * hi_dim1] = xr * yi + xi * yr + hi[j + (j - 1) * 
		    hi_dim1] * zzi;
L560:
	    hr[i + (j - 1) * hr_dim1] = xr * yr - xi * yi + hi[j + (j - 1) * 
		    hi_dim1] * zzr;
	    hr[i + j * hr_dim1] = xr * zzr + xi * zzi - hi[j + (j - 1) * 
		    hi_dim1] * yr;
	    hi[i + j * hi_dim1] = xr * zzi - xi * zzr - hi[j + (j - 1) * 
		    hi_dim1] * yi;
/* L580: */
	}

/* L600: */
    }

    if (si == 0.) {
	goto L240;
    }

    i_1 = en;
    for (i = l; i <= i_1; ++i) {
	yr = hr[i + en * hr_dim1];
	yi = hi[i + en * hi_dim1];
	hr[i + en * hr_dim1] = sr * yr - si * yi;
	hi[i + en * hi_dim1] = sr * yi + si * yr;
/* L630: */
    }

    goto L240;
/*     .......... a root found .......... */
L660:
    wr[en] = hr[en + en * hr_dim1] + tr;
    wi[en] = hi[en + en * hi_dim1] + ti;
    en = enm1;
    goto L220;
/*     .......... set error -- all eigenvalues have not */
/*                converged after 30*n iterations .......... */
L1000:
    *ierr = en;
L1001:
    return 0;
} /* comqr_ */


/* Subroutine */ int csroot_(xr, xi, yr, yi)
doublereal *xr, *xi, *yr, *yi;
{
    /* Builtin functions */
    /*double sqrt(); */

    /* Local variables */
    static doublereal s, ti, tr;
    extern doublereal pythag_();


/*     (yr,yi) = complex dsqrt(xr,xi) */
/*     branch chosen so that yr .ge. 0.0 and sign(yi) .eq. sign(xi) */

    tr = *xr;
    ti = *xi;
    s = sqrt((pythag_(&tr, &ti) + abs(tr)) * .5);
    if (tr >= 0.) {
	*yr = s;
    }
    if (ti < 0.) {
	s = -s;
    }
    if (tr <= 0.) {
	*yi = s;
    }
    if (tr < 0.) {
	*yr = ti / *yi * .5;
    }
    if (tr > 0.) {
	*yi = ti / *yr * .5;
    }
    return 0;
} /* csroot_ */

/* Subroutine */ int corth_(nm, n, low, igh, ar, ai, ortr, orti)
integer *nm, *n, *low, *igh;
doublereal *ar, *ai, *ortr, *orti;
{
    /* System generated locals */
    integer ar_dim1, ar_offset, ai_dim1, ai_offset, i_1, i_2, i_3;
    doublereal d_1, d_2;

    /* Builtin functions */
    /* double sqrt(); */

    /* Local variables */
    static doublereal f, g, h;
    static integer i, j, m;
    static doublereal scale;
    static integer la;
    static doublereal fi;
    static integer ii, jj;
    static doublereal fr;
    static integer mp;
    extern doublereal pythag_();
    static integer kp1;

    /* Parameter adjustments */
    ar_dim1 = *nm;
    ar_offset = ar_dim1 + 1;
    ar -= ar_offset;
    ai_dim1 = *nm;
    ai_offset = ai_dim1 + 1;
    ai -= ai_offset;
    --ortr;
    --orti;

    /* Function Body */


/*     this subroutine is a translation of a complex analogue of */
/*     the algol procedure orthes, num. math. 12, 349-368(1968) */
/*     by martin and wilkinson. */
/*     handbook for auto. comp., vol.ii-linear algebra, 339-358(1971). */

/*     given a complex general matrix, this subroutine */
/*     reduces a submatrix situated in rows and columns */
/*     low through igh to upper hessenberg form by */
/*     unitary similarity transformations. */

/*     on input */

/*        nm must be set to the row dimension of two-dimensional */
/*          array parameters as declared in the calling program */
/*          dimension statement. */

/*        n is the order of the matrix. */

/*        low and igh are integers determined by the balancing */
/*          subroutine  cbal.  if  cbal  has not been used, */
/*          set low=1, igh=n. */

/*        ar and ai contain the real and imaginary parts, */
/*          respectively, of the complex input matrix. */

/*     on output */

/*        ar and ai contain the real and imaginary parts, */
/*          respectively, of the hessenberg matrix.  information */
/*          about the unitary transformations used in the reduction */
/*          is stored in the remaining triangles under the */
/*          hessenberg matrix. */

/*        ortr and orti contain further information about the */
/*          transformations.  only elements low through igh are used. */

/*     calls pythag for  dsqrt(a*a + b*b) . */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated august 1983. */

/*     ------------------------------------------------------------------ 
*/

    la = *igh - 1;
    kp1 = *low + 1;
    if (la < kp1) {
	goto L200;
    }

    i_1 = la;
    for (m = kp1; m <= i_1; ++m) {
	h = 0.;
	ortr[m] = 0.;
	orti[m] = 0.;
	scale = 0.;
/*     .......... scale column (algol tol then not needed) .......... 
*/
	i_2 = *igh;
	for (i = m; i <= i_2; ++i) {
/* L90: */
	    scale = scale + (d_1 = ar[i + (m - 1) * ar_dim1], abs(d_1)) + (
		    d_2 = ai[i + (m - 1) * ai_dim1], abs(d_2));
	}

	if (scale == 0.) {
	    goto L180;
	}
	mp = m + *igh;
/*     .......... for i=igh step -1 until m do -- .......... */
	i_2 = *igh;
	for (ii = m; ii <= i_2; ++ii) {
	    i = mp - ii;
	    ortr[i] = ar[i + (m - 1) * ar_dim1] / scale;
	    orti[i] = ai[i + (m - 1) * ai_dim1] / scale;
	    h = h + ortr[i] * ortr[i] + orti[i] * orti[i];
/* L100: */
	}

	g = sqrt(h);
	f = pythag_(&ortr[m], &orti[m]);
	if (f == 0.) {
	    goto L103;
	}
	h += f * g;
	g /= f;
	ortr[m] = (g + 1.) * ortr[m];
	orti[m] = (g + 1.) * orti[m];
	goto L105;

L103:
	ortr[m] = g;
	ar[m + (m - 1) * ar_dim1] = scale;
/*     .......... form (i-(u*ut)/h) * a .......... */
L105:
	i_2 = *n;
	for (j = m; j <= i_2; ++j) {
	    fr = 0.;
	    fi = 0.;
/*     .......... for i=igh step -1 until m do -- .......... */
	    i_3 = *igh;
	    for (ii = m; ii <= i_3; ++ii) {
		i = mp - ii;
		fr = fr + ortr[i] * ar[i + j * ar_dim1] + orti[i] * ai[i + j *
			 ai_dim1];
		fi = fi + ortr[i] * ai[i + j * ai_dim1] - orti[i] * ar[i + j *
			 ar_dim1];
/* L110: */
	    }

	    fr /= h;
	    fi /= h;

	    i_3 = *igh;
	    for (i = m; i <= i_3; ++i) {
		ar[i + j * ar_dim1] = ar[i + j * ar_dim1] - fr * ortr[i] + fi 
			* orti[i];
		ai[i + j * ai_dim1] = ai[i + j * ai_dim1] - fr * orti[i] - fi 
			* ortr[i];
/* L120: */
	    }

/* L130: */
	}
/*     .......... form (i-(u*ut)/h)*a*(i-(u*ut)/h) .......... */
	i_2 = *igh;
	for (i = 1; i <= i_2; ++i) {
	    fr = 0.;
	    fi = 0.;
/*     .......... for j=igh step -1 until m do -- .......... */
	    i_3 = *igh;
	    for (jj = m; jj <= i_3; ++jj) {
		j = mp - jj;
		fr = fr + ortr[j] * ar[i + j * ar_dim1] - orti[j] * ai[i + j *
			 ai_dim1];
		fi = fi + ortr[j] * ai[i + j * ai_dim1] + orti[j] * ar[i + j *
			 ar_dim1];
/* L140: */
	    }

	    fr /= h;
	    fi /= h;

	    i_3 = *igh;
	    for (j = m; j <= i_3; ++j) {
		ar[i + j * ar_dim1] = ar[i + j * ar_dim1] - fr * ortr[j] - fi 
			* orti[j];
		ai[i + j * ai_dim1] = ai[i + j * ai_dim1] + fr * orti[j] - fi 
			* ortr[j];
/* L150: */
	    }

/* L160: */
	}

	ortr[m] = scale * ortr[m];
	orti[m] = scale * orti[m];
	ar[m + (m - 1) * ar_dim1] = -g * ar[m + (m - 1) * ar_dim1];
	ai[m + (m - 1) * ai_dim1] = -g * ai[m + (m - 1) * ai_dim1];
L180:
    ;}

L200:
    return 0;
} /* corth_ */

doublereal pythag_(a, b)
doublereal *a, *b;
{
    /* System generated locals */
    doublereal ret_val, d_1, d_2, d_3;

    /* Local variables */
    static doublereal p, r, s, t, u;


/*     finds dsqrt(a**2+b**2) without overflow or destructive underflow */


/* Computing MAX */
    d_1 = abs(*a), d_2 = abs(*b);
    p = max(d_2,d_1);
    if (p == 0.) {
	goto L20;
    }
/* Computing MAX */
    d_2 = abs(*a), d_3 = abs(*b);
/* Computing 2nd power */
    d_1 = min(d_3,d_2) / p;
    r = d_1 * d_1;
L10:
    t = r + 4.;
    if (t == 4.) {
	goto L20;
    }
    s = r / t;
    u = s * 2. + 1.;
    p = u * p;
/* Computing 2nd power */
    d_1 = s / u;
    r = d_1 * d_1 * r;
    goto L10;
L20:
    ret_val = p;
    return ret_val;
} /* pythag_ */

/* Subroutine */ int cbal_(nm, n, ar, ai, low, igh, scale)
integer *nm, *n;
doublereal *ar, *ai;
integer *low, *igh;
doublereal *scale;
{
    /* System generated locals */
    integer ar_dim1, ar_offset, ai_dim1, ai_offset, i_1, i_2;
    doublereal d_1, d_2;

    /* Local variables */
    static integer iexc;
    static doublereal c, f, g;
    static integer i, j, k, l, m;
    static doublereal r, s, radix, b2;
    static integer jj;
    static logical noconv;

    /* Parameter adjustments */
    ar_dim1 = *nm;
    ar_offset = ar_dim1 + 1;
    ar -= ar_offset;
    ai_dim1 = *nm;
    ai_offset = ai_dim1 + 1;
    ai -= ai_offset;
    --scale;

    /* Function Body */


/*     this subroutine is a translation of the algol procedure */
/*     cbalance, which is a complex version of balance, */
/*     num. math. 13, 293-304(1969) by parlett and reinsch. */
/*     handbook for auto. comp., vol.ii-linear algebra, 315-326(1971). */

/*     this subroutine balances a complex matrix and isolates */
/*     eigenvalues whenever possible. */

/*     on input */

/*        nm must be set to the row dimension of two-dimensional */
/*          array parameters as declared in the calling program */
/*          dimension statement. */

/*        n is the order of the matrix. */

/*        ar and ai contain the real and imaginary parts, */
/*          respectively, of the complex matrix to be balanced. */

/*     on output */

/*        ar and ai contain the real and imaginary parts, */
/*          respectively, of the balanced matrix. */

/*        low and igh are two integers such that ar(i,j) and ai(i,j) */
/*          are equal to zero if */
/*           (1) i is greater than j and */
/*           (2) j=1,...,low-1 or i=igh+1,...,n. */

/*        scale contains information determining the */
/*           permutations and scaling factors used. */

/*     suppose that the principal submatrix in rows low through igh */
/*     has been balanced, that p(j) denotes the index interchanged */
/*     with j during the permutation step, and that the elements */
/*     of the diagonal matrix used are denoted by d(i,j).  then */
/*        scale(j) = p(j),    for j = 1,...,low-1 */
/*                 = d(j,j)       j = low,...,igh */
/*                 = p(j)         j = igh+1,...,n. */
/*     the order in which the interchanges are made is n to igh+1, */
/*     then 1 to low-1. */

/*     note that 1 is returned for igh if igh is zero formally. */

/*     the algol procedure exc contained in cbalance appears in */
/*     cbal  in line.  (note that the algol roles of identifiers */
/*     k,l have been reversed.) */

/*     arithmetic is real throughout. */

/*     questions and comments should be directed to burton s. garbow, */
/*     mathematics and computer science div, argonne national laboratory 
*/

/*     this version dated august 1983. */

/*     ------------------------------------------------------------------ 
*/

    radix = 16.;

    b2 = radix * radix;
    k = 1;
    l = *n;
    goto L100;
/*     .......... in-line procedure for row and */
/*                column exchange .......... */
L20:
    scale[m] = (doublereal) j;
    if (j == m) {
	goto L50;
    }

    i_1 = l;
    for (i = 1; i <= i_1; ++i) {
	f = ar[i + j * ar_dim1];
	ar[i + j * ar_dim1] = ar[i + m * ar_dim1];
	ar[i + m * ar_dim1] = f;
	f = ai[i + j * ai_dim1];
	ai[i + j * ai_dim1] = ai[i + m * ai_dim1];
	ai[i + m * ai_dim1] = f;
/* L30: */
    }

    i_1 = *n;
    for (i = k; i <= i_1; ++i) {
	f = ar[j + i * ar_dim1];
	ar[j + i * ar_dim1] = ar[m + i * ar_dim1];
	ar[m + i * ar_dim1] = f;
	f = ai[j + i * ai_dim1];
	ai[j + i * ai_dim1] = ai[m + i * ai_dim1];
	ai[m + i * ai_dim1] = f;
/* L40: */
    }

L50:
    switch (iexc) {
	case 1:  goto L80;
	case 2:  goto L130;
    }
/*     .......... search for rows isolating an eigenvalue */
/*                and push them down .......... */
L80:
    if (l == 1) {
	goto L280;
    }
    --l;
/*     .......... for j=l step -1 until 1 do -- .......... */
L100:
    i_1 = l;
    for (jj = 1; jj <= i_1; ++jj) {
	j = l + 1 - jj;

	i_2 = l;
	for (i = 1; i <= i_2; ++i) {
	    if (i == j) {
		goto L110;
	    }
	    if (ar[j + i * ar_dim1] != 0. || ai[j + i * ai_dim1] != 0.) {
		goto L120;
	    }
L110:
	;}

	m = l;
	iexc = 1;
	goto L20;
L120:
    ;}

    goto L140;
/*     .......... search for columns isolating an eigenvalue */
/*                and push them left .......... */
L130:
    ++k;

L140:
    i_1 = l;
    for (j = k; j <= i_1; ++j) {

	i_2 = l;
	for (i = k; i <= i_2; ++i) {
	    if (i == j) {
		goto L150;
	    }
	    if (ar[i + j * ar_dim1] != 0. || ai[i + j * ai_dim1] != 0.) {
		goto L170;
	    }
L150:
	;}

	m = k;
	iexc = 2;
	goto L20;
L170:
    ;}
/*     .......... now balance the submatrix in rows k to l .......... */
    i_1 = l;
    for (i = k; i <= i_1; ++i) {
/* L180: */
	scale[i] = 1.;
    }
/*     .......... iterative loop for norm reduction .......... */
L190:
    noconv = FALSE_;

    i_1 = l;
    for (i = k; i <= i_1; ++i) {
	c = 0.;
	r = 0.;

	i_2 = l;
	for (j = k; j <= i_2; ++j) {
	    if (j == i) {
		goto L200;
	    }
	    c = c + (d_1 = ar[j + i * ar_dim1], abs(d_1)) + (d_2 = ai[j + i * 
		    ai_dim1], abs(d_2));
	    r = r + (d_1 = ar[i + j * ar_dim1], abs(d_1)) + (d_2 = ai[i + j * 
		    ai_dim1], abs(d_2));
L200:
	;}
/*     .......... guard against zero c or r due to underflow 
.......... */
	if (c == 0. || r == 0.) {
	    goto L270;
	}
	g = r / radix;
	f = 1.;
	s = c + r;
L210:
	if (c >= g) {
	    goto L220;
	}
	f *= radix;
	c *= b2;
	goto L210;
L220:
	g = r * radix;
L230:
	if (c < g) {
	    goto L240;
	}
	f /= radix;
	c /= b2;
	goto L230;
/*     .......... now balance .......... */
L240:
	if ((c + r) / f >= s * .95) {
	    goto L270;
	}
	g = 1. / f;
	scale[i] *= f;
	noconv = TRUE_;

	i_2 = *n;
	for (j = k; j <= i_2; ++j) {
	    ar[i + j * ar_dim1] *= g;
	    ai[i + j * ai_dim1] *= g;
/* L250: */
	}

	i_2 = l;
	for (j = 1; j <= i_2; ++j) {
	    ar[j + i * ar_dim1] *= f;
	    ai[j + i * ai_dim1] *= f;
/* L260: */
	}

L270:
    ;}

    if (noconv) {
	goto L190;
    }

L280:
    *low = k;
    *igh = l;
    return 0;
} /* cbal_ */

