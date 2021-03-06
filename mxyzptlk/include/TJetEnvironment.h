/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.h
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
******  Mar 2007 ostiguy@fnal.gov  
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   to replace previous scheme based explicitly on monomial exponents tuples.
****** - monomial multiplication now handled via a lookup-table.
****** - added STL compatible monomial term iterators   
****** - eliminated dependence on class Cascade 
******    
*************************************************************************
*************************************************************************/
#ifndef TJETENV_H
#define TJETENV_H

#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/TMatrix.h>
#include <stack>
#include <deque>
#include <list>
#include <ostream>
#include <istream>
#include <basic_toolkit/ReferenceCounter.h>

// Forward declarations

template<typename T> 
class JLPtr;

template<typename T> 
class EnvPtr;

template<typename T> 
class ConstEnvPtr;

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

//----------------------------------------------------------------------------------------------

void createStandardEnvironments( int deg ); 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//----------------------------------------------------------------------------------------------
// class TJetEnvironment
//----------------------------------------------------------------------------------------------

template<typename T>
class DLLEXPORT TJetEnvironment: public ReferenceCounter<TJetEnvironment<T> > {

 public:
 
template <typename U>
struct ScratchArea { 
    int                                maxWeight_;
    int                                numVar_;
    int                                maxTerms_;       // Maximum number of monomial terms.
    std::vector<U>                     monomial_;       // Storage area for monomials used in multinomial evaluation. 
    std::vector<JLPtr<U> >             TJLmonomial_;    // Storage area for TJL monomials used in concatenation.
    std::vector<TJLterm<U> >           TJLmml_;         // Same as above, but used for collecting terms
                                                        //   during multiplication.
    std::vector<IntArray>              index_table_;    // table of monomial exponents ( offset = ordering index)  
    std::vector<std::vector<int> >     multTable_;      // monomial multiplication table 
    std::vector<int>                   weight_offsets_; // Offsets of the weights groups within the scratchpad     

   ScratchArea( TJetEnvironment<U>* pje, int weight, int numvar);
  ~ScratchArea();
 
   int  multOffset( int const& rhs, int const& lhs) const 
                { return ( rhs > multTable_[lhs].size()-1 ) ?  multTable_[lhs][rhs]: multTable_[rhs][lhs]; }  

   int  offsetIndex(IntArray const& exp) const;

   void debug()     const; 

   private:

  ScratchArea(ScratchArea const&); 

 };

 template<typename U>
 class Exists {
 private:

  int maxWeight_;
  int numVar_;

 public:

   Exists(int maxweight, int numvar): maxWeight_(maxweight), numVar_(numvar) {}

