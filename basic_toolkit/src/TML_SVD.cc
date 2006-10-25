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
******  - Modified to use smart pointers
******  
******  Oct 2006   ostiguy@fnal.gov
******
******  - fixed issues with indexing that made SVD fail with rectangular
******    matrices. 
******  - factors automatically allocated with the right dimensions.
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

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/TML.h>
#include <basic_toolkit/MLPtr.h>

using namespace std;
using FNAL::pcerr;


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

  // ----------------------------------------------------------------------------------------------------
  // IMPORTANT:
  // ----------
  //
  // *** WARNING ****
  // 
  // The original NR C version of the SVD routine has been translated from fortran (by its authors, not us). 
  // The indexing was kludged to emulate fortran 77 style array with the index starting at 1 rather than 0. 
  //
  //  *** IN THIS VERSION: arrays start at 0. 
  //
  //  Dimensions:
  //  -----------
  //  Given  a linear system where A x = b
  //
  //  A is m x n , that is has been declared as Matrix(m.n).
  //  U is m x n
  //  W is n x 1  
  //  V is n x n
  //
  //  calling parameters: ***there is no need to dimension the calling parameters. They will be
  //                         automatically dimensioned.    
  //
  //
  //  upon return, U W and V will contain the factors U, W and V from the factorization, correctly dimensioned.
  //
  //  A = U  W V^T  
  // 
  //  
  //
  // -----------------------------------------------------------------------------------------------------
 

  int m = _nrows;
  int n = _ncols;


  U = MLPtr<double>( new TML<double>( *this) ); 


  if ((V->_nrows == _ncols) && (V->_ncols == _ncols) ) 
    V->clear();
  else
    V = MLPtr<double>( new TML<double>( _ncols, _ncols )); 


  if ( (W->_nrows != _ncols) && (W->_ncols != 1) ) {
    W = MLPtr<double>( new TML<double>( _ncols, 1 )); 
  }


  double*  rv1 = new double[n];

  for (i=0; i<n; ++i) {
    l=i+1;
    rv1[i]=scale*g;
    g = s = scale=0.0;
    if (i < m) {
      for (k=i;  k<m; ++k) scale += std::abs((*U)(k,i));
      if (scale) {
        for (k=i; k<m; ++k) {
          (*U)(k,i) /= scale;
          s += (*U)(k,i)*(*U)(k,i);
        }
        f=(*U)(i,i);
        g = -SIGN(sqrt(s),f);
        h=f*g-s;
        (*U)(i,i) = f-g;
        for (j=l; j<n; ++j) {
            for (s=0.0, k=i; k<m; ++k) s += (*U)(k,i)*(*U)(k,j);
            f=s/h;
            for (k=i; k<m; ++k) (*U)(k,j) += f*(*U)(k,i);
        }
        for (k=i; k<m; ++k) (*U)(k,i) *= scale;
      }
    }
    (*W)(i)=scale*g;
    g=s=scale=0.0;
    if (i <m && i != n) {
      for (k=l; k<n; ++k) scale += std::abs((*U)(i,k));
      if (scale) {
        for (k=l; k<n; ++k) {
          (*U)(i,k) /= scale;
          s += (*U)(i,k)*(*U)(i,k);
        }
        f=(*U)(i,l);
        g = -SIGN(sqrt(s),f);
        h=f*g-s;
        (*U)(i,l)=f-g;
        for (k=l; k<n; ++k) rv1[k]=(*U)(i,k)/h;
        if (i != m) {
          for (j=l; j<m; ++j) {
            for (s=0.0,k=l; k<n; ++k) s += (*U)(j,k)*(*U)(i,k);
            for (k=l; k<n; ++k) (*U)(j,k) += s*rv1[k];
          }
        }
        for (k=l;k<n; ++k) (*U)(i,k) *= scale;
      }
    }
    anorm= std::max( anorm, ( std::abs((*W)(i)) + std::abs(rv1[i]) ));
  }
  for (i=n-1; i>=0; --i) {
    if (i < n) {
      if (g) {
        for (j=l; j<n; ++j)
          (*V)(j,i)=((*U)(i,j)/(*U)(i,l))/g;
        for (j=l;j<n; ++j) {
          for (s=0.0,k=l;k<n; ++k) s += (*U)(i,k)*(*V)(k,j);
          for (k=l; k<n; ++k) (*V)(k,j) += s*(*V)(k,i);
        }
      }
      for (j=l; j<n; ++j) (*V)(i,j)=(*V)(j,i)=0.0;
    }
    (*V)(i,i)=1.0;
    g=rv1[i];
    l=i;
  }
  for ( i=std::min(m,n)-1; i>=0; --i) {
    l=i+1;
    g=(*W)(i);
    for (j=l; j<n; ++j) (*U)(i,j) =0.0;
    if (g) {
      g=1.0/g;
      for (j=l; j<n; ++j) {
         for (s=0.0, k=l; k<m; ++k) s += (*U)(k,i)*(*U)(k,j);
         f=(s/(*U)(i,i))*g;
         for (k=i; k<m; ++k) (*U)(k,j) += f*(*U)(k,i);
      }
      for (j=i; j<m; ++j) (*U)(j,i) *= g;
    } else {
      for (j=i; j<m; ++j) (*U)(j,i) = 0.0;
    }
    ++(*U)(i,i);
    }
  for (k=n-1; k>=0; --k) {
    for (its=0; its<30; ++its) {
      flag=1;
      for (l=k; l>=0; --l) {
        nm=l-1;
        if (fabs(rv1[l])+anorm == anorm) {
          flag=0;
          break;
        }
        if (fabs((*W)(nm))+anorm == anorm) break;
      }
      if (flag) {
        c=0.0;
        s=1.0;
        for (i=l; i<=k; ++i) {
          f=s*rv1[i];
          if (fabs(f)+anorm != anorm) {
            g=(*W)(i);
            h=PYTHAG(f,g);
            (*W)(i)=h;
            h=1.0/h;
            c=g*h;
            s=(-f*h);
            for (j=0; j<m; ++j) {
              y=(*U)(j,nm);
              z=(*U)(j,i);
              (*U)(j,nm)= y*c+z*s;
              (*U)(j,i) = z*c-y*s;
            }
          }
        }
      }
      z=(*W)(k);
      if (l == k) {
        if (z < 0.0) {
          (*W)(k) = -z;
          for (j=0; j<n; ++j) (*V)(j,k)=(-(*V)(j,k));
        }
        break;
      }

      if (its == 30) { 
        throw( _nrows, _ncols, 
               "void TML<double>::SVD (  MLPtr<double>& U, MLPtr<double>& W, MLPtr<double>& V) const",
               "No convergence in 30 SVDCMP iterations." );
      }

      x  = (*W)(l);
      nm = k-1;
      y  = (*W)(nm);
      g  = rv1[nm];
      h  = rv1[k];
      f  = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g  = PYTHAG(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm; ++j) {
        i=j+1;
        g=rv1[i];
        y=(*W)(i);
        h=s*g;
        g=c*g;
        z=PYTHAG(f,h);
        rv1[j]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g=g*c-x*s;
        h=y*s;
        y=y*c;
        for (jj=0; jj<n; ++jj) {
          x=(*V)(jj,j);
          z=(*V)(jj,i);
          (*V)(jj,j)=x*c+z*s;
          (*V)(jj,i)=z*c-x*s;
        }
        z=PYTHAG(f,h);
        (*W)(j)=z;
        if (z) {
          z=1.0/z;
          c=f*z;
          s=h*z;
        }
        f=(c*g)+(s*y);
        x=(c*y)-(s*g);
        for (jj=0; jj<m; ++jj) {
          y=(*U)(jj,j);
          z=(*U)(jj,i);
            (*U)(jj, j)=y*c+z*s;
            (*U)(jj, i)=z*c-y*s;
        }
      }
      rv1[l]=0.0;
      rv1[k]=f;
      (*W)(k)=x;
    }
  }

  delete [] rv1;
 

}

#undef SIGN
#undef MAX
#undef PYTHAG
