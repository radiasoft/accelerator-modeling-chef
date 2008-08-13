/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Edge.h
******
******  Copyright (c) Fermi Alliance / Fermilab    
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
******  Author:    Jean-Francois Ostiguy                       
******             ostiguy@fnal.gov
******
****** REVISION HISTORY 
******
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/
#ifndef EDGE_H
#define EDGE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;
class quadrupole;
class Edge;

typedef boost::shared_ptr<Edge>              EdgePtr; 
typedef boost::shared_ptr<Edge const>   ConstEdgePtr; 

class DLLEXPORT Edge : public bmlnElmnt {

  class Propagator;  

public:

  Edge();
  Edge( std::string const& name, double const& strength );    
  Edge( Edge const& );

  Edge* Clone() const;

 ~Edge();

  Edge& operator=(Edge const& rhs);

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;

  char const* Type()       const;

  bool        isMagnet()   const;
  bool        isThin()     const;
  bool        isPassive()  const;

};

#endif // EDGE_H
