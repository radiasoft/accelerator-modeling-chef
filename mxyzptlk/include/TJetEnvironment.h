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
****** - new code based on a single template parameter (rather than two)
****** - Mixed mode handled using conversion operators.
****** - centralized environment management
******
******  Nov 2005   ostiguy@fnal.gov
******  
****** - reference counted environments based on boost::intrusive_ptr<>
****** - shared scratchpads:  implemented ScratchArea as a distinct 
******   private class. Only one instance of ScratchArea is created for 
******   for *all* environments sharing the same values of maxweight, 
******   and numvar.   
******  
******  May 2006   ostiguy@fnal.gov
****** 
****** - EnvPtr<> is now a true class rather than a typedef wrapper 
****** - template syntax declaration cleanup to better conform to standard. 
******   Code now compiles cleanly with g++ 4.X   
******      
*************************************************************************
*************************************************************************/
#ifndef TJETENV_H
#define TJETENV_H

#include <IntArray.h>
#include <VectorD.h>
#include <Cascade.h>
#include <TMatrix.h>
#include <deque>
#include <list>
#include <JLPtr.h>
#include <ostream>
#include <istream>
#include <ReferenceCounter.h>

// Forward declarations


template<typename T> 
class EnvPtr;

template<typename T> 
class TJet;

template<typename T> 
class Tcoord;

template<typename T> 
class Tparam;

template<typename T> 
class TJetEnvironment;

template<typename T> 
class TJLterm;

template<typename T>
class TMapping;

template<typename T> 
std::ostream& operator<<( std::ostream& os, const TJetEnvironment<T>& pje);

template<typename T> 
std::istream& streamIn(std::istream&, EnvPtr<T>& pje );

//-------------------------------------------------------------------------
// the wrapper class EnvList is a kludge to prevent static EnvPtr<> objects
// from iterating trough a potentially no longer existing 
//  _environments static container upon program exit. 
//-------------------------------------------------------------------------  

template<typename T>
class EnvList : public std::list<EnvPtr<T> > {
 
public:
  EnvList(): _destructor_called(false) {}  
  virtual  ~EnvList();   
  bool _destructor_called;    
 
};

//----------------------------------------------------------------------------------------------
// class TJetEnvironment
//----------------------------------------------------------------------------------------------

template<typename T>
class TJetEnvironment: public ReferenceCounter<TJetEnvironment<T> >
{

 public:
 
 template<typename U> 
 struct ScratchArea { 
    int                        _maxWeight;
    int                        _numVar;
    int                        _maxTerms;       // Maximum number of monomial terms.
    Cascade                    _offset;         // Switching functor for fast access to offset
    int*                       _exponent;       //  Used by nexcom (as called by TJL::operator()
                                                //   when storing monomials.
                                                //   indices in the _TJLmonomial and _TJLmml arrays.
    U*                         _monomial;       // Storage area for monomials used in multinomial
                                                // evaluation. 
    JLPtr<U>*                  _TJLmonomial;    // Storage area for TJL monomials used in concatenation.
 
    TJLterm<U>*                _TJLmml;         // Same as above, but used for collecting terms
                                                //   during multiplication.
    const IntArray  _allZeroes;                 // An integer array containing zeroes.  Used to 
                                                //   filter the standard part of a variable.

   ScratchArea( TJetEnvironment<U>* pje, int weight, int numvar);
  ~ScratchArea();
 
   private:

  ScratchArea(const ScratchArea&); 

 };

 template<typename U>
 class Exists {
 private:

  int _maxWeight;
  int _numVar;

 public:

   Exists(int maxweight, int numvar): _maxWeight(maxweight), _numVar(numvar) {}

   bool operator()( const ScratchArea<U>* p ) const {
     return ( ( p->_numVar == _numVar )&&(p->_maxWeight == _maxWeight) );
   } 
 };


 // friends  ----------------------------------------------------
 
   friend class Tcoord<T>;
   friend class Tparam<T>;

   template<typename U>
   friend class TJetEnvironment;

 // Public Member functions -------------------------------------

 ~TJetEnvironment();

  // factory functions -------------------------------------------

  static void                       BeginEnvironment(int maxweight); 
  static EnvPtr<T>   EndEnvironment(double* scale=0);

  static EnvPtr<T>   makeJetEnvironment(int maxweight, int nvar, int spacedim, T* refpoints=0, double* scale=0);
  static EnvPtr<T>   makeInverseJetEnvironment(const TMapping<T>& map); 
  static EnvPtr<T>   makeJetEnvironment(int maxweight, const Vector&, double* scale=0);