   bool operator()( ScratchArea<U> const* p ) const {
     return ( ( p->numVar_ == numVar_ )&&(p->maxWeight_ == maxWeight_) );
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

  static void        BeginEnvironment(int maxweight); 
  static EnvPtr<T>   EndEnvironment();

  static EnvPtr<T>   makeJetEnvironment(int maxweight, int nvar, int spacedim, TVector<T> const& = TVector<T>() );
  static EnvPtr<T>   makeJetEnvironment(int maxweight, TVector<double> const& );

  template<typename U>
  static EnvPtr<T>   makeJetEnvironment( EnvPtr<U> const& );

  static EnvPtr<T>   makeInverseJetEnvironment(TMapping<T> const& map); 

  template<typename U>
  static EnvPtr<double>   makeRealJetEnvironment( EnvPtr<U> const& );

  static EnvPtr<double>   getApproxJetEnvironment(int maxweight, Vector const& refpoints);

  void dispose();

  
  TJetEnvironment& operator=( TJetEnvironment<T> const& rhs);
              
  template <typename U>
  bool operator==( TJetEnvironment<U> const& ) const;

  bool operator!=( TJetEnvironment<T> const& ) const;

  bool is_equivalent(  TJetEnvironment<T> const&, TVector<double> const& tol ) const;



  int               numVar()      const { return   numVar_;      }
  int               spaceDim()    const { return   spaceDim_;    }
  TVector<T> const& refPoint()    const { return   refPoint_;    }
  int               maxWeight()   const { return   maxWeight_;   }
 
  std::vector<T>&            monomial()        const { return   scratch_->monomial_;    }
  std::vector<TJLterm<T> >&  TJLmml()          const { return   scratch_->TJLmml_;      }
  std::vector<JLPtr<T> >&    TJLmonomial()     const { return   scratch_->TJLmonomial_; }

  int             multOffset    (int const& lhs, int const& rhs)  const   { return  scratch_->multOffset(lhs, rhs); }


  int           offsetIndex( IntArray const& exp) const; 
  int                weight( int const& offset )  const;
  int         weight_offset( int const& weight )  const;  
  IntArray const& exponents( int const& offset )  const  { return scratch_->index_table_[offset]; }

  int             maxTerms()  const                      { return scratch_->maxTerms_;}


  static EnvPtr<T>        topEnv ();

  static void             pushEnv( EnvPtr<T> const& );
  static void             popEnv();


  void debug()              const                        { scratch_->debug(); }


  friend std::ostream& operator<< <>( std::ostream&, const TJetEnvironment& );
  friend std::istream& streamIn<>(std::istream&, EnvPtr<T>& pje );

 private:

  static EnvPtr<T>   lastEnv_;             // default environment  

  int                numVar_;              // Number of scalar variables associated with 
                                           //   the JLC variable.
  int                spaceDim_;            // This is, if you will, the dimension of phase space.
  TVector<T>         refPoint_;            // Reference point in phase space about which 
                                           //   derivatives are taken.
  int                maxWeight_;           // Maximum acceptable weight of a variable,
                                           //   equivalent to the maximum order of derivatives
                                           //   to be carried through calculations.

  ScratchArea<T>*    scratch_;             // shared pointer to a common scratch area. Scratch is 
                                           // unique for (maxweight, nvar) 


  static std::deque<Tcoord<T>*>&        coordinates_;    // used only during new environment creation 
  static std::deque<Tparam<T>*>&        parameters_;     // used only during new environment creation
  static std::list<ScratchArea<T>* >&   scratch_areas_;  // list of existing scratch areas 
  static std::list<EnvPtr<T> >&         environments_;   // list of existing environments
  static std::stack<EnvPtr<T> >         envstack_;       // env stack 

  static int   tmp_maxWeight_; // used by Begin/EndEnvironment() 

  ScratchArea<T>*  buildScratchPads(int maxweight, int numvar);

  TJetEnvironment(int maxweight, int nvar, int spacedim );

  TJetEnvironment(int maxweight, int nvar, int spacedim, TVector<T> const& = TVector<T>() );

  TJetEnvironment(TJetEnvironment const&);

};

//------------------------------------------------------------------------------------------------------------
// specializations
//------------------------------------------------------------------------------------------------------------


template<>
template<>
EnvPtr<std::complex<double> >  TJetEnvironment<std::complex<double> >::makeJetEnvironment( EnvPtr<double> const&);

template <>
template <>
bool  TJetEnvironment<std::complex<double> >::operator==( TJetEnvironment<double> const& ) const;

template <>
template <>
bool  TJetEnvironment<double>::operator==( TJetEnvironment<std::complex<double> > const& ) const;


//------------------------------------------------------------------------------------------------------------
// inline members
//------------------------------------------------------------------------------------------------------------


template <typename T>
inline int  TJetEnvironment<T>::offsetIndex(IntArray const& exp) const
{
  return scratch_->offsetIndex(exp);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
inline int      TJetEnvironment<T>::weight( int const& offset )       const  
{ 
   return scratch_->index_table_[offset].Weight(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
inline int      TJetEnvironment<T>::weight_offset( int const& weight )       const  
{ 
   return scratch_->weight_offsets_[weight]; 
}


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJetEnvironment.tcc>
#endif

#endif // TJETENV_H
