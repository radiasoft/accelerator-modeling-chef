/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Barnacle.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab    
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
****** - eliminated class BarnacleData 
****** - use std::list instead of dlist
****** - use refs in member function signatures instead of ptrs. 
****** - use boost::any with value semantics intead of void* objs to 
******   store arbitrary user data 
******
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

struct Barnacle 
{
  Barnacle(): id(), info() {} 
  Barnacle( std::string s,  boost::any  a): id(s), info(a) {} 

  bool operator==( std::string str  ) const;   
  bool operator==( Barnacle const& b) const;   

  bool operator< ( Barnacle const& b) const;   

  std::string    id;
  boost::any     info;
};


//-----------------------------------------------------------------------------



class DLLEXPORT BarnacleList 
{
 public:
  typedef  std::list<Barnacle>::iterator iterator;

  void             append    ( Barnacle const& );
  void             insert    ( Barnacle const& );

  void             eraseFirst( std::string   identifier );
  void             eraseAll  ( std::string   = "" );


  iterator find  ( std::string   identifier,  int instance = 1 );

  void             remove( iterator pos );
   
  iterator begin()       { return theList_.begin(); }
  iterator end()         { return theList_.end();   }

  bool empty()    const  { return theList_.empty(); }   

 private:
  std::list<Barnacle> theList_;
};

#endif // BARNACLE_H
