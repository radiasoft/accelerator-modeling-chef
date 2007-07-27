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
// *** The following typedef can be safely redefined without 
// *** breaking mxyzptlk. 
 
typedef int exponent_t;


// **************************************************************************


class IntArray;

 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT IntArray {

public:


  typedef   std::vector<exponent_t >::iterator                 iterator;
  typedef   std::vector<exponent_t >::const_iterator     const_iterator;

  typedef   std::vector<exponent_t >::reverse_iterator             reverse_iterator;
  typedef   std::vector<exponent_t >::const_reverse_iterator const_reverse_iterator;
   
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

  void     Set              ( int const* , int  n);
  void     Set              ( int  );

  exponent_t const&   operator()  (  int i )  const;
  exponent_t&         operator()  (  int i );  

  exponent_t const&   operator[]  (  int i )  const;
  exponent_t&         operator[]  (  int i );  


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

   int  Dim()    const;
   int  Sum()    const;
   int  Weight() const;

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

  std::vector<exponent_t>   comp_;
  mutable int               weight_;
  mutable bool              weight_is_valid_;

};
 
//----------------------------------------------------------------------------------------------------------

inline  IntArray::iterator IntArray::begin()                        { return comp_.begin();    }
inline  IntArray::iterator IntArray::end()                          { return comp_.end();      } 

inline  IntArray::const_iterator IntArray::begin() const            { return comp_.begin();    }  
inline  IntArray::const_iterator IntArray::end()   const            { return comp_.end();      } 

inline  IntArray::reverse_iterator IntArray::rbegin()               { return comp_.rbegin();   } 
inline  IntArray::reverse_iterator IntArray::rend()                 { return comp_.rend();     } 

inline  IntArray::const_reverse_iterator IntArray::rbegin() const   { return comp_.rbegin();   }  
inline  IntArray::const_reverse_iterator IntArray::rend()   const   { return comp_.rend();     } 


inline IntArray::IntArray( IntArray const& x) 
  : comp_(x.comp_), weight_(x.weight_), weight_is_valid_ (x.weight_is_valid_)
{}


inline int IntArray::Sum() const 
{
  if (weight_is_valid_) {
    return weight_;
  }
  else{ 
    weight_is_valid_ = true;
    return ( weight_ = std::accumulate( begin(), end(), int()) );
  }
}


inline int IntArray::Weight() const
{
  return Sum(); 
}


inline int  IntArray::Dim() const 
{ 
  return comp_.size();
}


inline  exponent_t const&  IntArray::operator()(  int i )  const
{ 
   return comp_[i]; 
}    


inline  exponent_t&       IntArray::operator()  ( int  i )
{
  weight_is_valid_ = false;
  return comp_[i]; 
}    


inline exponent_t const&  IntArray::operator[](  int i )  const
{ 
   return comp_[i]; 
}    


inline  exponent_t&       IntArray::operator[]  ( int  i )
{
  weight_is_valid_ = false;
  return comp_[i]; 
}    


#endif // INTARRAY_H
