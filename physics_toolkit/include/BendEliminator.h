/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BendEliminator.h
******
******             Header file for class BendEliminator
******             - concatenates sbends and rbends removes passive
******               elements of zero length between them.
******                                                                
******  Copyright (c) 2007  Universities Research Association, Inc.   
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
******             Batavia, IL   60510
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** REVISION HISTORY
******
****** October 17, 2007  Leo Michelotti
******                   michelotti@fnal.gov
****** - original version: modeled after QuadEliminator.h
******                                                                
**************************************************************************
*************************************************************************/


#ifndef BENDELIMINATOR_H
#define BENDELIMINATOR_H

#include <boost/shared_ptr.hpp>
#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

class sbend;
class rbend;
class beamline;

typedef boost::shared_ptr<sbend>     SbendPtr;
typedef boost::shared_ptr<rbend>     RbendPtr;
typedef boost::shared_ptr<beamline>  BmlPtr;

class BendEliminator : public ConstBmlVisitor {

 public:
   BendEliminator();
  ~BendEliminator();

  void visit( beamline   const&  );
  void visit( bmlnElmnt  const&  );
  void visit( sbend      const&  ); 
  void visit( rbend      const&  ); 

  void setRelaxed( bool );
  bool getRelaxed();
  bool toggleRelaxed();

  BmlPtr beamlinePtr();

  // Invoking routine is responsible for
  // using this beamline before the 
  // BendEliminator goes out of scope.

 private: 
  BendEliminator( BendEliminator const& ); // forbidden

  BmlPtr        bmlPtr_;
  bool          relaxed_;
  bool          build_sbend_;  // It should be impossible for both
  bool          build_rbend_;  // of these to be true simultaneously.
  bool          not_started_;
  double        length_;
  double        field_;
  double        bendAngle_;
  double        usFaceAngle_;
  double        dsFaceAngle_;
  std::string   new_bend_name_;
  Alignment     new_alignment_;
};


#endif // BENDELIMINATOR_H
