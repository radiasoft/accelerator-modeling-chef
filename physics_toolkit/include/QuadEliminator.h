/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      QuadEliminator.h
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - visitor takes advantage of dynamic typing
******                                                   
****** Aug 2008           ostiguy@fnal.gov
****** - revised to conform to the conventional semantics 
******   of the Visitor pattern. QuadEliminator now modifies the 
******   beamline in-place rather than return a new one through
******   a private member accessor.   
******                                                           
**************************************************************************
*************************************************************************/


/*
 * File: QuadEliminator.h
 * 
 * Header for class QuadEliminator
 * 
 * Concatenates quads and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * March 11, 2002
 * 
 */

#ifndef QUADELIMINATOR_H
#define QUADELIMINATOR_H

#include <boost/shared_ptr.hpp>
#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

class quadrupole;
class beamline;

typedef boost::shared_ptr<quadrupole>  QuadrupolePtr;
typedef boost::shared_ptr<beamline>  BmlPtr;




class QuadEliminator : public BmlVisitor {

 public:

   QuadEliminator();
  ~QuadEliminator();

  void visit( beamline&    );
  void visit( BmlnElmnt&   );
  void visit( quadrupole&  ); 

 private: 

  QuadEliminator( QuadEliminator const& ); // forbidden

  BmlPtr            bml_;
  QuadrupolePtr quadPtr_;
};


#endif // QUADELIMINATOR_H
