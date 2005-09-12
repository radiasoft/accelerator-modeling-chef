/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TemplateInstantiations.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                                   
******             
******             - segregated explicit template instantiations
******
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TMatrix.h>
#include <complex>
#include <ostream>
#include <complexAddon.h>
#include <MathConstants.h>
#ifdef WIN32
#include <Distribution.h> // for drand48
#endif

#include <iostream>
#include <iomanip>
#include <sstream>


#include <TMatrix.tcc>
#include <TML.tcc>


using namespace std;
using FNAL::Complex;
using std::ostringstream;

const double limit = double(1e-14);
#define WID setw(8)
#define PREC setprecision(8)


template class TMatrix<double>;
template class TMatrix<FNAL::Complex>;
template class TMatrix<int>;

template bool operator==( const TMatrix<double>&, const TMatrix<double>& );
template bool operator==( const TMatrix<double>&, const double& );
template bool operator==( const double&, const TMatrix<double>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<double>&);
template TMatrix<double> operator+(const TMatrix<double>&, const TMatrix<double>&);
template TMatrix<double> operator+(const TMatrix<double>&, const double&); 
template TMatrix<double> operator+(const double&, const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const double&); 
template TMatrix<double> operator-(const double&, const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const double);
template TMatrix<double> operator*(const double, const TMatrix<double>&);
template TMatrix<double> operator/(const TMatrix<double>&, const double);
template TMatrix<double> operator/(const double, TMatrix<double>&);
template TMatrix<double> operator/(TMatrix<double>&, TMatrix<double>&);

template bool operator==( const TMatrix<int>&, const TMatrix<int>& );
template bool operator==( const TMatrix<int>&, const int& );
template bool operator==( const int&, const TMatrix<int>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<int>&);
template TMatrix<int> operator+(const TMatrix<int>&, const TMatrix<int>&);
template TMatrix<int> operator+(const TMatrix<int>&, const int&); 
template TMatrix<int> operator+(const int&, const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const int&); 
template TMatrix<int> operator-(const int&, const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const int);
template TMatrix<int> operator*(const int, const TMatrix<int>&);
template TMatrix<int> operator/(const TMatrix<int>&, const int);
template TMatrix<int> operator/(const int, TMatrix<int>&);
template TMatrix<int> operator/(TMatrix<int>&, TMatrix<int>&);

template bool operator==( const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>& );
template bool operator==( const TMatrix<FNAL::Complex>&, const FNAL::Complex& );
template bool operator==( const FNAL::Complex&, const TMatrix<FNAL::Complex>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator+(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator+(const TMatrix<FNAL::Complex>&, const FNAL::Complex&); 
template TMatrix<FNAL::Complex> operator+(const FNAL::Complex&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&, const FNAL::Complex&); 
template TMatrix<FNAL::Complex> operator-(const FNAL::Complex&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator*(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator*(const TMatrix<FNAL::Complex>&, const FNAL::Complex);
template TMatrix<FNAL::Complex> operator*(const FNAL::Complex, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator/(const TMatrix<FNAL::Complex>&, const FNAL::Complex);
template TMatrix<FNAL::Complex> operator/(const FNAL::Complex, TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator/(TMatrix<FNAL::Complex>&, TMatrix<FNAL::Complex>&);

// Instantiations for class TML<T>

template class TML<double>;
template class TML<FNAL::Complex>;
template class TML<int>;
