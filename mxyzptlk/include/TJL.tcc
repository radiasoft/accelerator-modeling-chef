/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL.tcc
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
******
******  REVISION HISTORY:
******
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  - Efficiency improvements.
******  - new memory management scheme
******
******  Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to use a single class template parameter
******   instead of two. Mixed mode operations handled using 
******   implicit conversions.
****** - reference counting based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to implementation class TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
****** 
****** Sep 2006  ostiguy@fnal.gov
******
****** -  eliminated doubly linked list representation for polynomials
****** -  count_ now represents the no of stored monomials with weight > 1 
****** -  getCount() returns the no of monomials with a non-zero coefficient     
****** -  non-sparse representation for linear terms 
****** -  efficiency improvements taking advantage of the above
****** -  some members inlined on the basis of profiling    
****** 
****** Mar 2007 ostiguy@fnal.gov  
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   to replace previous scheme based on explicit monomial exponent tuples.
****** - monomial multiplication handled via a lookup-table.
****** - added STL compatible monomial term iterators    
****** - added get/setTermCoefficient() to get/set specific monomial coefficient
******
******  Mar 2008 ostiguy@fnal
******  - composition and evaluation code refactored and optimized. 
******  - more optimizations in pow(), sqrt(), sin() and cos() to 
******    eliminated some superfluous deep copies.   
******
******  Aug 2008 ostiguy@fnal
******  - added in-place add/subtract implementation 
******  - member implementations using iterators rather than underlying 
******    raw pointer types. 
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <iomanip>
#include <fstream>
#include <limits>
#include <algorithm>
#include <iterator>
#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJetVector.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>             // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <basic_toolkit/GenericException.h>

#include <cstring> // for memmove


using FNAL::pcerr;
using FNAL::pcout;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  static data members
//------------------------------------------------------------------------


template<typename T>
//const double TJL<T>::mx_small_     = 10.0*std::numeric_limits<double>::epsilon();
const double TJL<T>::mx_small_     = 1.0e-12;

template<typename T>
const int TJL<T>::mx_maxiter_  = 100;          // Maximum number of iterations allowed  
                                              //  in iterative routines (e.g. transcendental functions)

// ******  a global container for discarded Jets. 

template <typename T> 
std::vector<TJL<T>* >& TJL<T>::thePool_ = *( new std::vector<TJL<T>* > ); 


// ================================================================
//      Implementation of exceptions
//------------------------------------------------------------------

