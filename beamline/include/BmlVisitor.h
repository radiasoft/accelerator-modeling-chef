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
class bmlnElmnt;
class combinedFunction;
class hkick;
class octupole;
class thinrfcavity;
class rfcavity;
class srot;
class vkick;
class monitor;
class marker;
class drift;
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
  virtual void visitBeamline( beamline* );

  virtual void visitBmlnElmnt( bmlnElmnt* ) {}

  virtual void visitHkick( hkick* x );

  virtual void visitOctupole( octupole* x ); 

  virtual void visitThinrfcavity( thinrfcavity* x ); 

  virtual void visitRfcavity( rfcavity* x ); 

  virtual void visitSrot( srot* x ); 

  virtual void visitVkick( vkick* x ); 

  virtual void visitMonitor( monitor* x ); 

  virtual void visitMarker( marker* x );

  virtual void visitDrift( drift* x ) ;

  virtual void visitRbend( rbend* x ) ;

  virtual void visitSbend( sbend* x ); 

  virtual void visitSector( sector* x ); 

  virtual void visitQuadrupole( quadrupole* x ); 

  virtual void visitThin2pole( thin2pole* x ); 

  virtual void visitThinQuad( thinQuad* x ); 

  virtual void visitThinSextupole( thinSextupole* x ); 

  virtual void visitThinOctupole( thinOctupole* x ); 

  virtual void visitThinDecapole( thinDecapole* x ); 

  virtual void visitThin12pole( thin12pole* x ); 

  virtual void visitThin14pole( thin14pole* x ); 

  virtual void visitThin16pole( thin16pole* x ); 

  virtual void visitThin18pole( thin18pole* x ); 

  virtual void visitThinMultipole( thinMultipole* x ); 

  virtual void visitSextupole( sextupole* x ); 

  virtual void visitBBLens( BBLens* x ); 

  virtual void visitThinSeptum( thinSeptum* x ); 

  virtual void visitThinLamb( thinLamb* x ); 

  virtual void visitCombinedFunction( combinedFunction* x );

  virtual void visitPinger( Pinger* x );

  virtual void visitHPinger( HPinger* x );

  virtual void visitVPinger( VPinger* x );

  virtual void visitKick( kick* x);

  virtual void visitSlot( Slot* x);

  virtual void visitCF_rbend( CF_rbend* x);

  virtual void visitCF_sbend( CF_sbend* x);


protected:
  BmlVisitor() {}
  virtual ~BmlVisitor() {}

};


// ************************************************************


class DLLEXPORT ConstBmlVisitor
{
public:
  virtual void visitBeamline( const beamline* );

  virtual void visitBmlnElmnt( const bmlnElmnt* ) {}

  virtual void visitHkick( const hkick* x );


  virtual void visitOctupole( const octupole* x ); 


  virtual void visitThinrfcavity( const thinrfcavity* x ); 


  virtual void visitRfcavity( const rfcavity* x ); 


  virtual void visitSrot( const srot* x ); 


  virtual void visitVkick( const vkick* x ); 


  virtual void visitMonitor( const monitor* x ); 


  virtual void visitMarker( const marker* x ); 


  virtual void visitDrift( const drift* x ); 


  virtual void visitRbend( const rbend* x ); 


  virtual void visitSbend( const sbend* x ); 


  virtual void visitSector( const sector* x );


  virtual void visitQuadrupole( const quadrupole* x ); 


  virtual void visitThin2pole( const thin2pole* x ); 


  virtual void visitThinQuad( const thinQuad* x ); 


  virtual void visitThinSextupole( const thinSextupole* x ); 


  virtual void visitThinOctupole( const thinOctupole* x ); 


  virtual void visitThinDecapole( const thinDecapole* x ); 


  virtual void visitThin12pole( const thin12pole* x ); 


  virtual void visitThin14pole( const thin14pole* x ); 


  virtual void visitThin16pole( const thin16pole* x ); 


  virtual void visitThin18pole( const thin18pole* x ); 


  virtual void visitThinMultipole( const thinMultipole* x ); 


  virtual void visitSextupole( const sextupole* x ); 


  virtual void visitBBLens( const BBLens* x ); 


  virtual void visitThinSeptum( const thinSeptum* x ); 


  virtual void visitThinLamb( const thinLamb* x ); 


  virtual void visitCombinedFunction( const combinedFunction* x );


  virtual void visitPinger( const Pinger* x );


  virtual void visitHPinger( const HPinger* x );


  virtual void visitVPinger( const VPinger* x );


  virtual void visitKick( const kick* x);


  virtual void visitSlot( const Slot* x);


  virtual void visitCF_rbend( const CF_rbend* x);


  virtual void visitCF_sbend( const CF_sbend* x);


protected:
  ConstBmlVisitor() {}
  virtual ~ConstBmlVisitor() {}

};

#endif // BMLVISITOR_H
