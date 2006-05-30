/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TML_SVD.cc
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Email: michelotti@fnal.gov                         
******
******  Revision History
******
******  Nov 2005   ostiguy@fnal.gov                                                                
******
****** - Modified to use smart pointers
****** 
**************************************************************************
*************************************************************************/


/*
 * This function for doing singular value decomposition is adapted
 * from Numerical Recipes in C, 1988 edition.
 * 
 * Programmer: Leo Michelotti
 * Date:       March 16, 1995
 *             February 24, 2004 (Initial Templatized version)
 * 
 */


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TML.h>
#include <MLPtr.h>

using namespace std;

template<>
void TML<double>::SVD ( MLPtr<double> & U, MLPtr<double> & W, MLPtr<double> & V ) const
{

#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
  (ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))
#define MAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? \
  (maxarg1) : (maxarg2))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

  double at,bt,ct;
  double maxarg1, maxarg2;
  double c,f,h,s,x,y,z;
  double anorm=0.0,g=0.0,scale=0.0;

  int flag,i,its,j,jj,k,l,nm;
  int m = _nrows;
  int n = _ncols;


  if (m < n) {
    throw GenericMatrixException( m, n, 
           "void TML<double>::SVD ( MLPtr<double>& U, MLPtr<double>& W, MLPtr<double>& V ) const",
           "Fewer rows than columns: augment matrix with extra rows" );
  } 
  
  if( (W->_nrows != n) || (W->_ncols != 1) ) {
    throw  GenericMatrixException(W->_nrows, W->_ncols, 
           "void TML<double>::SVD (  MLPtr<double>& U, MLPtr<double>& W, MLPtr<double>& V ) const",
           "W has the wrong dimensions." );
  }

  if( (V->_nrows != n) || (V->_ncols != n) ) {
    throw GenericMatrixException( V->_nrows, V->_ncols, 
           "void TML<double>::SVD ( MLPtr<double>& U, MLPtr<double>& W, MLPtr<double>& V ) const",
           "V has the wrong dimensions." );
  }



  U = MLPtr<double>( new TML<double>(*this) ); // deep copy

  double*  rv1 = new double[n];

  for (i=1;i<=n;i++) {
    l=i+1;
    rv1[i-1]=scale*g;
    g = s = scale=0.0;
 
    if (i <= m) {
      for (k=i;  k<=m; ++k) scale += std::abs((*U)(k-1,i-1));
      if (scale) {
        for (k=i; k<=m; ++k) {
          (*U)(k-1,i-1) /= scale;
          s += (*U)(k-1,i-1)*(*U)(k-1,i-1);
        }
        f=(*U)(i-1,i-1);
        g = -SIGN(sqrt(s),f);
        h=f*g-s;
        (*U)(i-1,i-1)=f-g;
        if (i != n) {
          for (j=l;j<=n; ++j) {
            for (s=0.0,k=i; k<=m; ++k) s += (*U)(k-1,i-1)*(*U)(k-1,j-1);
            f=s/h;
            for (k=i; k<=m; ++k) (*U)(k-1,j-1) += f*(*U)(k-1,i-1);
          }
        }
        for (k=i; k<=m; ++k) (*U)(k-1,i-1) *= scale;
      }
    }
    (*W)(i-1)=scale*g;
    g=s=scale=0.0;
    if (i <= m && i != n) {
      for (k=l; k<=n; ++k) scale += std::abs((*U)(i-1,k-1));
      if (scale) {
        for (k=l; k<=n; ++k) {
          (*U)(i-1,k-1) /= scale;
          s += (*U)(i-1,k-1)*(*U)(i-1,k-1);
        }
        f=(*U)(i-1,l-1);
        g = -SIGN(sqrt(s),f);
        h=f*g-s;
        (*U)(i-1,l-1)=f-g;
        for (k=l; k<=n; ++k) rv1[k-1]=(*U)(i-1,k-1)/h;
        if (i != m) {
          for (j=l;j<=m;j++) {
            for (s=0.0,k=l; k<=n; ++k) s += (*U)(j-1,k-1)*(*U)(i-1,k-1);
            for (k=l;k<=n;k++) (*U)(j-1,k-1) += s*rv1[k-1];
          }
        }
        for (k=l;k<=n; ++k) (*U)(i-1,k-1) *= scale;
      }
    }
    anorm= MAX(anorm,(std::abs((*W)(i-1))+std::abs(rv1[i-1])));
  }
  for (i=n; i>=1; --i) {
    if (i < n) {
      if (g) {
        for (j=l;j<=n; ++j)
          (*V)(j-1,i-1)=((*U)(i-1,j-1)/(*U)(i-1,l-1))/g;
        for (j=l;j<=n; ++j) {
          for (s=0.0,k=l;k<=n; ++k) s += (*U)(i-1,k-1)*(*V)(k-1,j-1);
          for (k=l; k<=n; ++k) (*V)(k-1,j-1) += s*(*V)(k-1,i-1);
        }
      }
      for (j=l; j<=n; ++j) (*V)(i-1,j-1)=(*V)(j-1,i-1)=0.0;
    }
    (*V)(i-1,i-1)=1.0;
    g=rv1[i-1];
    l=i;
  }
  for (i=n;i>=1; --i) {
    l=i+1;
    g=(*W)(i-1);
    if (i < n)
      for (j=l;j<=n;++j) (*U)(i-1,j-1)=0.0;
    if (g) {
      g=1.0/g;
      if (i != n) {
        for (j=l;j<=n; ++j) {
          for (s=0.0,k=l;k<=m; ++k) s += (*U)(k-1,i-1)*(*U)(k-1,j-1);
          f=(s/(*U)(i-1,i-1))*g;
          for (k=i;k<=m; ++k) (*U)(k-1,j-1) += f*(*U)(k-1,i-1);
        }
      }
      for (j=i;j<=m;j++) (*U)(j-1,i-1) *= g;
    } else {
      for (j=i;j<=m;j++) (*U)(j-1,i-1)=0.0;
    }
    ++(*U)(i-1,i-1);
  }
  for (k=n;k>=1;k--) {
    for (its=1;its<=30; ++its) {
      flag=1;
      for (l=k;l>=1; --l) {
        nm=l-1;
        if (fabs(rv1[l-1])+anorm == anorm) {
          flag=0;
          break;
        }
        if (fabs((*W)(nm-1))+anorm == anorm) break;
      }
      if (flag) {
        c=0.0;
        s=1.0;
        for (i=l;i<=k; ++i) {
          f=s*rv1[i-1];
          if (fabs(f)+anorm != anorm) {
            g=(*W)(i-1);
            h=PYTHAG(f,g);
            (*W)(i-1)=h;
            h=1.0/h;
            c=g*h;
            s=(-f*h);
            for (j=1;j<=m; ++j) {
              y=(*U)(j-1,nm-1);
              z=(*U)(j-1,i-1);
              (*U)(j-1,nm-1)=y*c+z*s;
              (*U)(j-1,i-1)=z*c-y*s;
            }
          }
        }
      }
      z=(*W)(k-1);
      if (l == k) {
        if (z < 0.0) {
          (*W)(k-1) = -z;
          for (j=1;j<=n;++j) (*V)(j-1,k-1)=(-(*V)(j-1,k-1));
        }
        break;
      }

      if (its == 30) { 
        throw( _nrows, _ncols, 
               "void TML<double>::SVD (  MLPtr<double>& U, MLPtr<double>& W, MLPtr<double>& V) const",
               "No convergence in 30 SVDCMP iterations." );
      }

      x=(*W)(l-1);
      nm=k-1;
      y=(*W)(nm-1);
      g=rv1[nm-1];
      h=rv1[k-1];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=PYTHAG(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm;++j) {
        i=j+1;
        g=rv1[i-1];
        y=(*W)(i-1);
        h=s*g;
        g=c*g;
        z=PYTHAG(f,h);
        rv1[j-1]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g=g*c-x*s;
        h=y*s;
        y=y*c;
        for (jj=1;jj<=n;++jj) {
          x=(*V)(jj-1,j-1);
          z=(*V)(jj-1,i-1);
          (*V)(jj-1,j-1)=x*c+z*s;
          (*V)(jj-1,i-1)=z*c-x*s;
        }
        z=PYTHAG(f,h);
        (*W)(j-1)=z;
        if (z) {
          z=1.0/z;
          c=f*z;
          s=h*z;
        }
        f=(c*g)+(s*y);
        x=(c*y)-(s*g);
        for (jj=1;jj<=m;++jj) {
          y=(*U)(jj-1,j-1);
          z=(*U)(jj-1,i-1);
          (*U)(jj-1,j-1)=y*c+z*s;
          (*U)(jj-1,i-1)=z*c-y*s;
        }
      }
      rv1[l-1]=0.0;
      rv1[k-1]=f;
      (*W)(k-1)=x;
    }
  }

  delete [] rv1;

#undef SIGN
#undef MAX
#undef PYTHAG
}
