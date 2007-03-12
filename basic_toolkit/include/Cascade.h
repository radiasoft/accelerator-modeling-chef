/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Cascade.h
******  Version:   1.0                    
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
****** Revision History
******
****** Sep 2006     Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******
****** - eliminated archaic "reconstruct" member functions. 
******   Use placemnent new instead.                                     
******
****** Mar 2006     ostiguy@fnal
******
****** -  refactored to take advantage of STL container/algorithms.
**************************************************************************
*************************************************************************/

#ifndef CASCADE_H
#define CASCADE_H

#include <basic_toolkit/globaldefs.h>
#include <list>
 
class IntArray;
class Cascade;

class Switch {

  friend class Cascade;

  public:

    Switch();
    Switch( int weight, int index, IntArray const& );
    Switch( Switch const& );
   ~Switch();

    int             index()           { return index_; }
    IntArray const& exponents() const { return xpt_; }

    struct PartialEqual {

      PartialEqual(int idx1, int idx2) : idx1_(idx1), idx2_(idx2) {}   
      bool operator()(Switch const* lhs, Switch const* rhs) 
      { return IntArray::PartialEqual (lhs->xpt_, rhs->xpt_, idx1_, idx2_); }

     int idx1_;
     int idx2_;
    };

    struct PartialLessThan {

      PartialLessThan(int idx1, int idx2):  idx1_(idx1), idx2_(idx2)  {}   
      bool operator()(Switch const* lhs, Switch const* rhs) 
      { return IntArray::PartialLessThan (lhs->xpt_, rhs->xpt_, idx1_, idx2_); }

     int idx1_;
     int idx2_;
    };


  private:
    int      w_;
    int      index_;
    IntArray xpt_;
    Switch** arrow_;

    DLLLOCAL void finishConstructor();
};



//----------------------------------------------------------------------
// Arguments: maxweight = maximum weight of possible IntArrays
//                    n = number of components in the IntArrays
//                    v = "verbosity." If true, the constructor
//                         will write to std::cout as it progresses.
// Restrictions: maxweight >= 0
//                       n >= 1
//----------------------------------------------------------------------

class DLLEXPORT Cascade
{
  public:
    Cascade( int maxweight = 2, int n = 2, bool v = false );
    Cascade( Cascade const& );
   ~Cascade();

    int index( IntArray    const& ) const;
    int index( int const*  const  ) const; // Warning: assumes appropriate dimension. 

    IntArray exponents( IntArray const& ) const;  // For diagnostic purposes. 
                                                  // returned value should be identical to argument.

  private:
    int                     maxWeight_;
    int                     numVar_;
    int                     numberOfMonomials_;
    int                     numberOfSwitches_;
    bool                    verbose_;
    std::vector<Switch>     arrayOfSwitches_;
    Switch**                startPoint_;

    void  finishConstructor();
    void  testSwitchAllocation( std::list<Switch*> const& ) const; 
    int   selfTest()                                        const;

};


inline int Cascade::index( IntArray const& e ) const
{

  IntArray::const_iterator it = e.begin();

  Switch* swPtr = startPoint_[ *it ];
  
  for( int j=1; j < numVar_; ++j ) {
    swPtr = (Switch*) ( swPtr->arrow_[ *(++it) ] ); 
  }

  return swPtr->index_;

}


#endif // CASCADE_H
