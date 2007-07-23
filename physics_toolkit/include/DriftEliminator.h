/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      DriftEliminator.h
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - updated visitor interface to advantage of dynamic typing
******                                                                
**************************************************************************
*************************************************************************/


/*
 * File: DriftEliminator.h
 * 
 * Header for class DriftEliminator
 * 
 * Concatenates drifts and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * May 17, 2001
 * 
 */

#ifndef DRIFTELIMINATOR_H
#define DRIFTELIMINATOR_H

#include <boost/shared_ptr.hpp>
#include <beamline/BmlVisitor.h>

class beamline;
class drift;
typedef boost::shared_ptr<beamline> BmlPtr;
typedef boost::shared_ptr<drift>    DriftPtr;


class DriftEliminator : public ConstBmlVisitor
{

 public:

  DriftEliminator();

 ~DriftEliminator();

  void visit(  beamline  const& );
  void visit(  bmlnElmnt const& );

  void visit(  monitor   const& ); 
  void visit(  marker    const& ); 
  void visit(  drift     const& ); 

  void visit(  Slot      const& );

  BmlPtr beamlinePtr();

 private: 

  DriftEliminator( DriftEliminator const& ); // forbidden

  void       handlePassiveElement( bmlnElmnt const& );

  BmlPtr     bmlPtr_;
  DriftPtr   driftPtr_;

};


#endif // DRIFTELIMINATOR_H