template<typename T>
TJL<T>::BadDimension::BadDimension( int i, int j, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: xdim(i), ydim(j)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** First  argument has " << xdim << " coordinates."
      << "\n*** ERROR *** Second argument has " << ydim << " coordinates."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadEnvironment::BadEnvironment( int a, int b, int c, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(a), env_1(b), env_2(c)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** comp[" << i << "].Env = " << env_1
      << "\n*** ERROR *** this->myEnv = " << env_2
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadReference::BadReference( int ndx, double u,
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(ndx), im(u)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** The imaginary part of _refPoint[ " << i << " ]"
      << "\n*** ERROR *** is equal to " << im <<  ", which is not zero."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadReference::what() const throw()
{
  return errorString.c_str();
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::initStore( int capacity) {

  jltermStoreCapacity_    = capacity;
  jltermStore_            = TJLterm<T>::array_allocate( jltermStoreCapacity_ ); 
  jltermStoreCurrentPtr_  = jltermStore_;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL(EnvPtr<T> const& pje, T x)
  :  myEnv_( pje ), 
     count_(0), 
     weight_(0),                        
     accuWgt_( pje->maxWeight() ),
     lowWgt_(0)
{
   initStore( pje->numVar() +1 );    // allocate mem for all terms at order 1  
   appendLinearTerms( pje->numVar());
   setStandardPart(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::makeTJL( EnvPtr<T> const& pje, T x )
{

  if (!pje) return JLPtr<T>();  // this form is called by TCoord and Tparam

  if (thePool_.empty() ) return (  JLPtr<T>(new  TJL<T>(pje, x ) )); 
 
  TJL<T>* p    = thePool_.back();  thePool_.pop_back();
  p->count_    = 0;
  p->weight_   = 0;
  p->accuWgt_  = pje->maxWeight();
  p->lowWgt_   = 0;
  p->myEnv_    = pje; 

  p->appendLinearTerms( pje->numVar());
  p->setStandardPart(x); 

  return  JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( IntArray const& e, const T& x, EnvPtr<T> const& pje ) :
 myEnv_(pje), 
 count_(0),
 weight_(0),
 accuWgt_(pje->maxWeight() ),
 lowWgt_(0),
 jltermStoreCapacity_(0), 
 jltermStore_(0), 
 jltermStoreCurrentPtr_(0)
{

 initStore( pje->numVar() +1 );    // allocate all terms at order 1  
 appendLinearTerms( pje->numVar() );
 
 TJLterm<T> term(e, x, pje);  

 int indy = 0;

 if ( term.weight_ == 0) {  
  jltermStore_[0].value_ = x;
  return;
 }
   
 if ( term.weight_ == 1) {  
   jltermStore_[term.offset_].value_ = x; 
   return; 
 }
 
 if ( term.weight_ > 1 ) {   
    append( term ); 
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template<typename T>
 JLPtr<T> TJL<T>::makeTJL( IntArray const& e, T const& x, EnvPtr<T> const& pje ) 
{

  if (!pje) return JLPtr<T>();  // cannot create a TJL without a properly constructed environemnt 

  if (thePool_.empty() ) return  JLPtr<T>( new TJL<T>(e,x,pje) );

  TJL<T>* p = thePool_.back(); thePool_.pop_back();

  p->count_     = 0;
  p->weight_    = 0;
  p->accuWgt_   = pje->maxWeight();
  p->lowWgt_    = 0;
  p->myEnv_     = pje; 


  TJLterm<T> term(e, x, pje);
  int indy = 0;

  p->appendLinearTerms( pje->numVar() );

  if ( term.weight_ == 0) {  
   p->jltermStore_[0].value_ = x;
   return JLPtr<T>(p);
  }
   
  if ( term.weight_ == 1) {  
    p->jltermStore_[term.offset_].value_ = x; 
    return  JLPtr<T>(p);
  }
 
  if ( term.weight_  > 1 ) {  
     p->append( term ); 
  }

  return JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( TJL<T> const& x ): ReferenceCounter<TJL<T> >(),  
 myEnv_(x.myEnv_),
 count_(x.count_),                     
 weight_(x.weight_),     
 accuWgt_(x.accuWgt_),
 lowWgt_(x.lowWgt_),
 jltermStore_(0), 
 jltermStoreCurrentPtr_(0), 
 jltermStoreCapacity_(0)
{
 
 initStore( x.jltermStoreCapacity_ );
 memcpy( jltermStore_, x.jltermStore_, (x.jltermStoreCurrentPtr_-x.jltermStore_)*sizeof(TJLterm<T>) );
 jltermStoreCurrentPtr_ = jltermStore_ + (x.jltermStoreCurrentPtr_ - x.jltermStore_);


} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename U>
JLPtr<T>  TJL<T>::makeTJL( TJL<U> const& x )
{

  if (thePool_.empty() ) 
     return  JLPtr<T>(new TJL<T>(x));
 
  TJL<T>* p = thePool_.back(); thePool_.pop_back(); 
  
  if (p->jltermStoreCapacity_ < x.jltermStoreCapacity_)  
  { 
       TJLterm<T>::array_deallocate( p->jltermStore_ );
       p->initStore(x.jltermStoreCapacity_);   
  
  }

  p->count_    = x.count_;   
  p->weight_   = x.weight_;  
  p->accuWgt_  = x.accuWgt_;
  p->lowWgt_   = x.lowWgt_;
  p->myEnv_    = x.myEnv_;
 
   memcpy( p->jltermStore_, x.jltermStore_, (x.jltermStoreCurrentPtr_-x.jltermStore_)*sizeof(TJLterm<T>) );
   p->jltermStoreCurrentPtr_ = p->jltermStore_ + (x.jltermStoreCurrentPtr_ - x.jltermStore_);

  return JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::~TJL() 
{
 clear();
 TJLterm<T>::array_deallocate( jltermStore_ );

 jltermStore_         = jltermStoreCurrentPtr_ = 0;
 jltermStoreCapacity_ = 0;
 

 // myEnv_ is purposely NOT deleted, of course.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::growStore( ) {
 
TJLterm<T>* old_jltermStore           = 0;
TJLterm<T>* old_jltermStoreCurrentPtr = 0;
int         old_jltermStoreCapacity   = 0;


  // grow the allocation vector ....

  // save the current store ptrs 

    old_jltermStore           = jltermStore_;
    old_jltermStoreCurrentPtr = jltermStoreCurrentPtr_;
    old_jltermStoreCapacity   = jltermStoreCapacity_;

    jltermStoreCapacity_ *= 2;  // double the capacity
    jltermStore_ = jltermStoreCurrentPtr_ = TJLterm<T>::array_allocate( jltermStoreCapacity_ );
 

    memcpy( jltermStore_, old_jltermStore, (old_jltermStoreCurrentPtr-old_jltermStore )*sizeof(TJLterm<T>) );
    jltermStoreCurrentPtr_ = jltermStore_ + (old_jltermStoreCurrentPtr - old_jltermStore );


    // now OK to zap the old store
 
    TJLterm<T>::array_deallocate( old_jltermStore );

}   

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||


template<typename T>
int TJL<T>::getCount() const { 

  const double eps = mx_small_;

  // returns the "non-zero" term count 

  int count = count_;

  int lterms = myEnv_->numVar(); 

  TJLterm<T> const * p =jltermStore_;

  //-----------------------------------------------------
  // add the non-zero *linear* terms, if any 
  // NOTE: the trigonometric function convergence depend 
  //       on getCount() reporting 0 ! 
  //-----------------------------------------------------

  for (TJLterm<T> const * p=jltermStore_; p<jltermStore_+lterms+1; ++p ) {

    if (std::abs(p->value_) > mx_small_ ) ++count;   

  }   

  return count; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::removeTerm( TJLterm<T> const& a) 
{

  for ( TJLterm<T>* p=jltermStore_; p<jltermStoreCurrentPtr_; ++p) { 

    if ( p->offset_  == a.offset_ ) {
         p->value_  = T();
         break; // term has been found
       }
  }

  //----------------- 
  // reset max weight 
  //-----------------

  for ( TJLterm<T>* p=jltermStoreCurrentPtr_-1;  p>=jltermStore_; --p) {
    
      if ( p->value_  !=  T() ) {
          weight_ = p->weight_;
          break;
      }
  }
    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::addTerm( TJLterm<T> const& a )
{

 int indy = 0;
 std::vector<TJLterm<T> >& tjlmml =  myEnv_->TJLmml(); 
 
 //------------------------
 // transfer to scratchpad
 //------------------------

 for (TJLterm<T> const* p = jltermStore_; p < jltermStoreCurrentPtr_; ++p) { 
 
    tjlmml[p->offset_].value_ = p->value_; 	 
 } 	 
 

 // add the term argument to value in scratch pad
 //----------------------------------------------

 tjlmml[a.offset_].value_ += a.value_;

 // -----------------------------------------------------------
 // clear current TJL , transfer back result from scratch pad,  
 // and cleanup
 //
 // NOTE:  accuWgt_ is *reset* by transferFromScratchPad
 //        because clear is called(). It needs to be 
 //        saved and restored. 
 //------------------------------------------------------------

 int weight  = weight_;
 int accuWgt = accuWgt_;

 transferFromScratchPad();   

 // adding a term may increase the maximum weight 
 // however, max accurate weight remains unchanged.

  weight_ =   std::max( weight,  a.weight_ );  
  accuWgt_ =  accuWgt;

 return;
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJL<T>::isNilpotent() const 
{
 return ( std::abs(standardPart()) <= mx_small_ ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::writeToFile( std::ofstream& outStr ) const 
{

 for ( TJLterm<T> const* p = jltermStore_; p < jltermStoreCurrentPtr_; ++p) { 

   outStr << "Weight: "   << p->weight_
          << "   Value: " << p->value_
          << std::endl;
   outStr << "Index:  ";

   for( int i=0; i < myEnv_->numVar(); ++i ) {
     outStr <<  myEnv_->exponents(p->offset_)[i]  << "  ";
   }
   outStr << std::endl;
   }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::Negate( ) 
{
 const_iterator pend = end();
 for ( iterator p = begin(); p != pend; ++p ) {
   p->value_ = -p->value_;
 }
 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setVariable( int const& j, T const& x, EnvPtr<T> pje)
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= myEnv_->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( int const& j, T const& x, EnvPtr<T> pje )", 
     "Index out of range") );
 }
 
 clear(); 

 myEnv_   = pje;
 accuWgt_ = pje->maxWeight();
 weight_  = 0;
 
 appendLinearTerms(  myEnv_->numVar() );
 setStandardPart(x);
 jltermStore_[j+1].value_ =  T(1.0);

 lowWgt_  = ( standardPart() != T() ) ? 0 : 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::clear() 
{
 accuWgt_ = 0;
 weight_  = 0;
 count_   = 0;
 jltermStoreCurrentPtr_  = jltermStore_; // do not release the store memory here !
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T  TJL<T>::weightedDerivative( IntArray const& exp ) const
{

   // Note: the monomial terms are always sorted w/r to offset index !

   int offset = myEnv_->offsetIndex(exp);

   TJLterm<T> term( T(), offset, exp.Sum() );

   std::pair<typename TJL<T>::const_iterator, typename TJL<T>::const_iterator> 
      result = std::equal_range( begin(), end(), term);
   

   if (result.first == result.second ) {
    return T(); // empty range;
   }

   return result.first->value_; 
   
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setWeightedDerivative( IntArray const& exp, T const&   value) 
{
  T oldvalue =  weightedDerivative( exp );
  addTerm( TJLterm<T>( exp, (-oldvalue + value), myEnv_ ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T TJL<T>::derivative( IntArray const& ind ) const 
{
 T d = weightedDerivative( ind );
 if ( d == T() ) return 0.0;
 
 double multiplier = 1.0;

 for ( int i=0; i < myEnv_->numVar(); ++i ) {
   double n = ind[i];
   while(n)  multiplier *= n--;
 }
 
 return multiplier*d;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setDerivative( IntArray const& ind, T const& value) 
{
 
 double multiplier = 1.0;

 for ( int i=0; i < myEnv_->numVar(); ++i ) {
   double n = ind[i];
   while(n)  multiplier *= n--;
 }
 
 setWeightedDerivative(ind, value/multiplier );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename const_iterator_t>
T TJL<T>::evaluate( const_iterator_t itb, const_iterator_t ite) const 
{

 std::vector<T> u( myEnv_->numVar() );
 
 // Subtract off the reference point.

 int i=0; 
 for( const_iterator_t it = itb; it != ite; ++it, ++i ) { 
   u[i] = (*it) - myEnv_->refPoint()[i]; 
 }
 
 // Evaluate and store monomials.
   
 std::vector<T>& monomial =  myEnv_->monomial(); 

 // The zeroth one.
 
 monomial[0] = T(1.0);

 //--------------------------------------------
 // For all weights higher than 0, 
 // Get the next set of exponents of weight w.
 //---------------------------------------------

 int const maxterms = bcfRec( weight_ + myEnv_->numVar(), myEnv_->numVar() );
 
 for ( int idx=1; idx<maxterms; ++idx) {

   IntArray exponents = myEnv_->exponents(idx);

   IntArray::iterator p = std::find_if( exponents.begin(), exponents.end(), std::bind2nd( std::not_equal_to<int>(),0 ) ); 
   int i =  std::distance(exponents.begin(),p);
 
   // The value of the _monomial associated with this composition
   // is obtained by multiplying a factor into a previously
   // computed monomial.

   --exponents[i];
   T term = monomial[ myEnv_->offsetIndex( exponents ) ];
   ++exponents[i];

   monomial[ myEnv_->offsetIndex( exponents ) ] = term * u[i];
 
 }
 
 // Monomials have been now be evaluated.
 // Now traverse the JL variable and evaluate.
 
 T v = T();
 
 for ( TJLterm<T>* p = jltermStore_; p < jltermStoreCurrentPtr_; ++p) {
   v += ( p->value_ ) * ( monomial[ p->offset_] );
 }
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename const_iterator_t>
JLPtr<T> TJL<T>::compose(  const_iterator_t itb, const_iterator_t ite ) const //  Jet composition operator  
{

 //-------------------------------------------------
 //  **** Composition operator ****
 //-------------------------------------------------

 std::vector<JLPtr<T> > u( myEnv_->numVar() );

 //------------------------------------------------
 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 //-------------------------------------------------

 int i = 0;
 for( const_iterator_t it = itb; it != ite; ++it, ++i) {
   if(  (*it)->getEnv() !=   (*itb)->getEnv() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "TJL<T>::compose(  const_iterator_t itb, const_iterator_t ite ) const ",
             "Inconsistent environments." ) );
    }
 

    u[i] =  (*it) + JLPtr<T>( makeTJL( (*it)->getEnv(), -( (*it)->getEnv()->refPoint()[i] ) ) ); // u[i] = y[i] - myEnv_->refPoint()[i];
 }


 std::vector<JLPtr<T> >& tjlmonomial = myEnv_->TJLmonomial(); // for monomials used in multinomial evaluation.  

 // -----------------------------
 // Evaluate and store monomials.
 // -----------------------------

 // The zeroth one ...

 tjlmonomial[0] = JLPtr<T>(makeTJL( u[0]->myEnv_, ((T) 1.0) ));

 // For all weights higher than 0 ...
 // Get the next set of exponents of weight w.

 int const maxterms = bcfRec( weight_ + myEnv_->numVar(), myEnv_->numVar() );

 for ( int idx=1; idx<maxterms; ++idx) {

   IntArray exponents = myEnv_->exponents(idx);

   // Find the first non-zero exponent.

   IntArray::iterator p = std::find_if( exponents.begin(), exponents.end(), std::bind2nd( std::not_equal_to<int>(),0 ) ); 
   int i =  std::distance( exponents.begin(), p );
 
   // The value of the monomial associated with this composition
   // is obtained by multiplying a factor into a previously
   // computed monomial.

   --exponents[i];
   JLPtr<T> term = tjlmonomial[ myEnv_->offsetIndex( exponents ) ];
   ++exponents[i];

   tjlmonomial[ myEnv_->offsetIndex( exponents )] = u[i]*term; 
   
 } 
 
 // -----------------------------------------------
 // Monomials have been stored.
 // Now traverse the TJL<T> variable and evaluate.
 //------------------------------------------------
 
 JLPtr<T> z( makeTJL(u[0]->myEnv_) ); 

 for(  TJLterm<T> const* p = jltermStore_; p< jltermStoreCurrentPtr_; ++p  ) {
    if( p->weight_ > accuWgt_ ) break;
    z = z + ( tjlmonomial[ p->offset_ ] *  (p->value_) ); 
 } 
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
double TJL<T>::maxAbs( ) const 
{

 double maxabs = 0;

 for(  TJLterm<T> const* p = jltermStore_; p< jltermStoreCurrentPtr_; ++p  ) {
   maxabs = std::max( maxabs, std::abs(p->value_) );
 }
 return maxabs;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::filter( int const& wgtLo, int const& wgtHi ) const 
{ 
 //----------------------------------------------------------------------------
 // Filters the terms with weights included in the closed interval [wgtLo,wgtHi]
 //----------------------------------------------------------------------------

 // Trivial case: nothing to filter ... return a *deep copy* of the current jl.
 
 if( ( wgtLo <= 0 ) && ( wgtHi >= weight_ ) ) {
   return clone();
 } 

 // Begin filtering ... 

 JLPtr<T> z( TJL<T>::makeTJL( myEnv_) );
 
 int      wgt = 0;
 int upperWgt = 0;
 
 for ( TJLterm<T> const *p = jltermStore_; p < jltermStoreCurrentPtr_; ++p) {

   wgt = p->weight_;
   if( (wgt < 2) && (wgt >= wgtLo) && ( wgt <= wgtHi ) ) {
     int indy = p->offset_;
     z->jltermStore_[indy].value_ = p->value_;
   }
   else if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     z->append(*p);
     upperWgt = std::max(  wgt, upperWgt); 
   }
 }
 
 z->weight_  = upperWgt;             // ??? Where is the copy of the re
 z->accuWgt_ = accuWgt_;             // ??? reference point???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::filter( bool (*f) ( IntArray const& index, T const& ) ) const 
{ 

 JLPtr<T> z( TJL<T>::makeTJL(myEnv_) );

 int nv = myEnv_->numVar();

 for ( TJLterm<T>* p = jltermStore_; p < jltermStoreCurrentPtr_; ++p) {

  if( (*f)(  myEnv_->exponents(p->offset_), p->value_ ) ) {

     if( p->weight_ < 2 ) {
       z->jltermStore_[p->offset_].value_ = p->value_;
     }
     else {
         z->append(*p);
     }
  }
 }
 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::truncMult( JLPtr<T> const& v, const int& wl ) const 
{ 
 //
 // Truncated multiplication: used only by the division operator.
 // No checks are made on the environments.
 //

 // If one of the arguments is void, return a *deep copy* of it ..

 if ( (getCount()    == 0) )     return  clone();
 if ( (v->getCount() == 0) )     return  v->clone();

 // .. otherwise continue normal operations.

 // Initializations

 JLPtr<T> z( TJL<T>::makeTJL( myEnv_) );

 std::vector<TJLterm<T> >& tjlmml =  myEnv_->TJLmml(); 

 const_iterator const vstart    = v->begin();
 const_iterator const vend      = v->end();
 const_iterator const thisstart = begin();
 const_iterator const thisend   = end();

 for (  const_iterator p = vstart; p != vend; ++p ) {
    for ( const_iterator q = thisstart; q != thisend; ++q ) {

      if( ( p->weight_ + q->weight_ ) > wl ) continue;
      
      int indy = myEnv_->multOffset( p->offset_ , q->offset_ );

   // Will work even when the exponents are all zero.

     T  product; 
     T& value = tjlmml[indy].value_ += ( product = ( p->value_ * q->value_ ) );

     if ( ( std::abs(value) <  TJL<T>::mx_small_* std::abs( product ) ) || 
          ( std::abs(value) <  TJL<T>::mx_small_ )  ) { 
             value = T();
     }
 }}

//--------------------------------------
// Transfer answer from the scratchpad.
//--------------------------------------

 int accuWgt    =  accuWgt_;
 z->transferFromScratchPad();
 
 z->accuWgt_ = std::min(accuWgt, v->accuWgt_);
 
 return  z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//------------------------------------------------------------------
//      Operators for class TJL
//------------------------------------------------------------------

template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL<T>& x ) 
{


 os << "\n Count  = " << x.getCount() << " , Weight = " << x.weight_;
 os << " , Max accurate weight = " << x.accuWgt_ << std::endl;
 os << "Begin Environment: \n"
    << *(x.myEnv_)
    << "\nEnd Environment: " << std::endl;

 
 for ( TJLterm<T> const* p = x.jltermStore_; p < x.jltermStoreCurrentPtr_; ++p) {

   //if ( std::abs(p->value_) < 10.0*std::numeric_limits<double>::epsilon() ) continue;

   os << "Index: ";
   os << x.myEnv_->exponents(p->offset_) << " ";
   os << "   Value: " << std::setprecision(30) << p->value_ << std::endl;
 }

 return os << "\n" << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TJL<T> const& lhs,  TJL<T> const& rhs ) {

 double const eps = 10.0e4* std::numeric_limits<double>::epsilon( ); // on a 32-bit Pentium  ~10e-12 

 if( lhs.myEnv_ != rhs.myEnv_ ) return false;

 if( ( lhs.weight_  != rhs.weight_ ) ) return false;
 
 // this is not very efficient because it involves copying the arguments ! 

 JLPtr<T> diff = lhs.clone() - rhs.clone() ; 

 for ( TJLterm<T> const* p = diff->jltermStore_; p < diff->jltermStoreCurrentPtr_; ++p ) {

   if ( std::abs(p->value_) > eps ) return false;
}
return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TJL<T> const& lhs, T const& rhs ) {

 double const eps = 10.0e4* std::numeric_limits<double>::epsilon( ); // on a 32-bit Pentium  ~10e-12 

 if ( std::abs( lhs.standardPart() - rhs ) > eps )   return false;

 // all terms of weight  >0  must also have value_ == 0 (coefficient)

 for (  TJLterm<T> const* p = lhs.jltermStore_+1; p < lhs.jltermStoreCurrentPtr_; ++p) {
   if ( std::abs(p->value_) >  eps ) return false;
 } 

 return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( T const& lhs, JLPtr<T> const& rhs )
{
 return rhs == lhs;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=(  TJL<T> const& lhs, TJL<T> const& rhs ) 
{
 return !( lhs == rhs );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( TJL<T> const& lhs, T const& rhs ) 
{
 return !( lhs == rhs);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=(  T const& lhs, TJL<T> const& rhs ) 
{
 return !(rhs == lhs );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator=( T const& x ) 
{
 clear();

 appendLinearTerms( myEnv_->numVar() ); 
 setStandardPart(x);

 accuWgt_ = myEnv_->maxWeight(); // is this correct ?

 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator=( TJL<T> const& x )  {

 if( this == &x ) return *this;

 ReferenceCounter<TJL<T> >::operator=(x);  

 TJLterm<T>* old_jltermStore = 0;
 clear(); 

 if ( myEnv_ == x.myEnv_ )  // LHS and RHS env are identical. Perform a fast copy and return.
 { 
  
     if ( jltermStoreCapacity_ < x.jltermStoreCapacity_ )  {
          old_jltermStore = jltermStore_;
          jltermStore_ = jltermStoreCurrentPtr_ =  TJLterm<T>::array_allocate(x.jltermStoreCapacity_);
          jltermStoreCapacity_ = x.jltermStoreCapacity_;
     };
      memcpy( jltermStore_, x.jltermStore_, (x.jltermStoreCurrentPtr_-x.jltermStore_)*sizeof(TJLterm<T>) );
     jltermStoreCurrentPtr_ = jltermStore_ + (x.jltermStoreCurrentPtr_-x.jltermStore_);

     if (old_jltermStore) { TJLterm<T>::array_deallocate( old_jltermStore ); }

     count_   = x.count_;    
     weight_  = x.weight_;
     accuWgt_ = x.accuWgt_;
     return *this;
 }

 if ( myEnv_->numVar() !=  x.myEnv_->numVar() ) {  // **** No of variables is not the same ! No conversion possible. 
     throw( GenericException( __FILE__, __LINE__, 
     "TJL<T>& TJL<T>::operator=( TJL<T> const& rhs)", 
     "Environments do not have the same no of variables. Conversion is impossible.)" ));
 } 

 bool shifting_required = false;

 std::vector<T> refshift;

   for ( int i=0; i< myEnv_->numVar() ; ++i) {
     refshift.push_back(  myEnv_->refPoint()[i] - x.myEnv_->refPoint()[i] );
     if ( refshift.back() != T() ) shifting_required = true;
   }  

  if ( shifting_required ) {
     throw( GenericException( __FILE__, __LINE__, 
     "TJL<T>& TJL<T>::operator=( TJL<T> const& rhs)", 
     "Environments do not have the same reference point. Shifting is not supported at this time.)" ));
  } 


  //------------------------------------------------------------------
  // If we get here, the two env are different but compatible.
  // Copy rhs terms, but drop orders higher than the maxweight in lhs.
  //------------------------------------------------------------------


  TJLterm<T>* p = jltermStore_;

  int const lhsmaxweight = myEnv_->maxWeight();

  if ( jltermStoreCapacity_ < x.jltermStoreCapacity_ )  {
          old_jltermStore = jltermStore_;
          jltermStore_ = jltermStoreCurrentPtr_ =  TJLterm<T>::array_allocate(x.jltermStoreCapacity_);
          jltermStoreCapacity_ = x.jltermStoreCapacity_;
  };

  count_ = 0; 
  for ( TJLterm<T> const* px = x.jltermStore_; 
                          px < x.jltermStoreCurrentPtr_; ++px, ++p) {
      
        if  ( ( x.myEnv_->weight(px->offset_) ) >  lhsmaxweight ) continue; 
        if  ( ( x.myEnv_->weight(px->offset_) ) <  2            ) ++count_; 
        
        *p = *px; 

  }

  if (old_jltermStore) { TJLterm<T>::array_deallocate( old_jltermStore ); }

  weight_  = lhsmaxweight;
  accuWgt_ = std::min( x.accuWgt_, lhsmaxweight);

 
  return   *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> & lhs, T const& rhs  )  
{

 typename TJL<T>::const_iterator pend = lhs->end();
 for ( typename TJL<T>::iterator p = lhs->begin(); p != pend; ++p ) {
   p->value_ *= rhs;               
 }

 return lhs;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T>& operator+=( JLPtr<T>& lhs, T const& x ) 
{   
  lhs->jltermStore_->value_ += x; 
  return lhs;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JLPtr<T>& operator-=( JLPtr<T>& lhs, T const& x ) 
{   
  lhs->jltermStore_->value_ -= x; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T>  TJL<T>::sin() const
{ 

 JLPtr<T> epsilon = clone(); 

 if( epsilon->standardPart() != T() ) {            // jet has non-zero standard part

   T cs = std::cos( epsilon->standardPart()  );
   T sn = std::sin( epsilon->standardPart()  );
   epsilon->setStandardPart( T() );                  // zero out the standard part;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------

   JLPtr<T> eps_c = epsCos( epsilon ); 
   JLPtr<T> eps_s = epsSin( epsilon ); 

   return  ( (eps_c *= sn) + (eps_s *=cs) );
  
 }

 else {                               

   return epsSin( epsilon ); // argument is already a pure infinitesimal

 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T>  TJL<T>::cos() const
{ 
 JLPtr<T> epsilon = clone(); 
 
 if( epsilon ->standardPart() != T() ) {                // jet has non-zero standard part
   T cs = std::cos( epsilon ->standardPart() );
   T sn = std::sin( epsilon ->standardPart() );
   epsilon->setStandardPart( T() );                            // zero out the standard part;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------
      
   JLPtr<T> eps_c = epsCos( epsilon ); 
   JLPtr<T> eps_s = epsSin( epsilon ); 

   return  (  (eps_c *= cs) - (eps_s *= sn) );
 }
 else {                               
   return epsCos( epsilon ); // epsilon is pure infinitesimal
 }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::epsSin( JLPtr<T> const& epsilon ) 
{ 
 
 JLPtr<T> z( epsilon->clone() );               //    z = epsilon -- deep copy 

 JLPtr<T> epsq = epsilon*epsilon;
 epsq->Negate();                               //   epsq = -epsilon*epsilon 

 JLPtr<T> term =  epsilon*epsq;                // term = 1/6 * epsilon*epsq
 term *= T(1.0/6.0);
 
 double n = 3.0;

  int nsteps = 0;
  while( term->getCount() > 0 ) {

    z    = z + term;    
    term *= epsq;  
    double n1 = ++n;
    double n2 = ++n;
    term *=  T(1.0/(n1*n2));                          
    ++nsteps;

    if ( nsteps > mx_maxiter_ ) { 
              (*pcerr)  << "*** WARNING **** More than " << mx_maxiter_ 
                        << "iterations in epsSin " << std::endl 
                        << "*** WARNING ***  Results may not be accurate."
                         << std::endl;
              break;
    }
  } 
  z->accuWgt_ = epsilon->accuWgt_;
  return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::epsCos( JLPtr<T> const& epsilon ) 
{ 
 
 JLPtr<T> z( makeTJL( epsilon->myEnv_, ((T) 1.0) ));             // z   = 1    

 JLPtr<T> epsq = epsilon*epsilon;
 epsq->Negate();                                // epsq = -epsilon*epsilon

 JLPtr<T> term(epsq->clone());
 term *= T(0.5);                                // term = epsq/2.0

 double n = 2.0;
 
  int nsteps =0;
  while( term->getCount() > 0 ) {
    z     = z +term;                                // z += term;
    term *= epsq;                                   // term->multiply(epsq);
    double n1  = ++n; 
    double n2  = ++n;
    term *= 1.0/static_cast<T>(n1*n2);              // term = ( ( term*epsq ) / ++n ) / ++n;  
    ++nsteps;
    if ( nsteps > mx_maxiter_ )  { 
            (*pcerr) << "*** WARNING **** More than " << mx_maxiter_
                     << "iterations in epsCos " << std::endl  
                     << "*** WARNING **** Results may not be accurate." 
                     << std::endl; 
            break;
    }   
 }
 
 z->accuWgt_ = epsilon->accuWgt_;

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::sqrt() const 
{

   if( getCount() == 0 ) {

   // * the code below should be replaced by a more permanent 
   // * solution; possibly a function that clears the scratchpads for all
   // * existing environments. 
   // * The scratchpads should be cleared when *any* exception is thrown
   // * within mxyzptlk because the TJLterm _value fields may contain  
   // * "Nan"s. 
   // *DO NOT REMOVE OR COMMENT OUT UNLESS YOU HAVE IMPLEMENTED A BETTER SOLUTION !* 
   //   
   // -JFO 

   //--------------------------------------------------------------------------

   (*pcout) << "Resetting Environment Scratchpad" << std::endl;
   for (int i=0; i< myEnv_->maxTerms(); i++ ) {
         myEnv_->TJLmml()[i].value_ = 0.0;
   }          
   
   //--------------------------------------------------------------------------

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() { ",
          "Argument is zero." ) );
 }
 

 if( standardPart() !=  T() ) {   // non-zero standard part
   if( getCount() == 1 ) {          // ... operand may have no derivatives
     return JLPtr<T>( makeTJL(myEnv_, std::sqrt(standardPart()) )); 
   }

   JLPtr<T> epsilon = clone(); // deep copy 
   T factor = standardPart();
   epsilon->setStandardPart( T() );                     
   epsilon *=  T(1.0)/factor;
   epsilon  =  epsPow( epsilon, 0.5);
   epsilon *=  T(std::pow(factor,0.5));
   return epsilon;
  }
 else { // nilpotent argument
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() ",
          "Argument's standard part vanishes; it is nilpotent." ) );
  
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T>  TJL<T>::exp() const
{
  JLPtr<T> epsilon = clone(); // deep copy 

  T factor = T();

  if( standardPart() !=  T() ) {                   // x has non-zero standard part
    factor = std::exp( standardPart() );
    epsilon->setStandardPart( T() );               // zero out the standard part off epsilon
    JLPtr<T> res = epsExp( epsilon );
    res *= factor;
    return res;
    }
  else {                               // x is already a pure infinitesimal
    return epsExp( epsilon );
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JLPtr<T>  TJL<T>::epsExp( JLPtr<T> const& epsilon ) 
{ 

 JLPtr<T> z( makeTJL(epsilon->myEnv_, ((T) 1.0)  ));    // z    = 1.0;
 JLPtr<T> term = epsilon->clone();                      // term = epsilon

 double n = 1.0;
 
 int nsteps = 0;
 while( term->getCount() > 0 ) {
   z  = z +term;                                          // z += term;
   term *= epsilon;  
   term *= T (1.0/(++n));                                 // term = ( term*epsilon ) / ++n;
 }
  
 z->accuWgt_ = epsilon->accuWgt_;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow( JLPtr<T>& x, int n ) // arg is not modified
{
  if ( n > 0 ) { 
     if( n%2 == 0 ) { 
        if( n == 0 ) { return JLPtr<T>( makeTJL( x->myEnv_, ((T) 1.0))); }  
        JLPtr<T> result  = TJL<T>::pow(x, n/2 );
        return ( result*result );
     }
     else{
        if( n == 1 )  return x;
        JLPtr<T> result  = x->clone();
        return (result * TJL<T>::pow(x, n-1) );
     } 
 }
 else { // exponent is negative
   JLPtr<T> xr = ( makeTJL( x->myEnv_, ((T) 1.0) ))/ x;
   return TJL<T>::pow( xr, -n);
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow( JLPtr<T>& x, double const& s)  
{

 if( x->getCount() == 0 ) 
       return JLPtr<T>( makeTJL( x->myEnv_,0.0 ) );  //  pow(0, s) = 0  
 

 if( x->standardPart()  !=  double()   ) {        // has non-zero standard part
   if(x->getCount() == 1 ) {              // may have no derivatives
     return JLPtr<T>( makeTJL( x->myEnv_, std::pow( x->standardPart(),s )) );
   }

   JLPtr<T> epsilon = x->clone();     //  deep copy
   double factor = std::abs( x->standardPart());
   epsilon->setStandardPart( double() );   
   epsilon *= T(1.0/factor);
   epsilon  = epsPow( epsilon, s );
   epsilon *= T(std::pow(factor,s) );
   return epsilon;
 }
 else {                                // x is pure infinitesimal
   int ni = nearestInteger( s );
   if( s != T(ni) ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T> pow( Jet<T> const&, const double& )",
              "Cannot use infinitesimal as base with non-integer exponent." ) );
   }
   return pow(x, -ni);  
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JLPtr<T> TJL<T>::epsPow( JLPtr<T> const& epsilon, double const& s ) 
{ 
 JLPtr<T> z( makeTJL(epsilon->myEnv_, T(1.0) )); // z = 1.0;
 JLPtr<T> term = epsilon->clone();

 double f = s;
 double n = 1.0;

 term *= T(f);                       //term = s*epsilon;
 
 int nsteps = 0;
 while( term->getCount() > 0 ) {
   z    =  z +term;                                     
   term *= epsilon;                                  
   term *= T(--f)/ static_cast<T>(++n);
   ++nsteps;   
   if ( nsteps > mx_maxiter_ ) { 
       (*pcerr)  << "*** WARNING *** More than " << mx_maxiter_ 
                 << "iterations in epsPow " 
                 << std::endl 
                 << "*** WARNING ***  Results may not be accurate."
                 << std::endl;
       break;          
   } 
 }
 
 z->accuWgt_ = epsilon->accuWgt_;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::log() const
{

 if( getCount() == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log () { ",
          "Argument is zero." ) );
 }
 
 if( standardPart() != T() )  // x has non-zero standard part
 {
   if( getCount() == 0 ) {             // x may have no derivatives
     return JLPtr<T>( makeTJL( myEnv_, std::log( standardPart() )));
   }
   
   //---------------------------------------------------------------
   // We use the formulae 
   //
   //  ln( s + e )   = ln s + ln( 1 + e/s ), and
   //
   //  ln( 1 + e/s ) = ln( 1 - ( -e/s )) = - sum ( - e/s )^n / n
   //----------------------------------------------------------------

   JLPtr<T> z( clone() );
   z->setStandardPart( T() );         //   zero out the standard part 
   z *=   -1.0/standardPart();

   JLPtr<T> u(z->clone() );  // initial value : -e/s                                        
   JLPtr<T> w(z->clone() );  // initial value : -e/s

   double n = 1.0;  
   while( u->getCount() > 0 ) { 
     u *= z;                                  // u =  (-e/s)^n  
     w  = w + ( u / static_cast<T>(++n) );    // w =  sum  (-e/s)^n / n   
   }
   
   JLPtr<T> tmp( makeTJL(myEnv_, std::log(standardPart()) ));
   return ( tmp - w );                            // std::log(standardPart()) - w
 
   }
 else                                 // operand has zero standard part
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log() ",
          "operand's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::istream& operator>>( std::istream& is,  TJL<T>& x ) 
{  

  char buf[100];
  buf[0] = '\0';

  int count;
  T value;

  x.clear();
 
  is >> buf;
  is >> buf;
  is >> count;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.weight_;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.accuWgt_;
  is >> buf;
  is >> buf;
  streamIn( is, x.myEnv_);
  is >> buf;
  is >> buf;

  x.appendLinearTerms( x.myEnv_->numVar() ); 

  IntArray ndx( x.getEnv()->numVar() );

  TJLterm<T> term;

  for( int j=0; j < count; ++j) {
    is >> buf;
    is >> ndx;
    is >> buf;
    is >> value;
 
   term = TJLterm<T>( ndx, value, x.myEnv_ );

    if ( term.weight_ == 0)  x.setStandardPart(value);
    if ( term.weight_ == 1)  { 
            x.jltermStore_[term.offset_].value_ = value;
    }  
    if ( term.weight_ >  1)  {
         x.append(term);
    }
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::asin() const
{ 
 T const zero = T(); 	 
 EnvPtr<T>  pje(myEnv_);
  	 
 // Initial Newton's step 	 
 
 JLPtr<T> z = clone();          // deep copy	        
 JLPtr<T> x = clone();           

   std::vector<TJLterm<T> >& tjlmml =  myEnv_->TJLmml(); 
	 
 // Iterated Newton's steps

 JLPtr<T> dz;
 int upperBound = 8; 	 
 int       iter = 0;	   
 bool converged = false; 	 
 while( !converged  ) 	 
 { 	 
     for ( int idx =0; idx < 8; ++idx ) { // do 8 iterations at a time 	 

      // These two lines are the heart of the calculation: 	 

        dz  = ( z->sin() - x ) / z->cos(); 	 
        z  = z - dz; 	 
     }
	 
     // The rest is just determining when to stop. 	  
     // This procedure could be improved, but it's better 	 
     // than the previous one, which was just comparing 	 
     // dz to zero. 	 
 

     // compare the terms in dz to those in z.  	   
     // NOTE: the last operation performed on z in an in-place add
     //       involving dz. This garantees that each term in dz
     //       has a corresponding term in z.
  
 
     typename TJL<T>::iterator itz  =  z->begin(); 	 
     typename TJL<T>::iterator itdz = dz->begin(); 	 

     const_iterator dzend = dz->end();

     for ( ; itdz != dzend; ++itdz, ++itz ) { 	 

       while ( itz->offset_ != itdz->offset_ ) ++itz;  
       converged = ( std::abs(itz->value_)  >  mx_small_ ) ? ( std::abs( itdz->value_ ) < mx_small_* std::abs(itz->value_) ) 
                                                           : ( std::abs( itdz->value_ ) < mx_small_                        );          
     } 	 
     if ( iter >= mx_maxiter_ ) break;
} 	 
 
 // If stopped because too many iterations,
 // print a warning message


 if( iter >= mx_maxiter_ ) {
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** JLPtr<T>& asin()      \n";
  (*pcerr) << "*** WARNING *** Over " 
       << mx_maxiter_ 
       << " iterations used;    \n";
  (*pcerr) << "*** WARNING *** result may be incorrect.    \n";
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** Initial value:              \n";
  (*pcerr) << "*** WARNING ***                             \n"
       << std::endl;

   this->printCoeffs();
  (*pcerr) << "*** WARNING *** A few iterations:             " << std::endl;
  for( iter = 0; iter < 4; ++iter ) {
   (*pcerr) << "*** WARNING *** Iteration No. " << ( iter+1 ) << std::endl;
   z  = z -dz;
   dz = ( z->sin() - x ) / z->cos();
   (*pcerr) << "*** WARNING *** dz = " << std::endl;
   dz->printCoeffs();
   (*pcerr) << "*** WARNING *** z = " << std::endl;
   z->printCoeffs();
  }
 }


 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::atan() const
{   
 
 JLPtr<T> z( clone() );      // deep copy
 JLPtr<T> x( clone() ); 

 JLPtr<T>   c = z->cos();                             // c = cos( z );
 JLPtr<T>  dz = c*( z->sin() - x*c );

 int iter = 0;
 while( ( dz->standardPart() != T() ) && ( iter++ < mx_maxiter_ ) ) { 
  z = z - dz;
  c = z->cos();                                       // c = cos( z );
  dz = c*( z->sin() - x*c );
  if( iter >= mx_maxiter_ ) {
    (*pcerr) <<  "*** WARNING ***                         \n" 
             <<  "*** WARNING *** JLPtr<T>& atan()            \n" 
             <<  "*** WARNING *** Over " 
             << mx_maxiter_  << " iterations used;    \n"
             <<  "*** WARNING *** result may be incorrect.    \n" 
             <<  "*** WARNING ***                             \n" 
             << std::endl;
     break;
  }
 }
 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::printCoeffs() const {


 (*pcout) << "\nCount  = " << getCount() 
          << ", Weight = " << weight_ 
          << ", Max accurate weight = " << accuWgt_ << std::endl;
 (*pcout) << "Reference point: " 
      << myEnv_->refPoint()[0];

 for( int i=1; i < myEnv_->numVar(); ++i) {
   (*pcout) << ", ";
   (*pcout) << myEnv_->refPoint()[i];
 }
 (*pcout)   << std::endl;


 for(  TJLterm<T> const* p = jltermStore_; p < jltermStoreCurrentPtr_; ++p  ) {

   if( p->weight_ > accuWgt_ ) break;

   if ( std::abs( p->value_ ) < 10.0*std::numeric_limits<double>::epsilon() ) continue; 

   (*pcout) << "Index:  " 
            << myEnv_->exponents(p->offset_) 
            << "   Value: "
            << p->value_
            << std::endl;
 }

 std::cout << "\n" << std::endl;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::D( IntArray const& n ) const 
{

 //----------------------------- 
 // Differentiation operator
 //-----------------------------

 bool doit = false;

 // --- Preliminary check of _index set.
 
 int w = 0;
 
 for( int i=0; i < myEnv_->numVar(); ++i ) {
   if( n[i] < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL<T>::D( const int* ) const",
            "Cannot differentiate with negative _index." ) );
     }
   w += n[i];
 }
 
 if( w > accuWgt_ ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::D( const int* ) const",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) return JLPtr<T>(makeTJL(myEnv_));

 JLPtr<T> z(makeTJL(myEnv_)); 

 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 

 int f = 0;
 int j = 0;

 for(  TJLterm<T>* p= jltermStore_;  p<jltermStoreCurrentPtr_; ++p  ) {
 
   TJLterm<T> q( *p );
   IntArray exponents( myEnv_->exponents( p->offset_) );  
 
   doit = true;
   // -- Reset the _index.

   for( int i=0; i< myEnv_->numVar(); ++i) 
     doit = doit && ( ( exponents[i] -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( int k = 0; k < myEnv_->numVar(); ++k ) {
       j = exponents[k];
       for( int i = 0; i < n[k]; i++ ) f *= ++j;
     }
     if( f <= 0 ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T> TJet<T>::D( const int* ) const",
              "Horrible, unexplainable error while differentiating!" ) );
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T> and
     //    absorb it into the answer.

     (q.value_)   *= f;              
     (q.weight_)  -= w;              
     (q.offset_)   = myEnv_->offsetIndex(exponents); 
   
     if ( q.weight_ < 2 ) {
      z->jltermStore_[q.offset_].value_ = q.value_;  
     }
       else {  
      z->append( q );

     } //  if (q->weight_ < 2 )
   } //  if( doit )
 }
 
  // --- Finally, adjust accuWgt_ and return value

 z->accuWgt_ = accuWgt_ - w;   //  WARNING: This is *correct* ! 


 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                 *** friend functions ******
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <void T_function(T&, T const&)>
JLPtr<T>  TJL<T>::add(JLPtr<T> const & x, JLPtr<T> const& y  )
{

// Check for consistency and set reference point of the sum.

  if( x->myEnv_ != y->myEnv_ ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::add(JLPtr<T> const& x, JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }
  JLPtr<T> z( TJL<T>::makeTJL(x->myEnv_) ); 

    const_iterator const xstart    = x->begin();
    const_iterator const xend      = x->end();
    const_iterator const ystart    = y->begin();
    const_iterator const yend      = y->end();
          iterator const zstart    = z->begin();

  T   value;
  int nlinear = x->myEnv_->numVar() + 1;  // the number of linear terms always present.
 
   const_iterator  px = xstart;
   const_iterator  py = ystart;
         iterator  pz = zstart;

 for (int i=0; i<nlinear; ++i, ++px, ++py, ++pz ) {
    pz->value_ =  px->value_;
    T_function( pz->value_, py->value_);
  }

  while(  (px < xend ) && (py < yend)  ) 
  {
     if ( px->offset_  ==  py->offset_) { 
          value  =  px->value_;  
          T_function( value, py->value_);  
          z->append( TJLterm<T>( value, px->offset_, px->weight_) ); 
          ++px;  ++py;
          continue; 
     }
            

     if ( px->offset_  <   py->offset_) { 
          z->append( TJLterm<T>(*px) );
          ++px; 
          continue; 
     }   


     if ( px->offset_  >   py->offset_)  { 
          T value = T();
          T_function (value, py->value_); 
          z->append( TJLterm<T>(value, py->offset_, py->weight_) );    
          ++py; 
          continue; 
     }   
  }
   
  while ( px<xend ) 
  {
       z->append(*px);
       ++px; 
  }   

  while  ( py<yend ) 
  {
       T value = T();
       T_function (value, py->value_); 
       z->append( TJLterm<T>(value, py->offset_, py->weight_ )); 
       ++py; 
  }   


  z->weight_  = std::max(x->weight_,  y->weight_  );
  z->lowWgt_  = std::min(x->lowWgt_,  y->lowWgt_ );   
  z->accuWgt_ = std::min(x->accuWgt_, y->accuWgt_);

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <void T_function(T&, T const&)>
JLPtr<T>&  TJL<T>::inplace_add(JLPtr<T>& x, JLPtr<T> const& y  )

{

// Check for consistency and set reference point of the sum.

  if( x->myEnv_ != y->myEnv_ ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::inplace_add(JLPtr<T>& x, JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }


  int weight  = x->weight_;
  int lowWgt  = x->lowWgt_;   
  int accuWgt = x->accuWgt_;

  const_iterator const xstart0    = x->begin();
  const_iterator const xend0      = x->end();

  const_iterator const ystart    = y->begin();
  const_iterator const yend      = y->end();

  T   value;
  int nlinear = x->myEnv_->numVar() + 1;  // the number of linear terms always present.

  TJLterm<T> xtmp[ std::distance( xstart0, xend0)] ;

  std::copy( xstart0, xend0, &xtmp[0] );

  const_iterator const xstart( &xtmp[0] );
  const_iterator const   xend( &xtmp[std::distance( xstart0, xend0)] );

  const_iterator px(xstart);
  const_iterator py(ystart);

 x->clear();

 for (int i=0; i<nlinear; ++i, ++px, ++py ) {
    value =  px->value_;
    T_function(value, py->value_);
    x->append( TJLterm<T>( value, px->offset_, px->weight_) ); 
  }
 x->count_ = 0;
 
  while(  (px < xend) && (py < yend)  ) {
     if ( px->offset_  ==  py->offset_) { 
          value  =  px->value_;  
          T_function( value, py->value_);  
          x->append( TJLterm<T>( value, px->offset_, px->weight_) ); 
          ++px;  ++py;
          continue; 
     }
            
     if ( px->offset_  <   py->offset_) { 
          x->append( *px ); 
          ++px; 
          continue; 
     }   

     if ( px->offset_  >   py->offset_)  { 
          T value = T(); // this is necessary for subtraction 
          T_function (value, py->value_); 
          x->append( TJLterm<T>(value, py->offset_, py->weight_) );    
          ++py; 
          continue; 
     }   
  }
   
  while ( px < xend ) {
       x->append( *px );
       ++px; 
  }   

  while  ( py<yend ) {
       T value = T();  // this is necessary for subtraction 
       T_function (value, py->value_); 
       x->append( TJLterm<T>(value, py->offset_, py->weight_ )); 
       ++py; 
  }   

  x->weight_  = std::max( weight,  y->weight_  );
  x->lowWgt_  = std::min( lowWgt,  y->lowWgt_ );   
  x->accuWgt_ = std::min( accuWgt, y->accuWgt_);

  return x;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator+(  JLPtr<T> const& x,  JLPtr<T> const& y  )
{  
  JLPtr<T> z = x->clone();
  return TJL<T>::template inplace_add<TJL<T>::op_add >(z,y); 
  //return TJL<T>::template add<TJL<T>::op_add >(x,y); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&   operator+=(JLPtr<T>& x, JLPtr<T> const& y  )
{  
  return TJL<T>::template inplace_add<TJL<T>::op_add >(x,y); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const & x,  JLPtr<T> const& y  )
{
  JLPtr<T> z = x->clone();
  return   TJL<T>::template inplace_add<TJL<T>::op_sub>(z,y); 
  //return   TJL<T>::template add<TJL<T>::op_sub>(x,y); 
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&   operator-=(JLPtr<T>& x, JLPtr<T> const& y  )
{  
  return TJL<T>::template inplace_add<TJL<T>::op_sub >(x,y); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const &x) 
{
 JLPtr<T> z( x->clone() );
 z->Negate();
 return z;
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & lhs,  T const& rhs  )
{  
  JLPtr<T> z (lhs->clone() );
  return ( z *= rhs );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*( T const & lhs,             JLPtr<T> const& rhs  )
{  
  return rhs * lhs;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  JLPtr<T> const& y  ){  

typedef typename TJL<T>::iterator             iterator;
typedef typename TJL<T>::const_iterator const_iterator;

//----------------------------
// Multiplication of two jets
//----------------------------

 // Consistency check 

 if( x->myEnv_ != y->myEnv_ ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJLPtr<T> operator*( const JLPtr<T>&, const JLPtr<T>& ) ",
          "Inconsistent environments." ) );
 };

 EnvPtr<T> pje(x->myEnv_);
 JLPtr<T> z( TJL<T>::makeTJL( pje ) );


 // ---------------------------------------------------------------------------------------
 // Determine the lowest weight of the product and the maximum weight computed accurately.
 // ---------------------------------------------------------------------------------------

 int testWeight = std::min( (x->accuWgt_ + y->lowWgt_), (y ->accuWgt_+ x->lowWgt_) );
     testWeight = std::min( testWeight,  pje->maxWeight() ); 

 //------------------------------------------------------------------------------
 // Is the max accurate weight == 1 ?  If so, no sparsity. perform the multiplication directly 
 //-------------------------------------------------------------------------------

 if ( pje->maxWeight() == 1 ) {

    T result;

    iterator px = x->begin();
    iterator py = y->begin();
    iterator pz = z->begin();
    
    T std_x =  px->value_;
    T std_y =  py->value_;
 
    pz->value_ = std_x * std_y; // std part
    ++px; ++py; ++pz; 
    
    for( int i=1; i < z->myEnv_->numVar()+1;  ++i, ++px, ++py, ++pz) { // NOTE: index starts at 1 since std part is at 0 ! 

     result  = ( std_x * py->value_ ) + ( std_y * px->value_ ); 

     if( std::abs( result ) < TJL<T>::mx_small_ )   result = T();

     pz->value_ = result;
   }

   z->accuWgt_ = testWeight;
   return z;
 }
 
 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 

 std::vector<TJLterm<T> >& tjlmml =  x->myEnv_->TJLmml(); // the environment scratchpad

 const_iterator const xstart    = x->begin();
 const_iterator const xend      = x->end();
 const_iterator const ystart    = y->begin();
 const_iterator const yend      = y->end();
 
for(   const_iterator p = ystart; p != yend; ++p  ) {
  for(   const_iterator q = xstart; q != xend; ++q  ) {

     if( ( p->weight_ + q->weight_ ) > testWeight ) continue;   

     int indy = pje->multOffset( p->offset_ , q->offset_ );

     // Will work even when the exponents are all zero. 

     T  product; 
     T& value = tjlmml[indy].value_ += ( product = ( p->value_ * q->value_ ) );

     if ( ( std::abs(value) <  TJL<T>::mx_small_* std::abs( product ) ) || 
          ( std::abs(value) <  TJL<T>::mx_small_ )  ) { 
             value = T();
     }

 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad
 //------------------------------------------------------------------------------------------------
   
 z->transferFromScratchPad(); // NOTE: this sets lowWgt_
 z->accuWgt_ = testWeight;

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> & x,     JLPtr<T> const& y  )  
{

typedef typename TJL<T>::iterator             iterator;
typedef typename TJL<T>::const_iterator const_iterator;

//------------------------------------
// In place Multiplication of two jets
//------------------------------------

 // Consistency check 

 if( x->myEnv_ != y->myEnv_ ) {
   throw( GenericException( __FILE__, __LINE__, 
          "multiply(JLPtr<T> const& ) ",
          "Inconsistent environments." ) );
 };

 // if one of the arguments is zero, clear x before returning 

 if( x->getCount() == 0 ) return x;
 if( y->getCount() == 0 ) { 

   x->clear();
   x->accuWgt_ = x->myEnv_->maxWeight();
   x->lowWgt_  = 0;
   x->weight_  = 0; 
   return x;
 }


 // -------------------------------------------------
 // Determine the maximum weight computed accurately.
 // -------------------------------------------------
 // lowWgt is the weight of the lowest weight non-zero term


 int testWeight = std::min( (x->accuWgt_ + y->lowWgt_), (y->accuWgt_+ x->lowWgt_) );
     testWeight = std::min( testWeight,  x->myEnv_->maxWeight() ); 
 
     x->accuWgt_ = testWeight;


 // -------------------------------------------------
 // Is this first order ?
 // -------------------------------------------------

  if ( x->myEnv_->maxWeight()  == 1 ) {

    T result;

    iterator px = x->begin();
    iterator py = y->begin();
    
    T std_x =  px->value_;
    T std_y =  py->value_;
 
    std_x *= std_y; // std part

    ++px; ++py;  
    
    for( int i=1; i < x->myEnv_->numVar()+1;  ++i, ++px, ++py ) { 
                      // NOTE: index starts at 1 since std part is at 0 ! 

     result  = ( std_x * py->value_ ) + ( std_y * px->value_ ); 

     if( std::abs( result ) < TJL<T>::mx_small_ )   result = T();

     px->value_ = result;
    
   }
   return x;
 }

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scratch pad.
 //  Use direct sequential access to access terms since order is
 //  immaterial here.
 //  ------------------------------------------------------------------ 
 
 EnvPtr<T> pje(x->myEnv_);

 std::vector<TJLterm<T> >& tjlmml =  x->myEnv_->TJLmml(); // the environment scratchpad

 const_iterator const xstart = x->begin();
 const_iterator const xend   = x->end();
 const_iterator const ystart = y->begin();
 const_iterator const yend   = y->end();

 int (TJetEnvironment<T>::* multOffset)( int const&, int const& ) const =  &TJetEnvironment<T>::multOffset;
 TJetEnvironment<T>& env = *pje; 

 for(   const_iterator p = ystart; p != yend; ++p  ) {
   for(  const_iterator q = xstart; q != xend; ++q  ) {

     if( ( p->weight_ + q->weight_ ) > testWeight ) continue;   

     int indy = (env.*multOffset)( p->offset_ , q->offset_ );

     // Will work even when the exponents are all zero.

     T  product; 
     T& value = tjlmml[indy].value_ += ( product = ( p->value_ * q->value_ ) );

     if ( ( std::abs(value) <  TJL<T>::mx_small_* std::abs( product ) ) || 
          ( std::abs(value) <  TJL<T>::mx_small_ )  ) { 
             value = T();
     }
 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad!
 //-------------------------------------------------------------------------------------------------

 x->transferFromScratchPad(); // NOTE: this sets x->lowWgt_
 x->accuWgt_  = testWeight;

 return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
JLPtr<T>  operator/(JLPtr<T> const& wArg,  JLPtr<T> const& uArg  )
{ 

 typedef typename TJL<T>::iterator             iterator;
 typedef typename TJL<T>::const_iterator const_iterator;

 if ( (wArg->myEnv_) != (uArg->myEnv_) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Inconsistent environments." ) );
 }

 if ( wArg->getCount() == 0) return JLPtr<T>( TJL<T>::makeTJL( wArg->myEnv_ ) );  // numerator is zero
 

 // Check for void operators ..

 if ( (uArg->getCount() == 0) ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Attempt to divide by a null TJL<T> variable." ) );
 }
 

 // ------------------------------------------
 // Is this a first order calculation ?
 // ------------------------------------------

 if ( uArg->myEnv_->maxWeight() == 1 ) {

     JLPtr<T>  z( TJL<T>::makeTJL(uArg->myEnv_) );

     iterator pu = uArg->begin();
     iterator pw = wArg->begin();
     iterator pz =    z->begin();

     T u_std =  pu->value_; 
     T w_std =  pw->value_;
 
     pz->value_ =  (w_std)/(u_std);
 
     ++pz; ++pu; ++pw;

     for( int i=0; i< z->myEnv_->numVar(); ++i, ++pu, ++pw, ++pz ) { 

     pz->value_ = ( ( u_std * pw->value_ ) - ( w_std  * pu->value_ ) )
                  / ( u_std * u_std); 

   };
   return z;

 }

 //-------------------------------------------------------------------
 // Initialize local variables and set the environment of the answer.
 //---------------------------------------------------------------------

 JLPtr<T>  v( TJL<T>::makeTJL(wArg->myEnv_) );
 JLPtr<T> vn( TJL<T>::makeTJL(wArg->myEnv_) );

 JLPtr<T>  w( TJL<T>::makeTJL(wArg->myEnv_) );
 JLPtr<T>  u( TJL<T>::makeTJL(*uArg )       );

 // Normalize the denominator

 if( uArg->standardPart() == T() ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JLPtr<T> const& wArg, JLPtr<T> const& uArg )",
          "Attempt to divide by nilpotent element." ) );
 }

 T   u0  = uArg->standardPart();
 // int wgt = uArg->weight_; // not used for the moment

 u *=  T(1.0)/ u0;

 //  -------------------
 //  Recursive algorithm
 //  ------------------- 

 int wl  = 0;
 int mw  =  wArg->myEnv_->maxWeight();

 v->setStandardPart( wArg->standardPart()); 
 
 wl = 0;

 while ( wl < mw ) {

 ++wl;
   vn  = u->truncMult( v, wl );
   vn  = vn->filter( wl, wl );
    w  = wArg->filter( wl, wl );
     
  // compute v = v + w - vn 

   v = v + (w-vn);      // v + w- vn                       
 
 }                                     
 
 // Correct normalization of the answer ..

 v *=  T(1.0)/ u0; 
 
 // Determine the maximum weight computed accurately. // IS THIS NEEDED ??? FIXME !
 // -------------------------------------------------

 v->lowWgt_  = std::min(  wArg->lowWgt_,  uArg->lowWgt_);
 v->accuWgt_ = std::min(  wArg->accuWgt_, uArg->accuWgt_);


 return v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator/( JLPtr<T> const & x,  T const& y  ){  

  JLPtr<T> z (x->clone() );
  return ( z *=  T(1.0) / y);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TJL<T>::transferFromScratchPad() {   

  clear();// clears all the terms of current TJL, including the std part ! 

  appendLinearTerms(  myEnv_->numVar() );                      

  typedef typename std::vector<TJLterm<T> >::iterator                 scpad_iterator;
  typedef typename std::vector<TJLterm<T> >::reverse_iterator reverse_scpad_iterator;
  
  scpad_iterator scpad_begin =   myEnv_->TJLmml().begin();
  scpad_iterator scpad_end   =   myEnv_->TJLmml().end();

  reverse_scpad_iterator scpad_rbegin =   myEnv_->TJLmml().rbegin();
  reverse_scpad_iterator scpad_rend   =   myEnv_->TJLmml().rend();
  
 // *Unconditionally* append the std part and the linear terms
 
  int i = 0;

  scpad_iterator it = scpad_begin;
  for( ;  i <myEnv_->numVar()+1 ; ++it, ++i) {
     jltermStore_[i].value_ = it->value_;
     it->value_ = T();
  }

  // then the non-linear terms

  for(  ; it < scpad_end; ++it) 
  {
   if (  it->value_  !=  T() )  {
     append( *it ); 
     it->value_ = T();
   }
  }

  //---------------------------------------------------------------------
  // **** Look for and set the weight of the lowest non-zero monomial ***
  //---------------------------------------------------------------------
  
  const_iterator itend = end();

  lowWgt_ = 0; 
  for( iterator it=begin();  it != itend; ++it) {
     if ( it->value_ != T() ) { 
        lowWgt_ = it->weight_; 
  	break;
      } 
  }

  //-------------------------------------------------
  // **** Look for and set the maximum weight present
  // -------------------------------------------------  	 

  weight_ = ( begin() == itend ) ? 0 : ( itend-1 )->weight_;
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TJL<T>::appendLinearTerms( int numvar ) {   

   TJLterm<T> term; 

  // --- append standard part

   term.offset_   = 0;  
   term.weight_   = 0;  

   TJLterm<T>* p = new( this->storePtr() ) TJLterm<T>(term); 

  // -- append weight == 1 terms 

  for (int i=0; i< myEnv_->numVar(); ++i) {  // terms of weight 1

   term.offset_   = i+1;                 // DANGER ! NOTE that the offset here is i+1, not 1  ! 
   term.weight_   = 1;                   // DANGER ! NOTE that the weight must be set here !

   p = new( this->storePtr() ) TJLterm<T>(term); 

   weight_  = std::max(weight_, p->weight_);
 }
  return;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template <typename T>
typename TJL<T>::iterator  TJL<T>::begin()
{
  return iterator( jltermStore_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TJL<T>::const_iterator  TJL<T>::begin() const 
{
  return const_iterator( jltermStore_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJL<T>::iterator    TJL<T>::end() 
{
  return iterator( jltermStoreCurrentPtr_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJL<T>::const_iterator TJL<T>::end()  const
{
  return const_iterator( jltermStoreCurrentPtr_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template<typename T>
typename TJL<T>::reverse_iterator    TJL<T>::rbegin() 
{
  return boost::make_reverse_iterator( end() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJL<T>::const_reverse_iterator    TJL<T>::rbegin()  const
{
  return  boost::make_reverse_iterator( end() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJL<T>::reverse_iterator    TJL<T>::rend() 
{
  return  boost::make_reverse_iterator( begin() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJL<T>::const_reverse_iterator    TJL<T>::rend()  const
{
  return  boost::make_reverse_iterator( begin() );
}

