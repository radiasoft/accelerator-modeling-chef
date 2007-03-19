/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      IntArray.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*******
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
****** Revision History:
******
******  Mar 2005  
******
******  Jean-Francois Ostiguy, ostiguy@fnal.gov
******
******  Efficiency improvements. 
******  - Data is no longer dynamically allocated, 
******    resulting in fixed size TJLterm<>.  
******  - exponents are represented by exponent_t type variables. 
******    exponent_t is defined by default as signed char (8 bits) 
******  - no explicit destructor 
******  - added iterator classes
******  - eliminated inefficient array style 
******    dereferencing 
*****
******  Sep 2006 ostiguy@fnal.gov 
******
****** - added MonomialOrderPredicate member. 
******   (useful for debugging)
******   
******  Feb 2007 ostiguy@fnal.gov
******  
****** - refactored to use STL iterators/algorithms
******
*************************************************************************
*************************************************************************
*************************************************************************/

#ifndef INTARRAY_H
#define INTARRAY_H

#include <iostream>
#include <exception>
#include <string>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <basic_toolkit/globaldefs.h>
#include <algorithm>
#include <numeric>
#include <vector>


// ***  Note:
// ***  Default max is set 8 variables (6 phase space vars + 2). 
// ***  Increase intarray_max_variables if more variables are needed.


// ***  Note:
// *** The following typedef can be safely redefined without 
// *** breaking mxyzptlk. 
 
typedef signed char exponent_t;


// **************************************************************************


class IntArray;

 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT IntArray {

public:

  // Iterators ... 

   template <class Value> 
   class intarray_iter : public boost::iterator_adaptor <
         intarray_iter<Value>                  // Derived
       , Value*                                // Base
       , boost::use_default                    // Value
       , boost::random_access_traversal_tag    // CategoryOrTraversal
    >
  {
   private:
      struct enabler {};  // a private type avoids misuse

   public:
      intarray_iter()
       : intarray_iter::iterator_adaptor_(0) {}

    explicit intarray_iter(Value* p)
      : intarray_iter::iterator_adaptor_(p) {}

    template <class OtherValue>
    intarray_iter(
        intarray_iter<OtherValue> const& other
      , typename boost::enable_if<
            boost::is_convertible<OtherValue*,Value*>
          , enabler
        >::type = enabler()
    )
      : intarray_iter::iterator_adaptor_(other.base()) {}

    private:
      friend class boost::iterator_core_access;
   };


  typedef   intarray_iter<exponent_t>                             iterator;
  typedef   intarray_iter<exponent_t const>                 const_iterator;

  typedef   boost::reverse_iterator<iterator>              reverse_iterator;
  typedef   boost::reverse_iterator<const_iterator>  const_reverse_iterator;
   
  iterator begin();                
  iterator end();

  const_iterator begin()    const;
  const_iterator end()      const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  const_reverse_iterator rbegin()  const; 
  const_reverse_iterator rend()    const;


  // Constructors and the destructor ...

  IntArray( int  dim=0, int const* const components = 0 );

  IntArray( IntArray::const_iterator it1, IntArray::const_iterator it2 );

  IntArray( IntArray const& x);


  // Assignment ...

  void     Set              ( int const* );
  void     Set              ( int  );

  int          operator()  (  int i )  const;
  exponent_t&  operator()  (  int i );  

  exponent_t   operator[]  (  int i )  const;
  exponent_t&  operator[]  (  int i );  


  // Functions ...

  IntArray&     operator=      ( IntArray const& );

  bool          operator==     ( IntArray const& ) const;

  bool          operator!=     ( IntArray const& ) const; 
  bool          operator<      ( IntArray const& ) const; // pure **reverse** lexicographical ordering. Does not compare weights
  bool          operator<=     ( IntArray const& ) const; // pure **reverse** lexicographical ordering. Does not compare weights
  bool          operator>=     ( IntArray const& ) const; // pure **reverse** lexicographical ordering. Does not compare weights
  bool          operator>      ( IntArray const& ) const; // pure **reverse** lexicographical ordering. Does not compare weights 

  bool          operator==     ( int ) const;
  bool          operator!=     ( int ) const;

  bool          IsNull         () const;

  static bool   PartialEqual         ( IntArray const& lhs, IntArray const& rhs, int idx1, int idx2); //  pure **standard** lexicographical ordering
  static bool   PartialLessThan      ( IntArray const& lhs, IntArray const& rhs, int idx1, int idx2); //  pure **standard** lexicographical ordering

  IntArray      operator+( IntArray const& ) const;

  // Queries ...

   int  Dim() const;
   int  Sum() const;

  // Utilities ..

  friend std::ostream& operator<<( std::ostream&, const IntArray& );
  friend std::istream& operator>>( std::istream&, IntArray& );

  // Exceptions ...
  struct GenericException : public std::exception
  {
    GenericException( const char*, const char* );
    // 1st argument: identifies function containing throw
    // 2nd         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };



private:

  static const int   intarray_max_variables = 8;

  int           size_;         
  exponent_t    comp_[ intarray_max_variables ];

  // ~IntArray();  // WARNING: MUST NOT BE DEFINED !

};
 
//----------------------------------------------------------------------------------------------------------

inline  IntArray::iterator IntArray::begin()                        { return IntArray::iterator(&comp_[0]);             }
inline  IntArray::iterator IntArray::end()                          { return IntArray::iterator(&comp_[size_]);         } 

inline  IntArray::const_iterator IntArray::begin() const            { return IntArray::const_iterator(&comp_[0]);       }  
inline  IntArray::const_iterator IntArray::end()   const            { return IntArray::const_iterator(&comp_[size_]);   } 

inline  IntArray::reverse_iterator IntArray::rbegin()               { return IntArray::reverse_iterator(end()  );       } 
inline  IntArray::reverse_iterator IntArray::rend()                 { return IntArray::reverse_iterator(begin());       } 

inline  IntArray::const_reverse_iterator IntArray::rbegin() const   { return IntArray::const_reverse_iterator(end());   }  
inline  IntArray::const_reverse_iterator IntArray::rend()   const   { return IntArray::const_reverse_iterator(begin()); } 


inline IntArray::IntArray( IntArray const& x) 
 : size_(x.size_)
{
   std::copy ( x.begin(), x.end(), begin() ) ;
}


inline int IntArray::Sum() const 
{
    
    return std::accumulate( begin(), end(), int());
}


inline IntArray IntArray::operator+( IntArray const& y) const 
{
     
     IntArray ret( *this );

     std::transform( begin(), end(), y.begin(), ret.begin(), std::plus<exponent_t>());
  
     return ret;
}


inline int  IntArray::Dim() const 
{ 
  return size_;
}


inline  int     IntArray::operator()(  int i )  const
{ 
   int value = comp_[i]; 
   return value;
}    


inline  exponent_t&  IntArray::operator()  ( int  i )
{
  return comp_[i]; 
}    


inline exponent_t  IntArray::operator[](  int i )  const
{ 
   int value = comp_[i]; 
   return value;
}    


inline  exponent_t&  IntArray::operator[]  ( int  i )
{
  return comp_[i]; 
}    


#endif // INTARRAY_H
