/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.h
******  Version:   1.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef TJETENV_H
#define TJETENV_H

#include "TMatrix.h"
#include "IntArray.h"
#include "slist.h"

// Forward declarations
template<typename T1, typename T2> 
class TJet;

template<typename T1, typename T2> 
class TJetEnvironment;

template<typename T1, typename T2> 
std::ostream& operator<<( std::ostream&, const TJetEnvironment<T1,T2>& );

template<typename T1, typename T2> 
std::istream& streamIn( std::istream&, TJetEnvironment<T1,T2>** );


// Struct TJetEnvironment template
template<typename T1, typename T2>
struct TJetEnvironment
{
 int       _numVar;       // Number of scalar variables associated with 
                          //   the JLC variable.
 int       _spaceDim;     // This is, if you will, the dimension of phase space.
 int       _dof;          // The number of degrees of freedom = SpaceDim / 2.
 T1*       _refPoint;     // Reference point in phase space about which 
                          //   derivatives are taken.
 double*   _scale;        // An array containing numbers which scale the
                          //   problem's variables.  Each entry is a "typical
                          //   size" for the variable of the same index.
 int       _maxWeight;    // Maximum acceptable weight of a variable,
                          //   equivalent to the maximum order of derivatives
                          //   to be carried through calculations.
 T1*       _monomial;     // Storage area for monomials used in multinomial
                          //   evaluation. 
 TJet<T1,T2>* _TJLmonomial;  // Storage area for TJL monomials used in concatenation.
 int*      _exponent;     // Used by nexcom (as called by TJL::operator()
                          //   when storing monomials.
 char*     _expCode;      // The Wilf code of a monomial. 
 bool      _pbok;         // Taking Poisson brackets is OK: the dimension of 
                          //   phase space is even.
 MatrixI*  _numPaths;     // An array counting the number of paths to a vertex 
                          //   in the Wilf graph.  Setup arranges that 
                          //   numPaths[w][n] = number of compositions of
                          //   a weight  w  into  n  parts.
 IntArray  _allZeroes;    // An integer array containing zeroes.  Used to 
                          //   filter the standard part of a variable.
 slist     _myCoords;     // Coordinates of this environment.


 // Member functions -------------------------------------
 TJetEnvironment();
 TJetEnvironment( const TJetEnvironment& );
 TJetEnvironment( const TJetEnvironment<T2,T1>& );
 ~TJetEnvironment();

 TJetEnvironment& operator=( const TJetEnvironment& );
 TJetEnvironment& operator=( const TJetEnvironment<T2,T1>& );
 bool operator==( const TJetEnvironment& ) const;
 bool operator!=( const TJetEnvironment& ) const;


 // Monomial ranking routines based on Wilf's algorithm
 void _monoCode();     // Computes the code of a monomial from its
                       // exponents.
 void _monoDecode();   // The inverse operation to monoCode.
 int  _monoRank();     // Computes the rank of a monomial.


 // Streams
 friend std::ostream& operator<<<>( std::ostream&, const TJetEnvironment& );
 friend std::istream& streamIn<>( std::istream&, TJetEnvironment** );


 // Static data members
 static bool _skipEnvEqTest;
};

typedef TJetEnvironment<double,FNAL::Complex> Jet__environment;
typedef TJetEnvironment<FNAL::Complex,double> JetC__environment;

#endif // TJETENV_H
