#ifndef MATHCONS_H
#define MATHCONS_H

#define MATH_SQRTPI        1.7724538509055160273
                                          // sqrt( pi )
#define MATH_TWOPI         6.2831853071795864769

#ifndef M_E
  #define M_E         2.71828182845904523536
#endif /* M_E */

#ifndef M_LOG2E
  #define M_LOG2E     1.44269504088896340736
#endif /* M_LOG2E */

#ifndef M_LOG10E
  #define M_LOG10E    0.434294481903251827651
#endif /* M_LOG10E */

#ifndef M_LN2
  #define M_LN2       0.693147180559945309417
#endif /* M_LN2 */

#ifndef M_LN10
  #define M_LN10      2.30258509299404568402
#endif /* M_LN10 */

#ifndef M_PI
  #define M_PI        3.14159265358979323846
#endif /* M_PI */

#ifndef M_TWOPI
  #define M_TWOPI     6.28318530717958647692
#endif /* M_TWOPI */

#ifndef M_PI_2
  #define M_PI_2      1.57079632679489661923
#endif /* M_PI_2 */

#ifndef M_PI_4
  #define M_PI_4      0.785398163397448309616
#endif /* M_PI_4 */

#ifndef M_1_PI
  #define M_1_PI      0.318309886183790671538
#endif  /* M_1_PI */

#ifndef M_2_PI
  #define M_2_PI      0.636619772367581343076
#endif /* M_2_PI */

#ifndef M_SQRTPI
  #define M_SQRTPI    1.7724538509055160273
#endif /* M_SQRTPI */

#ifndef M_1_SQRTPI
  #define M_1_SQRTPI  0.564189583547756286948
#endif /* M_1_SQRTPI */

#ifndef M_2_SQRTPI
  #define M_2_SQRTPI  1.12837916709551257390
#endif /* M_2_SQRTPI */

#ifndef M_SQRT2
#define M_SQRT2     1.41421356237309504880
#endif /* M_SQRT2 */

#ifndef M_SQRT_2
  #define M_SQRT_2    0.707106781186547524401
#endif /* M_SQRT_2 */


#ifdef __SUNPRO_CC

// The following code is for the Solaris native compiler.  It does not
// have any boolean basic data type.  Note that this does *not* make
// bool a basic type, but it does make some code more readable
// -- Elliott McCrory

typedef enum { false = 0, true = 1 } bool;

#endif // __SUNPRO_CC


#endif // MATHCONS_H
