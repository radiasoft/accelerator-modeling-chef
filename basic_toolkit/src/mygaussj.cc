/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      mygaussj.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include <iostream>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

using namespace std;

void mygaussj(MatrixD& a, int n, MatrixD& b, int m )
{
  int i,icol,irow,j,k,l,ll;
  double big,dum,pivinv;
  double temp;

  int indxc[n];
  int indxr[n];
  int ipiv[n];

  for (j=0;j<n;j++) ipiv[j]=0;
  for (i=0;i<n;i++) {
    big=0.0;
    for (j=0;j<n;j++)
      if (ipiv[j] != 1)
	for (k=0;k<n;k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a(j,k)) >= big) {
	      big=fabs(a(j,k));
	      irow=j;
	      icol=k;
	    }
	  } else if (ipiv[k] > 1) 
	    cerr << "GAUSSJ: Singular Matrix-1 " << ipiv << endl;
	}
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=0;l<n;l++) std::swap(a(irow,l),a(icol,l) );
      for (l=0;l<m;l++) std::swap( b(irow,l),b(icol,l) );
    }
    indxr[i]=irow;
    indxc[i]=icol;
    if (a(icol,icol) == 0.0) 
      cerr << "GAUSSJ: Singular Matrix-2 " << a << endl;
    pivinv=1.0/a(icol,icol);
    a(icol,icol) = 1.0;
    for (l=0;l<n;l++) a(icol,l) *= pivinv;
    for (l=0;l<m;l++) b(icol,l) *= pivinv;
    for (ll=0;ll<n;ll++)
      if (ll != icol) {
	dum=a(ll,icol);
	a(ll,icol)=0.0;
	for (l=0;l<n;l++) a(ll,l) -= a(icol,l)*dum;
	for (l=0;l<m;l++) b(ll,l) -= b(icol,l)*dum;
      }
  }
  for (l=n-1;l>=0;l--) {
    if (indxr[l] != indxc[l])
      for (k=0;k<n;k++)
	std::swap(a(k,indxr[l]),a(k,indxc[l]));
  }
}


void mygaussj(MatrixD& a, int n, Vector& b)
{

  MatrixD btmp(b.Dim(), 1);

  for (int i=0; i<b.Dim(); ++i ) btmp(i,0) = b[i]; 

  mygaussj( a, n, btmp, 1); 

  for (int i=0; i<b.Dim(); ++i ) b[i] = btmp(i,0); 
  
}


