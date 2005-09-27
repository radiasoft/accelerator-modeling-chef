/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.h
******  Version:   2.0
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
******  Revision History
******                                                                
******  Sept 2005   ostiguy@fnal.gov
******
******              - new code based on a single template parameter
******                instead of two. Mixed mode now handled
******                using conversion operators.
******
******
*************************************************************************
*************************************************************************/


#ifndef TJETENV_H
#define TJETENV_H

#include <slist.h>
#include <IntArray.h>
#include <Cascade.h>

#include <TMatrix.h>

// Forward declarations
class Vector;

template<typename T> 
class TJet;

template<typename T> 
class TJetEnvironment;

template<typename T> 
class TJLterm;

template<typename T> 
std::ostream& operator<<( std::ostream&, const TJetEnvironment<T>& );

template<typename T> 
std::istream& streamIn( std::istream&, TJetEnvironment<T>** );


// Struct TJetEnvironment template
template<typename T>
struct TJetEnvironment
{
 int       _numVar;       // Number of scalar variables associated with 
                          //   the JLC variable.
 int       _spaceDim;     // This is, if you will, the dimension of phase space.
 int       _dof;          // The number of degrees of freedom = SpaceDim / 2.
 T*       _refPoint;      // Reference point in phase space about which 
                          //   derivatives are taken.
 double*   _scale;        // An array containing numbers which scale the
                          //   problem's variables.  Each entry is a "typical
                          //   size" for the variable of the same index.
 int       _maxWeight;    // Maximum acceptable weight of a variable,
                          //   equivalent to the maximum order of derivatives
                          //   to be carried through calculations.
 T*       _monomial;      // Storage area for monomials used in multinomial
                          //   evaluation. 
 int       _maxTerms;     // Maximum number of monomial terms.
 TJet<T>* _TJLmonomial;   // Storage area for TJL monomials used in concatenation.
 TJLterm<T>* _TJLmml;     // Same as above, but used for collecting terms
                          //   during multiplication.
                          //   ??? There should be no need for a second array.
                          //   ??? Both are employed as scratchpads.
                          //   ??? Rewrite code so that only one is used.
 Cascade   _offset;       // Switching functor for fast access to offset
                          //   indices in the _TJLmonomial and _TJLmml arrays.
 int*      _exponent;     // Used by nexcom (as called by TJL::operator()
                          //   when storing monomials.

 bool      _pbok;         // Taking Poisson brackets is OK: the dimension of 
                          //   phase space is even.

 IntArray  _allZeroes;    // An integer array containing zeroes.  Used to 
                          //   filter the standard part of a variable.


 // Member functions -------------------------------------

  TJetEnvironment();
  TJetEnvironment( const TJetEnvironment& );
 ~TJetEnvironment();


 operator TJetEnvironment<std::complex<double> > () const;
 operator TJetEnvironment<double> () const;
 
 // factory functions

 static TJetEnvironment*                        makeJetEnvironment(); // is this really needed ? 
 static TJetEnvironment*                        makeJetEnvironment(const TJetEnvironment*);

 static TJetEnvironment*                        CreateEnvFrom( const Vector&, int );
 static TJetEnvironment<double>*                CreateEnvFrom( const TJetEnvironment<std::complex<double> >* );
 static TJetEnvironment<std::complex<double> >* CreateEnvFrom( const TJetEnvironment<double>* );


 TJetEnvironment& operator=( const TJetEnvironment& );

 void _buildScratchPads();

 bool operator==( const TJetEnvironment<T>& ) const;
 bool operator!=( const TJetEnvironment<T>& ) const;

 bool approxEq( const TJetEnvironment<T>&, const Vector& tolerance ) const;
 bool approxEq( const TJetEnvironment<T>&, const double* tolerance ) const;

     // Second argument is a "tolerance" Vector. There is
     //   no default: the invoking program must declare
     //   its tolerance.
     // The function returns "true" if the reference points
     //   match within the tolerance. 
     // The second version is riskier. There is no guarantee
     //   that the array of doubles will not be overrun.

 bool hasReferencePoint( const Vector& ) const;
 bool hasReferencePoint( const double* ) const;
 bool hasApproxReferencePoint( const Vector&, const Vector& tolerance ) const;
 bool hasApproxReferencePoint( const double*, const Vector& tolerance ) const;
 bool hasApproxReferencePoint( const Vector&, const double* tolerance ) const;
 bool hasApproxReferencePoint( const double*, const double* tolerance ) const;


 // Streams
 friend std::ostream& operator<<<>( std::ostream&, const TJetEnvironment& );
 friend std::istream& streamIn<>( std::istream&, TJetEnvironment** );


 // Static data members
 static bool _skipEnvEqTest;

 static slist _environments;   // A list of existing environments 
                               // note that there is a list of every template typename T

};


// specializations

template <>
 TJetEnvironment<double>* TJetEnvironment<double>::CreateEnvFrom( const TJetEnvironment<std::complex<double> >* );

 template<>
 TJetEnvironment<std::complex<double> >* TJetEnvironment<std::complex<double> >::CreateEnvFrom( const TJetEnvironment<double>* );

 template<>
 TJetEnvironment<double>* TJetEnvironment<double>::CreateEnvFrom( const Vector&, int );




#ifdef MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJetEnvironment.tcc>
#endif

#endif // TJETENV_H