  template<typename U>
  static EnvPtr<T>   makeJetEnvironment( EnvPtr<U> const&);

  template<typename U>
  static EnvPtr<double>   makeRealJetEnvironment( EnvPtr<U> const& );


  static EnvPtr<double>  getApproxJetEnvironment(int maxweight, const Vector& refpoints);

  void dispose();

  friend void EnvTerminate();
                
   // operators -------------------------------------------

  TJetEnvironment& operator=( const TJetEnvironment& env) { return  DeepCopy(env); }
  TJetEnvironment& DeepCopy( const TJetEnvironment& env);

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

   // queries --------------------------------------------------------


   int             numVar()      const { return  _numVar;      }
   int             spaceDim()    const { return  _spaceDim;    }
   int             dof()         const { return  _dof;         }
   const T*        refPoint()    const { return  _refPoint;    }
   int             maxWeight()   const { return  _maxWeight;   }
   const double*   scale()       const { return _scale;        }
   const T*        getRefPoint() const { return _refPoint; } 
 
   T*              monomial()    const { return  _scratch->_monomial;    }
   int*            exponent()    const { return  _scratch->_exponent;    }

   JLPtr<T>*       TJLmonomial() const { return  _scratch->_TJLmonomial; }

   TJLterm<T>*     TJLmml()      const { return  _scratch->_TJLmml;      }

   int             offsetIndex(const IntArray& exp) const    { return _scratch->_offset.index(exp); }  
   int             offsetIndex(const int*      exp) const    { return _scratch->_offset.index(exp); }  
   const IntArray& allZeroes() const                         { return _scratch->_allZeroes; }     
   int             maxTerms() const                          { return _scratch->_maxTerms;}

   // Streams --------------------------------------------------------- 

  friend std::ostream& operator<< <>( std::ostream&, const TJetEnvironment& );
  friend std::istream& streamIn<>(std::istream&, EnvPtr<T>& pje );

 private:

  int              _numVar;              // Number of scalar variables associated with 
                                         //   the JLC variable.
  int              _spaceDim;            // This is, if you will, the dimension of phase space.
  int              _dof;                 // The number of degrees of freedom = SpaceDim / 2.
  T*               _refPoint;            // Reference point in phase space about which 
                                         //   derivatives are taken.
  double*          _scale;               // An array containing numbers which scale the
                                         //   problem's variables.  Each entry is a "typical
                                         //   size" for the variable of the same index.
  int              _maxWeight;           // Maximum acceptable weight of a variable,
                                         //   equivalent to the maximum order of derivatives
                                         //   to be carried through calculations.

  bool             _pbok;                // Taking Poisson brackets is OK: the dimension of 
                                         //   phase space is even.

  ScratchArea<T>*  _scratch;             // pointer to a common scratch area. Scratch is 
                                         // unique for (maxweight, nvar) 


 // Static data members ------------------------------------------------
 
  static std::deque<Tcoord<T>*>         _coordinates;    // used only during new environment creation 
  static std::deque<Tparam<T>*>         _parameters;     // used only during new environment creation
  static std::list<ScratchArea<T>* >    _scratch_areas;  // list of existing scratch areas 
  static EnvList<T>                     _environments;  // A list of existing scratch areas 
                                                               // Note: there is a list for every typename parameter T
  static int   _tmp_maxWeight; // used by Begin/EndEnvironment() 


// Private Member  functions -------------------------------------

  ScratchArea<T>* _buildScratchPads(int maxweight, int numvar);

  TJetEnvironment(int maxweight, int nvar, int spacedim, T* refpoints=0, double* scale=0);

  TJetEnvironment(TJetEnvironment const&);

  template<typename U>
  TJetEnvironment(TJetEnvironment<U> const&);


};

//------------------------------------------------------------------------------------------------------------
// specializations
//------------------------------------------------------------------------------------------------------------

template<>
template<>
TJetEnvironment<std::complex<double> >::TJetEnvironment( TJetEnvironment<double> const& );

template<>
template<>
EnvPtr<std::complex<double> >  TJetEnvironment<std::complex<double> >::makeJetEnvironment( EnvPtr<double> const&);


template<>
template<>
EnvPtr<double>  TJetEnvironment<std::complex<double> >::makeRealJetEnvironment( EnvPtr<std::complex<double> > const& );



#ifdef MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJetEnvironment.tcc>
#endif

#endif // TJETENV_H
