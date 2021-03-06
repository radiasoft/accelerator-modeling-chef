/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BmlVisitor.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
****** Mar 2007 ostiguy@fnal.gov
****** - function signatures based on references rather than ptrs
****** - take advantage of use dynamic type resolution for visit() functions 
****** - eliminated unsafe casts
****** - no implementation in header file (to minimize file dependencies)
******                                                              
**************************************************************************
*************************************************************************/

/*
**
** File: BmlVisitor.h
** 
** Header file for the visitor class BmlVisitor.
** This class is to be used as the base class for
** all actual beamline visitor objects.
** 
** --- Leo Michelotti
** --- August 21, 1997
**
** Added ConstBmlVisitor class.
**
** --- Leo Michelotti
** --- August 21, 2001
**
*/

#ifndef BMLVISITOR_H
#define BMLVISITOR_H

#include <basic_toolkit/globaldefs.h>

// Predefined classes ...

class beamline;
class BmlnElmnt;
class combinedFunction;
class hkick;
class octupole;
class thinrfcavity;
class rfcavity;
class LinacCavity;
class Solenoid;
class srot;
class vkick;
class Monitor;
class HMonitor;
class VMonitor;
class marker;
class Drift;
class rbend;
class sbend;
class sector;
class quadrupole;
class thin2pole;
class thinQuad;
class thinSextupole;
class thinOctupole;
class thinDecapole;
class thin12pole;
class thin14pole;
class thin16pole;
class thin18pole;
class thinMultipole;
class sextupole;
class BBLens;
class thinSeptum;
class thinLamb;
class Pinger;
class HPinger;
class VPinger;
class kick;
class Slot;
class CF_rbend;
class CF_sbend;

// ---------------------------

class DLLEXPORT BmlVisitor 
{
public:

  void setInnerFlag( bool ) const; 
  bool getInnerFlag()       const;

  virtual void visit( beamline&      x);

  virtual void visit( BmlnElmnt&     x) {}

  virtual void visit( hkick&         x );

  virtual void visit( octupole&      x ); 

  virtual void visit( thinrfcavity&  x ); 

  virtual void visit( rfcavity&      x ); 

  virtual void visit( LinacCavity&   x ); 

  virtual void visit( Solenoid&      x ); 

  virtual void visit( srot&          x ); 

  virtual void visit( vkick&         x ); 

  virtual void visit( Monitor&       x ); 

  virtual void visit( HMonitor&      x ); 

  virtual void visit( VMonitor&      x ); 

  virtual void visit( marker&        x ); 

  virtual void visit( Drift&         x ) ;

  virtual void visit( rbend&         x ) ;

  virtual void visit( sbend&         x ); 

  virtual void visit( sector&        x ); 

  virtual void visit( quadrupole&    x ); 

  virtual void visit( thin2pole&     x ); 

  virtual void visit( thinQuad&      x ); 

  virtual void visit( thinSextupole& x ); 

  virtual void visit( thinOctupole&  x ); 

  virtual void visit( thinDecapole&  x ); 

  virtual void visit( thin12pole&    x ); 

  virtual void visit( thin14pole&    x ); 

  virtual void visit( thin16pole&    x ); 

  virtual void visit( thin18pole&    x ); 

  virtual void visit( thinMultipole& x ); 

  virtual void visit( sextupole&     x ); 

  virtual void visit( BBLens&        x ); 

  virtual void visit( thinSeptum&    x ); 

  virtual void visit( thinLamb&      x ); 

  virtual void visit( combinedFunction& x );

  virtual void visit( Pinger&        x );

  virtual void visit( HPinger&       x );

  virtual void visit( VPinger&       x );

  virtual void visit( kick&          x);

  virtual void visit( Slot&          x);

  virtual void visit( CF_rbend&      x);

  virtual void visit( CF_sbend&      x);


protected:

  BmlVisitor();
  virtual ~BmlVisitor();
  mutable bool inner_;

};

inline BmlVisitor::~BmlVisitor() {}

inline void BmlVisitor::setInnerFlag( bool set) const { inner_ = set; } 
inline bool BmlVisitor::getInnerFlag()          const { return inner_;}


class DLLEXPORT ConstBmlVisitor 
{
public:

  void setInnerFlag( bool ) const; 
  bool getInnerFlag()       const;

  virtual void visit( beamline      const& x );

  virtual void visit( BmlnElmnt     const& x ) {}

  virtual void visit( hkick         const& x );

  virtual void visit( octupole      const& x ); 

  virtual void visit( thinrfcavity  const& x ); 

  virtual void visit( rfcavity      const& x ); 

  virtual void visit( LinacCavity   const& x ); 

  virtual void visit( Solenoid      const& x ); 

  virtual void visit( srot          const& x ); 

  virtual void visit( vkick         const& x ); 

  virtual void visit( Monitor       const& x ); 

  virtual void visit( HMonitor      const& x ); 

  virtual void visit( VMonitor      const& x ); 

  virtual void visit( marker        const& x );

  virtual void visit( Drift         const& x ) ;

  virtual void visit( rbend         const& x ) ;

  virtual void visit( sbend         const& x ); 

  virtual void visit( sector        const& x ); 

  virtual void visit( quadrupole    const& x ); 

  virtual void visit( thin2pole     const& x ); 

  virtual void visit( thinQuad      const& x ); 

  virtual void visit( thinSextupole const& x ); 

  virtual void visit( thinOctupole  const& x ); 

  virtual void visit( thinDecapole  const& x ); 

  virtual void visit( thin12pole    const& x ); 

  virtual void visit( thin14pole    const& x ); 

  virtual void visit( thin16pole    const& x ); 

  virtual void visit( thin18pole    const& x ); 

  virtual void visit( thinMultipole const& x ); 

  virtual void visit( sextupole     const& x ); 

  virtual void visit( BBLens        const& x ); 

  virtual void visit( thinSeptum    const& x ); 

  virtual void visit( thinLamb      const& x ); 

  virtual void visit( combinedFunction const& x );

  virtual void visit( Pinger        const& x );

  virtual void visit( HPinger       const& x );

  virtual void visit( VPinger       const& x );

  virtual void visit( kick          const& x);

  virtual void visit( Slot          const& x);

  virtual void visit( CF_rbend      const& x);

  virtual void visit( CF_sbend      const& x);

protected:

  ConstBmlVisitor();
  virtual ~ConstBmlVisitor();
  mutable bool inner_; 

};

inline ConstBmlVisitor::~ConstBmlVisitor() {}

inline void ConstBmlVisitor::setInnerFlag( bool set) const { inner_ = set; } 
inline bool ConstBmlVisitor::getInnerFlag()          const { return inner_;}


#endif // BMLVISITOR_H
