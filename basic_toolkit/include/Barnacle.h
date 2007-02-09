/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******                                    
******  File:      Barnacle.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab    
******                All Rights Reserved                             
******                                                                
******  Authors:   Leo Michelotti (Original version)
******             Jean-Francois Ostiguy                                     
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
****** REVISION HISTORY
******
****** Dec 2006   Jean-Francois Ostiguy
******            ostiguy@fnal.gov
******
****** - eliminated abstract class BarnacleData    
****** - use std::list instead of dlist
****** - use refs in member function signatures instead of ptrs. 
****** - use boost::any with value semantics intead of void* objs to 
******   store arbitrary user data. No inheritance involved. 
******
****** Feb 2007  ostiguy@fnal.gov
******
****** - added iterators, erase member functions, empty() and size()                                                      
******             
******                                                                
**************************************************************************
*************************************************************************/

#ifndef BARNACLE_H
#define BARNACLE_H

#include <basic_toolkit/globaldefs.h>
#include <boost/any.hpp>
#include <string>
#include <list>

struct Barnacle {

  Barnacle(): id(), info() {} 
  Barnacle( std::string s,  boost::any  a): id(s), info(a) {} 

  bool operator==( std::string str  ) const;   
  bool operator==( Barnacle const& b) const;   

  bool operator< ( Barnacle const& b) const;   

  std::string    id;
  boost::any     info;


};


//-----------------------------------------------------------------------------



class DLLEXPORT BarnacleList {


public:
 
  typedef  std::list<Barnacle>::iterator                     iterator;
  typedef  std::list<Barnacle>::const_iterator const_iterator;

  typedef  std::list<Barnacle>::reverse_iterator             reverse_iterator;
  typedef  std::list<Barnacle>::const_reverse_iterator const_reverse_iterator;

  void             append    ( Barnacle const& );
  void             insert    ( Barnacle const& );

  void             eraseFirst( std::string   identifier );
  void              eraseLast( std::string   identifier );  
  void               eraseAll( std::string   identifier );  
  void                  erase( iterator&           pos );
  void                  erase( reverse_iterator&   pos );
  void                  clear();


  iterator          find  ( std::string   identifier,  int instance = 1 ); // forward find
  reverse_iterator rfind  ( std::string   identifier,  int instance = 1 ); // reverse find

   
  iterator begin()                      { return theList_.begin();  }
  iterator end()                        { return theList_.end();    }

  const_iterator begin()      const     { return theList_.begin();  }
  const_iterator end()        const     { return theList_.end();    }

  reverse_iterator rbegin()             { return theList_.rbegin(); }
  reverse_iterator rend()               { return theList_.rend();   }

  const_reverse_iterator rbegin() const { return theList_.rbegin(); }
  const_reverse_iterator rend()   const { return theList_.rend();   }

  bool empty()    const                 { return theList_.empty();  }   
  int  size()     const                 { return theList_.size();   }   

 private:

  std::list<Barnacle> theList_;

};

#endif // BARNACLE_H
