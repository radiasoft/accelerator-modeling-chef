/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Barnacle.cc
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
******  Authors:   Leo Michelotti (Original Version)                                    
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <functional>
#include <basic_toolkit/Barnacle.h>

   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Barnacle::operator<(  Barnacle const& b ) const 
{ 
  return ( id  < b.id  );  
}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Barnacle::operator==( Barnacle const& b) const 
{ 
  return ( id  == b.id ); 
}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Barnacle::operator==( std::string str  ) const 
{ 
  return ( id  == str  );  
}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BarnacleList::append( Barnacle const& x ) {
 theList_.push_back( x );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BarnacleList::insert( Barnacle const& x ) {

 theList_.push_front( x );

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void BarnacleList::clear() 
{

  theList_.clear();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void BarnacleList::eraseAll( std::string ident ) {


 bool (Barnacle::* fptr)(std::string) const = &Barnacle::operator==;

 theList_.erase( remove_if(theList_.begin(), theList_.end(),  
                 std::bind2nd( std::mem_fun_ref(fptr), ident ) ),  theList_.end() ); 
  

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BarnacleList::eraseFirst( std::string ident ) 
{

   
  for ( std::list<Barnacle>::iterator it = theList_.begin();  it != theList_.end(); ++it ) 
  {

    if ( (it->id) == ident ) 
    {
        theList_.erase(it);
        break;
    }

  }
  return;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BarnacleList::eraseLast( std::string ident ) 
{

  for ( std::list<Barnacle>::reverse_iterator rit  = theList_.rbegin();  
                                              rit != theList_.rend();  ++rit ) 
  {

    if ( (rit->id) == ident ) 
    {
        theList_.erase( --(rit.base()) );
        break;
    }

  }
  return;


}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BarnacleList::erase(  std::list<Barnacle>::reverse_iterator& rit ) 
{

  theList_.erase( --(rit.base()) );
  return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BarnacleList::erase(  std::list<Barnacle>::iterator& it ) 
{

  theList_.erase(it);
  return;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


BarnacleList::iterator BarnacleList::find( std::string ident, int n ) {

  int count = 0;

  std::list<Barnacle>::iterator it;
  for ( it = theList_.begin();  it != theList_.end(); ++it ) 
  {

    if ( it->id == ident ) ++count;
    if ( count  == n     ) return it;

  }
  
  return it;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


BarnacleList::reverse_iterator BarnacleList::rfind( std::string ident, int n ) 
{

  int count = 0;

  std::list<Barnacle>::reverse_iterator rit;
  for ( rit = theList_.rbegin();  rit != theList_.rend(); ++rit ) 
  {

    if ( rit->id == ident ) ++count;
    if ( count  == n     )  return rit;

  }
  
  return rit;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



